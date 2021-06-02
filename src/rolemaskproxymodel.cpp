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

MaskedItem::MaskedItem(const RolesContainer &data, const QModelIndex &index)
    : m_data(data)
    , m_index(index)
{ }

MaskedItem::MaskedItem(const RolesContainer &data, const QPersistentModelIndex &index)
    : m_data(data)
    , m_index(index)
{ }

RoleMaskProxyModelPrivate::RoleMaskProxyModelPrivate(RoleMaskProxyModel *q)
    : q_ptr(q)
    , m_transparentIfEmpty(true)
    , m_mergeDisplayEdit(true)
    , m_maskHeaderData(false)
{
    Q_ASSERT(q_ptr);
}

void RoleMaskProxyModelPrivate::interceptDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
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
    for (int singleRole : roles) {
        if (m_mergeDisplayEdit && singleRole == Qt::EditRole) {
            if (filteredRoles.contains(Qt::DisplayRole))
                continue;
            singleRole = Qt::DisplayRole;
        }
        if (m_maskedRoles.contains(singleRole)) {
            if (m_transparentIfEmpty) {
                bool allOpaque = true;
                for (int i = topLeft.row(); allOpaque && i <= bottomRight.row(); ++i) {
                    for (int j = topLeft.column(); allOpaque && j <= bottomRight.column(); ++j) {
                        const RolesContainer *data = dataForIndex(q->sourceModel()->index(i, j, topLeft.parent()));
                        if (data)
                            allOpaque = data->value(singleRole).isValid();
                        else
                            allOpaque = false;
                    }
                }
                if (allOpaque)
                    continue;
            } else {
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

const RolesContainer *RoleMaskProxyModelPrivate::dataForIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return nullptr;
    Q_ASSERT(index.model() == q_func()->sourceModel());
    const QPair<int, int> hashKey = qMakePair(index.row(), index.column());
    for (auto i = m_masked.constFind(hashKey), iEnd = m_masked.constEnd(); i != iEnd && i.key() == hashKey; ++i) {
        if (i->m_index == index)
            return &(i->m_data);
    }
    return nullptr;
}

RolesContainer *RoleMaskProxyModelPrivate::dataForIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return nullptr;
    Q_ASSERT(index.model() == q_func()->sourceModel());
    const QPair<int, int> hashKey = qMakePair(index.row(), index.column());
    for (auto i = m_masked.find(hashKey), iEnd = m_masked.end(); i != iEnd && i.key() == hashKey; ++i) {
        if (i->m_index == index)
            return &(i->m_data);
    }
    return nullptr;
}

void RoleMaskProxyModelPrivate::insertData(const QModelIndex &index, const RolesContainer &data)
{
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.model() == q_func()->sourceModel());
    const QPair<int, int> hashKey = qMakePair(index.row(), index.column());
    m_masked.insert(hashKey, MaskedItem(data, index));
}

void RoleMaskProxyModelPrivate::onRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    const int count = end - start + 1;
    Q_ASSERT(!parent.isValid() || parent.model() == q_func()->sourceModel());
    decltype(m_masked) maskedToAdd;
    for (auto i = m_masked.begin(), iEnd = m_masked.end(); i != iEnd;) {
        if (i->m_index.parent() == parent) {
            if (i.key().first >= start) {
                maskedToAdd.insert(qMakePair(i.key().first + count, i.key().second), i.value());
                i = m_masked.erase(i);
                continue;
            }
        }
        ++i;
    }
    for (auto i = maskedToAdd.begin(), iEnd = maskedToAdd.end(); i != iEnd; ++i)
        m_masked.insert(i.key(), i.value());
}

void RoleMaskProxyModelPrivate::onColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    const int count = end - start + 1;
    Q_ASSERT(!parent.isValid() || parent.model() == q_func()->sourceModel());
    decltype(m_masked) maskedToAdd;
    for (auto i = m_masked.begin(), iEnd = m_masked.end(); i != iEnd;) {
        if (i->m_index.parent() == parent) {
            if (i.key().second >= start) {
                maskedToAdd.insert(qMakePair(i.key().first, i.key().second + count), i.value());
                i = m_masked.erase(i);
                continue;
            }
        }
        ++i;
    }
    for (auto i = maskedToAdd.begin(), iEnd = maskedToAdd.end(); i != iEnd; ++i)
        m_masked.insert(i.key(), i.value());
}

