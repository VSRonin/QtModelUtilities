/****************************************************************************\
   Copyright 2018 Luca Beldi
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
\****************************************************************************/

#include "htmlmodelserialiser.h"
#include "private/htmlmodelserialiser_p.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

HtmlModelSerialiserPrivate::HtmlModelSerialiserPrivate(HtmlModelSerialiser *q)
    : AbstractStringSerialiserPrivate(q)
    , m_printStartDocument(true)
{ }

void HtmlModelSerialiserPrivate::writeHtmlElement(QXmlStreamWriter &destination, const QModelIndex &parent) const
{
    Q_ASSERT(m_constModel);
    const int rowCount = m_constModel->rowCount(parent);
    const int colCount = m_constModel->columnCount(parent);
    if (rowCount + colCount == 0)
        return;
    bool foundVhead = false;
    bool foundHhead = false;
    if (!parent.isValid()) {
        for (int i = 0; i < rowCount && !foundVhead; ++i) {
            for (QList<int>::const_iterator roleIter = m_rolesToSave.constBegin(); roleIter != m_rolesToSave.constEnd(); ++roleIter) {
                if (!m_constModel->headerData(i, Qt::Vertical, *roleIter).isNull()) {
                    foundVhead = true;
                    break;
                }
            }
        }
        for (int i = 0; i < colCount && !foundHhead; ++i) {
            for (QList<int>::const_iterator roleIter = m_rolesToSave.constBegin(); roleIter != m_rolesToSave.constEnd(); ++roleIter) {
                if (!m_constModel->headerData(i, Qt::Horizontal, *roleIter).isNull()) {
                    foundHhead = true;
                    break;
                }
            }
        }
    }

    destination.writeStartElement(QStringLiteral("table"));
    destination.writeAttribute(QStringLiteral("data-rowcount"), QString::number(rowCount));
    destination.writeAttribute(QStringLiteral("data-colcount"), QString::number(colCount));
    if (foundHhead) {
        destination.writeStartElement(QStringLiteral("tr"));
        if (foundVhead) {
            destination.writeStartElement(QStringLiteral("th"));
            destination.writeCharacters(QString());
            destination.writeEndElement(); // th
        }
        for (int i = 0; i < m_constModel->columnCount(); ++i) {
            destination.writeStartElement(QStringLiteral("th"));
            destination.writeAttribute(QStringLiteral("scope"), QStringLiteral("col"));
            for (QList<int>::const_iterator roleIter = m_rolesToSave.constBegin(); roleIter != m_rolesToSave.constEnd(); ++roleIter) {
                const QVariant headData = m_constModel->headerData(i, Qt::Horizontal, *roleIter);
                if (!headData.isNull()) {
                    destination.writeStartElement(QStringLiteral("div"));
                    destination.writeAttribute(QStringLiteral("data-rolecode"), QString::number(*roleIter));
                    destination.writeAttribute(QStringLiteral("data-varianttype"), QString::number(headData.userType()));
                    writeHtmlVariant(destination, headData);
                    destination.writeEndElement(); // div
                }
            }
            destination.writeEndElement(); // th
        }
        destination.writeEndElement(); // tr
    }
    for (int i = 0; i < rowCount; ++i) {
        destination.writeStartElement(QStringLiteral("tr"));
        if (foundVhead) {

            destination.writeStartElement(QStringLiteral("th"));
            destination.writeAttribute(QStringLiteral("scope"), QStringLiteral("row"));
            for (QList<int>::const_iterator roleIter = m_rolesToSave.constBegin(); roleIter != m_rolesToSave.constEnd(); ++roleIter) {
                const QVariant headData = m_constModel->headerData(i, Qt::Vertical, *roleIter);
                if (!headData.isNull()) {
                    destination.writeStartElement(QStringLiteral("div"));
                    destination.writeAttribute(QStringLiteral("data-rolecode"), QString::number(*roleIter));
                    destination.writeAttribute(QStringLiteral("data-varianttype"), QString::number(headData.userType()));
                    writeHtmlVariant(destination, headData);
                    destination.writeEndElement(); // div
                }
            }
            destination.writeEndElement(); // th
        }
        for (int j = 0; j < colCount; ++j) {
            destination.writeStartElement(QStringLiteral("td"));
            const QModelIndex currIndex = m_constModel->index(i, j, parent);
            for (QList<int>::const_iterator roleIter = m_rolesToSave.constBegin(); roleIter != m_rolesToSave.constEnd(); ++roleIter) {
                const QVariant roleData = currIndex.data(*roleIter);
                if (!roleData.isNull()) {
                    destination.writeStartElement(QStringLiteral("div"));
                    destination.writeAttribute(QStringLiteral("data-rolecode"), QString::number(*roleIter));
                    destination.writeAttribute(QStringLiteral("data-varianttype"), QString::number(roleData.userType()));
                    writeHtmlVariant(destination, roleData);
                    destination.writeEndElement(); // div
                }
            }
            if (m_constModel->hasChildren(currIndex))
                writeHtmlElement(destination, currIndex);
            destination.writeEndElement(); // td
        }
        destination.writeEndElement(); // tr
    }
    destination.writeEndElement(); // table
}

