#include "rolemaskproxymodel.h"
#include "private/rolemaskproxymodel_p.h"
#include <QVector>
RoleMaskProxyModelPrivate::RoleMaskProxyModelPrivate(RoleMaskProxyModel* q)
    :q_ptr(q)
    , m_transparentIfEmpty(true)
    , m_mergeDisplayEdit(true)
{
    Q_ASSERT(q_ptr);
}
void RoleMaskProxyModelPrivate::interceptDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    Q_Q(RoleMaskProxyModel);
    Q_ASSERT(topLeft.isValid() ? topLeft.model() == q->sourceModel() : true);
    Q_ASSERT(bottomRight.isValid() ? bottomRight.model() == q->sourceModel() : true);
    if (roles.isEmpty()) {
        q->dataChanged(q->mapFromSource(topLeft), q->mapFromSource(bottomRight), roles);
        return;
    }
    QVector<int> filteredRoles;
    filteredRoles.reserve(roles.size());
    for (int singleRole : roles){
        if (m_mergeDisplayEdit && singleRole == Qt::EditRole)
            singleRole = Qt::DisplayRole;
        if(m_maskedRoles.contains(singleRole)){
            if(m_transparentIfEmpty){
                bool allOpaque = true;
                for (int i = topLeft.row(); allOpaque && i <= bottomRight.row(); ++i) {
                    for (int j = topLeft.column(); allOpaque &&j <= bottomRight.column(); ++j) {
                        allOpaque = m_maskedData.value(q->sourceModel()->index(i, j, topLeft.parent())).value(singleRole).isValid();
                    }
                }
                if (allOpaque)
                    continue;
            }
            else {
                continue;
            }
        }
        filteredRoles << singleRole;
        if (m_mergeDisplayEdit && singleRole == Qt::DisplayRole)
            filteredRoles << Qt::EditRole;
    }
    if (filteredRoles.isEmpty())
        return;
    q->dataChanged(q->mapFromSource(topLeft), q->mapFromSource(bottomRight), filteredRoles);
}

void RoleMaskProxyModelPrivate::clearUnusedMaskedRoles(const QSet<int>& newRoles)
{
    if (newRoles.isEmpty()) {
        m_maskedData.clear();
        return;
    }
    const auto idxEnd = m_maskedData.end();
    for (auto idxIter = m_maskedData.begin(); idxIter != idxEnd;) {
        const auto roleEnd = idxIter->end();
        for (auto roleIter = idxIter->begin(); roleIter != roleEnd;) {
            if (!newRoles.contains(roleIter.key()))
                roleIter = idxIter->erase(roleIter);
            else
                ++roleIter;
        }
        if (idxIter->isEmpty())
            idxIter = m_maskedData.erase(idxIter);
        else
            ++idxIter;
    }
}

bool RoleMaskProxyModelPrivate::removeRole(const QPersistentModelIndex& idx, int role)
{
    const auto idxIter = m_maskedData.find(idx);
    return removeRole(idxIter, role);
}

bool RoleMaskProxyModelPrivate::removeRole(const QHash<QPersistentModelIndex, RolesContainer >::iterator& idxIter, int role)
{
    if (idxIter == m_maskedData.end())
        return false;
    if (idxIter->remove(role) > 0) {
        if (idxIter->isEmpty())
            m_maskedData.erase(idxIter);
        return true;
    }
    return false;
}



void RoleMaskProxyModelPrivate::signalAllChanged(const QVector<int>& roles, const QModelIndex& parent)
{
    Q_Q(RoleMaskProxyModel);
    const int rowCnt = q->rowCount(parent);
    const int colCnt = q->columnCount(parent);
    for (int i = 0; i < rowCnt;++i){
        for (int j = 0; j < colCnt; ++j) {
            const QModelIndex currPar = q->index(i, j, parent);
            if (q->hasChildren(currPar))
                signalAllChanged(roles, currPar);
        }
    }
    q->dataChanged(q->index(0, 0, parent), q->index(rowCnt, colCnt, parent), roles);
}

/*!
Constructs a new proxy model with the given \a parent.
*/
RoleMaskProxyModel::RoleMaskProxyModel(QObject* parent)
    :QIdentityProxyModel(parent)
    , d_ptr(new RoleMaskProxyModelPrivate(this))
{}

/*!
Destructor
*/
RoleMaskProxyModel::~RoleMaskProxyModel()
{
    Q_D(RoleMaskProxyModel);
    if (sourceModel())
        QObject::disconnect(d->m_sourceDataChangedConnection);
    delete d_ptr;
}

QList<int> RoleMaskProxyModel::maskedRoles() const
{
    Q_D(const RoleMaskProxyModel);
    return d->m_maskedRoles.toList();
}

