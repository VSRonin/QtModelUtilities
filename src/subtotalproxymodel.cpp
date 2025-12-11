/****************************************************************************\
   Copyright 2021 Luca Beldi
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
#include "private/subtotalproxymodel_p.h"
#include "subtotalproxymodel.h"
#include <functional>
#include <QSize>

SubtotalAggregator::SubtotalAggregator()
    : m_aggregateFunction(nullptr)
    , m_role(Qt::DisplayRole)
{ }

SubtotalAggregator::SubtotalAggregator(SubtotalProxyModel::AggregateFunction aggr, const QVariant &start, int role)
    : m_startingVal(start)
    , m_aggregateFunction(aggr)
    , m_role(role)
{ }

SubtotalProxyModelPrivate::SubtotalProxyModelPrivate(SubtotalProxyModel *q)
    : q_ptr(q)
    , m_overwriteParentData(false)
    , m_recursive(false)
    , m_totalLocations(SubtotalProxyModel::slNone)
{
    Q_ASSERT(q_ptr);
}

SubtotalProxyModel::SubtotalLocations SubtotalProxyModelPrivate::totalLocations() const
{
    if (m_subtotalMap.isEmpty())
        return SubtotalProxyModel::slNone;
    return m_totalLocations;
}

bool SubtotalProxyModelPrivate::isOnTotalRow(int row, const QModelIndex &parent) const
{
    Q_Q(const SubtotalProxyModel);
    Q_ASSERT(!parent.isValid() || parent.model() == q);
    if (parent.isValid() && !m_recursive)
        return false;
    if (row == 0 && totalLocations() & SubtotalProxyModel::slTop)
        return true;
    if ((row == q->rowCount(parent) - 1) && totalLocations() & SubtotalProxyModel::slBottom)
        return true;
    return false;
}

SubtotalProxyModelPrivate::TotalRowOrTotalParent SubtotalProxyModelPrivate::isOnTotalRowOrTotalParent(const QModelIndex &idx) const
{
    Q_Q(const SubtotalProxyModel);
    Q_ASSERT(idx.isValid() && idx.model() == q);
    if (isOnTotalRow(idx.row(), idx.parent()))
        return onTotalRow;
    if ((totalLocations() & SubtotalProxyModel::slOnParent) && q->hasChildren(idx))
        return onTotalParent;
    return onNothing;
}

void SubtotalProxyModelPrivate::emitRecursiveDataChanged(int col, const QModelIndex &parent, QList<int> *roles)
{
    Q_Q(SubtotalProxyModel);
    if (!q->sourceModel())
        return;
    auto mapIdx = m_subtotalMap.constFind(col);
    if (mapIdx == m_subtotalMap.constEnd())
        return;
    std::unique_ptr<QList<int>> rolesPtr(nullptr);
    if (!roles) {
        rolesPtr = std::make_unique<QList<int>>();
        for (auto i = mapIdx, mapEnd = m_subtotalMap.constEnd(); i != mapEnd && i.key() == col; ++i)
            rolesPtr->append(i->m_role);
        roles = rolesPtr.get();
    }
    const int rowCnt = q->rowCount(parent);
    if (m_recursive) {
        for (int j = totalLocations() & SubtotalProxyModel::slTop ? 1 : 0,
                 jEnd = totalLocations() & SubtotalProxyModel::slBottom ? rowCnt - 1 : rowCnt;
             j < jEnd; ++j) {
            const QModelIndex idx = q->index(j, col, parent);
            if (q->hasChildren(idx)) {
                if (totalLocations() & SubtotalProxyModel::slOnParent) {
                    if (m_overwriteParentData) {
                        Q_EMIT q->dataChanged(idx, idx, *roles);
                    } else {
                        QList<int> rolesChanged;
                        for (auto i = roles->constBegin(), iEnd = roles->constEnd(); i != iEnd; ++i) {
                            if (!q->mapToSource(idx).data(*i).isValid())
                                rolesChanged.append(*i);
                        }
                        if (!rolesChanged.isEmpty())
                            Q_EMIT q->dataChanged(idx, idx, rolesChanged);
                    }
                }
                emitRecursiveDataChanged(col, idx, roles);
            }
        }
    }

    if (totalLocations() & SubtotalProxyModel::slTop) {
        const QModelIndex idx = q->index(0, col, parent);
        Q_EMIT q->dataChanged(idx, idx, *roles);
    }
    if (totalLocations() & SubtotalProxyModel::slBottom) {
        const QModelIndex idx = q->index(rowCnt - 1, col, parent);
        Q_EMIT q->dataChanged(idx, idx, *roles);
    }
}

void SubtotalProxyModelPrivate::emitRecursiveDataChanged()
{
    const QList<int> allCols = m_subtotalMap.uniqueKeys();
    for (int i : allCols)
        emitRecursiveDataChanged(i);
}

void SubtotalProxyModelPrivate::calculateTotal(int column, const QModelIndex &sourceParent, int role, QVariant &runningSum,
                                               SubtotalProxyModel::AggregateFunction aggr) const
{
    Q_Q(const SubtotalProxyModel);
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == q->sourceModel());
    Q_ASSERT(q->sourceModel());
    for (int i = 0, iEnd = q->sourceModel()->rowCount(sourceParent); i != iEnd; ++i) {
        const QModelIndex sourceIndex = q->sourceModel()->index(i, column, sourceParent);
        if (m_recursive && q->sourceModel()->hasChildren(sourceIndex))
            calculateTotal(column, sourceIndex, role, runningSum, aggr);
        aggr(runningSum, sourceIndex.data(role));
    }
}

QVariant SubtotalProxyModelPrivate::calculateTotal(int column, const QModelIndex &parent, int role) const
{
    Q_Q(const SubtotalProxyModel);
    if (!q->sourceModel())
        return QVariant();
    for (auto mapIdx = m_subtotalMap.constFind(column), mapEnd = m_subtotalMap.constEnd(); mapIdx != mapEnd && mapIdx.key() == column; ++mapIdx) {
        if (mapIdx->m_role == role) {
            QVariant result = mapIdx->m_startingVal;
            calculateTotal(column, q->mapToSource(parent), role, result, mapIdx->m_aggregateFunction);
            return result;
        }
    }
    return QVariant();
}

void SubtotalProxyModelPrivate::onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    // #TODO
}

void SubtotalProxyModelPrivate::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    // #TODO
}

void SubtotalProxyModelPrivate::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    // #TODO
}

void SubtotalProxyModelPrivate::onRowsRemoved(const QModelIndex &parent, int first, int last)
{
    // #TODO
}

void SubtotalProxyModelPrivate::onRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent,
                                                     int dest)
{
    // #TODO
}

void SubtotalProxyModelPrivate::onRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
{
    // #TODO
}

void SubtotalProxyModelPrivate::onColumnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent,
                                               int dest)
{
    // #TODO
}

void SubtotalProxyModelPrivate::onColumnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                                                        const QModelIndex &destParent, int dest)
{
    // #TODO
}

void SubtotalProxyModelPrivate::onColumnsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    // #TODO
}

void SubtotalProxyModelPrivate::onColumnsInserted(const QModelIndex &parent, int first, int last)
{
    // #TODO
}

void SubtotalProxyModelPrivate::onColumnsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    // #TODO
}

void SubtotalProxyModelPrivate::onColumnsRemoved(const QModelIndex &parent, int first, int last)
{
    // #TODO
}

void SubtotalProxyModelPrivate::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_Q(SubtotalProxyModel);
    Q_EMIT q->dataChanged(q->mapFromSource(topLeft), q->mapFromSource(bottomRight), roles);
}

void SubtotalProxyModelPrivate::onLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint)
{
    // #TODO
}

void SubtotalProxyModelPrivate::onLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint)
{
    // #TODO
}

/*!
Constructs a new proxy model with the given \a parent.
*/
SubtotalProxyModel::SubtotalProxyModel(QObject *parent)
    : SubtotalProxyModelBaseClass(parent)
    , m_dptr(new SubtotalProxyModelPrivate(this))
{ }

