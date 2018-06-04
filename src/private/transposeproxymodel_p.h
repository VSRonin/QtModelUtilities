#include "transposeproxymodel.h"
#include <QVector>
#include <QMetaObject>
class TransposeProxyModelPrivate
{
    Q_DECLARE_PUBLIC(TransposeProxyModel)
    Q_DISABLE_COPY(TransposeProxyModelPrivate)
private:
    TransposeProxyModelPrivate(TransposeProxyModel* q);
    TransposeProxyModel* q_ptr;
    QVector<QMetaObject::Connection> m_sourceConnections;
    QVector<QPersistentModelIndex> m_layoutChangePersistentIndexes;
    QModelIndexList m_layoutChangeProxyIndexes;
    void onLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);

};


