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
#include "private/columnstorolesproxymodel_p.h"
#include "columnstorolesproxymodel.h"

ColumnToRoleMapping::ColumnToRoleMapping()
    : sourceCol(0)
    , destinationCol(0)
    , sourceRole(Qt::DisplayRole)
    , destinationRole(Qt::UserRole)
{ }

bool ColumnToRoleMapping::isValid() const
{
    return sourceCol != destinationCol;
}

ColumnsToRolesProxyModelPrivate::ColumnsToRolesProxyModelPrivate(ColumnsToRolesProxyModel *q)
    : q_ptr(q)

{
    Q_ASSERT(q_ptr);
}

/*!
Constructs a new proxy model with the given \a parent.
*/
ColumnsToRolesProxyModel::ColumnsToRolesProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
    , m_dptr(new ColumnsToRolesProxyModelPrivate(this))
{ }

/*!
\internal
*/
ColumnsToRolesProxyModel::ColumnsToRolesProxyModel(ColumnsToRolesProxyModelPrivate &dptr, QObject *parent)
    : QIdentityProxyModel(parent)
    , m_dptr(&dptr)
{ }

/*!
Destructor
*/
ColumnsToRolesProxyModel::~ColumnsToRolesProxyModel()
{
    Q_D(ColumnsToRolesProxyModel);
    for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
        QObject::disconnect(*discIter);
    delete m_dptr;
}

/*!
\reimp
*/
void ColumnsToRolesProxyModel::setSourceModel(QAbstractItemModel *newSourceModel)
{
    if (newSourceModel == sourceModel())
        return;
    Q_D(ColumnsToRolesProxyModel);
    if (sourceModel()) {
        for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
            QObject::disconnect(*discIter);
    }
    d->m_sourceConnections.clear();
    QIdentityProxyModel::setSourceModel(newSourceModel);
    if (sourceModel()) {
        d->m_sourceConnections
                << QObject::connect(sourceModel(), &QAbstractItemModel::destroyed, [this]() -> void { setSourceModel(Q_NULLPTR); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::dataChanged,
                                    [d](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) {
                                        d->interceptDataChanged(topLeft, bottomRight, roles);
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::headerDataChanged, this, &QAbstractItemModel::headerDataChanged)
                << QObject::connect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged,
                                    [d](const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint) {
                                        d->beforeLayoutChange(parents, hint);
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::layoutChanged,
                                    [d](const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint) {
                                        d->afterLayoutChange(parents, hint);
                                    })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsInserted,
                                    [d](const QModelIndex &parent, int first, int last) -> void { d->onColumnsInserted(parent, first, last); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsRemoved,
                                    [d](const QModelIndex &parent, int first, int last) -> void { d->onColumnsRemoved(parent, first, last); })
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsMoved,
                                    [d](const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column) -> void {
                                        d->onColumnsMoved(parent, start, end, destination, column);
                                    });
    }
}

/*!
\reimp
*/
QVariant ColumnsToRolesProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid())
        return QVariant();
    if (!sourceModel())
        return QVariant();
    Q_D(const ColumnsToRolesProxyModel);
    for (auto i = d->m_colToRoleMap.constBegin(), iEnd = d->m_colToRoleMap.constEnd(); i != iEnd; ++i) {
        if (i->destinationCol == proxyIndex.column() && i->destinationRole == role)
            return QIdentityProxyModel::data(index(proxyIndex.row(), i->sourceCol, proxyIndex.parent()), i->sourceRole);
    }
    return QIdentityProxyModel::data(proxyIndex, role);
}

/*!
\reimp
*/
bool ColumnsToRolesProxyModel::setData(const QModelIndex &proxyIndex, const QVariant &value, int role)
{
    if (!proxyIndex.isValid())
        return false;
    if (!sourceModel())
        return false;
    const int sourceRows = sourceModel()->rowCount();
    const int sourceCols = sourceModel()->columnCount();
    Q_D(ColumnsToRolesProxyModel);
    for (auto i = d->m_colToRoleMap.constBegin(), iEnd = d->m_colToRoleMap.constEnd(); i != iEnd; ++i) {
        if (i->destinationCol == proxyIndex.column() && i->destinationRole == role)
            return QIdentityProxyModel::setData(index(proxyIndex.row(), i->sourceCol, proxyIndex.parent()), value, i->sourceRole);
    }
    return QIdentityProxyModel::setData(proxyIndex, value, role);
}