/*!
\internal
*/
SubtotalProxyModel::SubtotalProxyModel(SubtotalProxyModelPrivate &dptr, QObject *parent)
    : SubtotalProxyModelBaseClass(parent)
    , m_dptr(&dptr)
{ }

/*!
Destructor
*/
SubtotalProxyModel::~SubtotalProxyModel()
{
    Q_D(SubtotalProxyModel);
    for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
        QObject::disconnect(*discIter);
    delete m_dptr;
}

void SubtotalProxyModel::setSubTotalColumn(int column, AggregateFunction aggregator, const QVariant &initialValue, int role)
{
    if (!aggregator)
        return removeSubtotalColumn(column, role);
    Q_D(SubtotalProxyModel);
    int firstColumnInMap = sourceModel() && d->m_subtotalMap.isEmpty();
    const auto doInsertion = [d, column, &aggregator, &initialValue, role]() {
        auto columnIter = d->m_subtotalMap.find(column);
        if (columnIter == d->m_subtotalMap.end()) {
            d->m_subtotalMap.insert(column, SubtotalAggregator(aggregator, initialValue, role));
        } else {
            const auto iEnd = d->m_subtotalMap.end();
            for (; columnIter != iEnd && columnIter.key() == column; ++columnIter) {
                if (columnIter->m_role == role) {
                    columnIter->m_startingVal = initialValue;
                    columnIter->m_aggregateFunction = aggregator;
                    break;
                }
            }
            if (columnIter == iEnd || columnIter.key() != column) {
                d->m_subtotalMap.insert(column, SubtotalAggregator(aggregator, initialValue, role));
            }
        }
    };
    if (firstColumnInMap) {
        if (((d->m_totalLocations & slTop) && (d->m_totalLocations & slBottom))
            || (d->m_recursive && ((d->m_totalLocations & slTop) || (d->m_totalLocations & slBottom)))) {
            // top and bottom can't be added at the same time
            // multiple rows in different parents can't be inserted at once
            beginResetModel();
            doInsertion();
            endResetModel();
        } else if (d->m_totalLocations & slTop) {
            beginInsertRows(QModelIndex(), 0, 0);
            doInsertion();
            endInsertRows();
        } else if (d->m_totalLocations & slBottom) {
            const int rowCnt = rowCount();
            beginInsertRows(QModelIndex(), rowCnt, rowCnt);
            doInsertion();
            endInsertRows();
        } else {
            doInsertion();
            d->emitRecursiveDataChanged();
        }
    } else {
        doInsertion();
        d->emitRecursiveDataChanged();
    }
}

