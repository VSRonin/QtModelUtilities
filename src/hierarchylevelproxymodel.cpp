/****************************************************************************\
   Copyright 2022 Luca Beldi
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
#include "private/hierarchylevelproxymodel_p.h"
#include "hierarchylevelproxymodel.h"
#include <QSize>

int HierarchyLevelProxyModelPrivate::rootOf(QModelIndex sourceIndex) const
{
    Q_ASSERT(sourceIndex.isValid());
    Q_ASSERT(sourceIndex.model() == q_func()->sourceModel());
    do {
        sourceIndex = sourceIndex.parent();
        for (int i = 0; i < m_roots.size(); ++i) {
            if (m_roots.at(i).root == sourceIndex)
                return i;
        }
    } while (sourceIndex.isValid());
    return -1;
}

int HierarchyLevelProxyModelPrivate::levelOf(QModelIndex idx)
{
    int result = 0;
    for (; idx.isValid(); idx = idx.parent())
        ++result;
    return result;
}

bool HierarchyLevelProxyModelPrivate::isAncestor(const QModelIndex &targetparent, QModelIndex root)
{
    if (!targetparent.isValid())
        return true;
    for (; root.parent().isValid(); root = root.parent()) {
        if (root.parent() == targetparent)
            return true;
    }
    return false;
}

bool HierarchyLevelProxyModelPrivate::inexistentAtSource(const QModelIndex &idx) const
{
    return idx.internalPointer() == &m_inexistentSourceIndexFlag;
}

void HierarchyLevelProxyModelPrivate::rebuildMapping()
{
    m_roots.clear();
    m_maxCol = 0;
    Q_Q(HierarchyLevelProxyModel);
    if (!q->sourceModel())
        return;
    if (m_targetLevel == 0) {
        m_roots.append(HierarchyRootData(QModelIndex(), 0));
        m_maxCol = q->sourceModel()->columnCount();
        return;
    }
    int rootsRowCount = 0;
    rebuildMappingBranch(QModelIndex(), 0, rootsRowCount);
}

void HierarchyLevelProxyModelPrivate::rebuildMappingBranch(const QModelIndex &parent, int levl, int &rootsRowCount)
{
    Q_Q(HierarchyLevelProxyModel);
    for (int i = 0, iMax = q->sourceModel()->rowCount(parent); i < iMax; ++i) {
        const QModelIndex currIdx = q->sourceModel()->index(i, 0, parent);
        if (levl == m_targetLevel - 1) {
            m_roots.append(HierarchyRootData(currIdx, rootsRowCount));
            rootsRowCount += q->sourceModel()->rowCount(currIdx);
            m_maxCol = qMax(m_maxCol, q->sourceModel()->columnCount(currIdx));
        } else {
            rebuildMappingBranch(currIdx, levl + 1, rootsRowCount);
        }
    }
}

void HierarchyLevelProxyModelPrivate::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_Q(HierarchyLevelProxyModel);
    Q_ASSERT(topLeft.isValid() && bottomRight.isValid() && topLeft.model() == q->sourceModel() && topLeft.model() == bottomRight.model()
             && bottomRight.parent() == topLeft.parent());
    const QModelIndex mappedTopLeft = q->mapFromSource(topLeft);
    if (mappedTopLeft.isValid())
        Q_EMIT q->dataChanged(mappedTopLeft, q->mapFromSource(bottomRight), roles);
}

void HierarchyLevelProxyModelPrivate::onHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
    Q_Q(HierarchyLevelProxyModel);
    Q_EMIT q->headerDataChanged(orientation, first, last);
}

void HierarchyLevelProxyModelPrivate::onColumnsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    Q_Q(HierarchyLevelProxyModel);
    Q_ASSERT(first >= 0 && first <= m_maxCol && last >= first);
    const QModelIndex mappedParent = q->mapFromSource(parent);
    if (mappedParent.isValid())
        return q->beginInsertColumns(mappedParent, first, last);
    const int currColumnCount = q->sourceModel()->columnCount(parent);
    const int proxyRowCount = q->rowCount();
    for (auto &root : qAsConst(m_roots)) {
        if (root.root == parent) {
            if (m_maxCol >= currColumnCount + last - first + 1)
                return;
            // calculate if it's more efficient to add the column in place or at the end
            const int currRowCount = q->sourceModel()->rowCount(parent);
            if (currColumnCount == first || currRowCount == 0
                || static_cast<double>(currRowCount) / std::max(0.01, static_cast<double>(proxyRowCount)) < 0.5)
                // append at the end
                return q->beginInsertColumns(QModelIndex(), m_maxCol, m_maxCol + last - first);
            return q->beginInsertColumns(QModelIndex(), first, last);
        }
        if (first == 0 && isAncestor(parent, root.root))
            return q->beginResetModel();
    }
}

void HierarchyLevelProxyModelPrivate::onColumnsAboutToBeMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination,
                                                              int column)
{
    Q_Q(HierarchyLevelProxyModel);
    const QModelIndex mappedParent = q->mapFromSource(parent);
    const QModelIndex mappedDestination = q->mapFromSource(destination);
    if(mappedParent.isValid() && mappedDestination.isValid()){
        Q_ASSUME(q->beginMoveColumns(mappedParent, start, end,mappedDestination,column));
        return;
    }
    const int currColumnCount = q->sourceModel()->columnCount(parent);
    const int proxyRowCount = q->rowCount();
    for (auto &root : qAsConst(m_roots)) {
        if (root.root == parent) {
            if(mappedDestination.isValid()){
                if(currColumnCount==m_maxCol){
                    for (auto i=m_roots.cbegin(), iEnd=m_roots.cend();i!=iEnd;++i){
                        if(root.root != i->root && q->sourceModel()->columnCount(i->root)==m_maxCol){
                            // destination inserts new col, source just dataChanged
                            q->beginInsertColumns(destination,column,column+end-start);
                            return;
                        }
                    }
                    // TODO
                }
                // destination inserts new col, source just dataChanged
                q->beginInsertColumns(destination,column,column+end-start);
                return;
            }
            else if(parent==destination)
                return; //just needs dataChanged
            else{
                for (auto &rootIter : qAsConst(m_roots)){
                     if (rootIter.root == destination){
                         // TODO
                     }
                }
            }
        }
        if ((start == 0 && isAncestor(parent, root.root)) || (column==0 && isAncestor(destination, root.root)))
            return q->beginResetModel();
    }
}

void HierarchyLevelProxyModelPrivate::onColumnsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    Q_Q(HierarchyLevelProxyModel);
    Q_ASSERT(first >= 0 && first <= m_maxCol && last >= first);
    const QModelIndex mappedParent = q->mapFromSource(parent);
    if (mappedParent.isValid())
        return q->beginRemoveColumns(mappedParent, first, last);
    const int currColumnCount = q->sourceModel()->columnCount(parent);
    const int proxyRowCount = q->rowCount();
    for (auto &root : qAsConst(m_roots)) {
        if (root.root == parent) {
            Q_ASSERT(first < currColumnCount);
            if(currColumnCount==m_maxCol){
                int secondMaxColCount = 0;
                for (auto i=m_roots.cbegin(), iEnd=m_roots.cend();i!=iEnd;++i){
                    if(root.root != i->root)
                        continue;
                    const int tempColCount = q->sourceModel()->columnCount(i->root);
                    if(tempColCount==m_maxCol)
                        return; //just needs dataChanged
                     if(tempColCount>secondMaxColCount)
                        secondMaxColCount=tempColCount;
                }
                const int numColsToRemove = std::min(last-first+1,m_maxCol-secondMaxColCount); // TODO add test for when m_maxCol-secondMaxColCount prevails
                // calculate if it's more efficient to remove the column in place or at the end
                const int currRowCount = q->sourceModel()->rowCount(parent);
                if (m_maxCol-1 == last
                    || currRowCount == 0
                    || static_cast<double>(currRowCount) / std::max(0.01, static_cast<double>(proxyRowCount)) < 0.5
                )// remove at the end
                    return q->beginRemoveColumns(QModelIndex(),m_maxCol-numColsToRemove, m_maxCol-1);
                return q->beginRemoveColumns(QModelIndex(), first, first+numColsToRemove-1);
            }
            return; //just needs dataChanged
        }
        if (first == 0 && isAncestor(parent, root.root))
            return q->beginResetModel();
    }
}

void HierarchyLevelProxyModelPrivate::onColumnsInserted(const QModelIndex &parent, int first, int last)
{
    Q_Q(HierarchyLevelProxyModel);
    Q_ASSERT(first >= 0 && first <= m_maxCol && last >= first);
    if (q->mapFromSource(parent).isValid())
        return q->endInsertColumns();
    const int currColumnCount = q->sourceModel()->columnCount(parent);
    const int currRowCount = q->sourceModel()->rowCount(parent);
    const int proxyRowCount = q->rowCount();
    for (int i = 0; i < m_roots.size(); ++i) {
        if (m_roots.at(i).root == parent) {
            if (m_maxCol < currColumnCount) {
                const int oldMaxCol = m_maxCol;
                m_maxCol = currColumnCount;
                q->endInsertColumns();
                if (oldMaxCol == first)
                    return;
                if (currRowCount == 0 || static_cast<double>(currRowCount) / std::max(0.01, static_cast<double>(proxyRowCount)) < 0.5) {
                    if (currRowCount > 0)
                        Q_EMIT q->dataChanged(q->index(m_roots.at(i).cachedCumRowCount, first),
                                              q->index(m_roots.at(i).cachedCumRowCount + currRowCount - 1, m_maxCol - 1));
                } else {
                    if (m_roots.at(i).cachedCumRowCount > 0)
                        Q_EMIT q->dataChanged(q->index(0, first), q->index(m_roots.at(i).cachedCumRowCount - 1, m_maxCol - 1));
                    if (proxyRowCount > m_roots.at(i).cachedCumRowCount + currRowCount)
                        Q_EMIT q->dataChanged(q->index(m_roots.at(i).cachedCumRowCount + currRowCount, first),
                                              q->index(proxyRowCount - 1, m_maxCol - 1));
                }
            } else if (currRowCount > 0) {
                Q_EMIT q->dataChanged(q->index(m_roots.at(i).cachedCumRowCount, first),
                                      q->index(m_roots.at(i).cachedCumRowCount + currRowCount - 1, m_maxCol - 1));
            }
            return;
        } else if (first == 0 && isAncestor(parent, m_roots.at(i).root)) {
            rebuildMapping();
            return q->endResetModel();
        }
    }
    if (levelOf(parent) == m_targetLevel - 1 && currRowCount > 0 && currColumnCount - (last - first + 1) == 0) {
        auto rootI = m_roots.begin();
        int newCachedCumRowCount = 0;
        for (; rootI != m_roots.end(); ++rootI) {
            if (rootI->root.row() > parent.row())
                break;
            newCachedCumRowCount = rootI->cachedCumRowCount;
        }
        if (rootI == m_roots.end()) {
            for (int i = 0; i < currRowCount; ++i)
                m_roots.append(HierarchyRootData(q->sourceModel()->index(i, 0, parent), newCachedCumRowCount));
        } else {
            for (int i = 0; i < currRowCount; ++i)
                m_roots.insert(rootI, HierarchyRootData(q->sourceModel()->index(i, 0, parent), newCachedCumRowCount));
            for (; rootI != m_roots.end(); ++rootI)
                rootI->cachedCumRowCount += currRowCount;
        }
    }
}

void HierarchyLevelProxyModelPrivate::onColumnsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::onColumnsRemoved(const QModelIndex &parent, int first, int last)
{
    Q_Q(HierarchyLevelProxyModel);
    Q_ASSERT(first >= 0 && first <= m_maxCol && last >= first);
    if (q->mapFromSource(parent).isValid())
        return q->endRemoveColumns();
    const int currColumnCount = q->sourceModel()->columnCount(parent);
    const int currRowCount = q->sourceModel()->rowCount(parent);
    const int oldColumnCount = currColumnCount -last+first-1;
    const int proxyRowCount = q->rowCount();
    for (auto root = m_roots.cbegin(), rootEnd = m_roots.cend();root!=rootEnd;++root) {
        if (root->root == parent) {
            if(oldColumnCount==m_maxCol){
                int newMaxCol = 0;
                for (auto i=m_roots.cbegin(), iEnd=m_roots.cend();i!=iEnd;++i){
                    const int iColCount = q->sourceModel()->columnCount(i->root);
                    if(root->root != i->root && iColCount==m_maxCol){
                        Q_EMIT q->dataChanged(q->index(root->cachedCumRowCount, first), q->index(root->cachedCumRowCount+currRowCount-1, m_maxCol-1));
                        return;
                    }
                    if(iColCount>newMaxCol)
                    newMaxCol = iColCount;
                }
                m_maxCol=newMaxCol;
                q->endRemoveColumns();
                // calculate if it's more efficient to remove the column in place or at the end
                // TODO change to match changes in onColumnsAboutToBeRemoved
                if (oldColumnCount == first || currRowCount == 0
                    || static_cast<double>(currRowCount) / std::max(0.01, static_cast<double>(proxyRowCount)) < 0.5){
                    // remove at the end
                    Q_EMIT q->dataChanged(q->index(root->cachedCumRowCount, first), q->index(root->cachedCumRowCount+currRowCount-1, m_maxCol-1));
                }
                else{
                    if(root->cachedCumRowCount>0)
                        Q_EMIT q->dataChanged(q->index(0, first), q->index(root->cachedCumRowCount-1, m_maxCol-1));
                    if(std::distance(root,rootEnd)>1)
                        Q_EMIT q->dataChanged(q->index(root->cachedCumRowCount+currRowCount, first), q->index(proxyRowCount, m_maxCol-1));
                }
            }
            Q_EMIT q->dataChanged(q->index(root->cachedCumRowCount, first), q->index(root->cachedCumRowCount+currRowCount-1, m_maxCol-1));
            return;
        }
        if (first == 0 && isAncestor(parent, root->root)){
            //TODO check if still relevant here
            rebuildMapping();
            return q->endResetModel();
        }
    }

}

void HierarchyLevelProxyModelPrivate::onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    Q_Q(HierarchyLevelProxyModel);
    for (auto i = m_roots.begin(), iEnd = m_roots.end(); i != iEnd; ++i) {
        if (i->root == parent) {
            q->beginInsertRows(QModelIndex(), i->cachedCumRowCount + first, i->cachedCumRowCount + last);
            return;
        }
    }
    const QModelIndex mappedParent = q->mapFromSource(parent);
    if (mappedParent.isValid())
        q->beginInsertRows(mappedParent, first, last);
}

void HierarchyLevelProxyModelPrivate::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_Q(HierarchyLevelProxyModel);
    for (auto i = m_roots.begin(), iEnd = m_roots.end(); i != iEnd; ++i) {
        if (i->root == parent) {
            for (++i; i != iEnd; ++i)
                i->cachedCumRowCount += last - first + 1;
            q->endInsertRows();
            return;
        }
    }
    if (levelOf(parent) == m_targetLevel - 1 && q->sourceModel()->columnCount(parent) > 0) {
        int newCachedCumRowCount = 0;
        if (first < m_roots.size())
            newCachedCumRowCount = m_roots.at(first).cachedCumRowCount;
        else if (!m_roots.isEmpty())
            newCachedCumRowCount = m_roots.last().cachedCumRowCount + q->sourceModel()->rowCount(m_roots.last().root);
        for (int i = first; i <= last; ++i)
            m_roots.insert(i, HierarchyRootData(q->sourceModel()->index(i, 0, parent), newCachedCumRowCount));
    } else if (q->mapFromSource(parent).isValid())
        q->endInsertRows();
}

void HierarchyLevelProxyModelPrivate::onRowsAboutToBeMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::onRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    Q_Q(HierarchyLevelProxyModel);
    Q_ASSERT(m_rootsIndexesToRemove.isEmpty());
    std::pair<int, int> rootsAboutToBeRemoved(-1, -1);
    for (int i = 0, iEnd = m_roots.count(); i < iEnd; ++i) {
        if (m_roots.at(i).root == parent) {
            q->beginRemoveRows(QModelIndex(), m_roots.at(i).cachedCumRowCount + first, m_roots.at(i).cachedCumRowCount + last);
            return;
        }
        if (isAncestor(parent, m_roots.at(i).root)) {
            int targetRow = m_roots.at(i).root.row();
            for (QModelIndex parentIter = m_roots.at(i).root.parent(); parentIter != parent; parentIter = parentIter.parent())
                targetRow = parentIter.row();
            if (targetRow >= first && targetRow <= last) {
                const int countRowsRemoved = q->sourceModel()->rowCount(m_roots.at(i).root);
                m_rootsIndexesToRemove.append(std::make_pair(i, countRowsRemoved > 0));
                if (countRowsRemoved > 0) {
                    if (rootsAboutToBeRemoved.first < 0)
                        rootsAboutToBeRemoved.first = m_roots.at(i).cachedCumRowCount;
                    Q_ASSERT(rootsAboutToBeRemoved.second <= m_roots.at(i).cachedCumRowCount + countRowsRemoved - 1);
                    rootsAboutToBeRemoved.second = m_roots.at(i).cachedCumRowCount + countRowsRemoved - 1;
                }
            }
        }
    }
    const QModelIndex mappedParent = q->mapFromSource(parent);
    if (mappedParent.isValid())
        return q->beginRemoveRows(mappedParent, first, last);
    if (rootsAboutToBeRemoved.first >= 0)
        return q->beginRemoveRows(QModelIndex(), rootsAboutToBeRemoved.first, rootsAboutToBeRemoved.second);
}

void HierarchyLevelProxyModelPrivate::onRowsRemoved(const QModelIndex &parent, int first, int last)
{
    Q_Q(HierarchyLevelProxyModel);
    if (!m_rootsIndexesToRemove.isEmpty()) {
        Q_ASSERT(std::is_sorted(m_rootsIndexesToRemove.cbegin(), m_rootsIndexesToRemove.cend()));
        bool needEndRemoveRows = false;
        for (auto i = m_rootsIndexesToRemove.crbegin(), iEnd = m_rootsIndexesToRemove.crend(); i != iEnd; ++i) {
            needEndRemoveRows = needEndRemoveRows || i->second;
            if (i->first < m_roots.count() - 1) {
                const int cachedCumRowAdjustment = m_roots[(i->first) + 1].cachedCumRowCount - m_roots.at(i->first).cachedCumRowCount;
                for (int j = (i->first) + 1; j < m_roots.count(); ++j)
                    m_roots[j].cachedCumRowCount -= cachedCumRowAdjustment;
            }
            m_roots.removeAt(i->first);
        }
        m_rootsIndexesToRemove.clear();
        if (needEndRemoveRows)
            q->endRemoveRows();
        return;
    }
    for (auto i = m_roots.begin(); i != m_roots.end(); ++i) {
        if (i->root == parent) {
            for (++i; i != m_roots.end(); ++i)
                i->cachedCumRowCount -= last - first + 1;
            q->endRemoveRows();
            return;
        }
    }
    if (q->mapFromSource(parent).isValid())
        q->endRemoveRows();
}

void HierarchyLevelProxyModelPrivate::beforeLayoutChange(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::afetrLayoutChange(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::afterReset()
{
    rebuildMapping();
    Q_Q(HierarchyLevelProxyModel);
    q->endResetModel();
}

HierarchyLevelProxyModelPrivate::HierarchyLevelProxyModelPrivate(HierarchyLevelProxyModel *q)
    : q_ptr(q)
    , m_maxCol(0)
    , m_targetLevel(0)
    , m_insertBehaviour(HierarchyLevelProxyModel::InsertToNext)
    , m_inexistentSourceIndexFlag(0)
{
    Q_ASSERT(q_ptr);
}

/*!
Constructs a new proxy model with the given \a parent.
*/
HierarchyLevelProxyModel::HierarchyLevelProxyModel(QObject *parent)
    : HierarchyLevelProxyModel(*new HierarchyLevelProxyModelPrivate(this), parent)
{ }