void RoleMaskProxyModel::setMaskedRoles(const QList<int>& newRoles)
{
    Q_D(RoleMaskProxyModel);
    const QSet<int> roles = newRoles.toSet();
    if (d->m_maskedRoles!=roles){
        QVector<int> changedRoles;
        QSet<int> changedRolesSet = roles;
        changedRolesSet.unite(d->m_maskedRoles);
        if (d->m_mergeDisplayEdit && (changedRolesSet.contains(Qt::EditRole) || changedRolesSet.contains(Qt::DisplayRole)))
            changedRolesSet << Qt::EditRole << Qt::DisplayRole;
        changedRoles.reserve(changedRolesSet.size());
        for (auto i = changedRolesSet.cbegin(); i != changedRolesSet.cend();++i)
            changedRoles.append(*i);
        d->m_maskedRoles = roles;
        if (d->m_mergeDisplayEdit && d->m_maskedRoles.contains(Qt::EditRole)){
            d->m_maskedRoles.remove(Qt::EditRole);
            d->m_maskedRoles.insert(Qt::DisplayRole);
        }
        d->clearUnusedMaskedRoles(d->m_maskedRoles);
        maskedRolesChanged();
        d->signalAllChanged(changedRoles);
    }
}

void RoleMaskProxyModel::clearMaskedRoles()
{
    Q_D(RoleMaskProxyModel);
    if (!d->m_maskedRoles.isEmpty()){
        QVector<int> changedRoles;
        changedRoles.reserve(d->m_maskedRoles.size()+1);
        const bool hasEdit = d->m_maskedRoles.contains(Qt::EditRole);
        const bool hasDisplay = d->m_maskedRoles.contains(Qt::DisplayRole);
        for (auto i = d->m_maskedRoles.cbegin(); i != d->m_maskedRoles.cend();++i)
            changedRoles.append(*i);
        if (hasEdit != hasDisplay)
            changedRoles.append(hasEdit ? Qt::DisplayRole : Qt::EditRole);
        d->m_maskedRoles.clear();
        d->clearUnusedMaskedRoles(d->m_maskedRoles);
        maskedRolesChanged();
        d->signalAllChanged(changedRoles);
    }
}

void RoleMaskProxyModel::addMaskedRole(int role)
{
    Q_D(RoleMaskProxyModel);
    int adjRole = role;
    if (d->m_mergeDisplayEdit && role == Qt::EditRole)
        adjRole = Qt::DisplayRole;
    if (!d->m_maskedRoles.contains(adjRole)) {
        d->m_maskedRoles.insert(adjRole);
        maskedRolesChanged();
        if (d->m_mergeDisplayEdit && adjRole == Qt::DisplayRole)
            d->signalAllChanged(QVector<int>{{Qt::EditRole, Qt::DisplayRole}});
        else
            d->signalAllChanged(QVector<int>(1,adjRole));
    }
}

void RoleMaskProxyModel::removeMaskedRole(int role)
{
    Q_D(RoleMaskProxyModel);
    int adjRole = role;
    if (d->m_mergeDisplayEdit && role == Qt::EditRole)
        adjRole = Qt::DisplayRole;
    if (d->m_maskedRoles.remove(adjRole)) {
        d->clearUnusedMaskedRoles(d->m_maskedRoles);
        maskedRolesChanged();
        if (d->m_mergeDisplayEdit && adjRole == Qt::DisplayRole)
            d->signalAllChanged(QVector<int>{{Qt::EditRole, Qt::DisplayRole}});
        else
            d->signalAllChanged(QVector<int>(1,adjRole));
    }
}

/*!
\reimp
*/
void RoleMaskProxyModel::setSourceModel(QAbstractItemModel *sourceMdl)
{
    Q_D(RoleMaskProxyModel);
    if (sourceMdl == sourceModel())
        return; 
    d->m_maskedData.clear();
    if (sourceModel())
        Q_ASSUME(QObject::disconnect(d->m_sourceDataChangedConnection));
    QIdentityProxyModel::setSourceModel(sourceMdl);
    if (sourceModel()) {
        Q_ASSUME(sourceModel()->disconnect(SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this));
        d->m_sourceDataChangedConnection = connect(sourceModel(), &QAbstractItemModel::dataChanged, [d](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)->void{d->interceptDataChanged(topLeft,bottomRight,roles);});
    }
}

