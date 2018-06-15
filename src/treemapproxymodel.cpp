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
#include "treemapproxymodel.h"
#include "private/treemapproxymodel_p.h"
#include <functional>
/*!
\internal
*/
TreeMapProxyNode::TreeMapProxyNode(QPersistentModelIndex* srcIdx, TreeMapProxyNode* parNode)
    : sourceIdx(srcIdx)
    , parentNode(parNode)
{}

/*!
\internal
*/
TreeMapProxyModelPrivate::TreeMapProxyModelPrivate(TreeMapProxyModel* q)
    :q_ptr(q)
{
    Q_ASSERT(q_ptr);
}

/*!
\internal
*/
TreeMapProxyNode::~TreeMapProxyNode()
{
    clear();
}

/*!
\internal
*/
void TreeMapProxyNode::clear()
{
    parentNode = Q_NULLPTR;
    delete sourceIdx;
    sourceIdx = Q_NULLPTR;
    for (auto i = children.cbegin(), j = children.cend(); i != j; ++i)
        delete *i;
    children.clear();
}

/*!
Destructor
*/
TreeMapProxyModel::~TreeMapProxyModel()
{
    Q_D(TreeMapProxyModel);
    clearTreeMap();
    for (auto discIter = d->m_sourceConnections.cbegin(), discEnd = d->m_sourceConnections.cend(); discIter != discEnd; ++discIter)
        QObject::disconnect(*discIter);
    delete d_ptr;
}

/*!
Constructs a new proxy model with the given \a parent.
*/
TreeMapProxyModel::TreeMapProxyModel(QObject* parent)
    :TreeMapProxyModel(*new TreeMapProxyModelPrivate(this), parent)
{}

/*!
Constructor used only while subclassing the private class.
Not part of the public API
*/
TreeMapProxyModel::TreeMapProxyModel(TreeMapProxyModelPrivate& dptr, QObject* parent)
    : QAbstractProxyModel(parent)
    , d_ptr(&dptr)
{}


/*!
\internal
*/
void TreeMapProxyModelPrivate::rebuildTreeMap(const QModelIndex& parent, TreeMapProxyNode* container)
{
    Q_Q(TreeMapProxyModel);
    Q_ASSERT(q->sourceModel());
    Q_ASSERT(container);
    const int numRows = q->sourceModel()->rowCount(parent);
    const int numCols = q->sourceModel()->columnCount(parent);
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            const QModelIndex currIdx = q->sourceModel()->index(i, j, parent);
            if (q->sourceModel()->hasChildren(currIdx)) {
                auto newNode = new TreeMapProxyNode(new QPersistentModelIndex(currIdx), container);
                rebuildTreeMap(currIdx, newNode);
                container->children.append(newNode);
            }
        }
    }
}

/*!
Rebuilds the entire mapping for the source model
*/
void TreeMapProxyModel::rebuildTreeMap()
{
    clearTreeMap();
    if (!sourceModel())
        return;
    Q_D(TreeMapProxyModel);
    d->rebuildTreeMap(QModelIndex(),&d->m_treeMapper);
}

/*!
Clears all mapped items
*/
void TreeMapProxyModel::clearTreeMap()
{
    Q_D(TreeMapProxyModel);
    d->m_treeMapper.clear();
}

/*!
\internal
*/
TreeMapProxyNode* TreeMapProxyModelPrivate::findNode(const QModelIndex& sourceIdx) const
{
    if (!sourceIdx.isValid())
        return Q_NULLPTR;
    Q_ASSERT(q_func()->sourceModel());
    Q_ASSERT(sourceIdx.model() == q_func()->sourceModel());
    const TreeMapProxyNode* const  parentNode = sourceIdx.parent().isValid() ? findNode(sourceIdx.parent()) : &m_treeMapper;
    const auto childIter = std::find_if(parentNode->children.cbegin(), parentNode->children.cend(), [&sourceIdx](const TreeMapProxyNode* const node) {return sourceIdx == *node->sourceIdx; }); 
    Q_ASSERT(childIter != parentNode->children.cend());
    return *childIter;
}

/*!
\brief Returns a pointer that can be used as internalPointer argument for createIndex that maps to \a sourceIdx
\sa internalPointerToIndex
*/
void* TreeMapProxyModel::indexToInternalPoiner(const QModelIndex& sourceIdx) const
{
    Q_D(const TreeMapProxyModel);
    TreeMapProxyNode* const nodeIter = d->findNode(sourceIdx);
    if (d->nodeFound(nodeIter))
        return nodeIter;
    return Q_NULLPTR;
}

/*!
\brief Returns an index of the source model that maps to the data in \a internalPoiner
\sa indexToInternalPoiner
*/
QModelIndex TreeMapProxyModel::internalPointerToIndex(void* internalPoiner) const 
{
    if (!internalPoiner)
        return QModelIndex();
    TreeMapProxyNode* const nodeIter = reinterpret_cast<TreeMapProxyNode*>(internalPoiner);
    Q_ASSERT(nodeIter->sourceIdx->model() == sourceModel());
    return *nodeIter->sourceIdx;
}

