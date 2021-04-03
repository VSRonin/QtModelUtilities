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
#include "csvmodelserialiser.h"
#include "private/csvmodelserialiser_p.h"
#include <QTextStream>
#include <QDateTime>
#include <QStringBuilder>
#include <QStringList>
#include <QIODevice>
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#    include <QTextCodec>
#endif

CsvModelSerialiserPrivate::CsvModelSerialiserPrivate(CsvModelSerialiser *q)
    : AbstractSingleRoleSerialiserPrivate(q)
    , m_firstRowIsHeader(true)
    , m_firstColumnIsHeader(true)
    , m_csvSeparator(QLatin1Char(','))
{ }

QString CsvModelSerialiserPrivate::escapedCSV(QString unexc) const
{
    if (!unexc.contains(m_csvSeparator))
        return unexc;
    return '\"' % unexc.replace(QLatin1Char('\"'), QStringLiteral("\"\"")) % '\"';
}

QString CsvModelSerialiserPrivate::unescapedCSV(QString exc) const
{
    if (exc.isEmpty())
        return exc;
    if (*exc.constBegin() == QLatin1Char('\"') && *(exc.constEnd() - 1) == QLatin1Char('\"')) {
        exc.chop(1);
        exc.remove(0, 1);
        exc.replace(QStringLiteral("\"\""), QStringLiteral("\""));
    }
    return exc;
}

int CsvModelSerialiserPrivate::guessVarType(const QString &val)
{
    bool checkConversion;
    val.toInt(&checkConversion);
    if (checkConversion)
        return QMetaType::Int;
    val.toDouble(&checkConversion);
    if (checkConversion)
        return QMetaType::Double;
    if (!QDate::fromString(val, Qt::ISODate).isNull())
        return QMetaType::QDate;
    if (!QTime::fromString(val, Qt::ISODate).isNull())
        return QMetaType::QTime;
    if (!QDateTime::fromString(val, Qt::ISODate).isNull())
        return QMetaType::QDateTime;
    return QMetaType::QString;
}

bool CsvModelSerialiserPrivate::writeCsv(QTextStream &writer) const
{
    if (!m_constModel)
        return false;
    Q_Q(const CsvModelSerialiser);
    if (m_firstRowIsHeader) {
        for (int j = 0; j < m_constModel->columnCount(); ++j) {
            const QVariant headData = m_constModel->headerData(j, Qt::Horizontal, q->roleToSave());
            if (j > 0 || m_firstColumnIsHeader)
                writer << m_csvSeparator;
            if (!headData.isNull())
                writer << escapedCSV(saveVariant(headData));
        }
        writer << '\n';
    }
    for (int i = 0, maxRow = m_constModel->rowCount(); i < maxRow; ++i) {
        if (m_firstColumnIsHeader) {
            const QVariant headData = m_constModel->headerData(i, Qt::Vertical, q->roleToSave());
            if (!headData.isNull())
                writer << escapedCSV(saveVariant(headData));
        }
        for (int j = 0, maxCol = m_constModel->columnCount(); j < maxCol; ++j) {
            const QVariant roleData = m_constModel->index(i, j).data(q->roleToSave());
            if (j > 0 || m_firstColumnIsHeader)
                writer << m_csvSeparator;
            if (!roleData.isNull())
                writer << escapedCSV(saveVariant(roleData));
        }
        writer << '\n';
    }
    return writer.status() == QTextStream::Ok;
}

