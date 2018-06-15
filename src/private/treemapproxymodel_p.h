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
#ifndef treemapproxymodel_p_h__
#define treemapproxymodel_p_h__

#include "treemapproxymodel.h"
#include <QVector>
#include <QMetaObject>
class TreeMapProxyNode;
using NodeContainer = QVector<TreeMapProxyNode*>;
class TreeMapProxyNode{
    Q_DISABLE_COPY(TreeMapProxyNode)
public:
    TreeMapProxyNode* parentNode;
    QPersistentModelIndex* sourceIdx;
    NodeContainer children;
    TreeMapProxyNode(QPersistentModelIndex* srcIdx = Q_NULLPTR, TreeMapProxyNode* parNode = Q_NULLPTR);
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
    void rebuildTreeMap(const QModelIndex& parent, TreeMapProxyNode* container);
    void onRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent);
    void onColsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent);
    QVector<QMetaObject::Connection> m_sourceConnections;
    TreeMapProxyNode m_treeMapper;
};
#endif // treemapproxymodel_p_h__