/*!
\reimp
\details If the map contains both Qt::DisplayRole and Qt::EditRole and mergeDisplayEdit is set to true,
the value of Qt::DisplayRole will prevail.
*/
bool InsertProxyModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    if (!index.isValid())
        return false;
    if (!sourceModel())
        return false;
    const int sourceRows = sourceModel()->rowCount();
    const int sourceCols = sourceModel()->columnCount();
    Q_D(InsertProxyModel);
    if (index.parent().isValid() || index.row() > sourceRows || index.column() > sourceCols
        || (index.row() == sourceRows && !(d->m_insertDirection & InsertRow))
        || (index.column() == sourceCols && !(d->m_insertDirection & InsertColumn)))
        return false;
    const bool isExtraRow = index.row() == sourceRows && d->m_insertDirection & InsertRow;
    const bool isExtraCol = index.column() == sourceCols && d->m_insertDirection & InsertColumn;
    if (!(isExtraRow || isExtraCol))
        return sourceModel()->setItemData(sourceModel()->index(index.row(), index.column()), roles);

    const int sectionIdx = isExtraRow ? index.column() : index.row();
    RolesContainer *const dataContainer = (isExtraRow && isExtraCol) ? &d->m_dataForCorner : &d->m_extraData[isExtraRow][sectionIdx];
    QVector<int> changedRoles;
    for (auto i = roles.cbegin(); i != roles.cend(); ++i) {
        if (i.value().isValid()) {
            const auto oldDataIter = dataContainer->find(i.key());
            if (oldDataIter == dataContainer->end()) {
                if (i.value().isValid()) {
                    changedRoles << i.key();
                    dataContainer->insert(i.key(), i.value());
                }
            } else if (!i.value().isValid()) {
                changedRoles << i.key();
                dataContainer->erase(oldDataIter);
                continue;
            } else if (oldDataIter.value() != i.value()) {
                changedRoles << i.key();
                oldDataIter.value() = i.value();
            }
        }
    }
    if (changedRoles.isEmpty())
        return true;
    Q_ASSERT(std::is_sorted(changedRoles.cbegin(), changedRoles.cend()));
    const bool hasDisplay = std::binary_search(changedRoles.cbegin(), changedRoles.cend(), Qt::DisplayRole);
    const bool hasEdit = std::binary_search(changedRoles.cbegin(), changedRoles.cend(), Qt::EditRole);
    if (!d->m_separateEditDisplay) {
        if (hasDisplay && hasEdit) {
            const auto displayIter = dataContainer->constFind(Qt::DisplayRole);
            const auto editIter = dataContainer->find(Qt::EditRole);
            // in QStandardItemModel DisplayRole wins over EditRole so we do the same
            if (displayIter == dataContainer->cend()) {
                if (editIter != dataContainer->end()) {
                    dataContainer->erase(editIter);
                }
            } else {
                if (editIter == dataContainer->end())
                    dataContainer->insert(Qt::EditRole, displayIter.value());
                else
                    editIter.value() = displayIter.value(); // assign without checking should optimise memory use due to implicit sharing
            }
        } else if (hasDisplay != hasEdit) {
            if (hasDisplay) {
                const auto displayIter = dataContainer->constFind(Qt::DisplayRole);
                if (displayIter == dataContainer->constEnd()) {
                    Q_ASSERT(dataContainer->contains(Qt::EditRole));
                    dataContainer->remove(Qt::EditRole);
                } else {
                    dataContainer->operator[](Qt::EditRole) = displayIter.value();
                }
                changedRoles << Qt::EditRole;
            } else {
                const auto editIter = dataContainer->constFind(Qt::EditRole);
                if (editIter == dataContainer->constEnd()) {
                    Q_ASSERT(dataContainer->contains(Qt::DisplayRole));
                    dataContainer->remove(Qt::DisplayRole);
                } else {
                    dataContainer->operator[](Qt::DisplayRole) = editIter.value();
                }
                changedRoles << Qt::DisplayRole;
            }
        }
    }
    Q_EMIT dataChanged(index, index, changedRoles);
    Q_EMIT extraDataChanged(index, index, changedRoles);
    return true;
}