bool HtmlModelSerialiserPrivate::readHtmlElement(QXmlStreamReader &source, const QModelIndex &parent)
{
    enum SableHeadCode { None = -1, Col = Qt::Horizontal, Row = Qt::Vertical };
    Q_ASSERT(m_model);
    if (source.name() != QStringLiteral("table"))
        return false;
    const QXmlStreamAttributes tableAttributes = source.attributes();
    if (!tableAttributes.hasAttribute(QStringLiteral("data-rowcount")) || !tableAttributes.hasAttribute(QStringLiteral("data-colcount")))
        return false;
    m_model->insertRows(0, tableAttributes.value(QStringLiteral("data-rowcount")).toInt(), parent);
    m_model->insertColumns(0, tableAttributes.value(QStringLiteral("data-colcount")).toInt(), parent);
    bool rowStarted = false;
    bool cellStarted = false;
    int currentRow = 0;
    int currentCol = 0;
    bool tdFound = false;
    SableHeadCode headCode = None;
    while (!source.atEnd() && !source.hasError()) {
        source.readNext();
        if (source.isStartElement()) {
            if (source.name() == QStringLiteral("tr")) {
                if (currentRow >= m_model->rowCount(parent))
                    return false;
                rowStarted = true;
            } else if (rowStarted && !parent.isValid() && source.name() == QStringLiteral("th")) {
                const QXmlStreamAttributes headAttributes = source.attributes();
                if (headAttributes.hasAttribute(QStringLiteral("scope"))) {
                    const auto headScope = headAttributes.value(QStringLiteral("scope"));
                    if (headScope.compare(QStringLiteral("row")) == 0)
                        headCode = Row;
                    else if (headScope.compare(QStringLiteral("col")) == 0)
                        headCode = Col;
                }
            } else if (rowStarted && source.name() == QStringLiteral("td")) {
                tdFound = true;
                if (currentCol >= m_model->columnCount(parent))
                    return false;
                cellStarted = true;
            } else if (source.name() == QStringLiteral("div")) {
                const QXmlStreamAttributes cellAttributes = source.attributes();
                if (!cellAttributes.hasAttribute(QStringLiteral("data-varianttype")) || !cellAttributes.hasAttribute(QStringLiteral("data-rolecode")))
                    return false;
                const int cellRole = cellAttributes.value(QStringLiteral("data-rolecode")).toInt();
                const int cellType = cellAttributes.value(QStringLiteral("data-varianttype")).toInt();
                if (cellStarted) {
                    m_model->setData(m_model->index(currentRow, currentCol, parent), readHtmlVariant(source, cellType), cellRole);
                } else if (headCode == Row) {
                    m_model->setHeaderData(currentRow, Qt::Vertical, readHtmlVariant(source, cellType), cellRole);
                } else if (headCode == Col) {
                    m_model->setHeaderData(currentCol, Qt::Horizontal, readHtmlVariant(source, cellType), cellRole);
                }
            } else if (cellStarted && source.name() == QStringLiteral("table")) {
                if (!readHtmlElement(source, m_model->index(currentRow, currentCol, parent)))
                    return false;
            }
        } else if (source.isEndElement()) {
            if (source.name() == QStringLiteral("tr")) {
                rowStarted = false;
                currentCol = 0;
                if (tdFound)
                    ++currentRow;
                tdFound = false;
            } else if (rowStarted && source.name() == QStringLiteral("td")) {
                ++currentCol;
                cellStarted = false;
            } else if (rowStarted && source.name() == QStringLiteral("th")) {
                if (headCode == Col)
                    ++currentCol;
                headCode = None;
            } else if (source.name() == QStringLiteral("table")) {
                return true;
            }
        }
    }
    return false;
}