void SubtotalProxyModel::removeSubtotalColumn(int column)
{
    Q_ASSERT_X(false, "SubtotalProxyModel::removeSubtotalColumn(int)", "TODO Implement");
}

void SubtotalProxyModel::removeSubtotalColumn(int column, int role)
{
    Q_ASSERT_X(false, "SubtotalProxyModel::removeSubtotalColumn(int,int)", "TODO Implement");
}

bool SubtotalProxyModel::overwriteParentData() const
{
    Q_D(const SubtotalProxyModel);
    return d->m_overwriteParentData;
}

void SubtotalProxyModel::setOverwriteParentData(bool overwrite)
{
    Q_D(SubtotalProxyModel);
    if (d->m_overwriteParentData == overwrite)
        return;
    d->m_overwriteParentData = overwrite;
    Q_EMIT overwriteParentDataChanged(overwrite);
    if (d->totalLocations() & slOnParent)
        d->emitRecursiveDataChanged();
}

bool SubtotalProxyModel::recursive() const
{
    Q_D(const SubtotalProxyModel);
    return d->m_recursive;
}

void SubtotalProxyModel::setRecursive(bool recur)
{
    Q_D(SubtotalProxyModel);
    if (d->m_recursive == recur)
        return;
    const bool needReset = ((d->m_totalLocations & slTop) || (d->m_totalLocations & slBottom));
    if (sourceModel() && needReset)
        beginResetModel();
    d->m_recursive = recur;
    if (sourceModel() && needReset)
        endResetModel();
    if (sourceModel() && !needReset)
        d->emitRecursiveDataChanged();
    Q_EMIT recursiveChanged(recur);
}