QMap<int, QVariant> InsertProxyModel::itemData(const QModelIndex &index) const
{
    if (!index.isValid())
        return QMap<int, QVariant>();
    if (!sourceModel())
        return QMap<int, QVariant>();
    const int sourceRows = sourceModel()->rowCount();
    const int sourceCols = sourceModel()->columnCount();
    Q_D(const InsertProxyModel);
    if (index.parent().isValid() || index.row() > sourceRows || index.column() > sourceCols
        || (index.row() == sourceRows && !(d->m_insertDirection & InsertRow))
        || (index.column() == sourceCols && !(d->m_insertDirection & InsertColumn)))
        return QMap<int, QVariant>();
    const bool isExtraRow = index.row() == sourceRows && d->m_insertDirection & InsertRow;
    const bool isExtraCol = index.column() == sourceCols && d->m_insertDirection & InsertColumn;
    if (!(isExtraRow || isExtraCol))
        return sourceModel()->itemData(sourceModel()->index(index.row(), index.column()));
    if (isExtraRow && isExtraCol)
        return convertFromContainer<QMap<int, QVariant>>(d->m_dataForCorner);
    const int sectionIdx = isExtraRow ? index.column() : index.row();
    return convertFromContainer<QMap<int, QVariant>>(d->m_extraData[isExtraRow][sectionIdx]);
}

/*!
\reimp
*/
QModelIndex InsertProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex();
    if (sourceIndex.parent().isValid())
        return QModelIndex();
    return createIndex(sourceIndex.row(), sourceIndex.column());
}

/*!
\reimp
*/
QModelIndex InsertProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid())
        return QModelIndex();
    if (proxyIndex.parent().isValid())
        return QModelIndex();
    if (!sourceModel())
        return QModelIndex();
    if (proxyIndex.row() >= sourceModel()->rowCount() || proxyIndex.column() >= sourceModel()->columnCount())
        return QModelIndex();
    return sourceModel()->index(proxyIndex.row(), proxyIndex.column());
}

/*!
\property InsertProxyModel::insertDirection
\accessors %insertDirection(), setInsertDirection()
\notifier insertDirectionChanged()
\brief This property determines if the extra row, column or both are displayed
*/
InsertProxyModel::InsertDirections InsertProxyModel::insertDirection() const
{
    Q_D(const InsertProxyModel);
    return d->m_insertDirection;
}

/*!
\reimp
*/
Qt::ItemFlags InsertProxyModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    if (!sourceModel())
        return Qt::NoItemFlags;
    Q_D(const InsertProxyModel);
    const bool isExtraRow = index.row() == sourceModel()->rowCount() && d->m_insertDirection & InsertRow;
    const bool isExtraCol = index.column() == sourceModel()->columnCount() && d->m_insertDirection & InsertColumn;
    if (isExtraRow && isExtraCol)
        return Qt::NoItemFlags;
    if (isExtraRow)
        return flagForExtra(true, index.column()) | Qt::ItemNeverHasChildren;
    if (isExtraCol)
        return flagForExtra(false, index.row()) | Qt::ItemNeverHasChildren;
    return sourceModel()->flags(sourceModel()->index(index.row(), index.column())) | Qt::ItemNeverHasChildren;
}

/*!
\reimp
*/
QModelIndex InsertProxyModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QModelIndex();
}

