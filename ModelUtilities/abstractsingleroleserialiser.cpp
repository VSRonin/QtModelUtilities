
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
#ifdef Q_COMPILER_RVALUE_REFS
/*!
Creates a serialiser moving \a other.
*/
AbstractSingleRoleSerialiser::AbstractSingleRoleSerialiser(AbstractSingleRoleSerialiser&& other)
    :AbstractModelSerialiser(static_cast<AbstractModelSerialiser&&>(other))
{}

//! Move assignment
AbstractSingleRoleSerialiser& AbstractSingleRoleSerialiser::operator=(AbstractSingleRoleSerialiser&& other)
{
    AbstractModelSerialiser::operator=(static_cast<AbstractModelSerialiser&&>(other));
    return *this;
}

#endif // Q_COMPILER_RVALUE_REFS
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
Creates a copy of \a other.
*/
AbstractSingleRoleSerialiser::AbstractSingleRoleSerialiser(const AbstractSingleRoleSerialiser& other)
    : AbstractModelSerialiser(*new AbstractSingleRoleSerialiserPrivate(this))
{
    operator=(other);
}


/*!
Destroys the object.
*/
AbstractSingleRoleSerialiser::~AbstractSingleRoleSerialiser() = default;
/*!
Assigns \a other to this object.
*/
AbstractSingleRoleSerialiser& AbstractSingleRoleSerialiser::operator=(const AbstractSingleRoleSerialiser& other)
{
    if (!d_ptr)
        d_ptr = new AbstractSingleRoleSerialiserPrivate(this);
    Q_D(AbstractSingleRoleSerialiser);
    Q_ASSERT_X(other.d_func(), "AbstractSingleRoleSerialiser", "Trying to access data on moved object");
    d->m_roleToSave = other.d_func()->m_roleToSave;
    AbstractModelSerialiser::operator=(other);
    return *this;
}

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
    Q_ASSERT_X(d, "AbstractSingleRoleSerialiser", "Trying to access data on moved object");
    return d->m_roleToSave;
}

/*!
\brief setter of roleToSave property
*/
void AbstractSingleRoleSerialiser::setRoleToSave(int val)
{
    Q_D(AbstractSingleRoleSerialiser);
    Q_ASSERT_X(d, "AbstractSingleRoleSerialiser", "Trying to access data on moved object");
    d->m_roleToSave = val;
}