void SubtotalProxyModel::setTotalLocations(const SubtotalLocations &locations)
{
    Q_D(SubtotalProxyModel);
    if (d->m_totalLocations == locations)
        return;
    if (d->m_subtotalMap.isEmpty()) {
        d->m_totalLocations = locations;
    } else if (d->m_recursive
               && ((d->m_totalLocations & slTop) != (locations & slTop) || (d->m_totalLocations & slBottom) != (locations & slBottom))) {
        // d->m_totalLocations acts on all children but there is no way to use begin/end insert/remove rows on all of them at the same time
        beginResetModel();
        d->m_totalLocations = locations;
        endResetModel();
    } else {
        if ((d->m_totalLocations & slTop) && !(locations & slTop)) {
            beginRemoveRows(QModelIndex(), 0, 0);
            d->m_totalLocations &= ~slTop;
            endRemoveRows();
        }
        if ((d->m_totalLocations & slBottom) && !(locations & slBottom)) {
            const int rowCnt = rowCount();
            beginRemoveRows(QModelIndex(), rowCnt - 1, rowCnt - 1);
            d->m_totalLocations &= ~slBottom;
            endRemoveRows();
        }
        if ((locations & slTop) && !(d->m_totalLocations & slTop)) {
            beginInsertRows(QModelIndex(), 0, 0);
            d->m_totalLocations &= slTop;
            endRemoveRows();
        }
        if ((locations & slBottom) && !(d->m_totalLocations & slBottom)) {
            const int rowCnt = rowCount();
            beginInsertRows(QModelIndex(), rowCnt, rowCnt);
            d->m_totalLocations &= slBottom;
            endRemoveRows();
        }
        if ((d->m_totalLocations & slOnParent) != (locations & slOnParent)) {
            d->m_totalLocations = locations;
            d->emitRecursiveDataChanged();
        }
    }
    Q_ASSERT(d->m_totalLocations == locations);
    Q_EMIT totalLocationsChanged(d->m_totalLocations);
}

const SubtotalProxyModel::SubtotalLocations &SubtotalProxyModel::totalLocations() const
{
    Q_D(const SubtotalProxyModel);
    return d->m_totalLocations;
}

/*!
\reimp
*/
void SubtotalProxyModel::setSourceModel(QAbstractItemModel *newSourceModel)
{
    if (sourceModel() == newSourceModel)
        return;
    Q_D(SubtotalProxyModel);
    beginResetModel();
    for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
        QObject::disconnect(*discIter);
    d->m_sourceConnections.clear();
    QAbstractProxyModel::setSourceModel(newSourceModel);
    if (sourceModel()) {
        using namespace std::placeholders;
        d->m_sourceConnections
                << connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved,
                           std::bind(&SubtotalProxyModelPrivate::onRowsAboutToBeRemoved, d, _1, _2, _3))
                << connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeRemoved,
                           std::bind(&SubtotalProxyModelPrivate::onColumnsAboutToBeRemoved, d, _1, _2, _3))
                << connect(sourceModel(), &QAbstractItemModel::rowsRemoved, std::bind(&SubtotalProxyModelPrivate::onRowsRemoved, d, _1, _2, _3))
                << connect(sourceModel(), &QAbstractItemModel::columnsRemoved, std::bind(&SubtotalProxyModelPrivate::onColumnsRemoved, d, _1, _2, _3))
                << connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted,
                           std::bind(&SubtotalProxyModelPrivate::onRowsAboutToBeInserted, d, _1, _2, _3))
                << connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeInserted,
                           std::bind(&SubtotalProxyModelPrivate::onColumnsAboutToBeInserted, d, _1, _2, _3))
                << connect(sourceModel(), &QAbstractItemModel::rowsInserted, std::bind(&SubtotalProxyModelPrivate::onRowsInserted, d, _1, _2, _3))
                << connect(sourceModel(), &QAbstractItemModel::columnsInserted,
                           std::bind(&SubtotalProxyModelPrivate::onColumnsInserted, d, _1, _2, _3))
                << connect(sourceModel(), &QAbstractItemModel::dataChanged, std::bind(&SubtotalProxyModelPrivate::onDataChanged, d, _1, _2, _3))
                << connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeMoved,
                           std::bind(&SubtotalProxyModelPrivate::onRowsAboutToBeMoved, d, _1, _2, _3, _4, _5))
                << connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeMoved,
                           std::bind(&SubtotalProxyModelPrivate::onColumnsAboutToBeMoved, d, _1, _2, _3, _4, _5))
                << connect(sourceModel(), &QAbstractItemModel::rowsMoved, std::bind(&SubtotalProxyModelPrivate::onRowsMoved, d, _1, _2, _3, _4, _5))
                << connect(sourceModel(), &QAbstractItemModel::columnsMoved,
                           std::bind(&SubtotalProxyModelPrivate::onColumnsMoved, d, _1, _2, _3, _4, _5))
                << connect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged,
                           std::bind(&SubtotalProxyModelPrivate::onLayoutAboutToBeChanged, d, _1, _2))
                << connect(sourceModel(), &QAbstractItemModel::layoutChanged, std::bind(&SubtotalProxyModelPrivate::onLayoutChanged, d, _1, _2))

                << connect(sourceModel(), &QAbstractItemModel::headerDataChanged, this, &SubtotalProxyModel::headerDataChanged)
                << connect(sourceModel(), &QAbstractItemModel::modelAboutToBeReset, this, &SubtotalProxyModel::beginResetModel)
                << connect(sourceModel(), &QAbstractItemModel::modelReset, this, &SubtotalProxyModel::endResetModel);
    }
    endResetModel();
}

