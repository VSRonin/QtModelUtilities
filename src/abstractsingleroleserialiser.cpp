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
    : AbstractStringSerialiserPrivate(q)
{
    Q_ASSERT(q_ptr);
}

/*!
Construct a read/write serialiser
*/
AbstractSingleRoleSerialiser::AbstractSingleRoleSerialiser(QAbstractItemModel* model, QObject* parent)
    : AbstractStringSerialiser(*new AbstractSingleRoleSerialiserPrivate(this), parent)
{
    setRoleToSave(Qt::DisplayRole);
    setModel(model);
}

/*!
Construct a write-only serialiser
*/
AbstractSingleRoleSerialiser::AbstractSingleRoleSerialiser(const QAbstractItemModel* model, QObject* parent)
    : AbstractStringSerialiser(*new AbstractSingleRoleSerialiserPrivate(this), parent)
{
    setRoleToSave(Qt::DisplayRole);
    setModel(model);
}

/*!
Constructor used only while subclassing the private class.
Not part of the public API
*/
AbstractSingleRoleSerialiser::AbstractSingleRoleSerialiser(AbstractSingleRoleSerialiserPrivate& d, QObject* parent)
    :AbstractStringSerialiser(d, parent)
{}

/*!
Destructor
*/
AbstractSingleRoleSerialiser::~AbstractSingleRoleSerialiser() = default;


/*!
\property AbstractSingleRoleSerialiser::roleToSave
\accessors %roleToSave(), setRoleToSave()
\brief The role that will be serialised

By default this property is set to Qt::DisplayRole
*/

int AbstractSingleRoleSerialiser::roleToSave() const
{
    return rolesToSave().first();
}

void AbstractSingleRoleSerialiser::setRoleToSave(int val)
{
    setRoleToSave(QList<int>{val});
}

/*!
\reimp
If \a val contains more than one value only the first one is considered
*/
void AbstractSingleRoleSerialiser::setRoleToSave(const QList<int>& val)
{
    if (val.size() > 1)
        return AbstractModelSerialiser::setRoleToSave(QList<int>{val.first()});
    return AbstractModelSerialiser::setRoleToSave(val);
}

/*!
\reimp
Equivalent to setRoleToSave
*/
void AbstractSingleRoleSerialiser::addRoleToSave(int role)
{
    setRoleToSave(role);
}

/*!
\reimp
If \a role is the currently saved role, resets the role to save to Qt::DisplayRole
*/
void AbstractSingleRoleSerialiser::removeRoleToSave(int role)
{
    AbstractModelSerialiser::removeRoleToSave(role);
    if (rolesToSave().isEmpty())
        setRoleToSave(Qt::DisplayRole);
}

/*!
\reimp
Resets the role to save to Qt::DisplayRole
*/
void AbstractSingleRoleSerialiser::clearRoleToSave()
{
    setRoleToSave(Qt::DisplayRole);
}

/*!
\reimp
Resets the role to save to Qt::DisplayRole
*/
void AbstractSingleRoleSerialiser::resetRoleToSave()
{
    setRoleToSave(Qt::DisplayRole);
}