/*!
\internal
*/
HierarchyLevelProxyModel::HierarchyLevelProxyModel(HierarchyLevelProxyModelPrivate &dptr, QObject *parent)
    : QAbstractProxyModel(parent)
    , m_dptr(&dptr)
{ }

/*!
Destructor
*/
HierarchyLevelProxyModel::~HierarchyLevelProxyModel()
{
    delete m_dptr;
}

/*!
\reimp
*/
void HierarchyLevelProxyModel::setSourceModel(QAbstractItemModel *newSourceModel)
{
    if (newSourceModel == sourceModel())
        return;
    Q_D(HierarchyLevelProxyModel);
    beginResetModel();
    for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
        QObject::disconnect(*discIter);
    d->m_sourceConnections.clear();
    QAbstractProxyModel::setSourceModel(newSourceModel);
    d->rebuildMapping();
    if (sourceModel()) {
        using namespace std::placeholders;
        d->m_sourceConnections << QObject::connect(sourceModel(), &QAbstractItemModel::dataChanged, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onDataChanged, d, _1, _2, _3))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::headerDataChanged, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onHeaderDataChanged, d, _1, _2, _3))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::beforeLayoutChange, d, _1, _2))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::layoutChanged, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::afetrLayoutChange, d, _1, _2))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeInserted, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onColumnsAboutToBeInserted, d, _1, _2, _3))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeMoved, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onColumnsAboutToBeMoved, d, _1, _2, _3, _4, _5))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeRemoved, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onColumnsAboutToBeRemoved, d, _1, _2, _3))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::columnsInserted, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onColumnsInserted, d, _1, _2, _3))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::columnsRemoved, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onColumnsRemoved, d, _1, _2, _3))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::columnsMoved, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onColumnsMoved, d, _1, _2, _3, _4, _5))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onRowsAboutToBeInserted, d, _1, _2, _3))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeMoved, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onRowsAboutToBeMoved, d, _1, _2, _3, _4, _5))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onRowsAboutToBeRemoved, d, _1, _2, _3))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::rowsInserted, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onRowsInserted, d, _1, _2, _3))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onRowsRemoved, d, _1, _2, _3))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::rowsMoved, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::onRowsMoved, d, _1, _2, _3, _4, _5))
                               << QObject::connect(sourceModel(), &QAbstractItemModel::modelAboutToBeReset, this,
                                                   &HierarchyLevelProxyModel::beginResetModel)
                               << QObject::connect(sourceModel(), &QAbstractItemModel::modelReset, this,
                                                   std::bind(&HierarchyLevelProxyModelPrivate::afterReset, d));
    }
    endResetModel();
}

