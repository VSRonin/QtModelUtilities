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

/*
#ifndef jsonmodelserialiser_h__
#define jsonmodelserialiser_h__

#include "modelutilities_global.h"
#include "abstractmultiroleserialiser.h"
#include <QJsonObject>
class JsonModelSerialiserPrivate;
class MODELUTILITIES_EXPORT JsonModelSerialiser : public AbstractMultiRoleSerialiser
{
    Q_GADGET
    Q_DECLARE_PRIVATE(JsonModelSerialiser)
    Q_DISABLE_COPY(JsonModelSerialiser)
public:
    JsonModelSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    JsonModelSerialiser(const QAbstractItemModel* model);
    ~JsonModelSerialiser();
    Q_INVOKABLE bool saveModel(QIODevice* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE bool saveModel(QByteArray* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool saveModel(QString* destination) const;
    Q_INVOKABLE virtual QJsonObject toJsonObject() const;
    Q_INVOKABLE bool loadModel(QIODevice* source) Q_DECL_OVERRIDE;
    Q_INVOKABLE bool loadModel(const QByteArray& source) Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool loadModel(QString* source);
    Q_INVOKABLE virtual bool fromJsonObject(const QJsonObject& source);
protected:
    JsonModelSerialiser(JsonModelSerialiserPrivate& d);
};
#endif // jsonmodelserialiser_h__*/