/*!
\reimp
*/
QModelIndex InsertProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() || row < 0 || column < 0 || row >= rowCount() || column >= columnCount())
        return QModelIndex();
    return createIndex(row, column);
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
\brief flag function for the extra row or column
\details If \a isRow is true this method is referring to the extra row, otherwise it will be the extra column,
\a section then determines the row or column this flag refers to
*/
Qt::ItemFlags InsertProxyModel::flagForExtra(bool isRow, int section) const
{
    Q_UNUSED(isRow)
    Q_UNUSED(section)
    return (Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

void InsertProxyModel::setInsertDirection(const InsertDirections &direction)
{
    Q_D(InsertProxyModel);
    if (d->m_insertDirection == direction)
        return;
    if (sourceModel()) {
        const InsertDirections oldDirection{std::move(d->m_insertDirection)};
        if ((oldDirection & InsertRow) && !(direction & InsertRow)) {
            const int rowToRemove = sourceModel()->rowCount();
            beginRemoveRows(QModelIndex(), rowToRemove, rowToRemove);
            d->m_insertDirection &= ~InsertRow;
            endRemoveRows();
        } else if (!(oldDirection & InsertRow) && (direction & InsertRow)) {
            const int rowToInsert = sourceModel()->rowCount();
            beginInsertRows(QModelIndex(), rowToInsert, rowToInsert);
            d->m_insertDirection |= InsertRow;
            endInsertRows();
        }
        if ((oldDirection & InsertColumn) && !(direction & InsertColumn)) {
            const int colToRemove = sourceModel()->columnCount();
            beginRemoveColumns(QModelIndex(), colToRemove, colToRemove);
            d->m_insertDirection &= ~InsertColumn;
            endRemoveColumns();
        }
        if (!(oldDirection & InsertColumn) && (direction & InsertColumn)) {
            const int colToInsert = sourceModel()->columnCount();
            beginInsertColumns(QModelIndex(), colToInsert, colToInsert);
            d->m_insertDirection |= InsertColumn;
            endInsertColumns();
        }
    } else {
        d->m_insertDirection = direction;
    }
    Q_ASSERT(direction == d->m_insertDirection);
    Q_EMIT insertDirectionChanged(direction);
}

/*!
Gets the data for the corner cell at the intersection between the extra row and extra column
*/
QVariant InsertProxyModel::dataForCorner(int role) const
{
    Q_D(const InsertProxyModel);
    return d->m_dataForCorner.value(role);
}

/*!
Set the data for the corner cell at the intersection between the extra row and extra column
*/
void InsertProxyModel::setDataForCorner(const QVariant &value, int role)
{
    Q_D(InsertProxyModel);
    const QVector<int> changedRoles = d->setDataInContainer(d->m_dataForCorner, role, value);
    if (changedRoles.isEmpty())
        return;
    Q_EMIT dataForCornerChanged(changedRoles);
    if (sourceModel() && (d->m_insertDirection & (InsertProxyModel::InsertRow | InsertProxyModel::InsertColumn))) {
        const QModelIndex cornerIndex = index(sourceModel()->rowCount(), sourceModel()->columnCount());
        Q_EMIT dataChanged(cornerIndex, cornerIndex, changedRoles);
    }
}

/*!
Adds the extra row to the source model
*/
bool InsertProxyModel::commitRow()
{
    Q_D(InsertProxyModel);
    return d->commitRow();
}

/*!
Adds the extra column to the source model
*/
bool InsertProxyModel::commitColumn()
{
    Q_D(InsertProxyModel);
    return d->commitColumn();
}

/*!
\property InsertProxyModel::mergeDisplayEdit
\accessors %mergeDisplayEdit(), setMergeDisplayEdit()
\notifier mergeDisplayEditChanged()
\brief This property determines if the \a Qt::DisplayRole and \a Qt::EditRole should be merged in the extra row/column
\details By default the two roles are one and the same you can use this property to separate them.
If there's any data in the role when you set this property to false it will be duplicated for both roles.
If there is data both in \a Qt::DisplayRole and \a Qt::EditRole when you set this property to true \a Qt::DisplayRole will prevail.
This property only affects the extra row/column. Data in the source model is not affected.
*/

bool InsertProxyModel::mergeDisplayEdit() const
{
    Q_D(const InsertProxyModel);
    return !d->m_separateEditDisplay;
}

void InsertProxyModel::setMergeDisplayEdit(bool val)
{
    Q_D(InsertProxyModel);
    if (d->m_separateEditDisplay == val) {
        d->m_separateEditDisplay = !val;
        if (sourceModel() && val) {
            const int sourceCol = sourceModel()->columnCount();
            const int sourceRows = sourceModel()->rowCount();
            // orientation == Qt::Vertical
            int roleChange = d->mergeEditDisplayHash(d->m_extraHeaderData[1]);
            if (roleChange >= 0) {
                Q_EMIT headerDataChanged(Qt::Vertical, sourceRows, sourceRows);
            }
            roleChange = d->mergeEditDisplayHash(d->m_extraHeaderData[0]);
            if (roleChange >= 0) {
                Q_EMIT headerDataChanged(Qt::Horizontal, sourceCol, sourceCol);
            }
            roleChange = d->mergeEditDisplayHash(d->m_dataForCorner);
            if (roleChange >= 0) {
                const QVector<int> roleChangeVect(1, roleChange);
                Q_EMIT dataForCornerChanged(roleChangeVect);
                if (d->m_insertDirection & (InsertRow | InsertColumn)) {
                    const QModelIndex cornerIdx = index(sourceRows, sourceCol);
                    Q_EMIT dataChanged(cornerIdx, cornerIdx, roleChangeVect);
                }
            }

            // 0 rows 1 cols
            for (int i = 0; i < sourceRows; ++i) {
                roleChange = d->mergeEditDisplayHash(d->m_extraData[0][i]);
                if (roleChange >= 0) {
                    const QVector<int> roleChangeVect(1, roleChange);
                    const QModelIndex currentIdx = index(i, sourceCol);
                    Q_EMIT extraDataChanged(currentIdx, currentIdx, roleChangeVect);
                    if (d->m_insertDirection & InsertColumn)
                        Q_EMIT dataChanged(currentIdx, currentIdx, roleChangeVect);
                }
            }
            for (int i = 0; i < sourceCol; ++i) {
                if (roleChange >= 0) {
                    const QVector<int> roleChangeVect(1, roleChange);
                    const QModelIndex currentIdx = index(sourceRows, i);
                    Q_EMIT extraDataChanged(currentIdx, currentIdx, roleChangeVect);
                    if (d->m_insertDirection & InsertRow)
                        Q_EMIT dataChanged(currentIdx, currentIdx, roleChangeVect);
                }
            }
        }
        Q_EMIT mergeDisplayEditChanged(val);
    }
}

/*!
\brief Returns true if the extra row can be merged in the main model
\details The default implementation never commits the row to the model.

If, for example, you want the row to be added to the source model as soon as there's any data in the Qt::DisplayRole of any cell you can repliement
this method as: \code{.cpp}if (!sourceModel())
    return false;
const int sourceCols = sourceModel()->columnCount();
const int sourceRows = sourceModel()->rowCount();
for (int i = 0; i < sourceCols; ++i) {
    if (index(sourceRows,i).data().isValid())
        return true;
}
return false;
\endcode
*/
bool InsertProxyModel::validRow() const
{
    return false;
}

/*!
\brief Returns true if the extra column can be merged in the main model
\details The default implementation never commits the column to the model.

If, for example, you want the row to be added to the source model as soon as there's any data in the Qt::DisplayRole of any cell you can repliement
this method as: \code{.cpp} if (!sourceModel()) return false; const int sourceCols = sourceModel()->columnCount(); const int sourceRows =
sourceModel()->rowCount(); for (int i = 0; i < sourceRows; ++i) { if (index(i, sourceCols).data().isValid()) return true;
}
return false;
\endcode
*/
bool InsertProxyModel::validColumn() const
{
    return false;
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

/*!
\fn void InsertProxyModel::extraDataChanged(int role)

Same as dataChanged but is emitted only for the extra row/column
*/

/*! \enum InsertProxyModel::InsertDirection
Direction of the Insertion
*/

/*! \var InsertProxyModel::InsertDirection InsertProxyModel::NoInsert
No Insertion
*/

/*! \var InsertProxyModel::InsertDirection InsertProxyModel::InsertRow
Insert new rows
*/

/*! \var InsertProxyModel::InsertDirection InsertProxyModel::InsertColumn
Insert new columns
*/
