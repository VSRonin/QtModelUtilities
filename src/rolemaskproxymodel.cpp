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
#include "rolemaskproxymodel.h"
#include "private/rolemaskproxymodel_p.h"
#include <QVector>

/*!
\internal
*/
RoleMaskProxyModelPrivate::RoleMaskProxyModelPrivate(RoleMaskProxyModel* q)
    :q_ptr(q)
    , m_transparentIfEmpty(true)
    , m_mergeDisplayEdit(true)
{
    Q_ASSERT(q_ptr);
}

/*!
\internal
*/
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
        if (m_mergeDisplayEdit && singleRole == Qt::EditRole) {
            if (filteredRoles.contains(Qt::DisplayRole))
                continue;
            singleRole = Qt::DisplayRole;
        }
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

/*!
\internal
*/
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

/*!
\internal
*/
bool RoleMaskProxyModelPrivate::removeRole(const QPersistentModelIndex& idx, int role)
{
    const auto idxIter = m_maskedData.find(idx);
    return removeRole(idxIter, role);
}

/*!
\internal
*/
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


/*!
\internal
*/
void RoleMaskProxyModelPrivate::signalAllChanged(const QVector<int>& roles, const QModelIndex& parent)
{
    Q_Q(RoleMaskProxyModel);
    if (!q->sourceModel())
        return;
    const int rowCnt = q->rowCount(parent);
    const int colCnt = q->columnCount(parent);
    for (int i = 0; i < rowCnt;++i){
        for (int j = 0; j < colCnt; ++j) {
            const QModelIndex currPar = q->index(i, j, parent);
            if (q->hasChildren(currPar))
                signalAllChanged(roles, currPar);
        }
    }
    q->dataChanged(q->index(0, 0, parent), q->index(rowCnt - 1, colCnt - 1, parent), roles);
}

/*!
Constructs a new proxy model with the given \a parent.
*/
RoleMaskProxyModel::RoleMaskProxyModel(QObject* parent)
    : QIdentityProxyModel(parent)
    , d_ptr(new RoleMaskProxyModelPrivate(this))
{}

/*!
Constructor used only while subclassing the private class.
Not part of the public API
*/
RoleMaskProxyModel::RoleMaskProxyModel(RoleMaskProxyModelPrivate& dptr, QObject* parent)
    : QIdentityProxyModel(parent)
    , d_ptr(&dptr)
{}

/*!
Destructor
*/
RoleMaskProxyModel::~RoleMaskProxyModel()
{
    Q_D(RoleMaskProxyModel);
    for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
        QObject::disconnect(*discIter);
    delete d_ptr;
}

QList<int> RoleMaskProxyModel::maskedRoles() const
{
    Q_D(const RoleMaskProxyModel);
    #if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    return d->m_maskedRoles.toList();
    #else
    return QList<int>(d->m_maskedRoles.begin(), d->m_maskedRoles.end());
    #endif
}

void RoleMaskProxyModel::setMaskedRoles(const QList<int>& newRoles)
{
    Q_D(RoleMaskProxyModel);
    #if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    const QSet<int> roles = newRoles.toSet();
    #else
    const QSet<int> roles(newRoles.begin(), newRoles.end());
    #endif
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
    if (sourceMdl == sourceModel())
        return; 
    Q_D(RoleMaskProxyModel);
    for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
        QObject::disconnect(*discIter);
    d->m_sourceConnections.clear();
    d->m_maskedData.clear();
    QIdentityProxyModel::setSourceModel(sourceMdl);
    if (sourceModel()) {
        Q_ASSUME(sourceModel()->disconnect(SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this));
        d->m_sourceConnections 
            << QObject::connect(sourceModel(), &QAbstractItemModel::dataChanged, [d](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)->void {d->interceptDataChanged(topLeft, bottomRight, roles); })
            << QObject::connect(sourceModel(), &QAbstractItemModel::modelReset, [d]()->void { d->m_maskedData.clear(); })
            << QObject::connect(sourceModel(), &QAbstractItemModel::destroyed, [this]()->void { setSourceModel(Q_NULLPTR); })
        ;
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
            maskedDataChanged(proxyIndex,proxyIndex,changedRolesVector);
            dataChanged(proxyIndex, proxyIndex, changedRolesVector);
            return true;
        }
    }
    if (!value.isValid()){
        if (d->removeRole(idxIter, role)){
            maskedDataChanged(proxyIndex,proxyIndex,changedRolesVector);
            dataChanged(proxyIndex, proxyIndex, changedRolesVector);
        }
        return true;
    }
    const auto roleIter = idxIter->find(role);
    if (roleIter == idxIter->end()) 
        idxIter->insert(role, value);
    else if (roleIter.value() != value)
        roleIter.value() = value;
    else
        return true;
    maskedDataChanged(proxyIndex,proxyIndex,changedRolesVector);
    dataChanged(proxyIndex, proxyIndex, changedRolesVector);
    return true;
}