bool HtmlModelSerialiserPrivate::writeHtml(QXmlStreamWriter &writer) const
{
    if (!m_constModel)
        return false;
    if (m_printStartDocument) {
        writer.writeDTD(QStringLiteral("<!DOCTYPE html>"));
        writer.writeStartElement(QStringLiteral("html"));
        writer.writeStartElement(QStringLiteral("head"));
        writer.writeEmptyElement(QStringLiteral("meta"));
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        writer.writeAttribute(QStringLiteral("charset"), QString::fromLatin1(writer.codec()->name()));
#else
        writer.writeAttribute(QStringLiteral("charset"), QStringLiteral("UTF-8"));
#endif
        writer.writeStartElement(QStringLiteral("title"));
        const QString modelName = m_constModel->objectName();
        writer.writeCharacters(modelName.isEmpty() ? QStringLiteral("Model") : modelName);
        writer.writeEndElement(); // title
        writer.writeStartElement(QStringLiteral("style"));
        writer.writeCharacters(QStringLiteral("table, th, td { border: 1px solid black; } table { border-collapse: collapse; width: 100%; }"));
        writer.writeEndElement(); // style
        writer.writeEndElement(); // head
        writer.writeStartElement(QStringLiteral("body"));
    }
    writer.writeStartElement(QStringLiteral("div"));
    writer.writeAttribute(QStringLiteral("data-modelcode"), Magic_Model_Header);
    writeHtmlElement(writer);
    writer.writeEndElement(); // div
    if (m_printStartDocument) {
        writer.writeEndElement(); // body
        writer.writeEndElement(); // html
    }
    return !writer.hasError();
}

bool HtmlModelSerialiserPrivate::readHtml(QXmlStreamReader &reader)
{
    if (!m_model)
        return false;
    m_model->removeColumns(0, m_model->columnCount());
    m_model->removeRows(0, m_model->rowCount());
    bool mainDivStarted = false;
    while (!reader.atEnd() && !reader.hasError()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == QStringLiteral("div")) {
                const QXmlStreamAttributes mainDivAttribute = reader.attributes();
                if (!mainDivAttribute.hasAttribute(QStringLiteral("data-modelcode")))
                    return false;
                if (mainDivAttribute.value(QStringLiteral("data-modelcode")).compare(Magic_Model_Header) != 0)
                    return false;
                mainDivStarted = true;
            } else if (mainDivStarted && reader.name() == QStringLiteral("table")) {
                if (!readHtmlElement(reader)) {
                    m_model->removeColumns(0, m_model->columnCount());
                    m_model->removeRows(0, m_model->rowCount());
                    return false;
                }
            }
        } else if (reader.isEndElement()) {
            if (reader.name() == QStringLiteral("div")) {
                mainDivStarted = false;
            }
        }
    }
    if (reader.hasError()) {
        m_model->removeColumns(0, m_model->columnCount());
        m_model->removeRows(0, m_model->rowCount());
        return false;
    }
    return true;
}

void HtmlModelSerialiserPrivate::writeHtmlVariant(QXmlStreamWriter &writer, const QVariant &val) const
{
    if (isImageType(val.userType())) {
        writer.writeEmptyElement(QStringLiteral("img"));
        writer.writeAttribute(QStringLiteral("src"), QLatin1String("data:image/png;base64,") + saveVariant(val));
        writer.writeAttribute(QStringLiteral("alt"), QStringLiteral("modelimage.png"));
        return;
    }
    writer.writeCharacters(saveVariant(val));
}

QVariant HtmlModelSerialiserPrivate::readHtmlVariant(QXmlStreamReader &reader, int valType) const
{
    if (isImageType(valType)) {
        if (!reader.readNextStartElement())
            return QVariant();
        if (reader.name() != QStringLiteral("img"))
            return QVariant();
        const QXmlStreamAttributes imgAttributes = reader.attributes();
        if (!imgAttributes.hasAttribute(QStringLiteral("src")))
            return QVariant();
        return loadVariant(valType, imgAttributes.value(QStringLiteral("src")).toString().remove(0, QStringLiteral("data:image/png;base64,").size()));
    }
    return loadVariant(valType, reader.readElementText());
}