void RoleMaskProxyModelPrivate::onRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    const int count = end - start + 1;
    Q_ASSERT(!parent.isValid() || parent.model() == q_func()->sourceModel());
    decltype(m_masked) maskedToAdd;
    for (auto i = m_masked.begin(), iEnd = m_masked.end(); i != iEnd;) {
        if (i->m_index.parent() == parent) {
            if (i.key().first >= start) {
                if (i.key().first > end)
                    maskedToAdd.insert(qMakePair(i.key().first - count, i.key().second), i.value());
                i = m_masked.erase(i);
                continue;
            }
        }
        ++i;
    }
    for (auto i = maskedToAdd.begin(), iEnd = maskedToAdd.end(); i != iEnd; ++i)
        m_masked.insert(i.key(), i.value());
}

void RoleMaskProxyModelPrivate::onColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    const int count = end - start + 1;
    Q_ASSERT(!parent.isValid() || parent.model() == q_func()->sourceModel());
    decltype(m_masked) maskedToAdd;
    for (auto i = m_masked.begin(), iEnd = m_masked.end(); i != iEnd;) {
        if (i->m_index.parent() == parent) {
            if (i.key().second >= start) {
                if (i.key().second > end)
                    maskedToAdd.insert(qMakePair(i.key().first, i.key().second - count), i.value());
                i = m_masked.erase(i);
                continue;
            }
        }
        ++i;
    }
    for (auto i = maskedToAdd.begin(), iEnd = maskedToAdd.end(); i != iEnd; ++i)
        m_masked.insert(i.key(), i.value());
}

void RoleMaskProxyModelPrivate::onLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
    Q_UNUSED(parents)
    Q_UNUSED(hint)
    decltype(m_masked) updatedMasked;
    for (auto i = m_masked.constBegin(), iMax = m_masked.constEnd(); i != iMax; ++i) {
        const auto newKey = qMakePair(i->m_index.row(), i->m_index.column());
        updatedMasked.insert(newKey, i.value());
    }
    m_masked = std::move(updatedMasked);
}

void RoleMaskProxyModelPrivate::onRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                                                     const QModelIndex &destinationParent, int destinationRow)
{
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == q_func()->sourceModel());
    Q_ASSERT(!destinationParent.isValid() || destinationParent.model() == q_func()->sourceModel());
    decltype(m_masked) maskedToAdd;
    const int count = sourceEnd - sourceStart + 1;
    if (sourceParent != destinationParent) {
        for (auto i = m_masked.begin(), iEnd = m_masked.end(); i != iEnd;) {
            if (i->m_index.parent() == sourceParent) {
                if (i.key().first >= sourceStart && i.key().first <= sourceEnd) {
                    maskedToAdd.insert(qMakePair(destinationRow + i.key().first - sourceStart, i.key().second), i.value());
                    i = m_masked.erase(i);
                    continue;
                }
                if (i.key().first > sourceEnd) {
                    maskedToAdd.insert(qMakePair(i.key().first - count, i.key().second), i.value());
                    i = m_masked.erase(i);
                    continue;
                }
            } else if (i->m_index.parent() == destinationParent) {
                if (i.key().first >= destinationRow) {
                    maskedToAdd.insert(qMakePair(i.key().first + count, i.key().second), i.value());
                    i = m_masked.erase(i);
                    continue;
                }
            }
            ++i;
        }
    } else {
        for (auto i = m_masked.begin(), iEnd = m_masked.end(); i != iEnd;) {
            if (i->m_index.parent() == sourceParent) {
                if (destinationRow < sourceStart) {
                    if (i.key().first >= destinationRow && i.key().first < sourceStart) {
                        maskedToAdd.insert(qMakePair(i.key().first + count, i.key().second), i.value());
                        i = m_masked.erase(i);
                        continue;
                    } else if (i.key().first >= sourceStart && i.key().first <= sourceEnd) {
                        maskedToAdd.insert(qMakePair(i.key().first - (sourceStart - destinationRow), i.key().second), i.value());
                        i = m_masked.erase(i);
                        continue;
                    }
                } else {
                    Q_ASSERT(destinationRow > sourceEnd);
                    if (i.key().first > sourceEnd && i.key().first < destinationRow) {
                        maskedToAdd.insert(qMakePair(i.key().first - count, i.key().second), i.value());
                        i = m_masked.erase(i);
                        continue;
                    } else if (i.key().first >= sourceStart && i.key().first <= sourceEnd) {
                        maskedToAdd.insert(qMakePair(i.key().first + (destinationRow - sourceEnd - 1), i.key().second), i.value());
                        i = m_masked.erase(i);
                        continue;
                    }
                }
            }
            ++i;
        }
    }
    for (auto i = maskedToAdd.begin(), iEnd = maskedToAdd.end(); i != iEnd; ++i)
        m_masked.insert(i.key(), i.value());
}

