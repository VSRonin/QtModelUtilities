/****************************************************************************\
   Copyright 2025 Luca Beldi
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
#ifndef SUBTOTALPROXYMODEL_P_H
#define SUBTOTALPROXYMODEL_P_H
#include "subtotalproxymodel.h"

class SubtotalAggregator
{
public:
    QVariant m_startingVal;
    SubtotalProxyModel::AggregateFunction m_aggregateFunction;
    int m_role;
    SubtotalAggregator();
    SubtotalAggregator(SubtotalProxyModel::AggregateFunction aggr, const QVariant &start, int role);
    SubtotalAggregator(const SubtotalAggregator &) = default;
    SubtotalAggregator &operator=(const SubtotalAggregator &) = default;
};

class SubtotalProxyModelPrivate
{
    Q_DECLARE_PUBLIC(SubtotalProxyModel)
    SubtotalProxyModel *q_ptr;
    SubtotalProxyModelPrivate(SubtotalProxyModel *q);
    QMultiMap<int, SubtotalAggregator> m_subtotalMap;
    bool m_overwriteParentData;
    bool m_recursive;
    QVector<QMetaObject::Connection> m_sourceConnections;
    SubtotalProxyModel::SubtotalLocations totalLocations() const;
    bool isOnTotalRow(int row, const QModelIndex &parent) const;
    enum TotalRowOrTotalParent { onNothing, onTotalRow, onTotalParent };
    TotalRowOrTotalParent isOnTotalRowOrTotalParent(const QModelIndex &idx) const;
    QVariant calculateTotal(int column, const QModelIndex &parent, int role) const;
    void calculateTotal(int column, const QModelIndex &sourceParent, int role, QVariant &runningSum,
                        SubtotalProxyModel::AggregateFunction aggr) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    void multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const;
#endif
    void emitRecursiveDataChanged();
    void emitRecursiveDataChanged(int column, const QModelIndex &parent = QModelIndex(), QList<int> *roles = nullptr);
    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void onRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void onColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void onRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void onColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void onLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void onRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent,
                              int destinationRow);
    void onColumnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent,
                                 int destinationColumn);
    void onRowsInserted(const QModelIndex &parent, int start, int end);
    void onColumnsInserted(const QModelIndex &parent, int start, int end);
    void onRowsRemoved(const QModelIndex &parent, int start, int end);
    void onColumnsRemoved(const QModelIndex &parent, int start, int end);
    void onRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void onColumnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn);

private:
    SubtotalProxyModel::SubtotalLocations m_totalLocations;
};

#endif // SUBTOTALPROXYMODEL_P_H
