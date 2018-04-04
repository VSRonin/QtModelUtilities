#ifndef insertproxy_p_h__
#define insertproxy_p_h__
#include "insertproxymodel.h"
#include <QHash>

class InsertProxyModelPrivate
{
    Q_DECLARE_PUBLIC(InsertProxyModel)
    Q_DISABLE_COPY(InsertProxyModelPrivate)
    InsertProxyModelPrivate(InsertProxyModel* q);
    InsertProxyModel::InsertDirections m_insertDirection;
    QList<QHash<int, QVariant> > m_extraData[2];
    QHash<int, QVariant> m_extraHeaderData[2];
    QHash<int, QVariant> m_dataForCorner;
    bool m_separateEditDisplay;
    QList<QMetaObject::Connection> m_sourceConnections;
    void checkExtraRowChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
    bool commitColumn();
    bool commitRow();
    bool commitToSource(const bool isRow);
    void setSeparateEditDisplayHash(QHash<int, QVariant>& singleHash);
    void onColumnsInserted(const QModelIndex &parent, int first, int last) { onInserted(false, parent, first, last); }
    void onColumnsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column) { onMoved(false, parent, start, end, destination, column); }
    void onColumnsRemoved(const QModelIndex &parent, int first, int last) { onRemoved(false, parent, first, last); }
    void onRowsInserted(const QModelIndex &parent, int first, int last) { onInserted(true, parent, first, last); }
    void onRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row) { onMoved(true, parent, start, end, destination, row); }
    void onRowsRemoved(const QModelIndex &parent, int first, int last) { onRemoved(true, parent, first, last); }
    void onInserted(bool isRow, const QModelIndex &parent, int first, int last);
    void onMoved(bool isRow, const QModelIndex &parent, int start, int end, const QModelIndex &destination, int destIdx);
    void onRemoved(bool isRow, const QModelIndex &parent, int first, int last);
    InsertProxyModel* q_ptr;
};
#endif // insertproxy_p_h__