/*!
\reimp
*/
QModelIndex SubtotalProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid())
        return QModelIndex();
    if (!sourceModel())
        return QModelIndex();
    Q_ASSERT(proxyIndex.model() == this);
    Q_D(const SubtotalProxyModel);
    if (proxyIndex.row() == 0 && (d->totalLocations() & slTop))
        return QModelIndex();
    const auto fetchSourceIndex = [this](int row, int col, void *internalPtr) -> QModelIndex {
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
        return createSourceIndex(row, col, internalPtr);
#else
        return QIdentityProxy::mapToSource(createIndex(row, col, internalPtr));
#endif
    };
    const QModelIndex sourceIdx =
            fetchSourceIndex(proxyIndex.row() - (d->totalLocations() & slTop ? 1 : 0), proxyIndex.column(), proxyIndex.internalPointer());
    if (sourceIdx.row() >= sourceModel()->rowCount(sourceIdx.parent()))
        return QModelIndex();
    return sourceIdx;
}

/*!
\reimp
*/
QModelIndex SubtotalProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceModel())
        return QModelIndex();
    if (!sourceIndex.isValid())
        return QModelIndex();
    Q_ASSERT(sourceIndex.model() == sourceModel());
    Q_D(const SubtotalProxyModel);
    const int rowAdjust = d->totalLocations() & slTop ? 1 : 0;
    return createIndex(sourceIndex.row() + rowAdjust, sourceIndex.column(), sourceIndex.internalPointer());
}

/*!
\reimp
*/
QModelIndex SubtotalProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return QModelIndex();
    Q_D(const SubtotalProxyModel);
    if (row == 0 && (d->totalLocations() & slTop))
        return createIndex(0, column, mapToSource(index(1, column, parent)).internalPointer());
    const int rowCnt = rowCount(parent);
    if (row == rowCnt - 1 && (d->totalLocations() & slBottom))
        return createIndex(row, column, mapToSource(index(row - 1, column, parent)).internalPointer());
    return mapFromSource(sourceModel()->index(row - (d->totalLocations() & slTop ? 1 : 0), column, mapToSource(parent)));
}

/*!
\reimp
*/
QModelIndex SubtotalProxyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();
    Q_ASSERT(child.model() == this);
    QModelIndex sourceIdx = mapToSource(child);
    Q_D(const SubtotalProxyModel);
    if (!sourceIdx.isValid())
        sourceIdx = mapToSource(createIndex(d->totalLocations() & slTop ? 1 : 0, child.column(), child.internalPointer()));
    return mapFromSource(sourceIdx.parent());
}

/*!
\reimp
*/
int SubtotalProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return 0;
    return sourceModel()->columnCount(mapToSource(parent));
}

/*!
\reimp
*/
QMap<int, QVariant> SubtotalProxyModel::itemData(const QModelIndex &index) const
{
    Q_D(const SubtotalProxyModel);
    return d->itemData(index);
}