/*!
\reimp
*/
int HierarchyLevelProxyModel::rowCount(const QModelIndex &parent) const
{
    if (!sourceModel())
        return 0;
    Q_D(const HierarchyLevelProxyModel);
    if (d->inexistentAtSource(parent))
        return 0;
    if (!parent.isValid()) {
        if (d->m_roots.isEmpty())
            return 0;
        const auto &lastRoot = d->m_roots.last();
        return sourceModel()->rowCount(lastRoot.root) + lastRoot.cachedCumRowCount;
    }
    return sourceModel()->rowCount(mapToSource(parent));
}

/*!
\reimp
*/
int HierarchyLevelProxyModel::columnCount(const QModelIndex &parent) const
{
    if (!sourceModel())
        return 0;
    Q_D(const HierarchyLevelProxyModel);
    if (d->inexistentAtSource(parent))
        return 0;
    if (!parent.isValid())
        return d->m_maxCol;
    return sourceModel()->columnCount(mapToSource(parent));
}

/*!
\reimp
*/
QVariant HierarchyLevelProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!sourceModel())
        return QVariant();
    return sourceModel()->headerData(section, orientation, role);
}

/*!
\reimp
*/
bool HierarchyLevelProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (!sourceModel())
        return false;
    return sourceModel()->setHeaderData(section, orientation, value, role);
}

