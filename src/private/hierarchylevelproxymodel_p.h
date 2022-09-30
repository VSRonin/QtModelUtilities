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
#ifndef HIERARCHYLEVELPROXY_P_H
#define HIERARCHYLEVELPROXY_P_H
#include "hierarchylevelproxymodel.h"
#include "private/modelutilities_common_p.h"
#include <QVector>
struct HierarchyRootData{
    QPersistentModelIndex root;
    int cachedCumRowCount=0;
    HierarchyRootData(const QPersistentModelIndex& rt, int rc);
    HierarchyRootData();
    HierarchyRootData(const HierarchyRootData& other) =default;
    HierarchyRootData& operator=(const HierarchyRootData& other)=default;
};

class HierarchyLevelProxyModelPrivate
{
    Q_DECLARE_PUBLIC(HierarchyLevelProxyModel)
    Q_DISABLE_COPY_MOVE(HierarchyLevelProxyModelPrivate)
    HierarchyLevelProxyModelPrivate(HierarchyLevelProxyModel *q);
    HierarchyLevelProxyModel *q_ptr;
    QVector<HierarchyRootData> m_roots;
    int m_maxCol;
    int m_targetLevel;
    HierarchyLevelProxyModel::InsertBehaviour m_insertBehaviour;
    const char m_inexistentSourceIndexFlag;
    QVector<QPersistentModelIndex> m_layoutChangePersistentIndexes;
    QModelIndexList m_layoutChangeProxyIndexes;
    QVector<QMetaObject::Connection> m_sourceConnections;
    int rootOf(QModelIndex sourceIndex) const;
    static int levelOf(QModelIndex idx);
    bool inexistentAtSource(const QModelIndex& idx) const;
    void rebuildMapping();
    void rebuildMappingBranch(const QModelIndex& parent, int levl, int &rootsRowCount);

    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void onHeaderDataChanged(Qt::Orientation orientation,int first, int last);
    void onColumnsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void onColumnsAboutToBeMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column);
    void onColumnsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void onColumnsInserted(const QModelIndex &parent, int first, int last);
    void onColumnsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column);
    void onColumnsRemoved(const QModelIndex &parent, int first, int last);
    void onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void onRowsAboutToBeMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void onRowsInserted(const QModelIndex &parent, int first, int last);
    void onRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void onRowsRemoved(const QModelIndex &parent, int first, int last);
    void beforeSortRows();
    void afterSortRows();
    void beforeSortCols();
    void afterSortCols();
    void beforeLayoutChange(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void afetrLayoutChange(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void afterReset();

};

#endif // HIERARCHYLEVELPROXY_P_H
