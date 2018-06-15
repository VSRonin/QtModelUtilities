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

#ifndef abstractsingleroleserialiser_p_h__
#define abstractsingleroleserialiser_p_h__
#include "private/abstractmodelserialiser_p.h"
#include "abstractsingleroleserialiser.h"
class AbstractSingleRoleSerialiserPrivate : public AbstractModelSerialiserPrivate
{
    Q_DISABLE_COPY(AbstractSingleRoleSerialiserPrivate);
    Q_DECLARE_PUBLIC(AbstractSingleRoleSerialiser)
protected:
    AbstractSingleRoleSerialiserPrivate(AbstractSingleRoleSerialiser* q);
    int m_roleToSave;
};
#endif // abstractsingleroleserialiser_p_h__