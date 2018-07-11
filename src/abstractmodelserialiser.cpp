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

#include "abstractmodelserialiser.h"
#include "private/abstractmodelserialiser_p.h"
#include <QDataStream>
#include <QVariant>
#include <QDateTime>
#include <cmath>
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

/*!
\internal
*/
int AbstractModelSerialiserPrivate::guessDecimals(double val)
{
    int precision = 0;
    for (double junk = 0; !qFuzzyIsNull(std::modf(val, &junk)); ++precision)
        val *= 10.0;
    return precision;
}

/*!
\internal
*/
QString AbstractModelSerialiserPrivate::guessDecimalsString(double val, QLocale* loca)
{
    if (loca)
        return loca->toString(val, 'f', guessDecimals(val));
    return QString::number(val, 'f', guessDecimals(val));
}

/*!
\internal
*/
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

/*!
\internal
*/
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

/*!
\internal
*/
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

/*!
\internal
*/
QString AbstractModelSerialiserPrivate::saveImageVariant(const QImage& imageData)
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    imageData.save(&buffer, "PNG");
    return QString::fromLatin1(byteArray.toBase64().constData());
}

/*!
\internal
*/
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

/*!
\internal
*/
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
Construct a read/write serialiser
*/
AbstractModelSerialiser::AbstractModelSerialiser(QAbstractItemModel* model)
    :d_ptr(new AbstractModelSerialiserPrivate(this))
{
    setModel(model);
}

/*!
Construct a write-only serialiser
*/
AbstractModelSerialiser::AbstractModelSerialiser(const QAbstractItemModel* model)
    : d_ptr(new AbstractModelSerialiserPrivate(this))
{
    setModel(model);
}

/*!
Constructor used only while subclassing the private class.
Not part of the public API
*/
AbstractModelSerialiser::AbstractModelSerialiser(AbstractModelSerialiserPrivate& d)
    :d_ptr(&d)
{}

/*!
Destructor
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
    
    return d->m_model;
}

/*!
    \brief the model over which the serialiser will operate

    loadModel will always fail as the model is not editable
*/
const QAbstractItemModel* AbstractModelSerialiser::constModel() const
{
    Q_D(const AbstractModelSerialiser);
    
    return d->m_constModel;
}

/*! 
    \brief setter of model property
*/
void AbstractModelSerialiser::setModel(QAbstractItemModel* val)
{
    Q_D(AbstractModelSerialiser);
    
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
    
    d->m_model = Q_NULLPTR;
    d->m_constModel = val;
}
