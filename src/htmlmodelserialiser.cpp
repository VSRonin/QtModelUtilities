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
HtmlModelSerialiserPrivate::HtmlModelSerialiserPrivate(HtmlModelSerialiser* q)
    :AbstractMultiRoleSerialiserPrivate(q)
    , m_printStartDocument(true)
{}

void HtmlModelSerialiserPrivate::writeHtmlElement(QXmlStreamWriter& destination, const QModelIndex& parent) const
{
    Q_ASSERT(m_constModel);
    if (m_constModel->rowCount(parent) + m_constModel->columnCount(parent) == 0)
        return;
    bool foundVhead = false;
    bool foundHhead = false;
    if (!parent.isValid()) {
        for (int i = 0; i < m_constModel->rowCount() && !foundVhead; ++i) {
            for (QList<int>::const_iterator roleIter = m_rolesToSave.constBegin(); roleIter != m_rolesToSave.constEnd(); ++roleIter) {
                if (!m_constModel->headerData(i, Qt::Vertical, *roleIter).isNull()) {
                    foundVhead = true;
                    break;
                }
            }
        }
        for (int i = 0; i < m_constModel->columnCount() && !foundHhead; ++i) {
            for (QList<int>::const_iterator roleIter = m_rolesToSave.constBegin(); roleIter != m_rolesToSave.constEnd(); ++roleIter) {
                if (!m_constModel->headerData(i, Qt::Horizontal, *roleIter).isNull()) {
                    foundHhead = true;
                    break;
                }
            }
        }
    }

    destination.writeStartElement(QStringLiteral("table"));
    destination.writeAttribute(QStringLiteral("data-rowcount"), QString::number(m_constModel->rowCount(parent)));
    destination.writeAttribute(QStringLiteral("data-colcount"), QString::number(m_constModel->columnCount(parent)));
#ifdef QT_DEBUG
    destination.writeAttribute("border", "1");
#endif // QT_DEBUG
    if (foundHhead) {
        destination.writeStartElement(QStringLiteral("tr"));
        if (foundVhead) {
            destination.writeStartElement(QStringLiteral("th"));
            destination.writeAttribute(QStringLiteral("scope"), QStringLiteral("col"));
            destination.writeEndElement(); //th
        }
        for (int i = 0; i < m_constModel->columnCount(); ++i) {
            destination.writeStartElement(QStringLiteral("th"));
            destination.writeAttribute(QStringLiteral("scope"), QStringLiteral("col"));
            for (QList<int>::const_iterator roleIter = m_rolesToSave.constBegin(); roleIter != m_rolesToSave.constEnd(); ++roleIter) {
                const QVariant headData = m_constModel->headerData(i, Qt::Horizontal, *roleIter);
                if (!headData.isNull()) {
                    destination.writeStartElement("div");
                    destination.writeAttribute(QStringLiteral("data-rolecode"), QString::number(*roleIter));
                    destination.writeAttribute(QStringLiteral("data-varianttype"), QString::number(headData.type()));
                    writeHtmlVariant(destination, headData);
                    destination.writeEndElement(); //div
                }
            }
            destination.writeEndElement(); //th
        }
        destination.writeEndElement(); //tr
    }
    for (int i = 0; i < m_constModel->rowCount(parent); ++i) {
        destination.writeStartElement(QStringLiteral("tr"));
        if (foundVhead) {

            destination.writeStartElement(QStringLiteral("th"));
            destination.writeAttribute(QStringLiteral("scope"), QStringLiteral("row"));
            for (QList<int>::const_iterator roleIter = m_rolesToSave.constBegin(); roleIter != m_rolesToSave.constEnd(); ++roleIter) {
                const QVariant headData = m_constModel->headerData(i, Qt::Vertical, *roleIter);
                if (!headData.isNull()) {
                    destination.writeStartElement("div");
                    destination.writeAttribute(QStringLiteral("data-rolecode"), QString::number(*roleIter));
                    destination.writeAttribute(QStringLiteral("data-varianttype"), QString::number(headData.type()));
                    writeHtmlVariant(destination, headData);
                    destination.writeEndElement(); //div
                }
            }
            destination.writeEndElement(); //th
        }
        for (int j = 0; j < m_constModel->columnCount(parent); ++j) {
            destination.writeStartElement(QStringLiteral("td"));
            const QModelIndex currIndex = m_constModel->index(i, j, parent);
            for (QList<int>::const_iterator roleIter = m_rolesToSave.constBegin(); roleIter != m_rolesToSave.constEnd(); ++roleIter) {
                const QVariant roleData = currIndex.data(*roleIter);
                if (!roleData.isNull()) {
                    destination.writeStartElement("div");
                    destination.writeAttribute(QStringLiteral("data-rolecode"), QString::number(*roleIter));
                    destination.writeAttribute(QStringLiteral("data-varianttype"), QString::number(roleData.type()));
                    writeHtmlVariant(destination, roleData);
                    destination.writeEndElement(); //div
                }
            }
            if (m_constModel->hasChildren(currIndex))
                writeHtmlElement(destination, currIndex);
            destination.writeEndElement(); //td
        }
        destination.writeEndElement(); //tr
    }
    destination.writeEndElement(); // table
}

