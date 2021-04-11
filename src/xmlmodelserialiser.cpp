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

#include "xmlmodelserialiser.h"
#include "private/xmlmodelserialiser_p.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

XmlModelSerialiserPrivate::XmlModelSerialiserPrivate(XmlModelSerialiser *q)
    : AbstractStringSerialiserPrivate(q)
    , m_printStartDocument(true)
{ }

void XmlModelSerialiserPrivate::writeXmlElement(QXmlStreamWriter &destination, const QModelIndex &parent) const
{
    Q_ASSERT(m_constModel);
    if (m_constModel->rowCount(parent) + m_constModel->columnCount(parent) == 0)
        return;
    destination.writeStartElement(QStringLiteral("Element"));
    destination.writeAttribute(QStringLiteral("RowCount"), QString::number(m_constModel->rowCount(parent)));
    destination.writeAttribute(QStringLiteral("ColumnCount"), QString::number(m_constModel->columnCount(parent)));
    for (int i = 0; i < m_constModel->rowCount(parent); ++i) {
        for (int j = 0; j < m_constModel->columnCount(parent); ++j) {
            destination.writeStartElement(QStringLiteral("Cell"));
            destination.writeStartElement(QStringLiteral("Row"));
            destination.writeCharacters(QString::number(i));
            destination.writeEndElement(); // Row
            destination.writeStartElement(QStringLiteral("Column"));
            destination.writeCharacters(QString::number(j));
            destination.writeEndElement(); // Column
            for (QList<int>::const_iterator singleRole = m_rolesToSave.constBegin(); singleRole != m_rolesToSave.constEnd(); ++singleRole) {
                const QVariant roleData = m_constModel->index(i, j, parent).data(*singleRole);
                if (roleData.isNull())
                    continue; // Skip empty roles
                const QString roleString = saveVariant(roleData);
                if (roleString.isEmpty())
                    continue; // Skip unhandled types
                destination.writeStartElement(QStringLiteral("DataPoint"));
                destination.writeAttribute(QStringLiteral("Role"), QString::number(*singleRole));
                destination.writeAttribute(QStringLiteral("Type"), QString::number(roleData.userType()));
                destination.writeCharacters(roleString);
                destination.writeEndElement(); // DataPoint
            }
            if (m_constModel->hasChildren(m_constModel->index(i, j, parent))) {
                writeXmlElement(destination, m_constModel->index(i, j, parent));
            }
            destination.writeEndElement(); // Cell
        }
    }
    destination.writeEndElement(); // Element
}

