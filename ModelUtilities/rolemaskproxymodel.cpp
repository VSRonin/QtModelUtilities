/**********************************************************************************\

Copyright 2018 Luca Beldi

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

\**********************************************************************************/
#include "rolemaskproxymodel.h"
#include <QPersistentModelIndex>
#include <QHash>
#include <QSet>
#include <QVector>
#include <functional>
class RoleMaskProxyModelPrivate{
    Q_DECLARE_PUBLIC(RoleMaskProxyModel)
    RoleMaskProxyModel* q_ptr;
    RoleMaskProxyModelPrivate(RoleMaskProxyModel* q);
    QSet<int> m_maskedRoles;
    QHash<QPersistentModelIndex, QHash<int, QVariant> > m_maskedData;
    bool m_transparentIfEmpty;
    bool m_mergeDisplayEdit;
    void clearUnusedMaskedRoles(const QSet<int>& roles);
    bool removeRole(const QPersistentModelIndex& idx, int role);
    bool removeRole(const QHash<QPersistentModelIndex, QHash<int, QVariant> >::iterator& idxIter, int role);
    void signalAllChanged(const QVector<int>& roles = QVector<int>(), const QModelIndex& parent = QModelIndex());
};

RoleMaskProxyModelPrivate::RoleMaskProxyModelPrivate(RoleMaskProxyModel* q)
    :q_ptr(q)
    , m_transparentIfEmpty(true)
    , m_mergeDisplayEdit(false)
{
    Q_ASSERT(q_ptr);
}

void RoleMaskProxyModelPrivate::clearUnusedMaskedRoles(const QSet<int>& newRoles)
{
    if (newRoles.isEmpty()) {
        m_maskedData.clear();
        return;
    }
    const QHash<QPersistentModelIndex, QHash<int, QVariant> >::iterator idxEnd = m_maskedData.end();
    for (QHash<QPersistentModelIndex, QHash<int, QVariant> >::iterator idxIter = m_maskedData.begin(); idxIter != idxEnd;) {
        const QHash<int, QVariant>::iterator roleEnd = idxIter->end();
        for (QHash<int, QVariant>::iterator roleIter = idxIter->begin(); roleIter != roleEnd;){
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
    const QHash<QPersistentModelIndex, QHash<int, QVariant> >::iterator idxIter = m_maskedData.find(idx);
    return removeRole(idxIter, role);
}

bool RoleMaskProxyModelPrivate::removeRole(const QHash<QPersistentModelIndex, QHash<int, QVariant> >::iterator& idxIter, int role)
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

RoleMaskProxyModel::RoleMaskProxyModel(QObject* parent)
    :QIdentityProxyModel(parent)
    , d_ptr(new RoleMaskProxyModelPrivate(this))
{}

RoleMaskProxyModel::~RoleMaskProxyModel()
{
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
        Q_FOREACH(int singleRole, changedRolesSet)
            changedRoles.append(singleRole);
        d->m_maskedRoles = roles;
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
        changedRoles.reserve(d->m_maskedRoles.size());
        Q_FOREACH(int singleRole, d->m_maskedRoles)
            changedRoles.append(singleRole);
        d->m_maskedRoles.clear();
        d->clearUnusedMaskedRoles(d->m_maskedRoles);
        maskedRolesChanged();
        d->signalAllChanged(changedRoles);
    }
}

void RoleMaskProxyModel::addMaskedRole(int role)
{
    Q_D(RoleMaskProxyModel);
    if(!d->m_maskedRoles.contains(role)){
        d->m_maskedRoles.insert(role);
        maskedRolesChanged();
        d->signalAllChanged(QVector<int>(1, role));
    }
}

void RoleMaskProxyModel::removeMaskedRole(int role)
{
    Q_D(RoleMaskProxyModel);
    if (d->m_maskedRoles.remove(role)) {
        d->clearUnusedMaskedRoles(d->m_maskedRoles);
        maskedRolesChanged();
        d->signalAllChanged(QVector<int>(1, role));
    }
}

void RoleMaskProxyModel::setSourceModel(QAbstractItemModel *sourceMdl)
{
    Q_D(RoleMaskProxyModel);
    if (sourceMdl != sourceModel()) {
        d->m_maskedData.clear();
    }
    QIdentityProxyModel::setSourceModel(sourceMdl);

}

QVariant RoleMaskProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    Q_D(const RoleMaskProxyModel);
    int adjRole = role;
    if (d->m_mergeDisplayEdit && role == Qt::EditRole)
        adjRole = Qt::DisplayRole;
    if (!d->m_maskedRoles.contains(adjRole))
        return QIdentityProxyModel::data(proxyIndex, role);
    const QModelIndex sourceIndex = mapToSource(proxyIndex);
    const QHash<QPersistentModelIndex, QHash<int, QVariant> >::const_iterator idxIter = d->m_maskedData.constFind(sourceIndex);
    if (idxIter != d->m_maskedData.constEnd()){
        const QHash<int, QVariant>::const_iterator roleIter = idxIter->constFind(adjRole);
        if (roleIter != idxIter->constEnd())
            return roleIter.value();
    }
    if(d->m_transparentIfEmpty)
        return QIdentityProxyModel::data(proxyIndex, role);
    return QVariant();
}

