#include "csvmodelserialiser.h"
#include "private/csvmodelserialiser_p.h"
#include <QTextStream>
#include <QDateTime>
#include <QStringBuilder>
#include <QTextCodec>
#include <QStringList>

CsvModelSerialiserPrivate::CsvModelSerialiserPrivate(CsvModelSerialiser* q)
    :AbstractSingleRoleSerialiserPrivate(q)
    , m_csvSeparator(QStringLiteral(","))
    , m_firstRowIsHeader(true)
    , m_firstColumnIsHeader(true)
{}

QString CsvModelSerialiserPrivate::escapedCSV(QString unexc) const
{
    if (!unexc.contains(m_csvSeparator))
        return unexc;
    if (unexc.contains(QChar('\"')))
        unexc.replace(QChar('\"'), QStringLiteral("\"\""));
    return '\"' % unexc % '\"';

}

QString CsvModelSerialiserPrivate::unescapedCSV(QString exc) const
{
    if (exc.isEmpty())
        return exc;
    if (*exc.constBegin() == QChar('\"') && *(exc.constEnd() - 1) == QChar('\"')) {
        exc.chop(1);
        exc.remove(0, 1);
        exc.replace(QStringLiteral("\"\""), QStringLiteral("\""));
    }
    return exc;
}

int CsvModelSerialiserPrivate::guessVarType(const QString& val)
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

bool CsvModelSerialiserPrivate::writeCsv(QTextStream& writer) const
{
    if (!m_constModel)
        return false;
    QTextCodec* const oldCodec = writer.codec();
    Q_ASSERT(QTextCodec::availableCodecs().contains("UTF-8"));
    writer.setCodec("UTF-8");
    if (m_firstRowIsHeader) {
        for (int j = 0; j < m_constModel->columnCount(); ++j) {
            const QVariant headData = m_constModel->headerData(j, Qt::Horizontal, m_roleToSave);
            if(j>0 || m_firstColumnIsHeader)
                writer << m_csvSeparator;
            if (!headData.isNull())
                writer << escapedCSV(saveVariant(headData));
        }
        writer << '\n';
    }
    for (int i = 0; i < m_constModel->rowCount(); ++i) {
        if (m_firstColumnIsHeader) {
            const QVariant headData = m_constModel->headerData(i, Qt::Vertical, m_roleToSave);
            if (!headData.isNull())
                writer << escapedCSV(saveVariant(headData));
        }
        for (int j = 0; j < m_constModel->columnCount(); ++j) {
            const QVariant roleData = m_constModel->data(m_constModel->index(i, j), m_roleToSave);
            if (j>0 || m_firstColumnIsHeader)
                writer << m_csvSeparator;
            if (!roleData.isNull()) {
                writer << escapedCSV(saveVariant(roleData));
            }
        }
        writer << '\n';
    }
    writer.setCodec(oldCodec);
    return writer.status() == QTextStream::Ok;
}


