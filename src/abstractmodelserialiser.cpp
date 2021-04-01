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

AbstractModelSerialiserPrivate::AbstractModelSerialiserPrivate(AbstractModelSerialiser *q)
    : q_ptr(q)
    , m_model(Q_NULLPTR)
    , m_constModel(Q_NULLPTR)
    , m_rolesToSave(AbstractModelSerialiser::modelDefaultRoles())
{
    Q_ASSERT(q_ptr);
}

/*!
Constructs a serialiser operating over \a model
*/
AbstractModelSerialiser::AbstractModelSerialiser(QAbstractItemModel *model, QObject *parent)
    : QObject(parent)
    , d_ptr(new AbstractModelSerialiserPrivate(this))
{
    setModel(model);
}

/*!
\overload

the model will only be allowed to be saved, not loaded
*/
AbstractModelSerialiser::AbstractModelSerialiser(const QAbstractItemModel *model, QObject *parent)
    : QObject(parent)
    , d_ptr(new AbstractModelSerialiserPrivate(this))
{
    setModel(model);
}

/*!
\internal
*/
AbstractModelSerialiser::AbstractModelSerialiser(AbstractModelSerialiserPrivate &d, QObject *parent)
    : d_ptr(&d)
{ }

/*!
Destructor
*/
AbstractModelSerialiser::~AbstractModelSerialiser() = default;

/*!
\property AbstractModelSerialiser::rolesToSave
\accessors %rolesToSave(), setRoleToSave()
\brief The roles that will be serialised
\details By default this property is set to all non obsolete Qt::ItemDataRole values
*/

const QList<int> &AbstractModelSerialiser::rolesToSave() const
{
    Q_D(const AbstractModelSerialiser);

    return d->m_rolesToSave;
}

void AbstractModelSerialiser::setRoleToSave(const QList<int> &val)
{
    Q_D(AbstractModelSerialiser);

    d->m_rolesToSave = val;
}

/*!
\brief Appends \a role to the list of roles to save
*/
void AbstractModelSerialiser::addRoleToSave(int role)
{
    Q_D(AbstractModelSerialiser);

    if (!d->m_rolesToSave.contains(role))
        d->m_rolesToSave.append(role);
}
/*!
\brief Removes \a role from the list of roles to save
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
Returns a list of all non-obsolete Qt::ItemDataRole values
*/
QList<int> AbstractModelSerialiser::modelDefaultRoles()
{
    return QList<int>{Qt::EditRole,
                      Qt::DecorationRole,
                      Qt::ToolTipRole,
                      Qt::StatusTipRole,
                      Qt::WhatsThisRole,
                      Qt::SizeHintRole,
                      Qt::FontRole,
                      Qt::TextAlignmentRole,
                      Qt::BackgroundRole,
                      Qt::ForegroundRole,
                      Qt::CheckStateRole,
                      Qt::InitialSortOrderRole,
                      Qt::AccessibleTextRole,
                      Qt::AccessibleDescriptionRole,
                      Qt::UserRole};
}

/*!
\property AbstractModelSerialiser::model
\accessors %model(), setModel()
\brief The model over which the serialiser will operate for reading/writing
\details If setModel(const QAbstractItemModel*) was used to se the model, this method will return \c nullptr
*/

QAbstractItemModel *AbstractModelSerialiser::model() const
{
    Q_D(const AbstractModelSerialiser);

    return d->m_model;
}

/*!
\property AbstractModelSerialiser::constModel
\accessors %constModel(), setModel()
\brief The model over which the serialiser will operate in read mode
*/

const QAbstractItemModel *AbstractModelSerialiser::constModel() const
{
    Q_D(const AbstractModelSerialiser);

    return d->m_constModel;
}

void AbstractModelSerialiser::setModel(QAbstractItemModel *val)
{
    Q_D(AbstractModelSerialiser);

    d->m_model = val;
    d->m_constModel = val;
}

/*!
Sets the model over which the serialiser will operate in read mode

Using this method will only allow the model to be saved, not loaded
*/
void AbstractModelSerialiser::setModel(const QAbstractItemModel *val)
{
    Q_D(AbstractModelSerialiser);

    d->m_model = Q_NULLPTR;
    d->m_constModel = val;
}

/*!
\class AbstractModelSerialiser
\brief The interface for model serialisers.

This class serve as a base for all serialisers
*/

/*!
    \fn bool AbstractModelSerialiser::loadModel(const QByteArray &source)
    Loads the model from the given \a source

    Data previously stored in the model will be removed
*/

/*!
    \fn bool AbstractModelSerialiser::loadModel(QIODevice *source)
    Loads the model from the given \a source

    Data previously stored in the model will be removed
*/

/*!
    \fn bool AbstractModelSerialiser::saveModel(QByteArray *destination) const
    Saves the model to the given \a destination
*/

/*!
    \fn bool AbstractModelSerialiser::saveModel(QIODevice *destination) const
    Saves the model to the given \a destination
*/