/*!
\reimp
*/
QVariant HierarchyLevelProxyModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || !index.isValid() || d->inexistentAtSource(index))
        return QVariant();
    return sourceModel()->data(mapToSource(index), role);
}

/*!
\reimp
*/
bool HierarchyLevelProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || !index.isValid() || d->inexistentAtSource(index))
        return false;
    return sourceModel()->setData(mapToSource(index), value, role);
}

/*!
\reimp
*/
void HierarchyLevelProxyModel::multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || !index.isValid() || d->inexistentAtSource(index)) {
        for (QModelRoleData &roleData : roleDataSpan)
            roleData.setData(QVariant());
        return;
    }
    return sourceModel()->multiData(mapToSource(index), roleDataSpan);
}

/*!
\reimp
*/
bool HierarchyLevelProxyModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || !index.isValid() || d->inexistentAtSource(index))
        return false;
    return sourceModel()->setItemData(mapToSource(index), roles);
}

/*!
\reimp
*/
bool HierarchyLevelProxyModel::clearItemData(const QModelIndex &index)
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || !index.isValid() || d->inexistentAtSource(index))
        return false;
    return sourceModel()->clearItemData(mapToSource(index));
}

/*!
\reimp
*/
QMap<int, QVariant> HierarchyLevelProxyModel::itemData(const QModelIndex &index) const
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || !index.isValid() || d->inexistentAtSource(index))
        return QMap<int, QVariant>();
    return sourceModel()->itemData(mapToSource(index));
}

