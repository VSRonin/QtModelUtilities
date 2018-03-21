
#include "abstractmodelserialiser.h"
#include "private/abstractmodelserialiser_p.h"
#include <QDataStream>
#include <QVariant>
#include <QDateTime>
#ifdef QT_GUI_LIB
#include <QImage>
#include <QPixmap>
#include <QBitmap>
#endif
#include <QBuffer>
#include <QLocale>

/*!
\class AbstractModelSerialiser

\brief The interface for model serialisers.

This class serve as a base for all serialisers
*/

AbstractModelSerialiserPrivate::AbstractModelSerialiserPrivate(AbstractModelSerialiser* q)
    : q_ptr(q)
    , m_model(Q_NULLPTR)
    , m_constModel(Q_NULLPTR)
{
    Q_ASSERT(q_ptr);
}
int AbstractModelSerialiserPrivate::guessDecimals(double val)
{
    int precision = 0;
    for (double junk = 0; !qFuzzyIsNull(std::modf(val, &junk)); ++precision)
        val *= 10.0;
    return precision;
}
QString AbstractModelSerialiserPrivate::guessDecimalsString(double val, QLocale* loca)
{
    if (loca)
        return loca->toString(val, 'f', guessDecimals(val));
    return QString::number(val, 'f', guessDecimals(val));
}
QString AbstractModelSerialiserPrivate::variantToString(const QVariant& val)
{
    QString result;
    QByteArray data;
    QDataStream outStream(&data, QIODevice::WriteOnly);
#ifdef MS_DATASTREAM_VERSION
    outStream.setVersion(MS_DATASTREAM_VERSION);
#endif // MS_DATASTREAM_VERSION
    outStream << val;
    data = qCompress(data);
    return QString::fromLatin1(data.toBase64());
}

QVariant AbstractModelSerialiserPrivate::stringToVariant(const QString& val)
{
    QByteArray data = QByteArray::fromBase64(val.toLatin1());
    data = qUncompress(data);
    QDataStream inStream(data);
#ifdef MS_DATASTREAM_VERSION
    inStream.setVersion(MS_DATASTREAM_VERSION);
#endif // MS_DATASTREAM_VERSION
    QVariant result;
    inStream >> result;
    return result;
}

QVariant AbstractModelSerialiserPrivate::loadVariant(int type, const QString& val)
{
    if (val.isEmpty())
        return QVariant();
    switch (type) {
    case QMetaType::UnknownType:
        Q_ASSERT_X(false, "ModelSerialisation::loadVariant", "Trying to load unregistered type.");
        return QVariant();
    case QMetaType::Bool: return val.toInt() == 1;
    case QMetaType::Long:
    case QMetaType::Int: return val.toInt();
    case QMetaType::ULong:
    case QMetaType::UInt: return val.toUInt();
    case QMetaType::LongLong: return val.toLongLong();
    case QMetaType::ULongLong: return val.toULongLong();
    case QMetaType::Double: return val.toDouble();
    case QMetaType::Short: return static_cast<short>(val.toInt());
    case QMetaType::SChar:
    case QMetaType::Char: return static_cast<char>(val.toInt());
    case QMetaType::UShort: return static_cast<unsigned short>(val.toUInt());
    case QMetaType::UChar: return static_cast<unsigned char>(val.toUInt());
    case QMetaType::Float: return val.toFloat();
    case QMetaType::QChar: return val.at(0);
    case QMetaType::QString: return val;
    case QMetaType::QByteArray: return QByteArray::fromBase64(val.toLatin1());
    case QMetaType::QDate: return QDate::fromString(val, Qt::ISODate);
    case QMetaType::QTime: return QTime::fromString(val, Qt::ISODate);
    case QMetaType::QDateTime: return QDateTime::fromString(val, Qt::ISODate);
#ifdef QT_GUI_LIB
    case QMetaType::QImage: return loadImageVariant(type, val);
    case QMetaType::QPixmap: return QPixmap::fromImage(loadImageVariant(type, val));
    case QMetaType::QBitmap: return QBitmap::fromImage(loadImageVariant(type, val));
#endif
    default:
        return stringToVariant(val);
    }
}
#ifdef QT_GUI_LIB
QString AbstractModelSerialiserPrivate::saveImageVariant(const QImage& imageData)
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    imageData.save(&buffer, "PNG");
    return QString::fromLatin1(byteArray.toBase64().constData());
}
QImage AbstractModelSerialiserPrivate::loadImageVariant(int type, const QString& val)
{
    Q_UNUSED(type)
    QByteArray byteArray = QByteArray::fromBase64(val.toLatin1());
    QBuffer buffer(&byteArray);
    QImage imageData;
    imageData.load(&buffer, "PNG");
    return imageData;
}
#endif
QString AbstractModelSerialiserPrivate::saveVariant(const QVariant& val)
{
    if (val.isNull())
        return QString();
    switch (val.type()) {
    case QMetaType::UnknownType:
        Q_ASSERT_X(false, "ModelSerialisation::saveVariant", "Trying to save unregistered type.");
        return QString();
    case QMetaType::Bool: return val.toBool() ? QStringLiteral("1") : QStringLiteral("0");
    case QMetaType::Long:
    case QMetaType::Short:
    case QMetaType::Char:
    case QMetaType::SChar:
    case QMetaType::Int: return QString::number(val.toInt());
    case QMetaType::ULong:
    case QMetaType::UShort:
    case QMetaType::UChar:
    case QMetaType::UInt: return QString::number(val.toUInt());
    case QMetaType::LongLong: return QString::number(val.toLongLong());
    case QMetaType::ULongLong:  return QString::number(val.toULongLong());
    case QMetaType::Double:  
    case QMetaType::Float: return guessDecimalsString(val.toDouble());
    case QMetaType::QChar: return QString(val.toChar());
    case QMetaType::QString: return val.toString();
    case QMetaType::QByteArray: return QString::fromLatin1(val.toByteArray().toBase64());
    case QMetaType::QDate: return val.toDate().toString(Qt::ISODate);
    case QMetaType::QTime: return val.toTime().toString(Qt::ISODate);
    case QMetaType::QDateTime: return val.toDateTime().toString(Qt::ISODate);
#ifdef QT_GUI_LIB
    case QMetaType::QImage: return saveImageVariant(val.value<QImage>());
    case QMetaType::QPixmap: return saveImageVariant(val.value<QPixmap>().toImage());
    case QMetaType::QBitmap: return saveImageVariant(val.value<QBitmap>().toImage());
#endif
    default:
        return variantToString(val);
    }
}
/*!
    Constructs a serialiser operating over \a model

    \sa isEmpty()
*/
AbstractModelSerialiser::AbstractModelSerialiser(QAbstractItemModel* model)
    :d_ptr(new AbstractModelSerialiserPrivate(this))
{
    setModel(model);
}
/*!
    \overload

    loadModel will always fail as the model is not editable
*/
AbstractModelSerialiser::AbstractModelSerialiser(const QAbstractItemModel* model)
    : d_ptr(new AbstractModelSerialiserPrivate(this))
{
    setModel(model);
}
/*! 
    \internal
*/
AbstractModelSerialiser::AbstractModelSerialiser(AbstractModelSerialiserPrivate& d)
    :d_ptr(&d)
{}
/*!
    Creates a copy of \a other.
*/
AbstractModelSerialiser::AbstractModelSerialiser(const AbstractModelSerialiser& other)
    : d_ptr(new AbstractModelSerialiserPrivate(this))
{
    operator=(other);
}