void RoleMaskProxyModelPrivate::onColumnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                                                        const QModelIndex &destinationParent, int destinationColumn)
{
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == q_func()->sourceModel());
    Q_ASSERT(!destinationParent.isValid() || destinationParent.model() == q_func()->sourceModel());
    decltype(m_masked) maskedToAdd;
    const int count = sourceEnd - sourceStart + 1;
    if (sourceParent != destinationParent) {
        for (auto i = m_masked.begin(), iEnd = m_masked.end(); i != iEnd;) {
            if (i->m_index.parent() == sourceParent) {
                if (i.key().second >= sourceStart && i.key().second <= sourceEnd) {
                    maskedToAdd.insert(qMakePair(i.key().first, destinationColumn + i.key().second - sourceStart), i.value());
                    i = m_masked.erase(i);
                    continue;
                }
                if (i.key().second > sourceEnd) {
                    maskedToAdd.insert(qMakePair(i.key().first, i.key().second - count), i.value());
                    i = m_masked.erase(i);
                    continue;
                }
            } else if (i->m_index.parent() == destinationParent) {
                if (i.key().second >= destinationColumn) {
                    maskedToAdd.insert(qMakePair(i.key().first, i.key().second + count), i.value());
                    i = m_masked.erase(i);
                    continue;
                }
            }
            ++i;
        }
    } else {
        for (auto i = m_masked.begin(), iEnd = m_masked.end(); i != iEnd;) {
            if (i->m_index.parent() == sourceParent) {
                if (destinationColumn < sourceStart) {
                    if (i.key().second >= destinationColumn && i.key().second < sourceStart) {
                        maskedToAdd.insert(qMakePair(i.key().first, i.key().second + count), i.value());
                        i = m_masked.erase(i);
                        continue;
                    } else if (i.key().second >= sourceStart && i.key().second <= sourceEnd) {
                        maskedToAdd.insert(qMakePair(i.key().first, i.key().second - (sourceStart - destinationColumn)), i.value());
                        i = m_masked.erase(i);
                        continue;
                    }
                } else {
                    Q_ASSERT(destinationColumn > sourceEnd);
                    if (i.key().second > sourceEnd && i.key().second < destinationColumn) {
                        maskedToAdd.insert(qMakePair(i.key().first, i.key().second - count), i.value());
                        i = m_masked.erase(i);
                        continue;
                    } else if (i.key().second >= sourceStart && i.key().second <= sourceEnd) {
                        maskedToAdd.insert(qMakePair(i.key().first, i.key().second + (destinationColumn - sourceEnd - 1)), i.value());
                        i = m_masked.erase(i);
                        continue;
                    }
                }
            }
            ++i;
        }
    }
    for (auto i = maskedToAdd.begin(), iEnd = maskedToAdd.end(); i != iEnd; ++i)
        m_masked.insert(i.key(), i.value());
}

void RoleMaskProxyModelPrivate::onRowsInserted(const QModelIndex &parent, int start, int end)
{
    if(m_maskHeaderData && !parent.isValid())
        m_vHeaderData.insert(start,end-start+1,RolesContainer());
}

void RoleMaskProxyModelPrivate::onColumnsInserted(const QModelIndex &parent, int start, int end)
{
    if(m_maskHeaderData && !parent.isValid())
        m_hHeaderData.insert(start,end-start+1,RolesContainer());
}

void RoleMaskProxyModelPrivate::onRowsRemoved(const QModelIndex &parent, int start, int end)
{
    if(m_maskHeaderData && !parent.isValid())
        m_vHeaderData.remove(start,end-start+1);
}

void RoleMaskProxyModelPrivate::onColumnsRemoved(const QModelIndex &parent, int start, int end)
{
    if(m_maskHeaderData && !parent.isValid())
        m_hHeaderData.remove(start,end-start+1);
}

void RoleMaskProxyModelPrivate::onRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    if(!m_maskHeaderData)
        return;
    if(sourceParent!=destinationParent){
        if(!sourceParent.isValid())
            m_vHeaderData.remove(sourceStart,sourceEnd-sourceStart+1);
        if(!destinationParent.isValid())
            m_vHeaderData.insert(destinationRow,sourceEnd-sourceStart+1,RolesContainer());
        return;
    }
    const auto sourceiBegin = m_vHeaderData.begin() + sourceStart;
    const auto sourceiEnd = m_vHeaderData.begin() + sourceEnd + 1;
    const auto destination = m_vHeaderData.begin() + destinationRow;
    if (destinationRow < sourceStart)
        std::rotate(destination, sourceiBegin, sourceiEnd);
    else
        std::rotate(sourceiBegin, sourceiEnd, destination);
}

