/****************************************************************************\
   Copyright 2021 Luca Beldi
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
#ifndef ROOTINDEXPROXY_P_H
#define ROOTINDEXPROXY_P_H
#include "rootindexproxymodel.h"
#include "private/modelutilities_common_p.h"
class RootIndexProxyModelPrivate
{
    Q_DECLARE_PUBLIC(RootIndexProxyModel)
    Q_DISABLE_COPY(RootIndexProxyModelPrivate)
    RootIndexProxyModelPrivate(RootIndexProxyModel *q);
    QVector<QMetaObject::Connection> m_sourceConnections;
    QPersistentModelIndex m_rootIndex;
    RootIndexProxyModel *q_ptr;
    bool m_rootRowDeleted;
    bool m_rootColumnDeleted;
    QList<QPersistentModelIndex> layoutChangePersistentIndexes;
    QModelIndexList proxyIndexes;
    bool isDescendant(QModelIndex childIdx, const QModelIndex &parentIdx) const;
    bool isRelevant(const QModelIndex &idx) const;
    void resetRootOnModelChange();
    void checkRootRowRemoved(const QModelIndex &parent, int first, int last);
    void checkRootColumnsRemoved(const QModelIndex &parent, int first, int last);
    void onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void onRowsInserted(const QModelIndex &parent, int first, int last);
    void onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void onRowsRemoved(const QModelIndex &parent, int first, int last);
    void onRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest);
    void onRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest);
    void onColumnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest);
    void onColumnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest);
    void onColumnsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void onColumnsInserted(const QModelIndex &parent, int first, int last);
    void onColumnsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void onColumnsRemoved(const QModelIndex &parent, int first, int last);
    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint);
    void onLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint);
};

#endif // ROOTINDEXPROXY_P_H