#ifdef Q_COMPILER_RVALUE_REFS
/*!
    Creates a serialiser moving \a other.
*/
AbstractModelSerialiser::AbstractModelSerialiser(AbstractModelSerialiser&& other)
    : d_ptr(other.d_ptr)
{
    other.d_ptr = Q_NULLPTR;
    if (d_ptr)
        d_ptr->q_ptr = this;
}

AbstractModelSerialiser& AbstractModelSerialiser::operator=(AbstractModelSerialiser&& other)
{
    std::swap(d_ptr, other.d_ptr);
    d_ptr->q_ptr = this;
    return *this;
}

#endif // Q_COMPILER_RVALUE_REFS

/*!
    Assigns \a other to this object.
*/
AbstractModelSerialiser& AbstractModelSerialiser::operator=(const AbstractModelSerialiser& other)
{
    if (!d_ptr)
        d_ptr = new AbstractModelSerialiserPrivate(this);
    Q_D(AbstractModelSerialiser);
    Q_ASSERT_X(other.d_func(), "AbstractModelSerialiser", "Trying to access data on moved object");
    d->m_model = other.d_func()->m_model;
    d->m_constModel = other.d_func()->m_constModel;
    return *this;
}
/*!
    Destroys the object.
*/
AbstractModelSerialiser::~AbstractModelSerialiser()
{
    if (d_ptr)
        delete d_ptr;
}

/*!
    \property AbstractModelSerialiser::model
    \brief the model over which the serialiser will operate
*/

/*!
    \brief getter of model property
*/
QAbstractItemModel* AbstractModelSerialiser::model() const
{
    Q_D(const AbstractModelSerialiser);
    Q_ASSERT_X(d, "AbstractModelSerialiser", "Trying to access data on moved object");
    return d->m_model;
}

/*!
    \brief the model over which the serialiser will operate

    loadModel will always fail as the model is not editable
*/
const QAbstractItemModel* AbstractModelSerialiser::constModel() const
{
    Q_D(const AbstractModelSerialiser);
    Q_ASSERT_X(d, "AbstractModelSerialiser", "Trying to access data on moved object");
    return d->m_constModel;
}

/*! 
    \brief setter of model property
*/
void AbstractModelSerialiser::setModel(QAbstractItemModel* val)
{
    Q_D(AbstractModelSerialiser);
    Q_ASSERT_X(d, "AbstractModelSerialiser", "Trying to access data on moved object");
    d->m_model = val;
    d->m_constModel = val;
}

/*!
    \brief set the model over which the serialiser will operate

    loadModel will always fail as the model is not editable
*/
void AbstractModelSerialiser::setModel(const QAbstractItemModel* val)
{
    Q_D(AbstractModelSerialiser);
    Q_ASSERT_X(d, "AbstractModelSerialiser", "Trying to access data on moved object");
    d->m_model = Q_NULLPTR;
    d->m_constModel = val;
}