/*!
\internal
*/
bool TreeMapProxyModelPrivate::nodeFound(const TreeMapProxyNode* node) const
{
    return node;
}



/*!
Checks \a parent if it needs to be added to the mapping
*/
void TreeMapProxyModel::possibleNewParent(const QModelIndex& possibleParent)
{
    if (!sourceModel() || !possibleParent.isValid())
        return;
    Q_ASSERT(sourceModel() == possibleParent.model());
    Q_D(TreeMapProxyModel);
    if (d->nodeFound(d->findNode(possibleParent)))
        return;
    if (sourceModel()->hasChildren(possibleParent)) {
        TreeMapProxyNode* const  parentNode = possibleParent.parent().isValid() ? d->findNode(possibleParent.parent()) : &d->m_treeMapper;
        d->rebuildTreeMap(possibleParent, parentNode);
        parentNode->children.append(new TreeMapProxyNode(new QPersistentModelIndex(possibleParent), parentNode));
    }
}

/*!
Checks \a parent if it needs to be removed from the mapping
*/
void TreeMapProxyModel::possibleRemovedParent(const QModelIndex& possibleParent)
{
    if (!sourceModel() || !possibleParent.isValid())
        return;
    Q_ASSERT(sourceModel() == possibleParent.model());
    Q_D(TreeMapProxyModel);
    TreeMapProxyNode* oldNode = d->findNode(possibleParent);
    if (!d->nodeFound(oldNode))
        return;
    if (!sourceModel()->hasChildren(possibleParent)) {
        TreeMapProxyNode* parentNode = oldNode->parentNode ? &d->m_treeMapper : oldNode->parentNode;
        const auto childIter = std::find_if(parentNode->children.begin(), parentNode->children.end(), [oldNode](const TreeMapProxyNode* const node) {return oldNode == node; });
        Q_ASSERT(childIter != parentNode->children.end());
        parentNode->children.erase(childIter);
        delete oldNode;
    }
}


/*!
\internal
*/
void TreeMapProxyModelPrivate::onRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent)
{
    Q_UNUSED(sourceStart)
    Q_UNUSED(sourceEnd)
    Q_Q(TreeMapProxyModel);
    if (sourceParent == destinationParent)
        return;
    q->possibleNewParent(destinationParent);
    q->possibleRemovedParent(sourceParent);
}

/*!
\internal
*/
void TreeMapProxyModelPrivate::onColsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent)
{
    Q_UNUSED(sourceStart)
    Q_UNUSED(sourceEnd)
    Q_Q(TreeMapProxyModel);
    if (sourceParent == destinationParent)
        return;
    q->possibleNewParent(destinationParent);
    q->possibleRemovedParent(sourceParent);
}


/*!
\reimp
*/
void TreeMapProxyModel::setSourceModel(QAbstractItemModel* newSourceModel)
{
    if (newSourceModel == sourceModel())
        return;
    Q_D(TreeMapProxyModel);
    if (sourceModel()) {
        for (auto discIter = d->m_sourceConnections.cbegin(), discEnd = d->m_sourceConnections.cend(); discIter != discEnd; ++discIter)
            QObject::disconnect(*discIter);
    }
    d->m_sourceConnections.clear();
    QAbstractProxyModel::setSourceModel(newSourceModel);
    if (sourceModel()) {
        d->m_sourceConnections
        << QObject::connect(sourceModel(), &QAbstractItemModel::destroyed, [this]()->void { setSourceModel(Q_NULLPTR); })
        << QObject::connect(sourceModel(), &QAbstractItemModel::modelReset, this, static_cast<void (TreeMapProxyModel::*)(void)>(&TreeMapProxyModel::rebuildTreeMap))
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsInserted, this, &TreeMapProxyModel::possibleNewParent)
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsRemoved, this, &TreeMapProxyModel::possibleRemovedParent)
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsMoved, [d](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent)->void { 
            d->onColsMoved(sourceParent, sourceStart, sourceEnd, destinationParent);
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &TreeMapProxyModel::possibleNewParent)
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &TreeMapProxyModel::possibleRemovedParent)
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsMoved, [d](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent)->void {
            d->onRowsMoved(sourceParent, sourceStart, sourceEnd, destinationParent);
        })
        ;
    }
    rebuildTreeMap();
}

/*!
\class TransposeProxyModel
\brief Convenience class to map parent indexes.
\details Qt does not expose an easy way to maintain a mapping between the source model parent indexes and the proxy model ones, this proxy model tries to solve this.

This proxy will maintain a list of all the items in the source model for which `hasChildren()` returns true. 
You can then use indexToInternalPoiner() and internalPointerToIndex() to create a mapping between the source model parent index and an internal pointer you can pass to `createIndex()`.
*/