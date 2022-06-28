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

int HierarchyLevelProxyModelPrivate::rootOf(QModelIndex sourceIndex) const
{
    Q_ASSERT(sourceIndex.isValid());
    Q_ASSERT(sourceIndex.model() == q_func()->sourceModel());
    while(sourceIndex.isValid()){
        for(int i=0;i<m_roots.size();++i){
            if(m_roots.at(i).root==sourceIndex)
                return i;
        }
        sourceIndex=sourceIndex.parent();
    }
    return -1;
}

void HierarchyLevelProxyModelPrivate::onColumnsInserted(const QModelIndex &parent, int first, int last)
{
    onInserted(false, parent, first, last);
}

void HierarchyLevelProxyModelPrivate::onColumnsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column)
{
    onMoved(false, parent, start, end, destination, column);
}

void HierarchyLevelProxyModelPrivate::onColumnsRemoved(const QModelIndex &parent, int first, int last)
{
    onRemoved(false, parent, first, last);
}

void HierarchyLevelProxyModelPrivate::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    onInserted(true, parent, first, last);
}

void HierarchyLevelProxyModelPrivate::onRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    onMoved(true, parent, start, end, destination, row);
}

void HierarchyLevelProxyModelPrivate::onRowsRemoved(const QModelIndex &parent, int first, int last)
{
    onRemoved(true, parent, first, last);
}

