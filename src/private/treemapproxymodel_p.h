#ifndef treemapproxymodel_p_h__
#define treemapproxymodel_p_h__

#include "treemapproxymodel.h"
#include <QVector>
#include <QMetaObject>
class TreeMapProxyNode;
using NodeContainer = QVector<TreeMapProxyNode*>;
class TreeMapProxyNode{
public:
    TreeMapProxyNode* parentNode;
    QPersistentModelIndex* sourceIdx;
    NodeContainer children;
    TreeMapProxyNode(QPersistentModelIndex* srcIdx = Q_NULLPTR, TreeMapProxyNode* parNode = Q_NULLPTR);
    TreeMapProxyNode(const TreeMapProxyNode& other) = delete;
    TreeMapProxyNode& operator=(const TreeMapProxyNode& other) = delete;
    TreeMapProxyNode(TreeMapProxyNode&& other);
    TreeMapProxyNode& operator=(TreeMapProxyNode&& other);
    ~TreeMapProxyNode();
    void clear();
};


class TreeMapProxyModelPrivate{
    Q_DECLARE_PUBLIC(TreeMapProxyModel)
    Q_DISABLE_COPY(TreeMapProxyModelPrivate)
public:
    TreeMapProxyModelPrivate(TreeMapProxyModel* q);
    TreeMapProxyNode* findNode(const QModelIndex& sourceIdx) const;
    bool nodeFound(const TreeMapProxyNode* node) const;
protected:
    TreeMapProxyModel* q_ptr;
private:
    void onRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent);
    void onColsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent);
    QVector<QMetaObject::Connection> m_sourceConnections;
    TreeMapProxyNode m_treeMapper;
};
#endif // treemapproxymodel_p_h__
