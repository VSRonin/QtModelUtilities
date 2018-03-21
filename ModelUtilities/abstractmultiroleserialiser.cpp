
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

#ifdef Q_COMPILER_RVALUE_REFS
/*!
Creates a serialiser moving \a other.
*/
AbstractMultiRoleSerialiser::AbstractMultiRoleSerialiser(AbstractMultiRoleSerialiser&& other)
    :AbstractModelSerialiser(static_cast<AbstractModelSerialiser&&>(other))
{}

//! Move assignment
AbstractMultiRoleSerialiser& AbstractMultiRoleSerialiser::operator=(AbstractMultiRoleSerialiser&& other)
{
    AbstractModelSerialiser::operator=(static_cast<AbstractModelSerialiser&&>(other));
    return *this;
}
#endif // Q_COMPILER_RVALUE_REFS
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
Creates a copy of \a other.
*/
AbstractMultiRoleSerialiser::AbstractMultiRoleSerialiser(const AbstractMultiRoleSerialiser& other)
    : AbstractModelSerialiser(*new AbstractMultiRoleSerialiserPrivate(this))
{
    operator=(other);
}

/*!
Destroys the object.
*/
AbstractMultiRoleSerialiser::~AbstractMultiRoleSerialiser() = default;
/*!
Assigns \a other to this object.
*/
AbstractMultiRoleSerialiser& AbstractMultiRoleSerialiser::operator=(const AbstractMultiRoleSerialiser& other)
{
    if (!d_ptr)
        d_ptr = new AbstractMultiRoleSerialiserPrivate(this);
    Q_D(AbstractMultiRoleSerialiser);
    Q_ASSERT_X(other.d_func(), "AbstractMultiRoleSerialiser", "Trying to access data on moved object");
    d->m_rolesToSave = other.d_func()->m_rolesToSave;
    AbstractModelSerialiser::operator=(other);
    return *this;
}
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
    Q_ASSERT_X(d, "AbstractMultiRoleSerialiser", "Trying to access data on moved object");
    return d->m_rolesToSave;
}
/*!
\brief setter of rolesToSave property
*/
void AbstractMultiRoleSerialiser::setRoleToSave(const QList<int>& val)
{
    Q_D(AbstractMultiRoleSerialiser);
    Q_ASSERT_X(d, "AbstractMultiRoleSerialiser", "Trying to access data on moved object");
    d->m_rolesToSave = val;
}
/*!
\brief appends \a role to the list of roles to save
*/
void AbstractMultiRoleSerialiser::addRoleToSave(int role)
{
    Q_D(AbstractMultiRoleSerialiser);
    Q_ASSERT_X(d, "AbstractMultiRoleSerialiser", "Trying to access data on moved object");
    if (!d->m_rolesToSave.contains(role))
        d->m_rolesToSave.append(role);
}
/*!
\brief removes \a role from the list of roles to save
*/
void AbstractMultiRoleSerialiser::removeRoleToSave(int role)
{
    Q_D(AbstractMultiRoleSerialiser);
    Q_ASSERT_X(d, "AbstractMultiRoleSerialiser", "Trying to access data on moved object");
    d->m_rolesToSave.removeAll(role);
}
/*!
\brief empties the list of roles to save
*/
void AbstractMultiRoleSerialiser::clearRoleToSave()
{
    Q_D(AbstractMultiRoleSerialiser);
    Q_ASSERT_X(d, "AbstractMultiRoleSerialiser", "Trying to access data on moved object");
    d->m_rolesToSave.clear();
}
/*!
\brief resetter of rolesToSave property

fills the list fo roles to save with all non obsolete Qt::ItemDataRole values
*/
void AbstractMultiRoleSerialiser::resetRoleToSave()
{
    Q_D(AbstractMultiRoleSerialiser);
    Q_ASSERT_X(d, "AbstractMultiRoleSerialiser", "Trying to access data on moved object");
    d->m_rolesToSave = AbstractMultiRoleSerialiser::modelDefaultRoles();
}
/*!
\brief returns a list of all non obsolete Qt::ItemDataRole values
*/
QList<int> AbstractMultiRoleSerialiser::modelDefaultRoles()
{
    return QList<int>()
        << Qt::DisplayRole
        << Qt::EditRole
        << Qt::DecorationRole
        << Qt::ToolTipRole
        << Qt::StatusTipRole
        << Qt::WhatsThisRole
        << Qt::SizeHintRole
        << Qt::FontRole
        << Qt::TextAlignmentRole
        << Qt::BackgroundRole
        << Qt::ForegroundRole
        << Qt::CheckStateRole
        << Qt::InitialSortOrderRole
        << Qt::AccessibleTextRole
        << Qt::AccessibleDescriptionRole
        << Qt::UserRole
        ;
}