HierarchyLevelProxyModelPrivate::HierarchyLevelProxyModelPrivate(HierarchyLevelProxyModel *q)
    : q_ptr(q)
    , m_maxCol(0)
    , m_targetLevel(0)
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
    Q_D(HierarchyLevelProxyModel);
    for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
        QObject::disconnect(*discIter);
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

    if (sourceModel()) {
        using namespace std::placeholders;
        d->m_sourceConnections
                << QObject::connect(sourceModel(), &QAbstractItemModel::dataChanged, this, std::bind(&HierarchyLevelProxyModelPrivate::onDataChanged,d,_1,_2,_3))
                << QObject::connect(sourceModel(), &QAbstractItemModel::headerDataChanged, this, &QAbstractItemModel::headerDataChanged)
                << QObject::connect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged,
                                    [d](const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint) {
                                        d->beforeLayoutChange(parents, hint);
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::layoutChanged,
                                    [d](const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint) {
                                        d->afetrLayoutChange(parents, hint);
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeInserted,
                                    [this](const QModelIndex &parent, int first, int last) {
                                        if (!parent.isValid()) {
                                            beginInsertColumns(QModelIndex(), first, last);
                                        }
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeMoved,
                                    [this](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent,
                                           int destinationColumn) {
                                        if (sourceParent.isValid())
                                            return;
                                        if (destinationParent.isValid())
                                            beginRemoveColumns(QModelIndex(), sourceStart, sourceEnd);
                                        else
                                            beginMoveColumns(QModelIndex(), sourceStart, sourceEnd, QModelIndex(), destinationColumn);
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeRemoved,
                                    [this](const QModelIndex &parent, int first, int last) {
                                        if (!parent.isValid()) {
                                            beginRemoveColumns(QModelIndex(), first, last);
                                        }
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsInserted,
                                    [d](const QModelIndex &parent, int first, int last) -> void { d->onColumnsInserted(parent, first, last); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsRemoved,
                                    [d](const QModelIndex &parent, int first, int last) -> void { d->onColumnsRemoved(parent, first, last); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsMoved,
                                    [d](const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column) -> void {
                                        d->onColumnsMoved(parent, start, end, destination, column);
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted,
                                    [this](const QModelIndex &parent, int first, int last) {
                                        if (!parent.isValid()) {
                                            beginInsertRows(QModelIndex(), first, last);
                                        }
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeMoved,
                                    [this](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent,
                                           int destinationRow) {
                                        if (sourceParent.isValid())
                                            return;
                                        if (destinationParent.isValid())
                                            beginRemoveRows(QModelIndex(), sourceStart, sourceEnd);
                                        else
                                            beginMoveRows(QModelIndex(), sourceStart, sourceEnd, QModelIndex(), destinationRow);
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved,
                                    [this](const QModelIndex &parent, int first, int last) {
                                        if (!parent.isValid()) {
                                            beginRemoveRows(QModelIndex(), first, last);
                                        }
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsInserted,
                                    [d](const QModelIndex &parent, int first, int last) -> void { d->onRowsInserted(parent, first, last); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsRemoved,
                                    [d](const QModelIndex &parent, int first, int last) -> void { d->onRowsRemoved(parent, first, last); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsMoved,
                                    [d](const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row) -> void {
                                        d->onRowsMoved(parent, start, end, destination, row);
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::modelAboutToBeReset, [this]() -> void { beginResetModel(); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::modelReset, [d]() -> void { d->afterReset(); });
        const int sourceCols = sourceModel()->columnCount();
        const int sourceRows = sourceModel()->rowCount();
        for (int i = 0; i < sourceRows; ++i)
            d->m_extraData[0].append(RolesContainer());
        for (int i = 0; i < sourceCols; ++i)
            d->m_extraData[1].append(RolesContainer());
    }

    endResetModel();
}

/*!
\internal
*/
void InsertProxyModelPrivate::afterReset()
{
    Q_Q(InsertProxyModel);
    for (auto &extraData : m_extraData)
        extraData.clear();
    const int sourceCols = q->sourceModel()->columnCount();
    const int sourceRows = q->sourceModel()->rowCount();
    for (int i = 0; i < sourceRows; ++i)
        m_extraData[0].append(RolesContainer());
    for (int i = 0; i < sourceCols; ++i)
        m_extraData[1].append(RolesContainer());
    q->endResetModel();
}

/*!
\reimp
*/
int HierarchyLevelProxyModel::rowCount(const QModelIndex &parent) const
{
    if (!sourceModel())
        return 0;
    Q_D(const HierarchyLevelProxyModel);
    if (!parent.isValid()){
        if(d->m_roots.isEmpty())
            return 0;
        const auto& lastRoot = d->m_roots.last();
        return sourceModel()->rowCount(lastRoot.root)+lastRoot.cachedCumRowCount;
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
    if (!parent.isValid())
        return d->m_maxCol;
    return sourceModel()->columnCount(mapToSource(parent));
}

/*!
\reimp
*/
QVariant HierarchyLevelProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(!sourceModel())
        return QVariant();
    if(orientation == Qt::Horizontal)
        return sourceModel()->headerData(section, orientation,  role);
    return QAbstractProxyModel::headerData(section, orientation,  role);
}



/*!
\reimp
*/
bool HierarchyLevelProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (!sourceModel())
        return false;
    if(orientation == Qt::Horizontal)
        return sourceModel()->setHeaderData(section, orientation, value, role);
    return QAbstractProxyModel::setHeaderData(section, orientation, value, role);
}

#if (QT_VERSION < QT_VERSION_CHECK(6, 3, 0))
/*!
\reimp
*/
bool HierarchyLevelProxyModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles){
    if(!sourceModel())
        return false;
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    return sourceModel()->setItemData(mapToSource(index), roles);
}
/*!
\reimp
*/
QMap<int, QVariant> HierarchyLevelProxyModel::itemData(const QModelIndex &index) const{
    if(!sourceModel())
        return QMap<int, QVariant>();
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    return sourceModel()->itemData(mapToSource(index));
}
#endif

/*!
\reimp
*/
QModelIndex HierarchyLevelProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    Q_ASSERT(sourceIndex.isValid() ? sourceIndex.model() == sourceModel() : true);
    if(!sourceModel())
        return QModelIndex();
    if (!sourceIndex.isValid())
        return QModelIndex();
    Q_D(const HierarchyLevelProxyModel);
    const int rootIdx = d->rootOf(sourceIndex);
    if(rootIdx<0)
        return QModelIndex();
    const auto& rootData = d->m_roots.at(rootIdx);
    if(rootData.root == sourceIndex.parent())
        return createIndex(rootData.cachedCumRowCount + sourceIndex.row(),sourceIndex.column());
    return createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());
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
    if(proxyIndex.parent().isValid() || d->m_roots.size()==1)
        return createSourceIndex(proxyIndex.row(), proxyIndex.column(), proxyIndex.internalPointer());
    if(proxyIndex.row()==0) // optimisation
        return sourceModel()->index(0, proxyIndex.column(),d->m_roots.first().root);
#ifdef QT_DEBUG
    for(int i=1;i<d->m_roots.size();++i)
        Q_ASSERT(d->m_roots.at(i).cachedCumRowCount>d->m_roots.at(i-1).cachedCumRowCount);
#endif
   for(const auto& rootData : d->m_roots){
       if(proxyIndex.row()>=rootData.cachedCumRowCount)
           return sourceModel()->index(proxyIndex.row()-rootData.cachedCumRowCount, proxyIndex.column(),rootData.root);
   }
}

/*!
\reimp
*/
Qt::ItemFlags HierarchyLevelProxyModel::flags(const QModelIndex &index) const
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    if (!sourceModel())
        return Qt::NoItemFlags;
    return sourceModel()->flags(mapToSource(index));
}

/*!
\reimp
*/
QModelIndex HierarchyLevelProxyModel::parent(const QModelIndex &index) const
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    return mapFromSource(mapToSource(index).parent());
}

/*!
\reimp
*/
QModelIndex HierarchyLevelProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    return mapFromSource(sourceModel()->index(row,column,mapToSource(parent)));
}
/*!
\reimp
*/
bool InsertProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;
    if (!sourceModel())
        return false;
    if (row < 0 || row > sourceModel()->rowCount())
        return false;
    return sourceModel()->insertRows(row, count);
}
/*!
\reimp
*/
bool InsertProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;
    if (!sourceModel())
        return false;
    if (row < 0 || row > sourceModel()->rowCount())
        return false;
    return sourceModel()->removeRows(row, count);
}

/*!
\reimp
*/
bool InsertProxyModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (sourceParent.isValid() || destinationParent.isValid())
        return false;
    if (!sourceModel())
        return false;
    if (sourceRow + count > sourceModel()->rowCount())
        return false;
    if (destinationChild > sourceModel()->rowCount())
        return false;
    return sourceModel()->moveRows(QModelIndex(), sourceRow, count, QModelIndex(), destinationChild);
}

/*!
\reimp
*/
bool InsertProxyModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;
    if (!sourceModel())
        return false;
    if (column < 0 || column > sourceModel()->columnCount())
        return false;
    return sourceModel()->insertColumns(column, count);
}

/*!
\reimp
*/
bool InsertProxyModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;
    if (!sourceModel())
        return false;
    if (column < 0 || column > sourceModel()->columnCount())
        return false;
    return sourceModel()->removeColumns(column, count);
}

/*!
\reimp
*/
bool InsertProxyModel::moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent,
                                   int destinationChild)
{
    if (sourceParent.isValid() || destinationParent.isValid())
        return false;
    if (!sourceModel())
        return false;
    if (sourceColumn + count > sourceModel()->columnCount())
        return false;
    if (destinationChild > sourceModel()->columnCount())
        return false;
    return sourceModel()->moveColumns(QModelIndex(), sourceColumn, count, QModelIndex(), destinationChild);
}