bool XmlModelSerialiserPrivate::readXmlElement(QXmlStreamReader &source, const QModelIndex &parent)
{
    Q_ASSERT(m_model);
    if (source.name() != QStringLiteral("Element"))
        return false;
    int rowCount, colCount;
    const QXmlStreamAttributes tableSizeAttribute = source.attributes();
    if (!(tableSizeAttribute.hasAttribute(QStringLiteral("RowCount")) && tableSizeAttribute.hasAttribute(QStringLiteral("ColumnCount"))))
        return false;
    rowCount = tableSizeAttribute.value(QStringLiteral("RowCount")).toInt();
    colCount = tableSizeAttribute.value(QStringLiteral("ColumnCount")).toInt();
    if (rowCount <= 0 || colCount <= 0)
        return false;
    if (m_model->rowCount(parent) < rowCount)
        m_model->insertRows(m_model->rowCount(parent), rowCount - m_model->rowCount(parent), parent);
    if (m_model->columnCount(parent) < colCount)
        m_model->insertColumns(m_model->columnCount(parent), colCount - m_model->columnCount(parent), parent);
    int rowIndex = -1;
    int colIndex = -1;
    bool cellStarted = false;
    while (!source.atEnd() && !source.hasError()) {
        source.readNext();
        if (source.isStartElement()) {
            if (source.name() == QStringLiteral("Cell")) {
                cellStarted = true;
            } else if (source.name() == QStringLiteral("Row") && cellStarted) {
                rowIndex = source.readElementText().toInt();
            } else if (source.name() == QStringLiteral("Column") && cellStarted) {
                colIndex = source.readElementText().toInt();
            } else if (source.name() == QStringLiteral("DataPoint") && cellStarted) {
                if (rowIndex < 0 || colIndex < 0)
                    return false;
                const QXmlStreamAttributes dataPointTattributes = source.attributes();
                if (!(dataPointTattributes.hasAttribute(QStringLiteral("Role")) && dataPointTattributes.hasAttribute(QStringLiteral("Type"))))
                    return false;
                const int dataRole = dataPointTattributes.value(QStringLiteral("Role")).toInt();
                const int dataType = dataPointTattributes.value(QStringLiteral("Type")).toInt();
                const QVariant roleVariant = loadVariant(dataType, source.readElementText());
                if (!roleVariant.isNull()) // skip unhandled types
                    m_model->setData(m_model->index(rowIndex, colIndex, parent), roleVariant, dataRole);
            } else if (source.name() == QStringLiteral("Element") && cellStarted) {
                if (rowIndex < 0 || colIndex < 0)
                    return false;
                readXmlElement(source, m_model->index(rowIndex, colIndex, parent));
            }
        } else if (source.isEndElement()) {
            if (source.name() == QStringLiteral("Cell")) {
                cellStarted = false;
                rowIndex = -1;
                colIndex = -1;
            } else if (source.name() == QStringLiteral("Element")) {
                if (!cellStarted)
                    return true;
            }
        }
    }
    return false;
}

bool XmlModelSerialiserPrivate::writeXml(QXmlStreamWriter &writer) const
{
    if (!m_constModel)
        return false;
    if (m_printStartDocument)
        writer.writeStartDocument();
    writer.writeStartElement(QStringLiteral("ItemModel"));
    writeXmlElement(writer);
    writer.writeStartElement(QStringLiteral("HeaderData"));
    writer.writeStartElement(QStringLiteral("Horizontal"));
    for (int i = 0; i < m_constModel->columnCount(); ++i) {
        for (QList<int>::const_iterator singleRole = m_rolesToSave.constBegin(); singleRole != m_rolesToSave.constEnd(); ++singleRole) {
            const QVariant roleData = m_constModel->headerData(i, Qt::Horizontal, *singleRole);
            if (roleData.isNull())
                continue;
            const QString roleString = saveVariant(roleData);
            if (roleString.isEmpty())
                continue; // Skip unhandled types
            writer.writeStartElement(QStringLiteral("HeaderDataPoint"));
            writer.writeAttribute(QStringLiteral("Section"), QString::number(i));
            writer.writeAttribute(QStringLiteral("Role"), QString::number(*singleRole));
            writer.writeAttribute(QStringLiteral("Type"), QString::number(roleData.userType()));
            writer.writeCharacters(roleString);
            writer.writeEndElement(); // HeaderDataPoint
        }
    }
    writer.writeEndElement(); // Horizontal
    writer.writeStartElement(QStringLiteral("Vertical"));
    for (int i = 0; i < m_constModel->rowCount(); ++i) {
        for (QList<int>::const_iterator singleRole = m_rolesToSave.constBegin(); singleRole != m_rolesToSave.constEnd(); ++singleRole) {
            const QVariant roleData = m_constModel->headerData(i, Qt::Vertical, *singleRole);
            if (roleData.isNull())
                continue;
            const QString roleString = saveVariant(roleData);
            if (roleString.isEmpty())
                continue; // Skip unhandled types
            writer.writeStartElement(QStringLiteral("HeaderDataPoint"));
            writer.writeAttribute(QStringLiteral("Section"), QString::number(i));
            writer.writeAttribute(QStringLiteral("Role"), QString::number(*singleRole));
            writer.writeAttribute(QStringLiteral("Type"), QString::number(roleData.userType()));
            writer.writeCharacters(roleString);
            writer.writeEndElement(); // HeaderDataPoint
        }
    }
    writer.writeEndElement(); // Vertical
    writer.writeEndElement(); // HeaderData
    writer.writeEndElement(); // ItemModel
    if (m_printStartDocument)
        writer.writeEndDocument();
    return !writer.hasError();
}

