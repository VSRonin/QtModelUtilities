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
#ifndef INSERTPROXY_P_H
#define INSERTPROXY_P_H
#include "insertproxymodel.h"
#include "private/modelutilities_common_p.h"
#include <QHash>
#include <QMap>
#include <QVector>
class InsertProxyModelPrivate
{
    Q_DECLARE_PUBLIC(InsertProxyModel)
    Q_DISABLE_COPY(InsertProxyModelPrivate)
    InsertProxyModelPrivate(InsertProxyModel *q);
    InsertProxyModel::InsertDirections m_insertDirection;
    QVector<QPersistentModelIndex> m_layoutChangePersistentIndexes;
    QModelIndexList m_layoutChangeProxyIndexes;
    QVector<QPersistentModelIndex> m_layoutChangeExtraPersistentIndexes;
    QModelIndexList m_layoutChangeExtraProxyIndexes;
    QList<RolesContainer> m_extraData[2];
    RolesContainer m_extraHeaderData[2];
    RolesContainer m_dataForCorner;
    bool m_separateEditDisplay;
    QVector<QMetaObject::Connection> m_sourceConnections;
    void checkExtraDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    bool commitColumn();
    bool commitRow();
    bool commitToSource(const bool isRow);
    int mergeEditDisplayHash(RolesContainer &singleHash);
    void onColumnsInserted(const QModelIndex &parent, int first, int last);
    void onColumnsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column);
    void onColumnsRemoved(const QModelIndex &parent, int first, int last);
    void onRowsInserted(const QModelIndex &parent, int first, int last);
    void onRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void onRowsRemoved(const QModelIndex &parent, int first, int last);
    void onInserted(bool isRow, const QModelIndex &parent, int first, int last);
    void onMoved(bool isRow, const QModelIndex &parent, int start, int end, const QModelIndex &destination, int destIdx);
    void onRemoved(bool isRow, const QModelIndex &parent, int first, int last);
    void beforeSortRows();
    void afterSortRows();
    void beforeSortCols();
    void afterSortCols();
    void beforeSort(bool isRow);
    void afterSort(bool isRow);
    void beforeLayoutChange(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void afetrLayoutChange(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void afterReset();
    QVector<int> setDataInContainer(RolesContainer &baseHash, int role, const QVariant &value);
    InsertProxyModel *q_ptr;
};

#endif // INSERTPROXY_P_H
