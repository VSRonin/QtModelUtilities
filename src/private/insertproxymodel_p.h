#ifndef insertproxy_p_h__
#define insertproxy_p_h__
#include "insertproxymodel.h"
#include "private/modelutilities_common_p.h"
#include <QHash>
#include <QMap>
#include <QVector>
class InsertProxyModelPrivate
{
    Q_DECLARE_PUBLIC(InsertProxyModel)
    Q_DISABLE_COPY(InsertProxyModelPrivate)
    InsertProxyModelPrivate(InsertProxyModel* q);
    InsertProxyModel::InsertDirections m_insertDirection;
    QVector<QPersistentModelIndex> m_layoutChangePersistentIndexes;
    QModelIndexList m_layoutChangeProxyIndexes;
    QVector<RolesContainer> m_extraData[2];
    RolesContainer m_extraHeaderData[2];
    RolesContainer m_dataForCorner;
    bool m_separateEditDisplay;
    QVector<QMetaObject::Connection> m_sourceConnections;
    void checkExtraDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
    bool commitColumn();
    bool commitRow();
    bool commitToSource(const bool isRow);
    int mergeEditDisplayHash(RolesContainer& singleHash);
    void onColumnsInserted(const QModelIndex &parent, int first, int last) { onInserted(false, parent, first, last); }
    void onColumnsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column) { onMoved(false, parent, start, end, destination, column); }
    void onColumnsRemoved(const QModelIndex &parent, int first, int last) { onRemoved(false, parent, first, last); }
    void onRowsInserted(const QModelIndex &parent, int first, int last) { onInserted(true, parent, first, last); }
    void onRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row) { onMoved(true, parent, start, end, destination, row); }
    void onRowsRemoved(const QModelIndex &parent, int first, int last) { onRemoved(true, parent, first, last); }
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
    QVector<int> setDataInContainer(RolesContainer& baseHash, int role, const QVariant& value);
    InsertProxyModel* q_ptr;
};

#endif // insertproxy_p_h__