bool CsvModelSerialiserPrivate::readCsv(QTextStream &reader)
{
    if (!m_model)
        return false;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QTextCodec *const oldCodec = reader.codec();
    Q_ASSERT(QTextCodec::availableCodecs().contains("UTF-8"));
    reader.setCodec("UTF-8");
#endif
    m_model->removeColumns(0, m_model->columnCount());
    m_model->removeRows(0, m_model->rowCount());
    QString line;
    QStringList fields;
    QString currentField;
    for (bool firstRow = true;; firstRow = false) {
        line = reader.readLine();
        if (line.isNull())
            break;
        if (line.count(QChar('\"')) % 2 > 0) {
            m_model->removeColumns(0, m_model->columnCount());
            m_model->removeRows(0, m_model->rowCount());
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
            reader.setCodec(oldCodec);
#endif
            return false;
        }
        fields.clear();
        currentField.clear();
        bool inQuoted = false;
        for (QString::const_iterator i = line.constBegin(); i != line.constEnd(); ++i) {
            if (*i == QChar('\"'))
                inQuoted = !inQuoted;
            currentField.append(*i);
            if (!inQuoted) {
                if (currentField.right(m_csvSeparator.size()).compare(m_csvSeparator) == 0) {
                    currentField.chop(m_csvSeparator.size());
                    fields.append(currentField);
                    currentField.clear();
                }
            }
        }
        fields.append(currentField);
        currentField.clear();
        const int startI = (m_firstColumnIsHeader ? 1 : 0);
        if (m_model->columnCount() == 0) {
            m_model->insertColumns(0, fields.size() - startI);
            if (m_model->columnCount() != fields.size() - startI)
                return false;
        } else if (m_model->columnCount() != fields.size() - startI) {
            m_model->removeColumns(0, m_model->columnCount());
            m_model->removeRows(0, m_model->rowCount());
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
            reader.setCodec(oldCodec);
#endif
            return false;
        }
        Q_Q(const CsvModelSerialiser);
        if (firstRow && m_firstRowIsHeader) {
            for (int i = startI; i < fields.size(); ++i)
                m_model->setHeaderData(i - startI, Qt::Horizontal, loadVariant(guessVarType(fields.at(i)), unescapedCSV(fields.at(i))),
                                       q->roleToSave());
        } else {
            const int newRow = m_model->rowCount();
            if (!m_model->insertRow(newRow))
                return false;
            for (int i = 0; i < fields.size(); ++i) {
                if (i == 0 && m_firstColumnIsHeader)
                    m_model->setHeaderData(newRow, Qt::Vertical, loadVariant(guessVarType(fields.at(i)), unescapedCSV(fields.at(i))),
                                           q->roleToSave());
                else
                    m_model->setData(m_model->index(newRow, i - startI), loadVariant(guessVarType(fields.at(i)), unescapedCSV(fields.at(i))),
                                     q->roleToSave());
            }
        }
    }
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    reader.setCodec(oldCodec);
#endif
    return true;
}

/*!
\property CsvModelSerialiser::csvSeparator
\accessors %csvSeparator(), setCsvSeparator()
\brief The column separator used in the CSV
\details This is the string that will be used to separate items in the same row but different columns

By default the separator is set to a comma: ,
*/
const QString &CsvModelSerialiser::csvSeparator() const
{
    Q_D(const CsvModelSerialiser);

    return d->m_csvSeparator;
}

void CsvModelSerialiser::setCsvSeparator(const QString &val)
{
    Q_D(CsvModelSerialiser);

    d->m_csvSeparator = val;
}

/*!
\property CsvModelSerialiser::firstRowIsHeader
\accessors %firstRowIsHeader(), setFirstRowIsHeader()
\brief Should the first row contain headers
\details If this property is set to true (the default), the serialisation will the write
 the horizontal headerData of the model as the first row of the csv file
 and will load the first row of the csv file as the model's horizontal headerData
*/
bool CsvModelSerialiser::firstRowIsHeader()
{
    Q_D(const CsvModelSerialiser);

    return d->m_firstRowIsHeader;
}

void CsvModelSerialiser::setFirstRowIsHeader(bool val)
{
    Q_D(CsvModelSerialiser);

    d->m_firstRowIsHeader = val;
}

/*!
\property CsvModelSerialiser::firstColumnIsHeader
\accessors %firstColumnIsHeader(), setFirstColumnIsHeader()
\brief Should the first column contain headers
\details If this property is set to true (the default), the serialisation will the write
 the vertical headerData of the model as the first column of the csv file
 and will load the first column of the csv file as the model's vertical headerData
*/
bool CsvModelSerialiser::firstColumnIsHeader()
{
    Q_D(const CsvModelSerialiser);

    return d->m_firstColumnIsHeader;
}

void CsvModelSerialiser::setFirstColumnIsHeader(bool val)
{
    Q_D(CsvModelSerialiser);

    d->m_firstColumnIsHeader = val;
}

/*!
Constructs a serialiser operating over \a model
*/
CsvModelSerialiser::CsvModelSerialiser(QAbstractItemModel *model, QObject *parent)
    : AbstractSingleRoleSerialiser(*new CsvModelSerialiserPrivate(this), parent)
{
    setModel(model);
}

/*!
\overload

the model will only be allowed to be saved, not loaded
        */
