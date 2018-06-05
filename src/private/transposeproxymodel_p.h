#include "transposeproxymodel.h"
#include <QVector>
#include <QMetaObject>
class TransposeProxyModelPrivate
{
    Q_DECLARE_PUBLIC(TransposeProxyModel)
    Q_DISABLE_COPY(TransposeProxyModelPrivate)
private:
    TransposeProxyModelPrivate(TransposeProxyModel* q);
    virtual ~TransposeProxyModelPrivate();
    TransposeProxyModel* q_ptr;
    QVector<QMetaObject::Connection> m_sourceConnections;
    QVector<QPersistentModelIndex> m_layoutChangePersistentIndexes;
    QVector<QPersistentModelIndex*> m_treeMapper;
    QModelIndexList m_layoutChangeProxyIndexes;
    void onLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void onRowsInserted(const QModelIndex& parent);
    void onRowsRemoved(const QModelIndex& parent);
    void onRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent);
    void onColsInserted(const QModelIndex& parent);
    void onColsRemoved(const QModelIndex& parent);
    void onColsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent);
    void onModelReset();
    void buildTree();
    void buildTree(const QModelIndex& parent);
    void clearTreeMap();
    void possibleNewParent(const QModelIndex& parent);
    void possibleRemovedParent(const QModelIndex& parent);
    QVector<QPersistentModelIndex*>::iterator findNode(const QModelIndex& sourceIdx);
    QVector<QPersistentModelIndex*>::const_iterator findNode(const QModelIndex& sourceIdx) const;
    bool nodeFound(QVector<QPersistentModelIndex*>::const_iterator node) const;
    bool nodeFound(QVector<QPersistentModelIndex*>::iterator node) const;
};





