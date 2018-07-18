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
#include "jsonmodelserialiser.h"
#include "private/jsonmodelserialiser_p.h"
#include <QJsonDocument>
#include <QTextStream>
#include <QJsonParseError>
#include <QJsonArray>

/*!
\internal
*/
JsonModelSerialiserPrivate::JsonModelSerialiserPrivate(JsonModelSerialiser* q)
    :AbstractStringSerialiserPrivate(q)
{}

/*!
\internal
*/
bool JsonModelSerialiserPrivate::fromJsonObject(const QJsonObject& source, const QModelIndex& parent)
{
    if (source.isEmpty())
        return false;
    QJsonValue tempValue = source.value(QLatin1String("rows"));
    if (tempValue.isUndefined() || !tempValue.isDouble())
        return false;
    const int maxRow =
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
        tempValue.toInt();
#else
        static_cast<int>(tempValue.toDouble());
#endif
    if(!m_model->insertRows(0, maxRow, parent))
        return false;
    tempValue = source.value(QLatin1String("columns"));
    if (tempValue.isUndefined() || !tempValue.isDouble())
        return false;
    const int maxCol = 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
        tempValue.toInt();
#else
        static_cast<int>(tempValue.toDouble());
#endif
    if (!m_model->insertColumns(0, maxCol, parent))
        return false;
    if (!parent.isValid()) {
        for (auto orientat : { std::make_pair(QLatin1String("verticalHeader"), Qt::Vertical), std::make_pair(QLatin1String("horizontalHeader"), Qt::Horizontal) }) {
            tempValue = source.value(orientat.first);
            if (!tempValue.isUndefined()) {
                if (!tempValue.isArray())
                    return false;
                const QJsonArray headerArray = tempValue.toArray();
                for (int i = 0, maxHeaderArray = headerArray.size(); i < maxHeaderArray; ++i) {
                    tempValue = headerArray.at(i);
                    if (!tempValue.isObject())
                        return false;
                    const QJsonObject sectionObject = tempValue.toObject();
                    tempValue = sectionObject.value(QLatin1String("section"));
                    if (tempValue.isUndefined() || !tempValue.isDouble())
                        return false;
                    const int sectIdx = 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
                        tempValue.toInt();
#else
                        static_cast<int>(tempValue.toDouble());
#endif
                    tempValue = sectionObject.value(QLatin1String("values"));
                    if (tempValue.isUndefined() || !tempValue.isArray())
                        return false;
                    const QJsonArray valuesArray = tempValue.toArray();
                    for (int j = 0, maxValuesArray = valuesArray.size(); j < maxValuesArray; ++j) {
                        tempValue = valuesArray.at(j);
                        if (tempValue.isUndefined() || !tempValue.isObject())
                            return false;
                        const QJsonObject roleObject = tempValue.toObject();
                        tempValue = roleObject.value(QLatin1String("role"));
                        if (tempValue.isUndefined() || !tempValue.isDouble())
                            return false;
                        const int tempRole = 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
                            tempValue.toInt();
#else
                            static_cast<int>(tempValue.toDouble());
#endif
                        tempValue = roleObject.value(QLatin1String("type"));
                        if (tempValue.isUndefined() || !tempValue.isDouble())
                            return false;
                        const int tempType = 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
                            tempValue.toInt();
#else
                            static_cast<int>(tempValue.toDouble());
#endif
                        tempValue = roleObject.value(QLatin1String("value"));
                        if (tempValue.isUndefined() || !tempValue.isString())
                            return false;
                        if (!m_model->setHeaderData(sectIdx, orientat.second, loadVariant(tempType, tempValue.toString()), tempRole))
                            return false;
                    }
                    
                }
            }
        }
    }

    tempValue = source.value(QLatin1String("data"));
    if (tempValue.isUndefined())
        return true;
    if (!tempValue.isArray())
        return false;
    const QJsonArray dataArray = tempValue.toArray();
    for (int i = 0, maxDataArray = dataArray.size(); i <maxDataArray; ++i){
        tempValue = dataArray.at(i);
        if (tempValue.isUndefined() || !tempValue.isObject())
            return false;
        const QJsonObject dataObject = tempValue.toObject();
        tempValue = dataObject.value(QLatin1String("row"));
        if (tempValue.isUndefined() || !tempValue.isDouble())
            return false;
        const int currRow = 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
            tempValue.toInt();
#else
            static_cast<int>(tempValue.toDouble());
#endif
        if (currRow >= maxRow || currRow < 0)
            return false;
        tempValue = dataObject.value(QLatin1String("col"));
        if (tempValue.isUndefined() || !tempValue.isDouble())
            return false;
        const int currCol = 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
            tempValue.toInt();
#else
            static_cast<int>(tempValue.toDouble());
#endif
        if (currCol >= maxCol || currCol < 0)
            return false;
        const QModelIndex currIdx = m_model->index(currRow, currCol, parent);
        tempValue = dataObject.value(QLatin1String("values"));
        if (!tempValue.isUndefined()) {
            if (!tempValue.isArray())
                return false;
            const QJsonArray valuesArray = tempValue.toArray();
            for (int j = 0, maxValuesArray = valuesArray.size(); j <maxValuesArray; ++j) {
                tempValue = valuesArray.at(j);
                if (tempValue.isUndefined() || !tempValue.isObject())
                    return false;
                const QJsonObject roleObject = tempValue.toObject();
                if (!roleForObject(roleObject, currIdx))
                    return false;
            }
        }
        tempValue = dataObject.value(QLatin1String("children"));
        if (!tempValue.isUndefined()) {
            if (!tempValue.isObject())
                return false;
            if (!fromJsonObject(tempValue.toObject(), currIdx))
                return false;
        }
    }


    return true;
}