bool HtmlModelSerialiserPrivate::readHtmlElement(QXmlStreamReader& source, const QModelIndex& parent)
{
    enum SableHeadCode
    {
        None = -1
        , Col = Qt::Horizontal
        , Row = Qt::Vertical
    };
    Q_ASSERT(m_model);
    if (source.name() != QStringLiteral("table"))
        return false;
    const QXmlStreamAttributes tableAttributes = source.attributes();
    if (
        !tableAttributes.hasAttribute(QStringLiteral("data-rowcount"))
        || !tableAttributes.hasAttribute(QStringLiteral("data-colcount"))
        )
        return false;
    m_model->insertRows(0, tableAttributes.value(QStringLiteral("data-rowcount"))
        #if QT_VERSION < QT_VERSION_CHECK(5, 1, 0)
        .toString()
        #endif
        .toInt(), parent);
    m_model->insertColumns(0, tableAttributes.value(QStringLiteral("data-colcount"))
       #if QT_VERSION < QT_VERSION_CHECK(5, 1, 0)
       .toString()
       #endif
       .toInt(), parent);
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
            }
            else if (rowStarted && !parent.isValid() && source.name() == QStringLiteral("th")) {
                const QXmlStreamAttributes headAttributes = source.attributes();
                if (!headAttributes.hasAttribute(QStringLiteral("scope")))
                    return false;
                const QStringRef headScope = headAttributes.value(QStringLiteral("scope"));
                if (headScope.compare(QStringLiteral("row")) == 0)
                    headCode = Row;
                else if (headScope.compare(QStringLiteral("col")) == 0)
                    headCode = Col;
                else
                    return false;
            }
            else if (rowStarted && source.name() == QStringLiteral("td")) {
                tdFound = true;
                if (currentCol >= m_model->columnCount(parent))
                    return false;
                cellStarted = true;
            }
            else if (source.name() == QStringLiteral("div")) {
                const QXmlStreamAttributes cellAttributes = source.attributes();
                if (
                    !cellAttributes.hasAttribute(QStringLiteral("data-varianttype"))
                    || !cellAttributes.hasAttribute(QStringLiteral("data-rolecode"))
                    )
                    return false;
                const int cellRole = cellAttributes.value(QStringLiteral("data-rolecode"))
                    #if QT_VERSION < QT_VERSION_CHECK(5, 1, 0)
                    .toString()
                    #endif
                    .toInt();
                const int cellType = cellAttributes.value(QStringLiteral("data-varianttype"))
                    #if QT_VERSION < QT_VERSION_CHECK(5, 1, 0)
                    .toString()
                    #endif
                    .toInt();
                if (cellStarted) {
                    m_model->setData(m_model->index(currentRow, currentCol, parent), readHtmlVariant(source, cellType), cellRole);
                }
                else if (headCode == Row) {
                    m_model->setHeaderData(currentRow, Qt::Vertical, readHtmlVariant(source, cellType), cellRole);
                }
                else if (headCode == Col) {
                    m_model->setHeaderData(currentCol, Qt::Horizontal, readHtmlVariant(source, cellType), cellRole);
                }
            }
            else if (cellStarted && source.name() == QStringLiteral("table")) {
                if (!readHtmlElement(source, m_model->index(currentRow, currentCol, parent)))
                    return false;
            }
        }
        else if (source.isEndElement()) {
            if (source.name() == QStringLiteral("tr")) {
                rowStarted = false;
                currentCol = 0;
                if (tdFound)
                    ++currentRow;
                tdFound = false;
            }
            else if (rowStarted && source.name() == QStringLiteral("td")) {
                ++currentCol;
                cellStarted = false;
            }
            else if (rowStarted && source.name() == QStringLiteral("th")) {
                if (headCode == Col)
                    ++currentCol;
                headCode = None;
            }
            else if (source.name() == QStringLiteral("table")) {
                return true;
            }
        }
    }
    return false;
}