void RoleMaskProxyModelPrivate::onColumnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn)
{
    if(!m_maskHeaderData)
        return;
    if(sourceParent!=destinationParent){
        if(!sourceParent.isValid())
            m_hHeaderData.remove(sourceStart,sourceEnd-sourceStart+1);
        if(!destinationParent.isValid())
            m_hHeaderData.insert(destinationColumn,sourceEnd-sourceStart+1,RolesContainer());
        return;
    }
    const auto sourceiBegin = m_hHeaderData.begin() + sourceStart;
    const auto sourceiEnd = m_hHeaderData.begin() + sourceEnd + 1;
    const auto destination = m_hHeaderData.begin() + destinationColumn;
    if (destinationColumn < sourceStart)
        std::rotate(destination, sourceiBegin, sourceiEnd);
    else
        std::rotate(sourceiBegin, sourceiEnd, destination);
}

void RoleMaskProxyModelPrivate::clearUnusedMaskedRoles(const QSet<int> &newRoles)
{
    if (newRoles.isEmpty()) {
        m_masked.clear();
        return;
    }
    const auto idxEnd = m_masked.end();
    for (auto idxIter = m_masked.begin(); idxIter != idxEnd;) {
        const auto roleEnd = idxIter->m_data.end();
        for (auto roleIter = idxIter->m_data.begin(); roleIter != roleEnd;) {
            if (!newRoles.contains(roleIter.key()))
                roleIter = idxIter->m_data.erase(roleIter);
            else
                ++roleIter;
        }
        if (idxIter->m_data.isEmpty())
            idxIter = m_masked.erase(idxIter);
        else
            ++idxIter;
    }
}

bool RoleMaskProxyModelPrivate::removeRole(const QModelIndex &idx, int role)
{
    if (!idx.isValid())
        return false;
    Q_ASSERT(idx.model() == q_func()->sourceModel());
    const QPair<int, int> hashKey = qMakePair(idx.row(), idx.column());
    for (auto i = m_masked.find(hashKey), iEnd = m_masked.end(); i != iEnd && i.key() == hashKey; ++i) {
        if (i->m_index == idx) {
            if (i->m_data.remove(role) > 0) {
                if (i->m_data.isEmpty())
                    m_masked.erase(i);
                return true;
            }
        }
    }
    return false;
}

bool RoleMaskProxyModelPrivate::removeIndex(const QModelIndex &idx)
{
    if (!idx.isValid())
        return false;
    Q_ASSERT(idx.model() == q_func()->sourceModel());
    const QPair<int, int> hashKey = qMakePair(idx.row(), idx.column());
    for (auto i = m_masked.find(hashKey), iEnd = m_masked.end(); i != iEnd && i.key() == hashKey; ++i) {
        if (i->m_index == idx) {
            m_masked.erase(i);
            return true;
        }
    }
    return false;
}

void RoleMaskProxyModelPrivate::signalAllChanged(const QVector<int> &roles, const QModelIndex &parent)
{
    Q_Q(RoleMaskProxyModel);
    if (!q->sourceModel())
        return;
    const int rowCnt = q->rowCount(parent);
    const int colCnt = q->columnCount(parent);
    for (int i = 0; i < rowCnt; ++i) {
        for (int j = 0; j < colCnt; ++j) {
            const QModelIndex currPar = q->index(i, j, parent);
            if (q->hasChildren(currPar))
                signalAllChanged(roles, currPar);
        }
    }
    if(rowCnt>0 && colCnt>0)
        q->dataChanged(q->index(0, 0, parent), q->index(rowCnt - 1, colCnt - 1, parent), roles);
    if(m_maskHeaderData && !parent.isValid()){
        if(colCnt>0)
            q->headerDataChanged(Qt::Horizontal,0,colCnt-1);
        if(rowCnt>0)
            q->headerDataChanged(Qt::Vertical,0,rowCnt-1);
    }
}

/*!
Constructs a new proxy model with the given \a parent.
*/
RoleMaskProxyModel::RoleMaskProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
    , d_ptr(new RoleMaskProxyModelPrivate(this))
{ }

/*!
Constructor used only while subclassing the private class.
Not part of the public API
*/
RoleMaskProxyModel::RoleMaskProxyModel(RoleMaskProxyModelPrivate &dptr, QObject *parent)
    : QIdentityProxyModel(parent)
    , d_ptr(&dptr)
{ }

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

