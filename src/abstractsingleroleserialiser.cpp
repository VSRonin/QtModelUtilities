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

#include "abstractsingleroleserialiser.h"
#include "private/abstractsingleroleserialiser_p.h"
/*!
\class AbstractSingleRoleSerialiser

\brief The interface for model serialisers saving only one role.
*/
AbstractSingleRoleSerialiserPrivate::AbstractSingleRoleSerialiserPrivate(AbstractSingleRoleSerialiser* q)
    : AbstractModelSerialiserPrivate(q)
    , m_roleToSave(Qt::DisplayRole)
{
    Q_ASSERT(q_ptr);
}

/*!
Constructs a serialiser operating over \a model

\sa isEmpty()
*/
AbstractSingleRoleSerialiser::AbstractSingleRoleSerialiser(QAbstractItemModel* model)
    : AbstractModelSerialiser(*new AbstractSingleRoleSerialiserPrivate(this))
{
    setModel(model);
}
/*!
\overload

loadModel will always fail as the model is not editable
*/
AbstractSingleRoleSerialiser::AbstractSingleRoleSerialiser(const QAbstractItemModel* model)
    : AbstractModelSerialiser(*new AbstractSingleRoleSerialiserPrivate(this))
{
    setModel(model);
}
/*!
\internal
*/
AbstractSingleRoleSerialiser::AbstractSingleRoleSerialiser(AbstractSingleRoleSerialiserPrivate& d)
    :AbstractModelSerialiser(d)
{}



/*!
Destroys the object.
*/
AbstractSingleRoleSerialiser::~AbstractSingleRoleSerialiser() = default;


/*!
\property AbstractSingleRoleSerialiser::roleToSave
\brief the role that will be serialised

by default this property is set to Qt::DisplayRole
*/

/*!
\brief getter of roleToSave property
*/
int AbstractSingleRoleSerialiser::roleToSave() const
{
    Q_D(const AbstractSingleRoleSerialiser);
    
    return d->m_roleToSave;
}

/*!
\brief setter of roleToSave property
*/
void AbstractSingleRoleSerialiser::setRoleToSave(int val)
{
    Q_D(AbstractSingleRoleSerialiser);
    
    d->m_roleToSave = val;
}
