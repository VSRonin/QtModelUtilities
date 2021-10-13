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

#ifndef JSONMODELSERIALISER_P_H
#define JSONMODELSERIALISER_P_H
#include "private/abstractstringserialiser_p.h"
#include "jsonmodelserialiser.h"
#include <QModelIndex>
#include <QJsonObject>
class JsonModelSerialiserPrivate : public AbstractStringSerialiserPrivate
{
    Q_DISABLE_COPY(JsonModelSerialiserPrivate);
    Q_DECLARE_PUBLIC(JsonModelSerialiser)
protected:
    JsonModelSerialiserPrivate(JsonModelSerialiser *q);
    QJsonObject toJsonObject(const QModelIndex &parent = QModelIndex()) const;
    bool fromJsonObject(const QJsonObject &source, const QModelIndex &parent = QModelIndex());
    QJsonObject objectForRole(int role, const QVariant &value) const;
    bool roleForObject(const QJsonObject &source, const QModelIndex &destination);
    QJsonDocument::JsonFormat m_format;
};

#endif // JSONMODELSERIALISER_P_H