bool HtmlModelSerialiserPrivate::writeHtml(QXmlStreamWriter& writer) const
{
    if (!m_constModel)
        return false;
    if (m_printStartDocument) {
        writer.writeDTD(QStringLiteral("<!DOCTYPE html>"));
        writer.writeStartElement(QStringLiteral("html"));
        writer.writeStartElement(QStringLiteral("body"));
    }
    writer.writeStartElement("div");
    writer.writeAttribute(QStringLiteral("data-modelcode"), Magic_Model_Header);
    writeHtmlElement(writer);
    writer.writeEndElement(); //div
    if (m_printStartDocument) {
        writer.writeEndElement(); //body
        writer.writeEndElement(); //html
    }
    return !writer.hasError();
}


bool HtmlModelSerialiserPrivate::readHtml(QXmlStreamReader& reader)
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
            }
            else if (mainDivStarted&& reader.name() == QStringLiteral("table")) {
                if (!readHtmlElement(reader)) {
                    m_model->removeColumns(0, m_model->columnCount());
                    m_model->removeRows(0, m_model->rowCount());
                    return false;
                }
            }
        }
        else if (reader.isEndElement()) {
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


void HtmlModelSerialiserPrivate::writeHtmlVariant(QXmlStreamWriter& writer, const QVariant& val)
{
    if (isImageType(val.type())) {
        writer.writeEmptyElement(QStringLiteral("img"));
        writer.writeAttribute(QStringLiteral("src"), "data:image/png;base64," + saveVariant(val));
        writer.writeAttribute(QStringLiteral("alt"), QStringLiteral("modelimage.png"));
        return;
    }
    writer.writeCharacters(saveVariant(val));
}

QVariant HtmlModelSerialiserPrivate::readHtmlVariant(QXmlStreamReader& reader, int valType)
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


HtmlModelSerialiser::HtmlModelSerialiser(QAbstractItemModel* model)
    : AbstractMultiRoleSerialiser(*new HtmlModelSerialiserPrivate(this))
{
    setModel(model);
}
HtmlModelSerialiser::HtmlModelSerialiser(const QAbstractItemModel* model)
    : AbstractMultiRoleSerialiser(*new HtmlModelSerialiserPrivate(this))
{
    setModel(model);
}

HtmlModelSerialiser::HtmlModelSerialiser(HtmlModelSerialiserPrivate& d)
    :AbstractMultiRoleSerialiser(d)
{}


HtmlModelSerialiser::~HtmlModelSerialiser() = default;


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


bool HtmlModelSerialiser::saveModel(QString* destination) const
{
    if (!destination)
        return false;
    Q_D(const HtmlModelSerialiser);
    
    if (!d->m_model)
        return false;
    QXmlStreamWriter witer(destination);
    return d->writeHtml(witer);
}

bool HtmlModelSerialiser::saveModel(QIODevice* destination) const
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
    
    if (!d->m_model)
        return false;
    QXmlStreamWriter witer(destination);
    return d->writeHtml(witer);
}

bool HtmlModelSerialiser::saveModel(QByteArray* destination) const
{
    if (!destination)
        return false;
    Q_D(const HtmlModelSerialiser);
    
    if (!d->m_model)
        return false;
    QXmlStreamWriter witer(destination);
    return d->writeHtml(witer);
}

bool HtmlModelSerialiser::loadModel(QIODevice* source)
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
bool HtmlModelSerialiser::loadModel(const QByteArray& source)
{
    Q_D(HtmlModelSerialiser);
    
    if (!d->m_model)
        return false;
    QXmlStreamReader reader(source);
    return d->readHtml(reader);
}
bool HtmlModelSerialiser::loadModel(QString* source)
{
    if (!source)
        return false;
    Q_D(HtmlModelSerialiser);
    
    if (!d->m_model)
        return false;
    QXmlStreamReader reader(*source);
    return d->readHtml(reader);
}