QMap<int, QVariant> SubtotalProxyModelPrivate::itemData(const QModelIndex &index) const
{
    Q_Q(const SubtotalProxyModel);
    if (!q->sourceModel())
        return QMap<int, QVariant>();
    if (!index.isValid())
        return QMap<int, QVariant>();
    Q_ASSERT(index.model() == q);

    const auto isOn = isOnTotalRowOrTotalParent(index);
    if (isOn == onTotalRow || (isOn == onTotalParent && m_overwriteParentData)) {
        QMap<int, QVariant> result;
        for (auto mapIdx = m_subtotalMap.constFind(index.column()), mapEnd = m_subtotalMap.constEnd();
             mapIdx != mapEnd && mapIdx.key() == index.column(); ++mapIdx)
            result.insert(mapIdx->m_role, calculateTotal(index.column(), index.parent(), mapIdx->m_role));
        return result;
    }
    QMap<int, QVariant> sourceData = q->sourceModel()->itemData(q->mapToSource(index));
    if (isOn == onTotalParent) {
        for (auto i = sourceData.begin(), iEnd = sourceData.end(); i != iEnd; ++i) {
            if (!i.value().isValid())
                i.value() = calculateTotal(index.column(), index.parent(), i.key());
        }
    }
    return sourceData;
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
/*!
\reimp
*/
bool SubtotalProxyModel::clearItemData(const QModelIndex &index)
{
    if (!sourceModel())
        return false;
    if (!index.isValid())
        return false;
    Q_ASSERT(index.model() == this);
    Q_D(const SubtotalProxyModel);
    if (d->isOnTotalRow(index.row(), index.parent()))
        return false;
    return sourceModel()->clearItemData(mapToSource(index));
}

/*!
\reimp
*/
void SubtotalProxyModel::multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const
{
    Q_D(const SubtotalProxyModel);
    return d->multiData(index, roleDataSpan);
}

void SubtotalProxyModelPrivate::multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const
{
    Q_Q(const SubtotalProxyModel);
    if (!q->sourceModel())
        return;
    if (!index.isValid())
        return;
    Q_ASSERT(index.model() == q);
    const auto isOn = isOnTotalRowOrTotalParent(index);
    if (isOn == onTotalRow || (isOn == onTotalParent && m_overwriteParentData)) {
        for (QModelRoleData &roleData : roleDataSpan)
            roleData.setData(calculateTotal(index.column(), index.parent(), roleData.role()));
        return;
    }
    if (isOn == onTotalParent) {
        q->sourceModel()->multiData(q->mapToSource(index), roleDataSpan);
        for (QModelRoleData &roleData : roleDataSpan) {
            if (!roleData.data().isValid())
                roleData.setData(calculateTotal(index.column(), index.parent(), roleData.role()));
        }
        return;
    }
    return q->sourceModel()->multiData(q->mapToSource(index), roleDataSpan);
}
#endif

/*!
\reimp
*/
Qt::ItemFlags SubtotalProxyModel::flags(const QModelIndex &index) const
{
    Q_ASSERT(!index.isValid() || index.model() == this);
    if (!sourceModel())
        return Qt::NoItemFlags;
    if (!index.isValid())
        return sourceModel()->flags(QModelIndex());
    Q_D(const SubtotalProxyModel);
    if (d->isOnTotalRow(index.row(), index.parent()))
        return flagsForTotalRow(index);
    return sourceModel()->flags(mapToSource(index));
}

/*!
\reimp
*/
QVariant SubtotalProxyModel::data(const QModelIndex &index, int role) const
{
    Q_D(const SubtotalProxyModel);
    return d->data(index, role);
}

QVariant SubtotalProxyModelPrivate::data(const QModelIndex &index, int role) const
{
    Q_Q(const SubtotalProxyModel);
    if (!q->sourceModel())
        return QVariant();
    if (!index.isValid())
        return QVariant();
    Q_ASSERT(index.model() == q);
    const auto isOn = isOnTotalRowOrTotalParent(index);
    if (isOn == onTotalRow || (isOn == onTotalParent && m_overwriteParentData))
        return calculateTotal(index.column(), index.parent(), role);
    const QVariant sourceData = q->sourceModel()->data(q->mapToSource(index), role);
    if (isOn == onTotalParent) {
        if (!sourceData.isValid())
            return calculateTotal(index.column(), index.parent(), role);
    }
    return sourceData;
}

/*!
\reimp
*/
bool SubtotalProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!sourceModel())
        return false;
    if (!index.isValid())
        return false;
    Q_ASSERT(index.model() == this);
    Q_D(const SubtotalProxyModel);
    if (d->isOnTotalRow(index.row(), index.parent()))
        return false;
    return sourceModel()->setData(mapToSource(index), value, role);
}

/*!
\reimp
*/
QModelIndex SubtotalProxyModel::buddy(const QModelIndex &index) const
{
    return index;
}

