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

#ifndef abstractmultiroleserialiser_h__
#define abstractmultiroleserialiser_h__

#include "modelutilities_global.h"
#include "abstractmodelserialiser.h"
class AbstractMultiRoleSerialiserPrivate;
class MODELUTILITIES_EXPORT AbstractMultiRoleSerialiser : public AbstractModelSerialiser
{
    Q_GADGET
    Q_PROPERTY(QList<int> rolesToSave READ rolesToSave WRITE setRoleToSave RESET resetRoleToSave)
    Q_DECLARE_PRIVATE(AbstractMultiRoleSerialiser)
    Q_DISABLE_COPY(AbstractMultiRoleSerialiser)
public:
    AbstractMultiRoleSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    AbstractMultiRoleSerialiser(const QAbstractItemModel* model);
    ~AbstractMultiRoleSerialiser() = 0;
    const QList<int>& rolesToSave() const;
    void setRoleToSave(const QList<int>& val);
    Q_INVOKABLE void addRoleToSave(int role);
    Q_INVOKABLE void removeRoleToSave(int role);
    Q_INVOKABLE void clearRoleToSave();
    void resetRoleToSave();
    static QList<int> modelDefaultRoles();
protected:
    AbstractMultiRoleSerialiser(AbstractMultiRoleSerialiserPrivate& d);
};
#endif // abstractmultiroleserialiser_h__
