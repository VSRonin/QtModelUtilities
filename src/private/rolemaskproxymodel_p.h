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
#ifndef ROLEMASKPROXYMODEL_P_H
#define ROLEMASKPROXYMODEL_P_H
#include <QPersistentModelIndex>
#include <QHash>
#include <QSet>
#include <QMetaObject>
#include <QVector>
#include "private/modelutilities_common_p.h"
#include "rolemaskproxymodel.h"
#include <memory>

struct FlaggedRolesContainer
{
    RolesContainer roles;
    std::unique_ptr<Qt::ItemFlags> flags;
    FlaggedRolesContainer();
    FlaggedRolesContainer(const RolesContainer &r, const Qt::ItemFlags *f);
    FlaggedRolesContainer(const FlaggedRolesContainer &other);
    FlaggedRolesContainer &operator=(const FlaggedRolesContainer &other);
};

struct MaskedItem
{
    FlaggedRolesContainer m_data;
    QPersistentModelIndex m_index;
    MaskedItem() = default;
    MaskedItem(const FlaggedRolesContainer &data, const QModelIndex &index);
    MaskedItem(const FlaggedRolesContainer &data, const QPersistentModelIndex &index);
    MaskedItem(const MaskedItem &other) = default;
    MaskedItem &operator=(const MaskedItem &other) = default;
};

class RoleMaskProxyModelPrivate
{
    Q_DECLARE_PUBLIC(RoleMaskProxyModel)
    RoleMaskProxyModel *q_ptr;
    RoleMaskProxyModelPrivate(RoleMaskProxyModel *q);
    QSet<int> m_maskedRoles;
    QMultiHash<QPair<int, int>, MaskedItem> m_masked;
    QVector<RolesContainer> m_hHeaderData;
    QVector<RolesContainer> m_vHeaderData;
    QVector<QPersistentModelIndex> m_sortVHeaders;
    QVector<QPersistentModelIndex> m_sortHHeaders;
    RolesContainer m_defaultValues;
    bool m_transparentIfEmpty;
    bool m_mergeDisplayEdit;
    bool m_maskHeaderData;
    QVector<QMetaObject::Connection> m_sourceConnections;
    void clearUnusedMaskedRoles(const QSet<int> &roles);
    bool removeRole(const QModelIndex &idx, int role);
    bool removeIndex(const QModelIndex &idx);
    void signalAllChanged(const QVector<int> &roles = QVector<int>(), const QModelIndex &parent = QModelIndex());
    void interceptDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    const FlaggedRolesContainer *dataForIndex(const QModelIndex &index) const;
    FlaggedRolesContainer *dataForIndex(const QModelIndex &index);
    void insertData(const QModelIndex &index, const FlaggedRolesContainer &data);
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
};

#endif // ROLEMASKPROXYMODEL_P_H
