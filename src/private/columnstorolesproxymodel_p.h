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
#ifndef COLUMNSTOROLESPROXY_P_H
#define COLUMNSTOROLESPROXY_P_H
#include "columnstorolesproxymodel.h"
#include "private/modelutilities_common_p.h"
#include <QMap>

struct ColumnToRoleMapping
{
    int sourceCol;
    int sourceRole;
    int destinationCol;
    int destinationRole;
    ColumnToRoleMapping();
    ColumnToRoleMapping(const ColumnToRoleMapping &other) = default;
    ColumnToRoleMapping &operator=(const ColumnToRoleMapping &other) = default;
    bool isValid() const;
};

class ColumnsToRolesProxyModelPrivate
{
    Q_DECLARE_PUBLIC(ColumnsToRolesProxyModel)
    Q_DISABLE_COPY(ColumnsToRolesProxyModelPrivate)
    ColumnsToRolesProxyModelPrivate(ColumnsToRolesProxyModel *q);
    QVector<QMetaObject::Connection> m_sourceConnections;
    void interceptDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void onColumnsInserted(const QModelIndex &parent, int first, int last);
    void onColumnsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column);
    void onColumnsRemoved(const QModelIndex &parent, int first, int last);
    void beforeLayoutChange(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void afterLayoutChange(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    ColumnsToRolesProxyModel *q_ptr;
    QVector<ColumnToRoleMapping> m_colToRoleMap;
};

#endif // COLUMNSTOROLESPROXY_P_H