/*!
\property RoleMaskProxyModel::maskedRoles
\accessors %maskedRoles()
\brief Returns the list of roles managed by the proxy model
*/
QList<int> RoleMaskProxyModel::maskedRoles() const
{
    Q_D(const RoleMaskProxyModel);
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    return d->m_maskedRoles.toList();
#else
    return QList<int>(d->m_maskedRoles.begin(), d->m_maskedRoles.end());
#endif
}

/*!
Set the list of roles managed by the proxy model
*/
void RoleMaskProxyModel::setMaskedRoles(const QList<int> &newRoles)
{
    Q_D(RoleMaskProxyModel);
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    const QSet<int> roles = newRoles.toSet();
#else
    const QSet<int> roles(newRoles.begin(), newRoles.end());
#endif
    if (d->m_maskedRoles != roles) {
        QVector<int> changedRoles;
        QSet<int> changedRolesSet = roles;
        changedRolesSet.unite(d->m_maskedRoles);
        if (d->m_mergeDisplayEdit && (changedRolesSet.contains(Qt::EditRole) || changedRolesSet.contains(Qt::DisplayRole)))
            changedRolesSet << Qt::EditRole << Qt::DisplayRole;
        changedRoles.reserve(changedRolesSet.size());
        for (auto i = changedRolesSet.cbegin(); i != changedRolesSet.cend(); ++i)
            changedRoles.append(*i);
        d->m_maskedRoles = roles;
        if (d->m_mergeDisplayEdit && d->m_maskedRoles.contains(Qt::EditRole)) {
            d->m_maskedRoles.remove(Qt::EditRole);
            d->m_maskedRoles.insert(Qt::DisplayRole);
        }
        d->clearUnusedMaskedRoles(d->m_maskedRoles);
        maskedRolesChanged();
        d->signalAllChanged(changedRoles);
    }
}

/*!
Removes all roles managed by the proxy model
*/
void RoleMaskProxyModel::clearMaskedRoles()
{
    Q_D(RoleMaskProxyModel);
    if (!d->m_maskedRoles.isEmpty()) {
        QVector<int> changedRoles;
        changedRoles.reserve(d->m_maskedRoles.size() + 1);
        const bool hasEdit = d->m_maskedRoles.contains(Qt::EditRole);
        const bool hasDisplay = d->m_maskedRoles.contains(Qt::DisplayRole);
        for (auto i = d->m_maskedRoles.cbegin(); i != d->m_maskedRoles.cend(); ++i)
            changedRoles.append(*i);
        if (hasEdit != hasDisplay)
            changedRoles.append(hasEdit ? Qt::DisplayRole : Qt::EditRole);
        d->m_maskedRoles.clear();
        d->clearUnusedMaskedRoles(d->m_maskedRoles);
        maskedRolesChanged();
        d->signalAllChanged(changedRoles);
    }
}

/*!
Adds \a role to the list of roles managed by the proxy model
*/
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
            d->signalAllChanged(QVector<int>(1, adjRole));
    }
}