/*!
\internal
*/
bool JsonModelSerialiserPrivate::roleForObject(const QJsonObject& source, const QModelIndex& destination)
{
    QJsonValue tempValue = source.value(QLatin1String("role"));
    if (tempValue.isUndefined() || !tempValue.isDouble())
        return false;
    const int tempRole = 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
        tempValue.toInt();
#else
        static_cast<int>(tempValue.toDouble());
#endif
    tempValue = source.value(QLatin1String("type"));
    if (tempValue.isUndefined() || !tempValue.isDouble())
        return false;
    const int tempType = 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
        tempValue.toInt();
#else
        static_cast<int>(tempValue.toDouble());
#endif
    tempValue = source.value(QLatin1String("value"));
    if (tempValue.isUndefined() || !tempValue.isString())
        return false;
    if (!m_model->setData(destination, loadVariant(tempType, tempValue.toString()), tempRole))
        return false;
    return true;
}

/*!
\internal
*/
QJsonObject JsonModelSerialiserPrivate::objectForRole(int role, const QVariant& value) const 
{
    QJsonObject roleObject;
    roleObject[QLatin1String("role")] = role;
    roleObject[QLatin1String("type")] = static_cast<int>(value.type());
    roleObject[QLatin1String("value")] = saveVariant(value);
    return roleObject;
}

/*!
\internal
*/
QJsonObject JsonModelSerialiserPrivate::toJsonObject(const QModelIndex& parent) const
{
    QJsonObject result;
    if (!m_model)
        return result;
    const int maxRow = m_model->rowCount(parent);
    const int maxCol = m_model->columnCount(parent);
    result[QLatin1String("rows")] = maxRow;
    result[QLatin1String("columns")] = maxCol;
    if (!parent.isValid()) {
        QJsonArray verticalHeadData;
        for (int i = 0; i < maxRow; ++i) {
            QJsonObject headerObject;
            QJsonArray rolesArray;
            for (auto roleIter = m_rolesToSave.constBegin(); roleIter != m_rolesToSave.constEnd(); ++roleIter){
                const QVariant roleVariant = m_model->headerData(i, Qt::Vertical, *roleIter);
                if (roleVariant.isValid())
                    rolesArray.append(objectForRole(*roleIter, roleVariant));
            }
            if (!rolesArray.isEmpty()) {
                headerObject[QLatin1String("section")] = i;
                headerObject[QLatin1String("values")] = rolesArray;
                verticalHeadData.append(headerObject);
            }
        }
        if (!verticalHeadData.isEmpty())
            result[QLatin1String("verticalHeader")] = verticalHeadData;
        QJsonArray horizontalHeadData;
        for (int i = 0; i < maxCol; ++i) {
            QJsonObject headerObject;
            QJsonArray rolesArray;
            for (auto roleIter = m_rolesToSave.constBegin(); roleIter != m_rolesToSave.constEnd(); ++roleIter) {
                const QVariant roleVariant = m_model->headerData(i, Qt::Horizontal, *roleIter);
                if (roleVariant.isValid())
                    rolesArray.append(objectForRole(*roleIter, roleVariant));
            }
            if (!rolesArray.isEmpty()) {
                headerObject[QLatin1String("section")] = i;
                headerObject[QLatin1String("values")] = rolesArray;
                horizontalHeadData.append(headerObject);
            }
        }
        if (!horizontalHeadData.isEmpty())
            result[QLatin1String("horizontalHeader")] = horizontalHeadData;
    }
    QJsonArray dataArray;
    for (int i = 0; i < maxRow;++i){
        for (int j = 0; j < maxCol; ++j) {
            QJsonObject dataObject;
            dataObject[QLatin1String("row")] = i;
            dataObject[QLatin1String("col")] = j;
            const QModelIndex currIdx = m_model->index(i, j, parent);
            QJsonArray valuesArray;
            for (auto roleIter = m_rolesToSave.constBegin(); roleIter != m_rolesToSave.constEnd(); ++roleIter){
                const QVariant roleVariant = currIdx.data(*roleIter);
                if (roleVariant.isValid())
                    valuesArray.append(objectForRole(*roleIter, roleVariant));
            }
            if (!valuesArray.isEmpty())
                dataObject[QLatin1String("values")] = valuesArray;
            if (m_model->hasChildren(currIdx))
                dataObject[QLatin1String("children")] = toJsonObject(currIdx);
            dataArray.append(dataObject);
        }
    }
    if (!dataArray.isEmpty())
        result[QLatin1String("data")] = dataArray;
    return result;
}