/*!
\reimp
\details Due to limitations in the architecture, the model might emit 2 separate dataChanged signals, one for the roles that were masked and one for the roles that are managed by the sorce model
*/
bool RoleMaskProxyModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    Q_D(RoleMaskProxyModel);
    QMap<int, QVariant> adjustedRoles = roles;
    if (d->m_mergeDisplayEdit) {
        const auto editIter = adjustedRoles.find(Qt::EditRole);
        const auto displayIter = adjustedRoles.constFind(Qt::DisplayRole);
        if (editIter != adjustedRoles.end()) {
            if (displayIter == adjustedRoles.cend()) {
                adjustedRoles.insert(Qt::DisplayRole, editIter.value());
            }
            adjustedRoles.erase(editIter);
        }
    }
    const QModelIndex sourceIndex = mapToSource(index);
    Q_ASSERT(sourceIndex.isValid());
    QMap<int, QVariant> rolesForSource;
    QVector<int> changedRoles;
    changedRoles.reserve(adjustedRoles.size());
    for (auto i = adjustedRoles.cbegin(), rolesEnd = adjustedRoles.cend(); i != rolesEnd; ++i) {
        if (!d->m_maskedRoles.contains(i.key())) {
            rolesForSource.insert(i.key(), i.value());
            continue;
        }
        auto idxIter = d->m_maskedData.find(sourceIndex);
        if (idxIter == d->m_maskedData.end()) {
            if (i->isValid()) {
                idxIter = d->m_maskedData.insert(sourceIndex, RolesContainer());
                idxIter->insert(i.key(), i.value());
                changedRoles << i.key();
            }
            continue;
        }
        if (!i->isValid()) {
            if (d->removeRole(idxIter, i.key()))
                changedRoles << i.key();
            continue;
        }
        const auto roleIter = idxIter->find(i.key());
        if (roleIter == idxIter->end())
            idxIter->insert(i.key(), i.value());
        else if (roleIter.value() != i.value())
            roleIter.value() = i.value();
        else
            continue;
        changedRoles << i.key();
    }
    if (d->m_mergeDisplayEdit && changedRoles.contains(Qt::DisplayRole))
        changedRoles << Qt::EditRole;
    if (!changedRoles.isEmpty()){
        maskedDataChanged(index,index,changedRoles);
        dataChanged(index, index, changedRoles);
    }
    if (!rolesForSource.isEmpty())
        return QIdentityProxyModel::setItemData(index, rolesForSource);
    return true;
}

/*!
\reimp
*/
QMap<int, QVariant> RoleMaskProxyModel::itemData(const QModelIndex &index) const 
{
    Q_D(const RoleMaskProxyModel);
    RolesContainer result;
    const auto maskedIter = d->m_maskedData.constFind(index);
    if (maskedIter != d->m_maskedData.cend()) {
        result = maskedIter.value();
        const auto displayIter = result.constFind(Qt::DisplayRole);
        if (d->m_mergeDisplayEdit && displayIter != result.cend())
            result.insert(Qt::EditRole, displayIter.value());
    }
    if(!d->m_transparentIfEmpty){
        const QMap<int, QVariant> baseData = QIdentityProxyModel::itemData(index);
        for (auto i = baseData.cbegin(), baseEnd = baseData.cend(); i != baseEnd;++i){
            if (!result.contains(i.key()))
                result.insert(i.key(), i.value());
        }
    }
    return convertFromContainer<QMap<int, QVariant> >(result);
}

QMap<int, QVariant> RoleMaskProxyModel::maskedItemData(const QModelIndex &index) const
{
    Q_D(const RoleMaskProxyModel);
    const auto maskedIter = d->m_maskedData.find(index);
    if(maskedIter == d->m_maskedData.end())
        return QMap<int, QVariant>();
    return convertFromContainer<QMap<int, QVariant> >(maskedIter.value());
}

void RoleMaskProxyModel::clearMaskedData(const QModelIndex &index)
{
    Q_D(RoleMaskProxyModel);
    const auto maskedIter = d->m_maskedData.find(index);
    if(maskedIter == d->m_maskedData.end())
        return;
    Q_ASSERT(!d->m_maskedData.isEmpty());
    const QList<int> changedRolesList = maskedIter->keys();
    const QVector<int> changedRoles = changedRolesList.toVector();
    d->m_maskedData.erase(maskedIter);
    maskedDataChanged(index,index,changedRoles);
    dataChanged(index,index,changedRoles);
}

/*!
\property RoleMaskProxyModel::transparentIfEmpty
\accessors %transparentIfEmpty(), setTransparentIfEmpty()
\brief This property determines if a mapped role containing no data should be transparent
\details If this property is set to true, a 

*/
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

/*!
\property RoleMaskProxyModel::mergeDisplayEdit
\accessors %mergeDisplayEdit(), setMergeDisplayEdit()
\brief This property determines if the Qt::DisplayRole and Qt::EditRole should be merged in the extra row/column
\details By default the two roles are one and the same you can use this property to separate them.
If there's any data in the role when you set this property to true it will be duplicated for both roles.
If there is data both in Qt::DisplayRole and Qt::EditRole when you set this property to false Qt::DisplayRole will prevail.
This property only has effect if Qt::DisplayRole and/or Qt::EditRole are masked by the proxy. Data in the source model is not affected.
*/
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

/*!
Same as maskedRoles but returns it in the way it is stored internally rather than converting it to QList
*/
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
