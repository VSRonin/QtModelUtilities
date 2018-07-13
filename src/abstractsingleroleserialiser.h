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

#ifndef abstractsingleroleserialiser_h__
#define abstractsingleroleserialiser_h__

#include "modelutilities_global.h"
#include "abstractstringserialiser.h"
class AbstractSingleRoleSerialiserPrivate;
class MODELUTILITIES_EXPORT AbstractSingleRoleSerialiser : public AbstractStringSerialiser
{
    Q_OBJECT
    Q_PROPERTY(int roleToSave READ roleToSave WRITE setRoleToSave)
    Q_DECLARE_PRIVATE(AbstractSingleRoleSerialiser)
    Q_DISABLE_COPY(AbstractSingleRoleSerialiser)
public:
    AbstractSingleRoleSerialiser(QAbstractItemModel* model = Q_NULLPTR, QObject* parent = Q_NULLPTR);
    AbstractSingleRoleSerialiser(const QAbstractItemModel* model, QObject* parent = Q_NULLPTR);
    ~AbstractSingleRoleSerialiser() = 0;
    int roleToSave() const;
    void setRoleToSave(int val);
    void setRoleToSave(const QList<int>& val) Q_DECL_OVERRIDE;
    void addRoleToSave(int role) Q_DECL_OVERRIDE;
    void removeRoleToSave(int role) Q_DECL_OVERRIDE;
    void clearRoleToSave() Q_DECL_OVERRIDE;
    void resetRoleToSave() Q_DECL_OVERRIDE;
protected:
    AbstractSingleRoleSerialiser(AbstractSingleRoleSerialiserPrivate& d, QObject* parent);
};
#endif // abstractsingleroleserialiser_h__