/*!
\reimp
*/
void InsertProxyModel::sort(int column, Qt::SortOrder order)
{
    if (!sourceModel())
        return;
    sourceModel()->sort(column, order);
}

void InsertProxyModelPrivate::beforeLayoutChange(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
    if (!parents.isEmpty() && std::all_of(parents.cbegin(), parents.cend(), [](const QPersistentModelIndex &idx) { return idx.isValid(); }))
        return;
    Q_Q(InsertProxyModel);
    Q_ASSERT(q->sourceModel());
    Q_EMIT q->layoutAboutToBeChanged(QList<QPersistentModelIndex>({QPersistentModelIndex()}), hint);
    if (hint == QAbstractItemModel::VerticalSortHint)
        beforeSortRows();
    else if (hint == QAbstractItemModel::HorizontalSortHint)
        beforeSortCols();
    // Not much we can do otherwise
    const QModelIndexList proxyPersistentIndexes = q->persistentIndexList();
    m_layoutChangeProxyIndexes.clear();
    m_layoutChangePersistentIndexes.clear();
    m_layoutChangeProxyIndexes.reserve(proxyPersistentIndexes.size());
    m_layoutChangePersistentIndexes.reserve(proxyPersistentIndexes.size());
    for (const QModelIndex &proxyPersistentIndex : proxyPersistentIndexes) {
        const QPersistentModelIndex srcPersistentIndex = q->mapToSource(proxyPersistentIndex);
        if (!srcPersistentIndex.isValid())
            continue;
        Q_ASSERT(proxyPersistentIndex.isValid());
        m_layoutChangeProxyIndexes << proxyPersistentIndex;
        m_layoutChangePersistentIndexes << srcPersistentIndex;
    }
}

