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

#include "abstractmodelserialiser.h"
#include "private/abstractmodelserialiser_p.h"
#include <QAbstractItemModel>
/*!
\class AbstractMultiRoleSerialiser

\brief The interface for model serialisers saving multiple roles.
*/

AbstractModelSerialiserPrivate::AbstractModelSerialiserPrivate(AbstractModelSerialiser* q)
    : q_ptr(q)
    , m_model(Q_NULLPTR)
    , m_constModel(Q_NULLPTR)
    , m_rolesToSave(AbstractModelSerialiser::modelDefaultRoles())
{
    Q_ASSERT(q_ptr);
}

/*!
Constructs a serialiser operating over \a model

\sa isEmpty()
*/
AbstractModelSerialiser::AbstractModelSerialiser(QAbstractItemModel* model, QObject* parent)
    : QObject(parent)
    , d_ptr(new AbstractModelSerialiserPrivate(this))
{
    setModel(model);
}
/*!
\overload

loadModel will always fail as the model is not editable
*/
AbstractModelSerialiser::AbstractModelSerialiser(const QAbstractItemModel* model, QObject* parent)
    : QObject(parent)
    , d_ptr(new AbstractModelSerialiserPrivate(this))
{
    setModel(model);
}
/*!
\internal
*/
AbstractModelSerialiser::AbstractModelSerialiser(AbstractModelSerialiserPrivate& d, QObject* parent)
    :d_ptr(&d)
{}

/*!
Destructor
*/
AbstractModelSerialiser::~AbstractModelSerialiser() = default;

/*!
\property AbstractSingleRoleSerialiser::rolesToSave
\brief the roles that will be serialised

by default this property is set to all non obsolete Qt::ItemDataRole values
*/

/*!
\brief getter of rolesToSave property
*/
const QList<int>& AbstractModelSerialiser::rolesToSave() const
{
    Q_D(const AbstractModelSerialiser);
    
    return d->m_rolesToSave;
}
/*!
\brief setter of rolesToSave property
*/
void AbstractModelSerialiser::setRoleToSave(const QList<int>& val)
{
    Q_D(AbstractModelSerialiser);
    
    d->m_rolesToSave = val;
}
/*!
\brief appends \a role to the list of roles to save
*/
void AbstractModelSerialiser::addRoleToSave(int role)
{
    Q_D(AbstractModelSerialiser);
    
    if (!d->m_rolesToSave.contains(role))
        d->m_rolesToSave.append(role);
}
/*!
\brief removes \a role from the list of roles to save
*/
void AbstractModelSerialiser::removeRoleToSave(int role)
{
    Q_D(AbstractModelSerialiser);
    
    d->m_rolesToSave.removeAll(role);
}
/*!
\brief empties the list of roles to save
*/
void AbstractModelSerialiser::clearRoleToSave()
{
    Q_D(AbstractModelSerialiser);
    
    d->m_rolesToSave.clear();
}
/*!
\brief resetter of rolesToSave property

fills the list fo roles to save with all non obsolete Qt::ItemDataRole values
*/
void AbstractModelSerialiser::resetRoleToSave()
{
    Q_D(AbstractModelSerialiser);
    
    d->m_rolesToSave = AbstractModelSerialiser::modelDefaultRoles();
}
/*!
Returns a list of all non obsolete Qt::ItemDataRole values
*/
QList<int> AbstractModelSerialiser::modelDefaultRoles()
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

QAbstractItemModel* AbstractModelSerialiser::model() const
{
    Q_D(const AbstractModelSerialiser);

    return d->m_model;
}


const QAbstractItemModel* AbstractModelSerialiser::constModel() const
{
    Q_D(const AbstractModelSerialiser);

    return d->m_constModel;
}

void AbstractModelSerialiser::setModel(QAbstractItemModel* val)
{
    Q_D(AbstractModelSerialiser);

    d->m_model = val;
    d->m_constModel = val;
}

void AbstractModelSerialiser::setModel(const QAbstractItemModel* val)
{
    Q_D(AbstractModelSerialiser);

    d->m_model = Q_NULLPTR;
    d->m_constModel = val;
}