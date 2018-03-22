
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