/*!
\reimp
*/
QModelIndex HierarchyLevelProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    Q_ASSERT(sourceIndex.isValid() ? sourceIndex.model() == sourceModel() : true);
    if (!sourceModel())
        return QModelIndex();
    if (!sourceIndex.isValid())
        return QModelIndex();
    Q_D(const HierarchyLevelProxyModel);
    const int rootIdx = d->rootOf(sourceIndex);
    if (rootIdx < 0)
        return QModelIndex();
    const auto &rootData = d->m_roots.at(rootIdx);
    if (rootData.root == sourceIndex.parent())
        return createIndex(rootData.cachedCumRowCount + sourceIndex.row(), sourceIndex.column());
    return createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());
}

/*!
\reimp
*/
bool HierarchyLevelProxyModel::hasChildren(const QModelIndex &parent) const
{
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || d->inexistentAtSource(parent))
        return false;
    if (!parent.isValid() && d->m_targetLevel > 0)
        return columnCount() > 0 && rowCount() > 0;
    return sourceModel()->hasChildren(mapToSource(parent));
}

/*!
\reimp
*/
QModelIndex HierarchyLevelProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    Q_ASSERT(proxyIndex.isValid() ? proxyIndex.model() == this : true);
    if (!sourceModel())
        return QModelIndex();
    if (!proxyIndex.isValid())
        return QModelIndex();
    Q_D(const HierarchyLevelProxyModel);
    if (d->inexistentAtSource(proxyIndex))
        return QModelIndex();
    if (proxyIndex.internalPointer())
        return createSourceIndex(proxyIndex.row(), proxyIndex.column(), proxyIndex.internalPointer());