bool RoleMaskProxyModel::setData(const QModelIndex &proxyIndex, const QVariant &value, int role)
{
    Q_D(RoleMaskProxyModel);
    if (!proxyIndex.isValid())
        return false;
    if (!d->m_maskedRoles.contains(role))
        return QIdentityProxyModel::setData(proxyIndex, value, role);
    if (d->m_mergeDisplayEdit && role == Qt::EditRole)
        role = Qt::DisplayRole;
    const QModelIndex sourceIndex = mapToSource(proxyIndex);
    Q_ASSERT(sourceIndex.isValid());
    QHash<QPersistentModelIndex, QHash<int, QVariant> >::iterator idxIter = d->m_maskedData.find(sourceIndex);
    if (idxIter == d->m_maskedData.end()){
        if (!value.isValid()) {
            return true;
        }
        else {
            idxIter = d->m_maskedData.insert(sourceIndex, QHash<int, QVariant>());
            idxIter->insert(role, value);
            dataChanged(proxyIndex, proxyIndex, QVector<int>(1, role));
            return true;
        }
    }
    if (!value.isValid()){
        if (d->removeRole(idxIter, role))
            dataChanged(proxyIndex, proxyIndex, QVector<int>(1, role));
        return true;
    }
    const QHash<int, QVariant>::iterator roleIter = idxIter->find(role);
    if (roleIter == idxIter->end()) 
        idxIter->insert(role, value);
    else if (roleIter.value() != value)
        roleIter.value() = value;
    else
        return true;
    dataChanged(proxyIndex, proxyIndex, QVector<int>(1, role));
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
        QVector<int> changedRoles(2, Qt::EditRole);
        changedRoles[0] = Qt::DisplayRole;
        d->m_mergeDisplayEdit=val;
        const QHash<QPersistentModelIndex, QHash<int, QVariant> >::iterator idxEnd = d->m_maskedData.end();
        for (QHash<QPersistentModelIndex, QHash<int, QVariant> >::iterator idxIter = d->m_maskedData.begin(); idxIter != idxEnd; ++idxIter){
            if (val){
                if (idxIter->contains(Qt::DisplayRole)) {
                    idxIter->remove(Qt::EditRole);
                }
                else {
                    const QHash<int, QVariant>::const_iterator roleIter = idxIter->constFind(Qt::EditRole);
                    if (roleIter != idxIter->constEnd()) {
                        idxIter->insert(Qt::DisplayRole, *roleIter);
                        idxIter->remove(Qt::EditRole);
                    }
                }
            }
            else{
                const QHash<int, QVariant>::const_iterator roleIter = idxIter->constFind(Qt::EditRole);
                if (roleIter != idxIter->constEnd()) {
                    idxIter->insert(Qt::DisplayRole, *roleIter);
                }
            }
        }
        mergeDisplayEditChanged(d->m_mergeDisplayEdit);
        d->signalAllChanged(changedRoles);
    }
}
