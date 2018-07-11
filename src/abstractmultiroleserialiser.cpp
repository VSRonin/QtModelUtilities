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

#include "abstractmultiroleserialiser.h"
#include "private/abstractmultiroleserialiser_p.h"

/*!
\class AbstractMultiRoleSerialiser

\brief The interface for model serialisers saving multiple roles.
*/

AbstractMultiRoleSerialiserPrivate::AbstractMultiRoleSerialiserPrivate(AbstractMultiRoleSerialiser* q)
    : AbstractModelSerialiserPrivate(q)
    , m_rolesToSave(AbstractMultiRoleSerialiser::modelDefaultRoles())
{}

/*!
Constructs a serialiser operating over \a model

\sa isEmpty()
*/
AbstractMultiRoleSerialiser::AbstractMultiRoleSerialiser(QAbstractItemModel* model)
    : AbstractModelSerialiser(*new AbstractMultiRoleSerialiserPrivate(this))
{
    setModel(model);
}
/*!
\overload

loadModel will always fail as the model is not editable
*/
AbstractMultiRoleSerialiser::AbstractMultiRoleSerialiser(const QAbstractItemModel* model)
    : AbstractModelSerialiser(*new AbstractMultiRoleSerialiserPrivate(this))
{
    setModel(model);
}
/*!
\internal
*/
AbstractMultiRoleSerialiser::AbstractMultiRoleSerialiser(AbstractMultiRoleSerialiserPrivate& d)
    :AbstractModelSerialiser(d)
{}

/*!
Destroys the object.
*/
AbstractMultiRoleSerialiser::~AbstractMultiRoleSerialiser() = default;
/*!
\property AbstractSingleRoleSerialiser::rolesToSave
\brief the roles that will be serialised

by default this property is set to all non obsolete Qt::ItemDataRole values
*/

/*!
\brief getter of rolesToSave property
*/
const QList<int>& AbstractMultiRoleSerialiser::rolesToSave() const
{
    Q_D(const AbstractMultiRoleSerialiser);
    
    return d->m_rolesToSave;
}
/*!
\brief setter of rolesToSave property
*/
void AbstractMultiRoleSerialiser::setRoleToSave(const QList<int>& val)
{
    Q_D(AbstractMultiRoleSerialiser);
    
    d->m_rolesToSave = val;
}
/*!
\brief appends \a role to the list of roles to save
*/
void AbstractMultiRoleSerialiser::addRoleToSave(int role)
{
    Q_D(AbstractMultiRoleSerialiser);
    
    if (!d->m_rolesToSave.contains(role))
        d->m_rolesToSave.append(role);
}
/*!
\brief removes \a role from the list of roles to save
*/
void AbstractMultiRoleSerialiser::removeRoleToSave(int role)
{
    Q_D(AbstractMultiRoleSerialiser);
    
    d->m_rolesToSave.removeAll(role);
}
/*!
\brief empties the list of roles to save
*/
void AbstractMultiRoleSerialiser::clearRoleToSave()
{
    Q_D(AbstractMultiRoleSerialiser);
    
    d->m_rolesToSave.clear();
}
/*!
\brief resetter of rolesToSave property

fills the list fo roles to save with all non obsolete Qt::ItemDataRole values
*/
void AbstractMultiRoleSerialiser::resetRoleToSave()
{
    Q_D(AbstractMultiRoleSerialiser);
    
    d->m_rolesToSave = AbstractMultiRoleSerialiser::modelDefaultRoles();
}
/*!
\brief returns a list of all non obsolete Qt::ItemDataRole values
*/
QList<int> AbstractMultiRoleSerialiser::modelDefaultRoles()
{
    return QList<int>{{
            Qt::EditRole
            , Qt::DecorationRole
            , Qt::ToolTipRole
            , Qt::StatusTipRole
            , Qt::WhatsThisRole
            , Qt::SizeHintRole
            , Qt::FontRole
            , Qt::TextAlignmentRole
            , Qt::BackgroundRole
            , Qt::ForegroundRole
            , Qt::CheckStateRole
            , Qt::InitialSortOrderRole
            , Qt::AccessibleTextRole
            , Qt::AccessibleDescriptionRole
            , Qt::UserRole
        }};
}