#ifdef QT_DEBUG
    for (int i = 1; i < d->m_roots.size(); ++i)
        Q_ASSERT(d->m_roots.at(i).cachedCumRowCount >= d->m_roots.at(i - 1).cachedCumRowCount);
#endif
    for (auto i = std::rbegin(d->m_roots), iEnd = std::rend(d->m_roots); i != iEnd; ++i) {
        if (proxyIndex.row() >= i->cachedCumRowCount)
            return sourceModel()->index(proxyIndex.row() - i->cachedCumRowCount, proxyIndex.column(), i->root);
    }
    Q_UNREACHABLE();
    return QModelIndex();
}

/*!
\reimp
*/
Qt::ItemFlags HierarchyLevelProxyModel::flags(const QModelIndex &index) const
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || d->inexistentAtSource(index))
        return Qt::NoItemFlags;
    return sourceModel()->flags(mapToSource(index));
}

/*!
\reimp
*/
QModelIndex HierarchyLevelProxyModel::parent(const QModelIndex &index) const
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || d->inexistentAtSource(index))
        return QModelIndex();
    return mapFromSource(mapToSource(index).parent());
}

/*!
\reimp
*/
QModelIndex HierarchyLevelProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (parent.isValid()) {
        const QModelIndex mappedParent = mapToSource(parent);
        if (sourceModel()->hasIndex(row, column, mappedParent))
            return mapFromSource(sourceModel()->index(row, column, mappedParent));
        return createIndex(row, column, &(d->m_inexistentSourceIndexFlag));
    }
    for (auto i = std::rbegin(d->m_roots), iEnd = std::rend(d->m_roots); i != iEnd; ++i) {
        if (row >= i->cachedCumRowCount) {
            if (sourceModel()->hasIndex(row - i->cachedCumRowCount, column, i->root))
                break;
            return createIndex(row, column, &(d->m_inexistentSourceIndexFlag));
        }
    }
    return createIndex(row, column);
}