bool XmlModelSerialiserPrivate::readXml(QXmlStreamReader &reader)
{
    if (!m_model)
        return false;
    m_model->removeColumns(0, m_model->columnCount());
    m_model->removeRows(0, m_model->rowCount());
    bool headerDataStarted = false;
    bool hHeaderDataStarted = false;
    bool vHeaderDataStarted = false;
    while (!reader.atEnd() && !reader.hasError()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == QStringLiteral("Element")) {
                if (!readXmlElement(reader)) {
                    m_model->removeColumns(0, m_model->columnCount());
                    m_model->removeRows(0, m_model->rowCount());
                    return false;
                }
            } else if (reader.name() == QStringLiteral("HeaderData")) {
                headerDataStarted = true;
            } else if (reader.name() == QStringLiteral("Vertical") && headerDataStarted) {
                if (hHeaderDataStarted)
                    return false;
                vHeaderDataStarted = true;
            } else if (reader.name() == QStringLiteral("Horizontal") && headerDataStarted) {
                if (vHeaderDataStarted)
                    return false;
                hHeaderDataStarted = true;
            } else if (reader.name() == QStringLiteral("HeaderDataPoint") && headerDataStarted) {
                if (!(vHeaderDataStarted || hHeaderDataStarted))
                    return false;
                const QXmlStreamAttributes headDataAttribute = reader.attributes();
                if (!(headDataAttribute.hasAttribute(QStringLiteral("Section")) && headDataAttribute.hasAttribute(QStringLiteral("Role"))
                      && headDataAttribute.hasAttribute(QStringLiteral("Type"))))
                    return false;
                int headerSection = headDataAttribute.value(QStringLiteral("Section")).toInt();
                int headerRole = headDataAttribute.value(QStringLiteral("Role")).toInt();
                int headerType = headDataAttribute.value(QStringLiteral("Type")).toInt();
                const QVariant roleVariant = loadVariant(headerType, reader.readElementText());
                if (!roleVariant.isNull()) // skip unhandled types
                    m_model->setHeaderData(headerSection, (vHeaderDataStarted ? Qt::Vertical : Qt::Horizontal), roleVariant, headerRole);
            }

        } else if (reader.isEndElement()) {
            if (reader.name() == QStringLiteral("HeaderData")) {
                headerDataStarted = false;
            } else if (reader.name() == QStringLiteral("Vertical") && headerDataStarted) {
                vHeaderDataStarted = false;
            } else if (reader.name() == QStringLiteral("Horizontal") && headerDataStarted) {
                hHeaderDataStarted = false;
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

/*!
\property XmlModelSerialiser::printStartDocument
\accessors %printStartDocument(), setPrintStartDocument()
\brief This property determines if the start of the xml document should be written
\details If this property is set to \c true (the default) the serialiser will write the \c <?xml> starting block.
Set this to false to save the model as part of a larger xml document
*/

bool XmlModelSerialiser::printStartDocument() const
{
    Q_D(const XmlModelSerialiser);

    return d->m_printStartDocument;
}

void XmlModelSerialiser::setPrintStartDocument(bool val)
{
    Q_D(XmlModelSerialiser);

    d->m_printStartDocument = val;
}

/*!
\reimp
*/
bool XmlModelSerialiser::saveModel(QString *destination) const
{
    if (!destination)
        return false;
    Q_D(const XmlModelSerialiser);

    if (!d->m_constModel)
        return false;
    QXmlStreamWriter writer(destination);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    writer.setCodec(textCodec());
#endif
    return d->writeXml(writer);
}

/*!
\reimp
*/
bool XmlModelSerialiser::saveModel(QIODevice *destination) const
{
    if (!destination)
        return false;
    if (!destination->isOpen()) {
        if (!destination->open(QIODevice::WriteOnly))
            return false;
    }
    if (!destination->isWritable())
        return false;
    Q_D(const XmlModelSerialiser);

    if (!d->m_constModel)
        return false;
    QXmlStreamWriter writer(destination);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    writer.setCodec(textCodec());
#endif
    return d->writeXml(writer);
}

/*!
\reimp
*/
bool XmlModelSerialiser::saveModel(QByteArray *destination) const
{
    if (!destination)
        return false;
    Q_D(const XmlModelSerialiser);

    if (!d->m_constModel)
        return false;
    QXmlStreamWriter writer(destination);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    writer.setCodec(textCodec());
#endif
    return d->writeXml(writer);
}

/*!
Saves the model to the given \a stream
*/
bool XmlModelSerialiser::saveModel(QXmlStreamWriter &stream) const
{
    Q_D(const XmlModelSerialiser);
    return d->writeXml(stream);
}

/*!
\reimp
*/
bool XmlModelSerialiser::loadModel(QIODevice *source)
{
    if (!source)
        return false;
    if (!source->isOpen()) {
        if (!source->open(QIODevice::ReadOnly))
            return false;
    }
    if (!source->isReadable())
        return false;
    Q_D(XmlModelSerialiser);

    if (!d->m_model)
        return false;
    QXmlStreamReader reader(source);
    return d->readXml(reader);
}

/*!
\reimp
*/
bool XmlModelSerialiser::loadModel(const QByteArray &source)
{
    Q_D(XmlModelSerialiser);

    if (!d->m_model)
        return false;
    QXmlStreamReader reader(source);
    return d->readXml(reader);
}

/*!
\reimp
*/
bool XmlModelSerialiser::loadModel(QString *source)
{
    if (!source)
        return false;
    Q_D(XmlModelSerialiser);

    QXmlStreamReader reader(*source);
    return d->readXml(reader);
}

/*!
Loads the model from the given \a stream

Data previously stored in the model will be removed
                */
bool XmlModelSerialiser::loadModel(QXmlStreamReader &stream)
{
    Q_D(XmlModelSerialiser);
    return d->readXml(stream);
}

/*!
Constructs a serialiser
*/
XmlModelSerialiser::XmlModelSerialiser(QObject *parent)
    : AbstractStringSerialiser(*new XmlModelSerialiserPrivate(this), parent)
{

}

/*!
Constructs a serialiser operating over \a model
*/
XmlModelSerialiser::XmlModelSerialiser(QAbstractItemModel *model, QObject *parent)
    : AbstractStringSerialiser(*new XmlModelSerialiserPrivate(this), parent)
{
    setModel(model);
}

/*!
\overload

the model will only be allowed to be saved, not loaded
*/
XmlModelSerialiser::XmlModelSerialiser(const QAbstractItemModel *model, QObject *parent)
    : AbstractStringSerialiser(*new XmlModelSerialiserPrivate(this), parent)
{
    setModel(model);
}

/*!
\internal
*/
XmlModelSerialiser::XmlModelSerialiser(XmlModelSerialiserPrivate &d, QObject *parent)
    : AbstractStringSerialiser(d, parent)
{ }

/*!
Destructor
*/
XmlModelSerialiser::~XmlModelSerialiser() = default;

#ifdef MS_DECLARE_STREAM_OPERATORS

QXmlStreamWriter &operator<<(QXmlStreamWriter &stream, const QAbstractItemModel &model)
{
    const QModelSerialiser mSer(&model);
    mSer.d_func()->writeXml(stream);
    return stream;
}

QXmlStreamReader &operator>>(QXmlStreamReader &stream, QAbstractItemModel &model)
{
    QModelSerialiser mSer(&model);
    mSer.d_func()->readXml(stream);
    return stream;
}

#endif // MS_DECLARE_STREAM_OPERATORS

/*!
\class XmlModelSerialiser

\brief Serialiser to save and load models in XML format
*/