/*!
\reimp
*/
int SubtotalProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return 0;
    Q_D(const SubtotalProxyModel);
    const QModelIndex sourceParent = mapToSource(parent);
    if (parent.isValid() && !sourceParent.isValid())
        return 0;
    int result = sourceModel()->rowCount(sourceParent);
    if (result > 0)
        result += (d->totalLocations() & slTop ? 1 : 0) + (d->totalLocations() & slBottom ? 1 : 0);
    return result;
}

/*!
\reimp
*/
bool SubtotalProxyModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(const SubtotalProxyModel);
    if (d->isOnTotalRow(row, parent))
        return false;
    return sourceModel()->canDropMimeData(data, action, row, column, mapToSource(parent));
}

/*!
\reimp
*/
bool SubtotalProxyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(const SubtotalProxyModel);
    if (d->isOnTotalRow(row, parent))
        return false;
    return sourceModel()->dropMimeData(data, action, row, column, mapToSource(parent));
}

/*!
\reimp
*/
bool SubtotalProxyModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(const SubtotalProxyModel);
    if (parent.isValid() && d->isOnTotalRow(parent.row(), parent.parent()))
        return false;
    return sourceModel()->insertColumns(column, count, mapToSource(parent));
}
/*!
\reimp
*/
bool SubtotalProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(const SubtotalProxyModel);
    if (row == 0 && d->totalLocations() & slTop)
        return false;
    if ((row == rowCount(parent)) && d->totalLocations() & slBottom)
        return false;
    return sourceModel()->insertRows(row, count, mapToSource(parent));
}

/*!
\reimp
*/
bool SubtotalProxyModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(const SubtotalProxyModel);
    if (parent.isValid() && d->isOnTotalRow(parent.row(), parent.parent()))
        return false;
    return sourceModel()->removeColumns(column, count, mapToSource(parent));
}

/*!
\reimp
*/
bool SubtotalProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(const SubtotalProxyModel);
    if (d->isOnTotalRow(row, parent))
        return false;
    return sourceModel()->removeRows(row, count, mapToSource(parent));
}

/*!
\reimp
*/
bool SubtotalProxyModel::hasChildren(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(const SubtotalProxyModel);
    if (parent.isValid() && d->isOnTotalRow(parent.row(), parent.parent()))
        return false;
    return sourceModel()->hasChildren(mapToSource(parent));
}

/*!
\reimp
*/
bool SubtotalProxyModel::canFetchMore(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(const SubtotalProxyModel);
    if (parent.isValid() && d->isOnTotalRow(parent.row(), parent.parent()))
        return false;
    return sourceModel()->canFetchMore(mapToSource(parent));
}

/*!
\reimp
*/
void SubtotalProxyModel::fetchMore(const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return;
    Q_D(const SubtotalProxyModel);
    if (parent.isValid() && d->isOnTotalRow(parent.row(), parent.parent()))
        return;
    sourceModel()->fetchMore(mapToSource(parent));
}

/*!
\reimp
*/
bool SubtotalProxyModel::moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent,
                                     int destinationChild)
{
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == this);
    Q_ASSERT(!destinationParent.isValid() || destinationParent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(const SubtotalProxyModel);
    if (sourceParent.isValid() && d->isOnTotalRow(sourceParent.row(), sourceParent.parent()))
        return false;
    if (destinationParent.isValid() && d->isOnTotalRow(destinationParent.row(), destinationParent.parent()))
        return false;
    return sourceModel()->moveColumns(mapToSource(sourceParent), sourceColumn, count, mapToSource(destinationParent), destinationChild);
}

/*!
\reimp
*/
bool SubtotalProxyModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent,
                                  int destinationChild)
{
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == this);
    Q_ASSERT(!destinationParent.isValid() || destinationParent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(const SubtotalProxyModel);
    if (d->isOnTotalRow(sourceRow, sourceParent))
        return false;
    if (d->isOnTotalRow(destinationChild, destinationParent))
        return false;
    if (sourceParent.isValid() && d->isOnTotalRow(sourceParent.row(), sourceParent.parent()))
        return false;
    if (destinationParent.isValid() && d->isOnTotalRow(destinationParent.row(), destinationParent.parent()))
        return false;
    return sourceModel()->moveColumns(mapToSource(sourceParent), sourceRow, count, mapToSource(destinationParent), destinationChild);
}

Qt::ItemFlags SubtotalProxyModel::flagsForTotalRow(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/*!
\class SubtotalProxyModel
\brief This proxy model will display subtotals for each column of the model and branch of a tree model
*/