/*!
\reimp
*/
bool HierarchyLevelProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || d->inexistentAtSource(parent))
        return false;
    if (d->m_roots.isEmpty())
        return false;
    if (!parent.isValid() && d->m_targetLevel > 0) {
        Q_ASSERT(d->m_roots.first().cachedCumRowCount == 0);
        for (auto i = std::rbegin(d->m_roots), iEnd = std::rend(d->m_roots); i != iEnd; ++i) {
            if (row > i->cachedCumRowCount) {
                if (row > i->cachedCumRowCount + sourceModel()->rowCount(i->root))
                    return false;
                return sourceModel()->insertRows(row - i->cachedCumRowCount, count, i->root);
            }
            if (row == i->cachedCumRowCount) {
                if (d->m_insertBehaviour == InsertToNext)
                    return sourceModel()->insertRows(0, count, i->root);
                for (++i; d->m_insertBehaviour == InsertToPreviousNonEmpty && row == i->cachedCumRowCount && i != iEnd; ++i) { }
                if (i == iEnd)
                    --i;
                return sourceModel()->insertRows(sourceModel()->rowCount(i->root), count, i->root);
            }
        }
    }
    return sourceModel()->insertRows(row, count, mapToSource(parent));
}

/*!
\reimp
*/
bool HierarchyLevelProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || d->inexistentAtSource(parent))
        return false;
    if (row < 0 || row + count - 1 >= rowCount(parent))
        return false;
    if (parent.isValid())
        return sourceModel()->removeRows(row, count, mapToSource(parent));
    QList<std::tuple<QModelIndex, int, int>> rowsToRemove;
    for (auto i = std::rbegin(d->m_roots), iEnd = std::rend(d->m_roots); i != iEnd; ++i) {
        if (row + count - 1 >= i->cachedCumRowCount) {
            if (row >= i->cachedCumRowCount) {
                rowsToRemove.append(std::make_tuple(i->root, row - i->cachedCumRowCount, count));
                break;
            }
            rowsToRemove.append(std::make_tuple(i->root, 0, count + row - i->cachedCumRowCount));
            count -= count + row - i->cachedCumRowCount;
        }
    }
    Q_ASSERT(!rowsToRemove.isEmpty());
    bool result = true;
    for (auto &&rowToRemove : qAsConst(rowsToRemove))
        result = result && sourceModel()->removeRows(std::get<1>(rowToRemove), std::get<2>(rowToRemove), std::get<0>(rowToRemove));
    return result;
}

/*!
\reimp
*/
bool HierarchyLevelProxyModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || d->inexistentAtSource(parent))
        return false;
    if (parent.isValid())
        return sourceModel()->insertColumns(column, count, mapToSource(parent));
    bool result = true;
    for (auto &&root : qAsConst(d->m_roots))
        result = result && sourceModel()->insertColumns(std::min(column, sourceModel()->columnCount(root.root)), count, root.root);
    return result;
}

