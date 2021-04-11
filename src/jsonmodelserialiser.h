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

#ifndef JSONMODELSERIALISER_H
#define JSONMODELSERIALISER_H

#include "modelutilities_global.h"
#include "abstractstringserialiser.h"
#include <QJsonObject>
#include <QJsonDocument>
class JsonModelSerialiserPrivate;
class MODELUTILITIES_EXPORT JsonModelSerialiser : public AbstractStringSerialiser
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(JsonModelSerialiser)
    Q_DISABLE_COPY(JsonModelSerialiser)
public:
    explicit JsonModelSerialiser(QObject *parent = Q_NULLPTR);
    JsonModelSerialiser(QAbstractItemModel *model, QObject *parent);
    JsonModelSerialiser(const QAbstractItemModel *model, QObject *parent);
    bool saveModel(QIODevice *destination) const Q_DECL_OVERRIDE;
    bool saveModel(QByteArray *destination) const Q_DECL_OVERRIDE;
    bool saveModel(QString *destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual QJsonObject toJsonObject() const;
    bool loadModel(QIODevice *source) Q_DECL_OVERRIDE;
    bool loadModel(const QByteArray &source) Q_DECL_OVERRIDE;
    bool loadModel(QString *source) Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool fromJsonObject(const QJsonObject &source);
    Q_INVOKABLE QJsonDocument::JsonFormat format() const;
public Q_SLOTS:
    void setFormat(QJsonDocument::JsonFormat val);

protected:
    JsonModelSerialiser(JsonModelSerialiserPrivate &d, QObject *parent);
};
#endif // JSONMODELSERIALISER_H