bool CsvModelSerialiserPrivate::readCsv(QTextStream& reader)
{
    if (!m_model)
        return false;
    QTextCodec* const oldCodec = reader.codec();
    Q_ASSERT(QTextCodec::availableCodecs().contains("UTF-8"));
    reader.setCodec("UTF-8");
    m_model->removeColumns(0, m_model->columnCount());
    m_model->removeRows(0, m_model->rowCount());
    QString line;
    QStringList fields;
    QString currentField;
    for (bool firstRow = true; ; firstRow = false) {
        line = reader.readLine();
        if (line.isNull())
            break;
        if (line.count(QChar('\"')) % 2 > 0) {
            m_model->removeColumns(0, m_model->columnCount());
            m_model->removeRows(0, m_model->rowCount());
            reader.setCodec(oldCodec);
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
                if (currentField.rightRef(m_csvSeparator.size()).compare(m_csvSeparator) == 0) {
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
        }
        else if (m_model->columnCount() != fields.size() - startI) {
            m_model->removeColumns(0, m_model->columnCount());
            m_model->removeRows(0, m_model->rowCount());
            reader.setCodec(oldCodec);
            return false;
        }
        if (firstRow && m_firstRowIsHeader) {
            for (int i = startI; i < fields.size(); ++i)
                m_model->setHeaderData(i - startI, Qt::Horizontal, loadVariant(guessVarType(fields.at(i)), unescapedCSV(fields.at(i))), m_roleToSave);
        }
        else {
            const int newRow = m_model->rowCount();
            m_model->insertRow(newRow);
            for (int i = 0; i < fields.size(); ++i) {
                if (i == 0 && m_firstColumnIsHeader)
                    m_model->setHeaderData(newRow, Qt::Vertical, loadVariant(guessVarType(fields.at(i)), unescapedCSV(fields.at(i))), m_roleToSave);
                else
                    m_model->setData(m_model->index(newRow, i - startI), loadVariant(guessVarType(fields.at(i)), unescapedCSV(fields.at(i))), m_roleToSave);
            }
        }
    }
    reader.setCodec(oldCodec);
    return true;
}

const QString& CsvModelSerialiser::csvSeparator() const
{
    Q_D(const CsvModelSerialiser);
    
    return d->m_csvSeparator;
}

void CsvModelSerialiser::setCsvSeparator(const QString& val)
{
    Q_D(CsvModelSerialiser);
    
    d->m_csvSeparator = val;
}


bool CsvModelSerialiser::firstRowIsHeader()
{
    Q_D(const CsvModelSerialiser);
    
    return d->m_firstRowIsHeader;
}

bool CsvModelSerialiser::firstColumnIsHeader()
{
    Q_D(const CsvModelSerialiser);
    
    return d->m_firstColumnIsHeader;
}

void CsvModelSerialiser::setFirstRowIsHeader(bool val)
{
    Q_D(CsvModelSerialiser);
    
    d->m_firstRowIsHeader = val;
}

void CsvModelSerialiser::setFirstColumnIsHeader(bool val)
{
    Q_D(CsvModelSerialiser);
    
    d->m_firstColumnIsHeader = val;
}


CsvModelSerialiser::CsvModelSerialiser(QAbstractItemModel* model)
    : AbstractSingleRoleSerialiser(*new CsvModelSerialiserPrivate(this))
{
    setModel(model);
}
CsvModelSerialiser::CsvModelSerialiser(const QAbstractItemModel* model)
    : AbstractSingleRoleSerialiser(*new CsvModelSerialiserPrivate(this))
{
    setModel(model);
}

CsvModelSerialiser::CsvModelSerialiser(CsvModelSerialiserPrivate& d)
    :AbstractSingleRoleSerialiser(d)
{}


CsvModelSerialiser::~CsvModelSerialiser() = default;




bool CsvModelSerialiser::saveModel(QString* destination) const
{
    if (!destination)
        return false;
    Q_D(const CsvModelSerialiser);
    
    if (!d->m_model)
        return false;
    QTextStream witer(destination, QIODevice::WriteOnly | QIODevice::Text);
    return d->writeCsv(witer);
}

bool CsvModelSerialiser::saveModel(QIODevice* destination) const
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
    
    if (!d->m_model)
        return false;
    destination->setTextModeEnabled(true);
    QTextStream witer(destination);
    return d->writeCsv(witer);
}

bool CsvModelSerialiser::saveModel(QByteArray* destination) const
{
    if (!destination)
        return false;
    Q_D(const CsvModelSerialiser);
    
    if (!d->m_model)
        return false;
    QTextStream witer(destination, QIODevice::WriteOnly | QIODevice::Text);
    return d->writeCsv(witer);
}

bool CsvModelSerialiser::loadModel(QIODevice* source)
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
    return d->readCsv(reader);
}
bool CsvModelSerialiser::loadModel(const QByteArray& source)
{
    Q_D(CsvModelSerialiser);
    
    if (!d->m_model)
        return false;
    QTextStream reader(source, QIODevice::ReadOnly | QIODevice::Text);
    return d->readCsv(reader);
}
bool CsvModelSerialiser::loadModel(QString* source)
{
    if (!source)
        return false;
    Q_D(CsvModelSerialiser);
    
    if (!d->m_model)
        return false;
    QTextStream reader(source, QIODevice::ReadOnly | QIODevice::Text);
    return d->readCsv(reader);
}


#ifdef MS_DECLARE_STREAM_OPERATORS
QTextStream& operator<<(QTextStream & stream, const QAbstractItemModel& model)
{
    const QModelSerialiser mSer(&model);
    mSer.d_func()->writeCsv(stream);
    return stream;
}

QTextStream& operator>>(QTextStream & stream, QAbstractItemModel& model)
{
    QModelSerialiser mSer(&model);
    mSer.d_func()->readCsv(stream);
    return stream;
}
#endif // MS_DECLARE_STREAM_OPERATORS