/*!
\reimp
*/
bool HierarchyLevelProxyModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || d->inexistentAtSource(parent))
        return false;
    if (column < 0 || column + count  > columnCount(parent))
        return false;
    if (parent.isValid())
        return sourceModel()->removeRows(column, count, mapToSource(parent));
    bool result = true;
    for (auto &&root : qAsConst(d->m_roots)){
        const int branchCount =sourceModel()->columnCount(root.root);
        if(column<branchCount)
            result = result && sourceModel()->removeColumns(column, std::min(count,branchCount-column), root.root);
    }
    return result;
}

/*!
\reimp
*/
void HierarchyLevelProxyModel::sort(int column, Qt::SortOrder order)
{
    if (sourceModel())
        sourceModel()->sort(column,order);
}

/*!
\reimp
*/
QModelIndex HierarchyLevelProxyModel::buddy(const QModelIndex &index) const
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || !index.isValid() || d->inexistentAtSource(index))
        return QModelIndex();
    return mapFromSource(sourceModel()->buddy(mapToSource(index)));
}

/*!
\reimp
*/
bool HierarchyLevelProxyModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || d->inexistentAtSource(parent))
        return false;
    return sourceModel()->canDropMimeData(data, action, row, column, mapToSource(parent));
}

/*!
\reimp
*/
bool HierarchyLevelProxyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || d->inexistentAtSource(parent))
        return false;
    return sourceModel()->dropMimeData(data, action, row, column, mapToSource(parent));
}

/*!
\reimp
*/
bool HierarchyLevelProxyModel::canFetchMore(const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || d->inexistentAtSource(parent))
        return false;
    return sourceModel()->canFetchMore(mapToSource(parent));
}

/*!
\reimp
*/
void HierarchyLevelProxyModel::fetchMore(const QModelIndex &parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || d->inexistentAtSource(parent))
        return;
    return sourceModel()->fetchMore(mapToSource(parent));
}

/*!
\reimp
*/
QModelIndexList HierarchyLevelProxyModel::match(const QModelIndex &start, int role, const QVariant &value, int hits, Qt::MatchFlags flags) const
{
    Q_ASSERT(start.isValid() ? start.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || !start.isValid() || d->inexistentAtSource(start))
        return QModelIndexList();
    return sourceModel()->match(mapToSource(start), role, value, hits, flags);
}

/*!
\reimp
*/
QMimeData *HierarchyLevelProxyModel::mimeData(const QModelIndexList &indexes) const
{
    if (!sourceModel())
        return nullptr;
    Q_D(const HierarchyLevelProxyModel);
    QModelIndexList adjList;
    for (auto i : indexes) {
        if (!d->inexistentAtSource(i))
            adjList.append(mapToSource(i));
    }
    return sourceModel()->mimeData(adjList);
}

/*!
\reimp
*/
QSize HierarchyLevelProxyModel::span(const QModelIndex &index) const
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    Q_D(const HierarchyLevelProxyModel);
    if (!sourceModel() || !index.isValid())
        return QSize(0, 0);
    if (d->inexistentAtSource(index))
        return QSize(1, 1);
    return sourceModel()->span(mapToSource(index));
}

int HierarchyLevelProxyModel::hierarchyLevel() const
{
    Q_D(const HierarchyLevelProxyModel);
    return d->m_targetLevel;
}

void HierarchyLevelProxyModel::setHierarchyLevel(int hierarchyLvl)
{
    Q_D(HierarchyLevelProxyModel);
    if (hierarchyLvl < 0)
        hierarchyLvl = 0;
    if (hierarchyLvl == d->m_targetLevel)
        return;
    if (sourceModel())
        beginResetModel();
    d->m_targetLevel = hierarchyLvl;
    if (sourceModel()) {
        d->rebuildMapping();
        endResetModel();
    }
}

HierarchyLevelProxyModel::InsertBehaviour HierarchyLevelProxyModel::insertBehaviour() const
{
    Q_D(const HierarchyLevelProxyModel);
    return d->m_insertBehaviour;
}

void HierarchyLevelProxyModel::setInsertBehaviour(InsertBehaviour behave)
{
    Q_D(HierarchyLevelProxyModel);
    if (d->m_insertBehaviour == behave)
        return;
    d->m_insertBehaviour = behave;
    Q_EMIT insertBehaviourChanged(behave);
}

HierarchyRootData::HierarchyRootData(const QPersistentModelIndex &rt, int rc)
    : root(rt)
    , cachedCumRowCount(rc)
{ }

HierarchyRootData::HierarchyRootData()
    : HierarchyRootData(QPersistentModelIndex(), 0)
{ }