/*!
Construct a read/write serialiser
*/
JsonModelSerialiser::JsonModelSerialiser(QAbstractItemModel* model, QObject* parent)
    : AbstractStringSerialiser(*new JsonModelSerialiserPrivate(this), parent)
{
    setModel(model);
}

/*!
Construct a write-only serialiser
*/
JsonModelSerialiser::JsonModelSerialiser(const QAbstractItemModel* model, QObject* parent)
    : AbstractStringSerialiser(*new JsonModelSerialiserPrivate(this), parent)
{
    setModel(model);
}


/*!
Constructor used only while subclassing the private class.
Not part of the public API
*/
JsonModelSerialiser::JsonModelSerialiser(JsonModelSerialiserPrivate& d, QObject* parent)
    :AbstractStringSerialiser(d, parent)
{}


/*!
Saves the model in JSON format to the \a destination string
*/
bool JsonModelSerialiser::saveModel(QString* destination) const
{
    Q_D(const JsonModelSerialiser);
    if (!d->m_model || !destination)
        return false;
    const QJsonDocument jDoc(toJsonObject());
    if (jDoc.isNull())
        return false;
    *destination = QString::fromUtf8(jDoc.toJson(d->m_format));
    return true;
}

/*!
Saves the model in JSON format to the \a destination byte array
*/
bool JsonModelSerialiser::saveModel(QByteArray* destination) const
{
    QString tempString;
    if (!saveModel(&tempString))
        return false;
    QTextStream writer(destination);
    writer.setCodec(textCodec());
    writer << tempString;
    return true;
}

/*!
Saves the model in JSON format to the \a destination device
*/
bool JsonModelSerialiser::saveModel(QIODevice* destination) const
{
    QString tempString;
    if (!saveModel(&tempString))
        return false;
    QTextStream writer(destination);
    writer.setCodec(textCodec());
    writer << tempString;
    return true;
}

/*!
Saves the model to the \a destination JSON object
*/
QJsonObject JsonModelSerialiser::toJsonObject() const
{
    Q_D(const JsonModelSerialiser);
    return d->toJsonObject();
}

/*!
Loads the model from a \a source string containing JSON data
*/
bool JsonModelSerialiser::loadModel(QString* source)
{
    Q_D(const JsonModelSerialiser);
    if (!d->m_model || !source)
        return false;
    QJsonParseError parseErr;
    const QJsonDocument jDoc = QJsonDocument::fromJson(source->toUtf8(), &parseErr);
    if (parseErr.error != QJsonParseError::NoError)
        return false;
    return fromJsonObject(jDoc.object());
}

/*!
Loads the model from a \a source device containing JSON data
*/
bool JsonModelSerialiser::loadModel(QIODevice* source)
{
    if (!source)
        return false;
    if (!source->isOpen()) {
        if (!source->open(QIODevice::ReadOnly | QIODevice::Text))
            return false;
    }
    if (!source->isReadable())
        return false;
    source->setTextModeEnabled(true);
    QTextStream reader(source);
    reader.setCodec(textCodec());
    QString loadString = reader.readAll();
    return loadModel(&loadString);
}

/*!
Loads the model from a \a source byte array containing JSON data
*/
bool JsonModelSerialiser::loadModel(const QByteArray& source)
{
    QTextStream reader(source, QIODevice::ReadOnly | QIODevice::Text);
    reader.setCodec(textCodec());
    QString loadString = reader.readAll();
    return loadModel(&loadString);
}

/*!
Loads the model from a \a source JSON object
*/
bool JsonModelSerialiser::fromJsonObject(const QJsonObject& source)
{
    Q_D(JsonModelSerialiser);
    if (!d->m_model)
        return false;
    d->m_model->removeColumns(0, d->m_model->columnCount());
    d->m_model->removeRows(0, d->m_model->rowCount());
    return d->fromJsonObject(source);
}

QJsonDocument::JsonFormat JsonModelSerialiser::format() const
{
    Q_D(const JsonModelSerialiser);
    return d->m_format;
}

void JsonModelSerialiser::setFormat(QJsonDocument::JsonFormat val)
{
    Q_D(JsonModelSerialiser);
    d->m_format = val;
}