/*!
Removes \a role from the list of roles managed by the proxy model
*/
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
            d->signalAllChanged(QVector<int>(1, adjRole));
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
    d->m_masked.clear();
    QIdentityProxyModel::setSourceModel(sourceMdl);
    if (sourceModel()) {
        Q_ASSUME(sourceModel()->disconnect(SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this));
        d->m_sourceConnections
                << QObject::connect(sourceModel(), &QAbstractItemModel::dataChanged,
                                    [d](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) -> void {
                                        d->interceptDataChanged(topLeft, bottomRight, roles);
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::modelReset, [d]() -> void { d->m_masked.clear(); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::destroyed, [this]() -> void { setSourceModel(Q_NULLPTR); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted,
                                    [d](const QModelIndex &parent, int start, int end) { d->onRowsAboutToBeInserted(parent, start, end); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeInserted,
                                    [d](const QModelIndex &parent, int start, int end) { d->onColumnsAboutToBeInserted(parent, start, end); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved,
                                    [d](const QModelIndex &parent, int start, int end) { d->onRowsAboutToBeRemoved(parent, start, end); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeRemoved,
                                    [d](const QModelIndex &parent, int start, int end) { d->onColumnsAboutToBeRemoved(parent, start, end); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsInserted,
                                    [d](const QModelIndex &parent, int start, int end) { d->onRowsInserted(parent, start, end); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsInserted,
                                    [d](const QModelIndex &parent, int start, int end) { d->onColumnsInserted(parent, start, end); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsRemoved,
                                    [d](const QModelIndex &parent, int start, int end) { d->onRowsRemoved(parent, start, end); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsRemoved,
                                    [d](const QModelIndex &parent, int start, int end) { d->onColumnsRemoved(parent, start, end); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::layoutChanged,
                                    [d](const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint) {
                                        d->onLayoutChanged(parents, hint);
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeMoved,
                                    [d](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent,
                                        int destinationRow) {
                                        d->onRowsAboutToBeMoved(sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow);
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeMoved,
                                    [d](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent,
                                        int destinationRow) {
                                        d->onColumnsAboutToBeMoved(sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow);
                                    })
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsMoved,
                            [d](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent,
                                int destinationRow) {
                                d->onRowsMoved(sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow);
                            })
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsMoved,
                            [d](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent,
                                int destinationRow) {
                                d->onColumnsMoved(sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow);
                            });
    }
}



#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
/*!
\reimp
*/
void RoleMaskProxyModel::multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const
{
    Q_ASSERT(sourceModel());
    Q_ASSERT(index.model() == this);
    Q_D(const RoleMaskProxyModel);
    const QModelIndex sourceIndex = mapToSource(index);
    sourceModel()->multiData(sourceIndex, roleDataSpan);
    const RolesContainer *idxData = d->dataForIndex(sourceIndex);
    for (QModelRoleData &roleData : roleDataSpan) {
        int role = roleData.role();
        if (d->m_mergeDisplayEdit && role == Qt::EditRole)
            role = Qt::DisplayRole;
        const auto roleIter = idxData->constFind(role);
        if (roleIter != idxData->constEnd())
            roleData.setData(roleIter.value());
        else if (!d->m_transparentIfEmpty && d->m_maskedRoles.contains(role))
            roleData.setData(QVariant());
    }
}
#endif

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
    const RolesContainer *idxData = d->dataForIndex(sourceIndex);
    if (idxData) {
        const auto roleIter = idxData->constFind(adjRole);
        if (roleIter != idxData->constEnd())
            return roleIter.value();
    }
    if (d->m_transparentIfEmpty)
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
    const QVector<int> changedRolesVector =
            ((d->m_mergeDisplayEdit && role == Qt::DisplayRole) ? QVector<int>{{Qt::EditRole, Qt::DisplayRole}} : QVector<int>(1, role));
    if (!d->m_maskedRoles.contains(role))
        return QIdentityProxyModel::setData(proxyIndex, value, role);
    const QModelIndex sourceIndex = mapToSource(proxyIndex);
    Q_ASSERT(sourceIndex.isValid());
    RolesContainer *idxData = d->dataForIndex(sourceIndex);
    if (!idxData) {
        if (value.isValid()){
            RolesContainer newData;
            newData.insert(role, value);
            d->insertData(sourceIndex, newData);
            maskedDataChanged(proxyIndex, proxyIndex, changedRolesVector);
            dataChanged(proxyIndex, proxyIndex, changedRolesVector);
        }
        return true;
    }
    if (!value.isValid()) {
        if (d->removeRole(sourceIndex, role)) {
            maskedDataChanged(proxyIndex, proxyIndex, changedRolesVector);
            dataChanged(proxyIndex, proxyIndex, changedRolesVector);
        }
        return true;
    }
    const auto roleIter = idxData->find(role);
    if (roleIter == idxData->end())
        idxData->insert(role, value);
    else if (roleIter.value() != value)
        roleIter.value() = value;
    else
        return true;
    maskedDataChanged(proxyIndex, proxyIndex, changedRolesVector);
    dataChanged(proxyIndex, proxyIndex, changedRolesVector);
    return true;
}

/*!
\reimp
*/
bool RoleMaskProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    Q_D(RoleMaskProxyModel);
    if(d->m_maskHeaderData && d->m_maskedRoles.contains(role)){
        QVector<RolesContainer>& dataContainerVect = orientation==Qt::Horizontal ? d->m_hHeaderData : d->m_vHeaderData;
        if(section<0 || section >= dataContainerVect.size())
            return false;
        RolesContainer& dataContainer=dataContainerVect[section];
        auto dataIter = dataContainer.find(role);
        if(dataIter==dataContainer.end()){
            if (value.isValid()){
                dataContainer.insert(role,value);
                headerDataChanged(orientation,section,section);
            }
            return true;
        }
        if(!value.isValid()){
            dataContainer.erase(dataIter);
            headerDataChanged(orientation,section,section);
        }
        else if(dataIter.value()!=value){
            dataIter.value()=value;
            headerDataChanged(orientation,section,section);
        }
        return true;
    }
    return QIdentityProxyModel::setHeaderData(section,orientation,value,role);
}

/*!
\reimp
\details This method will do nothing.

If you want to sort based on the marked data use a QSortFilterProxyModel
on top of this proxy.
*/
void RoleMaskProxyModel::sort(int column, Qt::SortOrder order)
{
    return;
}

/*!
Removes all the data managed by the proxy model for a certain \a index.
*/
void RoleMaskProxyModel::clearMaskedData(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    Q_ASSERT(index.model() == this);
    Q_D(RoleMaskProxyModel);
    const QModelIndex sourceIndex = mapToSource(index);
    const auto maskedIter = d->dataForIndex(sourceIndex);
    if (!maskedIter)
        return;
    Q_ASSERT(!maskedIter->isEmpty());
    const QList<int> changedRolesList = maskedIter->keys();
    const QVector<int> changedRoles = changedRolesList.toVector();
    Q_ASSUME(d->removeIndex(sourceIndex));
    maskedDataChanged(index, index, changedRoles);
    dataChanged(index, index, changedRoles);
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
/*!
\reimp
\details Due to limitations in the architecture, the model might emit 2 separate dataChanged signals, one for the roles that were masked and one for
the roles that are managed by the sorce model
*/
bool RoleMaskProxyModel::clearItemData(const QModelIndex &index)
{
    if (!index.isValid())
        return false;
    Q_ASSERT(index.model() == this);
    clearMaskedData(index);
    return QIdentityProxyModel::clearItemData(index);
}
#endif

/*!
\reimp
\details Due to limitations in the architecture, the model might emit 2 separate dataChanged signals, one for the roles that were masked and one for
the roles that are managed by the sorce model
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
        auto idxData = d->dataForIndex(sourceIndex);
        if (!idxData) {
            if (i->isValid()) {
                RolesContainer newData;
                newData.insert(i.key(), i.value());
                d->insertData(sourceIndex, newData);
                changedRoles << i.key();
            }
            continue;
        }
        if (!i->isValid()) {
            if (d->removeRole(sourceIndex, i.key()))
                changedRoles << i.key();
            continue;
        }
        const auto roleIter = idxData->find(i.key());
        if (roleIter == idxData->end())
            idxData->insert(i.key(), i.value());
        else if (roleIter.value() != i.value())
            roleIter.value() = i.value();
        else
            continue;
        changedRoles << i.key();
    }
    if (d->m_mergeDisplayEdit && changedRoles.contains(Qt::DisplayRole))
        changedRoles << Qt::EditRole;
    if (!changedRoles.isEmpty()) {
        maskedDataChanged(index, index, changedRoles);
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
    const auto maskedData = d->dataForIndex(mapToSource(index));
    if (maskedData) {
        result = *maskedData;
        const auto displayIter = result.constFind(Qt::DisplayRole);
        if (d->m_mergeDisplayEdit && displayIter != result.cend())
            result.insert(Qt::EditRole, displayIter.value());
    }
    if (!d->m_transparentIfEmpty) {
        const QMap<int, QVariant> baseData = QIdentityProxyModel::itemData(index);
        for (auto i = baseData.cbegin(), baseEnd = baseData.cend(); i != baseEnd; ++i) {
            if (!result.contains(i.key()))
                result.insert(i.key(), i.value());
        }
    }
    return convertFromContainer<QMap<int, QVariant>>(result);
}

/*!
\reimp
*/
QVariant RoleMaskProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_D(const RoleMaskProxyModel);
    if(d->m_maskHeaderData && d->m_maskedRoles.contains(role)){
        const QVector<RolesContainer>& dataContainerVect = orientation==Qt::Horizontal ? d->m_hHeaderData : d->m_vHeaderData;
        if(section<0 || section >= dataContainerVect.size())
            return QVariant();
        const RolesContainer& dataContainer=dataContainerVect.at(section);
        const auto roleIter = dataContainer.constFind(role);
        if(roleIter!=dataContainer.constEnd())
            return *roleIter;
        if(!d->m_transparentIfEmpty)
            return QVariant();
    }
    return QIdentityProxyModel::headerData(section,orientation,role);
}

/*!
Returns all the data managed by the proxy model for a certain \a index. The key of the map is the corresponding role
*/
QMap<int, QVariant> RoleMaskProxyModel::maskedItemData(const QModelIndex &index) const
{
    Q_D(const RoleMaskProxyModel);
    const auto maskedData = d->dataForIndex(mapToSource(index));
    if (!maskedData)
        return QMap<int, QVariant>();
    return convertFromContainer<QMap<int, QVariant>>(*maskedData);
}

/*!
\property RoleMaskProxyModel::transparentIfEmpty
\accessors %transparentIfEmpty(), setTransparentIfEmpty()
\notifier transparentIfEmptyChanged()
\brief This property determines if a mapped role containing no data should be transparent
\details If this property is set to true, roles managed by the proxy will show the source model data unless it gets ovewritten in the proxy
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
\property RoleMaskProxyModel::maskHeaderData
\accessors %maskHeaderData(), setMaskHeaderData()
\notifier maskHeaderDataChanged()
\brief This property determines if the role mask should apply to the headers too
\details By default this property is set to false
*/
bool RoleMaskProxyModel::maskHeaderData() const
{
    Q_D(const RoleMaskProxyModel);
    return d->m_maskHeaderData;
}

void RoleMaskProxyModel::setMaskHeaderData(bool val)
{
    Q_D(RoleMaskProxyModel);
    if (val==d->m_maskHeaderData)
        return;
    d->m_maskHeaderData = val;
    const int colCnt = columnCount();
    const int rowCnt = rowCount();
    if(val){
        if(colCnt>0)
            d->m_hHeaderData = QVector<RolesContainer>(colCnt,RolesContainer());
        if(rowCnt>0)
            d->m_vHeaderData = QVector<RolesContainer>(rowCnt,RolesContainer());
    }
    else{
        d->m_hHeaderData.clear();
        d->m_vHeaderData.clear();
    }
    maskHeaderDataChanged(val);
    if (colCnt>0)
        headerDataChanged(Qt::Horizontal, 0, colCnt - 1);
    if (rowCnt>0)
        headerDataChanged(Qt::Vertical, 0, rowCnt - 1);
}

/*!
\property RoleMaskProxyModel::mergeDisplayEdit
\accessors %mergeDisplayEdit(), setMergeDisplayEdit()
\notifier mergeDisplayEditChanged()
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
    const auto setMergeDisplayEditContainer = [val](RolesContainer& container){
        if (val) {
            if (container.contains(Qt::DisplayRole)) {
                container.remove(Qt::EditRole);
            } else {
                const auto roleIter = container.constFind(Qt::EditRole);
                if (roleIter != container.constEnd()) {
                    container.insert(Qt::DisplayRole, *roleIter);
                    container.remove(Qt::EditRole);
                }
            }
        } else {
            const auto roleIter = container.constFind(Qt::DisplayRole);
            if (roleIter != container.constEnd()) {
                container.insert(Qt::EditRole, *roleIter);
            }
        }
    };
    Q_D(RoleMaskProxyModel);
    if (d->m_mergeDisplayEdit != val) {
        QVector<int> changedRoles({Qt::DisplayRole, Qt::EditRole});
        d->m_mergeDisplayEdit = val;
        for (auto idxIter = d->m_masked.begin(), idxEnd = d->m_masked.end(); idxIter != idxEnd; ++idxIter)
            setMergeDisplayEditContainer(idxIter->m_data);
        if (val)
            d->m_maskedRoles.remove(Qt::EditRole);
        else if (d->m_maskedRoles.contains(Qt::DisplayRole))
            d->m_maskedRoles.insert(Qt::EditRole);
        if (d->m_maskHeaderData){
            for (auto idxIter = d->m_hHeaderData.begin(), idxEnd = d->m_hHeaderData.end(); idxIter != idxEnd; ++idxIter)
                setMergeDisplayEditContainer(*idxIter);
            for (auto idxIter = d->m_vHeaderData.begin(), idxEnd = d->m_vHeaderData.end(); idxIter != idxEnd; ++idxIter)
                setMergeDisplayEditContainer(*idxIter);
        }
        mergeDisplayEditChanged(d->m_mergeDisplayEdit);
        d->signalAllChanged(changedRoles);
    }
}

/*!
Same as maskedRoles but returns it in the way it is stored internally rather than converting it to QList
*/
const QSet<int> &RoleMaskProxyModel::maskedRolesSets() const
{
    Q_D(const RoleMaskProxyModel);
    return d->m_maskedRoles;
}

/*!
\class RoleMaskProxyModel
\brief This proxy will act as a mask on top of the source model to intercept data.
\details This proxy model can be used to manage data in roles not supported by the source model.
The user can select what roles should be managed by the proxy model and the rest will be left to the source model.
*/

/*!
\fn RoleMaskProxyModel::maskedDataChanged()
This signal is emitted whenever a dataChanged() signal is emitted for a role managed by the proxy
*/

/*!
\fn RoleMaskProxyModel::maskedRolesChanged()
This signal is emitted whenever the list of roles managed by the proxy changes
*/
