#ifndef transposeproxymodel_p_h__
#define transposeproxymodel_p_h__

#include "transposeproxymodel.h"
#include "private/treemapproxymodel_p.h"
#include <QVector>
#include <QMetaObject>
class TransposeProxyModelPrivate : public TreeMapProxyModelPrivate
{
    Q_DECLARE_PUBLIC(TransposeProxyModel)
    Q_DISABLE_COPY(TransposeProxyModelPrivate)
private:
    TransposeProxyModelPrivate(TransposeProxyModel* q);
    QVector<QMetaObject::Connection> m_sourceConnections;
    QVector<QPersistentModelIndex> m_layoutChangePersistentIndexes;
    QModelIndexList m_layoutChangeProxyIndexes;
    void onLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
};

#endif // transposeproxymodel_p_h__