/*!
\reimp
*/
QVariant RoleMaskProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    Q_D(const RoleMaskProxyModel);
    int adjRole = role;
    if (d->m_mergeDisplayEdit && role == Qt::EditRole)
        adjRole = Qt::DisplayRole;
    if (!d->m_maskedRoles.contains(adjRole))
        return QIdentityProxyModel::data(proxyIndex, role);
    const QModelIndex sourceIndex = mapToSource(proxyIndex);
    const auto idxIter = d->m_maskedData.constFind(sourceIndex);
    if (idxIter != d->m_maskedData.constEnd()){
        const auto roleIter = idxIter->constFind(adjRole);
        if (roleIter != idxIter->constEnd())
            return roleIter.value();
    }
    if(d->m_transparentIfEmpty)
        return QIdentityProxyModel::data(proxyIndex, role);
    return QVariant();
}

/*!
\reimp
*/
bool RoleMaskProxyModel::setData(const QModelIndex &proxyIndex, const QVariant &value, int role)
{
    Q_D(RoleMaskProxyModel);
    if (!proxyIndex.isValid())
        return false;
    if (d->m_mergeDisplayEdit && role == Qt::EditRole)
        role = Qt::DisplayRole;
    const QVector<int> changedRolesVector = ((d->m_mergeDisplayEdit && role == Qt::DisplayRole) ? QVector<int>{ { Qt::EditRole, Qt::DisplayRole } } : QVector<int>(1, role));
    if (!d->m_maskedRoles.contains(role))
        return QIdentityProxyModel::setData(proxyIndex, value, role);
    const QModelIndex sourceIndex = mapToSource(proxyIndex);
    Q_ASSERT(sourceIndex.isValid());
    auto idxIter = d->m_maskedData.find(sourceIndex);
    if (idxIter == d->m_maskedData.end()){
        if (!value.isValid()) {
            return true;
        }
        else {
            idxIter = d->m_maskedData.insert(sourceIndex, RolesContainer());
            idxIter->insert(role, value);
            dataChanged(proxyIndex, proxyIndex, changedRolesVector);
            return true;
        }
    }
    if (!value.isValid()){
        if (d->removeRole(idxIter, role))
            dataChanged(proxyIndex, proxyIndex, changedRolesVector);
        return true;
    }
    const auto roleIter = idxIter->find(role);
    if (roleIter == idxIter->end()) 
        idxIter->insert(role, value);
    else if (roleIter.value() != value)
        roleIter.value() = value;
    else
        return true;
    dataChanged(proxyIndex, proxyIndex, changedRolesVector);
    return true;
}

bool RoleMaskProxyModel::transparentIfEmpty() const
{
    Q_D(const RoleMaskProxyModel);
    return d->m_transparentIfEmpty;
}

void RoleMaskProxyModel::setTransparentIfEmpty(bool val)
{
    Q_D(RoleMaskProxyModel);
    if (d->m_transparentIfEmpty != val) {
        d->m_transparentIfEmpty = val;
        transparentIfEmptyChanged(d->m_transparentIfEmpty);
        d->signalAllChanged();
    }
}

bool RoleMaskProxyModel::mergeDisplayEdit() const
{
    Q_D(const RoleMaskProxyModel);
    return d->m_mergeDisplayEdit;
}

void RoleMaskProxyModel::setMergeDisplayEdit(bool val)
{
    Q_D(RoleMaskProxyModel);
    if (d->m_mergeDisplayEdit!=val) {
        QVector<int> changedRoles({ Qt::DisplayRole, Qt::EditRole });
        d->m_mergeDisplayEdit=val;
        const auto idxEnd = d->m_maskedData.end();
        for (auto idxIter = d->m_maskedData.begin(); idxIter != idxEnd; ++idxIter) {
            if (val){
                if (idxIter->contains(Qt::DisplayRole)) {
                    idxIter->remove(Qt::EditRole);
                }
                else {
                    const auto roleIter = idxIter->constFind(Qt::EditRole);
                    if (roleIter != idxIter->constEnd()) {
                        idxIter->insert(Qt::DisplayRole, *roleIter);
                        idxIter->remove(Qt::EditRole);
                    }
                }
            }
            else{
                const auto roleIter = idxIter->constFind(Qt::DisplayRole);
                if (roleIter != idxIter->constEnd()) {
                    idxIter->insert(Qt::EditRole, *roleIter);
                }
            }
        }
        if (val)
            d->m_maskedRoles.remove(Qt::EditRole);
        else if (d->m_maskedRoles.contains(Qt::DisplayRole))
            d->m_maskedRoles.insert(Qt::EditRole);
        mergeDisplayEditChanged(d->m_mergeDisplayEdit);
        d->signalAllChanged(changedRoles);
    }
}

const QSet<int>& RoleMaskProxyModel::maskedRolesSets() const
{
    Q_D(const RoleMaskProxyModel);
    return d->m_maskedRoles;
}


/*!
\class RoleMaskProxyModel
\brief This proxy will act as a mask on top of the source model to intercept data.
\details This proxy model will mask 

*/