/*!
Constructs a serialiser
*/
HtmlModelSerialiser::HtmlModelSerialiser(QObject *parent)
    : AbstractStringSerialiser(*new HtmlModelSerialiserPrivate(this), parent)
{ }

/*!
Constructs a serialiser operating over \a model
*/
HtmlModelSerialiser::HtmlModelSerialiser(QAbstractItemModel *model, QObject *parent)
    : AbstractStringSerialiser(*new HtmlModelSerialiserPrivate(this), parent)
{
    setModel(model);
}

/*!
\overload

the model will only be allowed to be saved, not loaded
*/
HtmlModelSerialiser::HtmlModelSerialiser(const QAbstractItemModel *model, QObject *parent)
    : AbstractStringSerialiser(*new HtmlModelSerialiserPrivate(this), parent)
{
    setModel(model);
}

/*!
\internal
*/
HtmlModelSerialiser::HtmlModelSerialiser(HtmlModelSerialiserPrivate &d, QObject *parent)
    : AbstractStringSerialiser(d, parent)
{ }

/*!
\property HtmlModelSerialiser::printStartDocument
\accessors %printStartDocument(), setPrintStartDocument()
\brief This property determines if the start of the html document should be written
\details If this property is set to \c true (the default) the serialiser will write the \c \<html\> starting block.
Set this to false to save the model as part of a larger html document or if you want to specify your custom css in the \c \<head\>
*/
bool HtmlModelSerialiser::printStartDocument() const
{
    Q_D(const HtmlModelSerialiser);

    return d->m_printStartDocument;
}

void HtmlModelSerialiser::setPrintStartDocument(bool val)
{
    Q_D(HtmlModelSerialiser);

    d->m_printStartDocument = val;
}

/*!
\reimp
*/
bool HtmlModelSerialiser::saveModel(QString *destination) const
{
    if (!destination)
        return false;
    Q_D(const HtmlModelSerialiser);

    if (!d->m_constModel)
        return false;
    QXmlStreamWriter writer(destination);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    writer.setCodec(textCodec());
#endif
    return d->writeHtml(writer);
}

/*!
\reimp
*/
bool HtmlModelSerialiser::saveModel(QIODevice *destination) const
{
    if (!destination)
        return false;
    if (!destination->isOpen()) {
        if (!destination->open(QIODevice::WriteOnly))
            return false;
    }
    if (!destination->isWritable())
        return false;
    Q_D(const HtmlModelSerialiser);

    if (!d->m_constModel)
        return false;
    QXmlStreamWriter writer(destination);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    writer.setCodec(textCodec());
#endif
    return d->writeHtml(writer);
}

/*!
\reimp
*/
bool HtmlModelSerialiser::saveModel(QByteArray *destination) const
{
    if (!destination)
        return false;
    Q_D(const HtmlModelSerialiser);

    if (!d->m_constModel)
        return false;
    QXmlStreamWriter writer(destination);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    writer.setCodec(textCodec());
#endif
    return d->writeHtml(writer);
}

/*!
\reimp
*/
bool HtmlModelSerialiser::loadModel(QIODevice *source)
{
    if (!source)
        return false;
    if (!source->isOpen()) {
        if (!source->open(QIODevice::ReadOnly))
            return false;
    }
    if (!source->isReadable())
        return false;
    Q_D(HtmlModelSerialiser);

    if (!d->m_model)
        return false;
    QXmlStreamReader reader(source);
    return d->readHtml(reader);
}

/*!
\reimp
*/
bool HtmlModelSerialiser::loadModel(const QByteArray &source)
{
    Q_D(HtmlModelSerialiser);

    if (!d->m_model)
        return false;
    QXmlStreamReader reader(source);
    return d->readHtml(reader);
}

/*!
\reimp
*/
bool HtmlModelSerialiser::loadModel(QString *source)
{
    if (!source)
        return false;
    Q_D(HtmlModelSerialiser);

    if (!d->m_model)
        return false;
    QXmlStreamReader reader(*source);
    return d->readHtml(reader);
}

/*!
\class HtmlModelSerialiser

\brief Serialiser to save and load models in HTML format
*/
