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
#include "abstractmodelserialiser.h"
class AbstractSingleRoleSerialiserPrivate;
class MODELUTILITIES_EXPORT AbstractSingleRoleSerialiser : public AbstractModelSerialiser
{
    Q_GADGET
    Q_PROPERTY(int roleToSave READ roleToSave WRITE setRoleToSave)
    Q_DECLARE_PRIVATE(AbstractSingleRoleSerialiser)
    Q_DISABLE_COPY(AbstractSingleRoleSerialiser)
public:
    AbstractSingleRoleSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    AbstractSingleRoleSerialiser(const QAbstractItemModel* model);
    ~AbstractSingleRoleSerialiser() = 0;
    int roleToSave() const;
    void setRoleToSave(int val);
protected:
    AbstractSingleRoleSerialiser(AbstractSingleRoleSerialiserPrivate& d);
};
#endif // abstractsingleroleserialiser_h__