CsvModelSerialiser::CsvModelSerialiser(const QAbstractItemModel *model, QObject *parent)
    : AbstractSingleRoleSerialiser(*new CsvModelSerialiserPrivate(this), parent)
{
    setModel(model);
}

/*!
\internal
*/
CsvModelSerialiser::CsvModelSerialiser(CsvModelSerialiserPrivate &d, QObject *parent)
    : AbstractSingleRoleSerialiser(d, parent)
{ }

/*!
\reimp
*/
bool CsvModelSerialiser::saveModel(QString *destination) const
{
    if (!destination)
        return false;
    Q_D(const CsvModelSerialiser);

    if (!d->m_constModel)
        return false;
    QTextStream writer(destination, QIODevice::WriteOnly | QIODevice::Text);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    writer.setCodec(textCodec());
#endif
    return d->writeCsv(writer);
}

/*!
\reimp
*/
bool CsvModelSerialiser::saveModel(QIODevice *destination) const
{
    if (!destination)
        return false;
    if (!destination->isOpen()) {
        if (!destination->open(QIODevice::WriteOnly | QIODevice::Text))
            return false;
    }
    if (!destination->isWritable())
        return false;
    Q_D(const CsvModelSerialiser);

    if (!d->m_constModel)
        return false;
    destination->setTextModeEnabled(true);
    QTextStream writer(destination);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    writer.setCodec(textCodec());
#endif
    return d->writeCsv(writer);
}

/*!
\reimp
*/
bool CsvModelSerialiser::saveModel(QByteArray *destination) const
{
    if (!destination)
        return false;
    Q_D(const CsvModelSerialiser);

    if (!d->m_constModel)
        return false;
    QTextStream writer(destination, QIODevice::WriteOnly | QIODevice::Text);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    writer.setCodec(textCodec());
#endif
    return d->writeCsv(writer);
}

/*!
Saves the model to the given \a stream
*/
bool CsvModelSerialiser::saveModel(QTextStream &stream) const
{
    Q_D(const CsvModelSerialiser);
    return d->writeCsv(stream);
}

/*!
\reimp
*/
bool CsvModelSerialiser::loadModel(QIODevice *source)
{
    if (!source)
        return false;
    if (!source->isOpen()) {
        if (!source->open(QIODevice::ReadOnly | QIODevice::Text))
            return false;
    }
    if (!source->isReadable())
        return false;
    Q_D(CsvModelSerialiser);

    if (!d->m_model)
        return false;
    source->setTextModeEnabled(true);
    QTextStream reader(source);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    reader.setCodec(textCodec());
#endif
    return d->readCsv(reader);
}

/*!
\reimp
*/
bool CsvModelSerialiser::loadModel(const QByteArray &source)
{
    Q_D(CsvModelSerialiser);

    if (!d->m_model)
        return false;
    QTextStream reader(source, QIODevice::ReadOnly | QIODevice::Text);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    reader.setCodec(textCodec());
#endif
    return d->readCsv(reader);
}

/*!
\reimp
*/
bool CsvModelSerialiser::loadModel(QString *source)
{
    if (!source)
        return false;
    Q_D(CsvModelSerialiser);

    if (!d->m_model)
        return false;
    QTextStream reader(source, QIODevice::ReadOnly | QIODevice::Text);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    reader.setCodec(textCodec());
#endif
    return d->readCsv(reader);
}

/*!
Loads the model from the given \a stream

Data previously stored in the model will be removed
*/
bool CsvModelSerialiser::loadModel(QTextStream &stream)
{
    Q_D(CsvModelSerialiser);
    return d->readCsv(stream);
}

/*!
\class CsvModelSerialiser

\brief Serialiser to save and load models in csv (comma separated values) format
*/

#ifdef MS_DECLARE_STREAM_OPERATORS
/*!
Text stream operator to write a model
*/
QTextStream &operator<<(QTextStream &stream, const QAbstractItemModel &model)
{
    const QModelSerialiser mSer(&model);
    mSer.d_func()->writeCsv(stream);
    return stream;
}

/*!
Text stream operator to read a model
*/
QTextStream &operator>>(QTextStream &stream, QAbstractItemModel &model)
{
    QModelSerialiser mSer(&model);
    mSer.d_func()->readCsv(stream);
    return stream;
}
#endif // MS_DECLARE_STREAM_OPERATORS