void InsertProxyModelPrivate::afetrLayoutChange(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{

    if (!parents.isEmpty() && std::all_of(parents.cbegin(), parents.cend(), [](const QPersistentModelIndex &idx) { return idx.isValid(); }))
        return;
    if (hint == QAbstractItemModel::VerticalSortHint)
        afterSortRows();
    else if (hint == QAbstractItemModel::HorizontalSortHint)
        afterSortCols();
    // Not much we can do otherwise
    Q_Q(InsertProxyModel);
    Q_ASSERT(q->sourceModel());
    QModelIndexList toList;
    toList.reserve(m_layoutChangePersistentIndexes.size());
    for (auto i = m_layoutChangePersistentIndexes.cbegin(), listEnd = m_layoutChangePersistentIndexes.cend(); i != listEnd; ++i)
        toList << q->mapFromSource(*i);
    q->changePersistentIndexList(m_layoutChangeProxyIndexes, toList);
    m_layoutChangeProxyIndexes.clear();
    m_layoutChangePersistentIndexes.clear();
    Q_EMIT q->layoutChanged(QList<QPersistentModelIndex>({QPersistentModelIndex()}), hint);
}

void InsertProxyModelPrivate::beforeSortRows()
{
    return beforeSort(true);
}

void InsertProxyModelPrivate::beforeSort(bool isRow)
{
    Q_Q(InsertProxyModel);
    if (!q->sourceModel())
        return;
    if ((isRow ? q->sourceModel()->columnCount() : q->sourceModel()->rowCount()) == 0)
        return;
    const int maxSortedIdx = isRow ? q->sourceModel()->rowCount() : q->sourceModel()->columnCount();
    const InsertProxyModel::InsertDirections directionCheck = isRow ? InsertProxyModel::InsertColumn : InsertProxyModel::InsertRow;
    if (m_insertDirection & directionCheck) {
        m_layoutChangeExtraProxyIndexes.clear();
        m_layoutChangeExtraPersistentIndexes.clear();
        m_layoutChangeExtraProxyIndexes.reserve(maxSortedIdx);
        m_layoutChangeExtraPersistentIndexes.reserve(maxSortedIdx);
        const int maxSecondaryIdx = isRow ? q->sourceModel()->columnCount() : q->sourceModel()->rowCount();
        for (int i = 0; i < maxSortedIdx; ++i) {
            m_layoutChangeExtraPersistentIndexes.append(isRow ? q->sourceModel()->index(i, 0) : q->sourceModel()->index(0, i));
            m_layoutChangeExtraProxyIndexes.append(isRow ? q->index(i, maxSecondaryIdx) : q->index(maxSecondaryIdx, i));
        }
    }
}

void InsertProxyModelPrivate::afterSort(bool isRow)
{
    Q_Q(InsertProxyModel);
    const int maxSortedIdx = isRow ? q->sourceModel()->rowCount() : q->sourceModel()->columnCount();
    const InsertProxyModel::InsertDirections directionCheck = isRow ? InsertProxyModel::InsertColumn : InsertProxyModel::InsertRow;
    if (m_insertDirection & directionCheck) {
        const auto oldlayout = m_extraData[!isRow];
        const int maxSecondaryIdx = isRow ? q->sourceModel()->columnCount() : q->sourceModel()->rowCount();
        QModelIndexList toList;
        toList.reserve(maxSortedIdx);
        Q_ASSERT(m_layoutChangeExtraPersistentIndexes.size() == maxSortedIdx);
        for (int i = 0; i < maxSortedIdx; ++i) {
            const int newSortedIdx = isRow ? m_layoutChangeExtraPersistentIndexes.at(i).row() : m_layoutChangeExtraPersistentIndexes.at(i).column();
            m_extraData[!isRow][newSortedIdx] = oldlayout.at(i);
            toList << (isRow ? q->index(newSortedIdx, maxSecondaryIdx) : q->index(maxSecondaryIdx, newSortedIdx));
        }
        q->changePersistentIndexList(m_layoutChangeExtraProxyIndexes, toList);
    }
    m_layoutChangeExtraProxyIndexes.clear();
    m_layoutChangeExtraPersistentIndexes.clear();
}

void InsertProxyModelPrivate::afterSortRows()
{
    return afterSort(true);
}

void InsertProxyModelPrivate::beforeSortCols()
{
    return beforeSort(false);
}

void InsertProxyModelPrivate::afterSortCols()
{
    return afterSort(false);
}



/*!
\class InsertProxyModel
\brief This proxy model provides an extra row and column to handle user insertions
\details This proxy will add an extra row, column or both to allow users to insert new sections with a familiar interface.

You can use setInsertDirection to determine whether to show an extra row or column. By default, this model will behave as QIdentityProxyModel

You can either call commitRow/commitColumn or reimplement validRow/validColumn to decide when a row/column should be added to the main model.

\warning Only flat models are supported. Branches of a tree will be hidden by the proxy
*/

/*!
\fn void InsertProxyModel::dataForCornerChanged(int role)

This signal is emitted whenever the data for the corner at the intersection of the extra row and column is changed
*/






