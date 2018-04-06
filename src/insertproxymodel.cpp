#include "private/insertproxymodel_p.h"
#include "insertproxymodel.h"
#include <functional>
#include <QSet>
#include <QVector>
#include <QByteArray>


/*!
\internal
*/
void InsertProxyModelPrivate::checkExtraRowChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(roles)
    Q_Q(InsertProxyModel);
    Q_ASSERT(!(bottomRight.column() >= q->sourceModel()->columnCount() && bottomRight.row() >= q->sourceModel()->rowCount()));
    if (bottomRight.column() >= q->sourceModel()->columnCount()) {
        if (q->validColumn())
            commitColumn();
        return;
    }
    if (bottomRight.row() >= q->sourceModel()->rowCount()) {
        if (q->validRow())
            commitRow();
    }
}

/*!
\internal
*/
bool InsertProxyModelPrivate::commitRow()
{
    return commitToSource(true);
}

bool InsertProxyModelPrivate::commitToSource(const bool isRow)
{
    Q_Q(InsertProxyModel);
    if (!q->sourceModel())
        return false;
    const bool canInsCol = m_insertDirection & InsertProxyModel::InsertColumn;
    const bool canInsRow = m_insertDirection & InsertProxyModel::InsertRow;
    if (!((isRow && canInsRow) || (!isRow && canInsCol)))
        return false;
    const int sourceCols = q->sourceModel()->columnCount();
    const int sourceRows = q->sourceModel()->rowCount();
    if (!(isRow ? q->sourceModel()->insertRow(sourceRows) : q->sourceModel()->insertColumn(sourceCols)))
        return false;
    const int loopEnd = isRow ? sourceCols : sourceRows;
    for (int i = 0; i < loopEnd; ++i) {
        QHash<int, QVariant>& allRoles = m_extraData[isRow][i];
        if (allRoles.isEmpty())
            continue;
        const auto endRoles = allRoles.constEnd();
        const QModelIndex currentIdx = 
            isRow ? 
            q->sourceModel()->index(sourceRows, i)
            : q->sourceModel()->index(i, sourceCols)
            ;
        Q_ASSERT(m_separateEditDisplay || (allRoles.contains(Qt::DisplayRole) == allRoles.contains(Qt::EditRole)));
        Q_ASSERT(m_separateEditDisplay || (allRoles.value(Qt::DisplayRole) == allRoles.value(Qt::EditRole)));
        if(!q->sourceModel()->setItemData(currentIdx, hashToMap(allRoles)))
            return false;
        QVector<int> aggregateRoles;
        aggregateRoles.reserve(allRoles.size());
        for (auto j = allRoles.begin(); !allRoles.isEmpty(); j = allRoles.erase(j))
            aggregateRoles << j.key();
        const QModelIndex proxyIdx = isRow ? q->index(sourceRows + 1, i) : q->index(i, sourceCols + 1);
        q->dataChanged(proxyIdx, proxyIdx, aggregateRoles);
        q->extraDataChanged(proxyIdx, proxyIdx, aggregateRoles);
    }
    return true;
}

/*!
\internal
*/
void InsertProxyModelPrivate::setSeparateEditDisplayHash(QHash<int, QVariant>& singleHash)
{
    const auto displayFind = singleHash.constFind(Qt::DisplayRole);
    if (m_separateEditDisplay && displayFind != singleHash.cend())
        m_extraHeaderData->insert(Qt::EditRole, displayFind.value());
    else if (!m_separateEditDisplay) {
        if (displayFind == singleHash.cend()) {
            const auto editFind = m_extraHeaderData->find(Qt::EditRole);
            if (editFind != singleHash.cend())
                m_extraHeaderData->insert(Qt::DisplayRole, editFind.value());
        }
        m_extraHeaderData->remove(Qt::EditRole);
    }
}

/*!
\internal
*/
void InsertProxyModelPrivate::onInserted(bool isRow, const QModelIndex &parent, int first, int last)
{
    if (parent.isValid())
        return;
    for (; first <= last; --last)
        m_extraData[!isRow].insert(first, QHash<int, QVariant>());
    Q_Q(InsertProxyModel);
    isRow ? q->endInsertRows() : q->endInsertColumns();
}

/*!
\internal
*/
void InsertProxyModelPrivate::onMoved(bool isRow, const QModelIndex &parent, int start, int end, const QModelIndex &destination, int destIdx)
{
    if (parent.isValid())
        return;
    if (destination.isValid())
        return onRemoved(isRow, parent, start, end);
    for (; start <= end; --end)
        m_extraData[!isRow].move(end, destIdx);
    Q_Q(InsertProxyModel);
    isRow ? q->endMoveRows() : q->endMoveColumns();
}

/*!
\internal
*/
void InsertProxyModelPrivate::onRemoved(bool isRow, const QModelIndex &parent, int first, int last)
{
    if (parent.isValid())
        return;
    for (; first <= last; --last)
        m_extraData[!isRow].removeAt(last);
    Q_Q(InsertProxyModel);
    isRow ? q->endRemoveRows() : q->endRemoveColumns();
}

/*!
\internal
*/
bool InsertProxyModelPrivate::commitColumn()
{
    return commitToSource(false);
}

/*!
\internal
*/
InsertProxyModelPrivate::InsertProxyModelPrivate(InsertProxyModel* q)
    :q_ptr(q)
    , m_insertDirection(InsertProxyModel::NoInsert)
    , m_separateEditDisplay(false)
{
    Q_ASSERT(q_ptr);
    QHash<int, QVariant> starHeader;
    starHeader.insert(Qt::DisplayRole, QVariant::fromValue(QStringLiteral("*")));
    m_extraHeaderData[true] = starHeader;
    m_extraHeaderData[false] = starHeader;
    QObject::connect(q_ptr, &QAbstractItemModel::dataChanged, [this](const QModelIndex& topLeft,const QModelIndex& bottomRight, const QVector<int>& roles)->void{checkExtraRowChanged(topLeft,bottomRight,roles);});
}

/*!
Constructs a new proxy model with the given \a parent.
*/
InsertProxyModel::InsertProxyModel(QObject* parent)
    : QAbstractProxyModel(parent)
    , m_dptr(new InsertProxyModelPrivate(this))
{}

/*!
Destructor
*/
InsertProxyModel::~InsertProxyModel()
{
    Q_D(InsertProxyModel);
    for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
        QObject::disconnect(*discIter);
    delete m_dptr;
}

/*!
\reimp
*/
void InsertProxyModel::setSourceModel(QAbstractItemModel* newSourceModel)
{
    Q_D(InsertProxyModel);
    beginResetModel();
    if (sourceModel()) {
        for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
            Q_ASSUME(QObject::disconnect(*discIter));
    }
    QAbstractProxyModel::setSourceModel(newSourceModel);
    d->m_sourceConnections.clear();
    for(auto& extraData : d->m_extraData)
        extraData.clear();
    if (sourceModel()) {
        d->m_sourceConnections
            << QObject::connect(sourceModel(), &QAbstractItemModel::modelAboutToBeReset, this, &QAbstractItemModel::modelAboutToBeReset)
        << QObject::connect(sourceModel(), &QAbstractItemModel::modelReset, this, &QAbstractItemModel::modelReset)
        << QObject::connect(sourceModel(), &QAbstractItemModel::dataChanged, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
            dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight), roles);
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::headerDataChanged, this, &QAbstractItemModel::headerDataChanged)
        << QObject::connect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged, [this](const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint) {
            QList<QPersistentModelIndex> mappedParents;
            for (const auto& parent : parents)
                mappedParents << QPersistentModelIndex(mapFromSource(parent));
            layoutAboutToBeChanged(mappedParents, hint);
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::layoutChanged, [this](const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint) {
            QList<QPersistentModelIndex> mappedParents;
            for (auto& parent : parents)
                mappedParents << QPersistentModelIndex(mapFromSource(parent));
            layoutChanged(mappedParents, hint);
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeInserted, [this](const QModelIndex &parent, int first, int last) {
            if (!parent.isValid()) {
                beginInsertColumns(QModelIndex(), first, last);
            }
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeMoved, [this](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn) {
            if (sourceParent.isValid())
                return;
            if (destinationParent.isValid())
                beginRemoveColumns(QModelIndex(), sourceStart, sourceEnd);
            else 
                beginMoveColumns(QModelIndex(), sourceStart, sourceEnd, QModelIndex(), destinationColumn);
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeRemoved, [this](const QModelIndex &parent, int first, int last) {
            if (!parent.isValid()) {
                beginRemoveColumns(QModelIndex(), first, last);
            }
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsInserted,[d](const QModelIndex &parent, int first, int last)->void{d->onColumnsInserted(parent,first,last);})
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsRemoved,[d](const QModelIndex &parent, int first, int last)->void{d->onColumnsRemoved(parent,first,last);})
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsMoved, [d](const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column)->void{d->onColumnsMoved(parent,start,end,destination,column);})
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted, [this](const QModelIndex &parent, int first, int last) {
            if (!parent.isValid()) {
                beginInsertRows(QModelIndex(), first, last);
            }
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeMoved, [this](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow) {
            if (sourceParent.isValid())
                return;
            if (destinationParent.isValid())
                beginRemoveRows(QModelIndex(), sourceStart, sourceEnd);
            else
                beginMoveRows(QModelIndex(), sourceStart, sourceEnd, QModelIndex(), destinationRow);
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, [this](const QModelIndex &parent, int first, int last) {
            if (!parent.isValid()) {
                beginRemoveRows(QModelIndex(), first, last);
            }
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsInserted, [d](const QModelIndex &parent, int first, int last)->void{d->onRowsInserted(parent,first,last);})
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsRemoved, [d](const QModelIndex &parent, int first, int last)->void{d->onRowsRemoved(parent,first,last);})
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsMoved, [d](const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)->void{d->onRowsMoved(parent,start,end,destination,row);})
        ;
        const int sourceCols = sourceModel()->columnCount();
        const int sourceRows = sourceModel()->rowCount();
        for (int i = 0; i < sourceRows; ++i)
            d->m_extraData[0].append(QHash<int, QVariant>());
        for (int i = 0; i < sourceCols; ++i)
            d->m_extraData[1].append(QHash<int, QVariant>());
    }
   
    endResetModel();
}

/*!
\reimp
*/
QModelIndex InsertProxyModel::buddy(const QModelIndex &index) const
{
    if (!sourceModel())
        return QModelIndex();
    Q_D(const InsertProxyModel);
    const bool isExtraRow = index.row() == sourceModel()->rowCount() && d->m_insertDirection & InsertRow;
    const bool isExtraCol = index.column() == sourceModel()->columnCount() && d->m_insertDirection & InsertColumn;
    if (isExtraCol || isExtraRow)
        return index;
    return mapFromSource(sourceModel()->buddy(mapToSource(index)));
}

/*!
\reimp
*/
bool InsertProxyModel::hasChildren(const QModelIndex &parent) const
{
    return !parent.isValid();
}

/*!
\reimp
*/
int InsertProxyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    if (!sourceModel())
        return 0;
    Q_D(const InsertProxyModel);
    const int originalCount = sourceModel()->rowCount(parent);
    if (d->m_insertDirection & InsertRow)
        return originalCount + 1;
    return originalCount;
}

/*!
\reimp
*/
int InsertProxyModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    if (!sourceModel())
        return 0;
    Q_D(const InsertProxyModel);
    const int originalCount = sourceModel()->columnCount(parent);
    if (d->m_insertDirection & InsertColumn)
        return originalCount + 1;
    return originalCount;
}

/*!
\reimp
*/
QVariant InsertProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!sourceModel())
        return QVariant();
    Q_D(const InsertProxyModel);
    const int adjRole = (!d->m_separateEditDisplay && (role == Qt::EditRole)) ? Qt::DisplayRole : role;
    if (
        (orientation == Qt::Horizontal && (d->m_insertDirection & InsertColumn) && section == sourceModel()->columnCount())
        || (orientation == Qt::Vertical && (d->m_insertDirection & InsertRow) && section == sourceModel()->rowCount())
        )
        return d->m_extraHeaderData[orientation == Qt::Vertical].value(adjRole);
    return sourceModel()->headerData(section, orientation, role);
}

/*!
\reimp
*/
bool InsertProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    Q_D(InsertProxyModel);
    if (!sourceModel())
        return false;
    const int adjRole = (!d->m_separateEditDisplay && (role == Qt::EditRole)) ? Qt::DisplayRole : role;
    if (
        (orientation == Qt::Horizontal && (d->m_insertDirection & InsertColumn) && section == sourceModel()->columnCount())
        || (orientation == Qt::Vertical && (d->m_insertDirection & InsertRow) && section == sourceModel()->rowCount())
        ) {
        auto& headIter = d->m_extraHeaderData[orientation == Qt::Vertical];
        if (value.isValid()) {
            auto roleIter = headIter.find(adjRole);
            if (roleIter == headIter.end()) {
                headIter.insert(adjRole, value);
                headerDataChanged(orientation, section, section);
                return true;
            }
            if (roleIter.value() == value)
                return true;
            roleIter.value() = value;
            headerDataChanged(orientation, section, section);
            return true;
        }
        else {
            auto roleIter = headIter.find(adjRole);
            if (roleIter == headIter.end())
                return true;
            headIter.erase(roleIter);
            headerDataChanged(orientation, section, section);
            return true;
        }
    }
    return sourceModel()->setHeaderData(section, orientation, value, role);
}

/*!
\reimp
*/
QVariant InsertProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid())
        return QVariant();
    if (proxyIndex.parent().isValid())
        return QVariant();
    if (!sourceModel())
        return QVariant();
    Q_D(const InsertProxyModel);
    const bool isExtraRow = proxyIndex.row() == sourceModel()->rowCount() && d->m_insertDirection & InsertRow;
    const bool isExtraCol = proxyIndex.column() == sourceModel()->columnCount() && d->m_insertDirection & InsertColumn;
    const int adjRole = (!d->m_separateEditDisplay && (role == Qt::EditRole)) ? Qt::DisplayRole : role;
    if (isExtraCol && isExtraRow)
        return d->m_dataForCorner.value(adjRole);
    if (isExtraRow)
        return d->m_extraData[true].value(proxyIndex.column()).value(adjRole);
    if (isExtraCol)
        return d->m_extraData[false].value(proxyIndex.row()).value(adjRole);
    return sourceModel()->data(sourceModel()->index(proxyIndex.row(), proxyIndex.column()), role);
}

/*!
\reimp
*/
bool InsertProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if (!sourceModel())
        return false;
    const int sourceRows = sourceModel()->rowCount();
    const int sourceCols = sourceModel()->columnCount();
    Q_D(InsertProxyModel);
    if (
        index.parent().isValid()
        || index.row() > sourceRows
        || index.column() > sourceCols
        || (index.row() == sourceRows && !(d->m_insertDirection & InsertRow))
        || (index.column() == sourceCols && !(d->m_insertDirection & InsertColumn))
        )
        return false;
    const bool isExtraRow = index.row() == sourceRows  && d->m_insertDirection & InsertRow;
    const bool isExtraCol = index.column() == sourceCols  && d->m_insertDirection & InsertColumn;
    if (isExtraRow && isExtraCol)
        return false;
    if (!(isExtraRow || isExtraCol))
        return sourceModel()->setData(sourceModel()->index(index.row(), index.column()), value, role);
    const int sectionIdx = isExtraRow ? index.column() : index.row();
    QVector<int> rolesToSet;
    QVector<int> changedRoles;
    rolesToSet.reserve(2);
    changedRoles.reserve(2);
    rolesToSet << role;
    if (!d->m_separateEditDisplay && (role == Qt::DisplayRole || role == Qt::EditRole))
        rolesToSet << (role == Qt::DisplayRole ? Qt::EditRole : Qt::DisplayRole);
    for (auto i = rolesToSet.cbegin(); i != rolesToSet.cend();++i){
        auto roleIter = d->m_extraData[isExtraRow][sectionIdx].find(*i);
        if (roleIter == d->m_extraData[isExtraRow][sectionIdx].end()){
            if (value.isValid()) {
                d->m_extraData[isExtraRow][sectionIdx].insert(*i, value);
                changedRoles << *i;
            }
        }
        else{
            if (roleIter.value() != value){
                roleIter.value() = value;
                changedRoles << *i;
            }
        }
    }
    if (!changedRoles.isEmpty()){
        dataChanged(index, index, changedRoles);
        extraDataChanged(index, index, changedRoles);
        return true;
    }
    return false;
}

/*!
\reimp
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
    if (
        index.parent().isValid()
        || index.row() > sourceRows
        || index.column() > sourceCols
        || (index.row() == sourceRows && !(d->m_insertDirection & InsertRow))
        || (index.column() == sourceCols && !(d->m_insertDirection & InsertColumn))
        )
        return false;
    const bool isExtraRow = index.row() == sourceRows  && d->m_insertDirection & InsertRow;
    const bool isExtraCol = index.column() == sourceCols  && d->m_insertDirection & InsertColumn;
    if (!(isExtraRow || isExtraCol))
        return sourceModel()->setItemData(sourceModel()->index(index.row(), index.column()), roles);

    const int sectionIdx = isExtraRow ? index.column() : index.row();
    QHash<int,QVariant>& oldData = d->m_extraData[isExtraRow][sectionIdx];
    QHash<int, QVariant> newData;
    QVector<int> changedRoles;
    bool hasDisplay = false;
    bool hasEdit = false;
    for (auto i = roles.cbegin(); i != roles.cend(); ++i) {
        if (i.value().isValid()) {
            newData.insert(i.key(), i.value());
            const auto oldDataIter = oldData.constFind(i.key());
            if (oldDataIter == oldData.constEnd() || oldDataIter.value() != i.value()) {
                changedRoles << i.key();
                if (i.key() == Qt::DisplayRole)
                    hasDisplay = true;
                if (i.key() == Qt::EditRole)
                    hasEdit = true;
            }
        }
    }    
    if (!d->m_separateEditDisplay && hasDisplay != hasEdit) {
        if(hasDisplay){
            const QVariant displayData = newData.value(Qt::DisplayRole);
            newData.insert(Qt::EditRole, displayData);
            const auto oldDataIter = oldData.constFind(Qt::EditRole);
            if (oldDataIter == oldData.constEnd() || oldDataIter.value() != displayData) {
                changedRoles << Qt::EditRole;
            }
        }
        else {
            const QVariant editData = newData.value(Qt::EditRole);
            newData.insert(Qt::DisplayRole, editData);
            const auto oldDataIter = oldData.constFind(Qt::DisplayRole);
            if (oldDataIter == oldData.constEnd() || oldDataIter.value() != editData) {
                changedRoles << Qt::DisplayRole;
            }
        }
    }
    for (auto i = oldData.cbegin(); i != oldData.cend(); ++i) {
        if (!newData.contains(i.key()))
            changedRoles << i.key();
    }
    oldData = newData;
    dataChanged(index, index, changedRoles);
    extraDataChanged(index, index, changedRoles);
    return true;
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
\property InsertProxy::insertDirection
\accessors %insertDirection(), setInsertDirection()
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
        return flagForExtra(true, index.column())
        #if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
        | Qt::ItemNeverHasChildren
        #endif
        ;
    if (isExtraCol)
        return flagForExtra(false, index.row())
        #if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
        | Qt::ItemNeverHasChildren
        #endif
        ;
    return sourceModel()->flags(sourceModel()->index(index.row(), index.column()))
        #if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
        | Qt::ItemNeverHasChildren
        #endif
    ;
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
    if (parent.isValid())
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
    if (row<0 || row>sourceModel()->rowCount())
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
    if (row < 0 || row >= sourceModel()->rowCount())
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
    if (column < 0 || column >= sourceModel()->columnCount())
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
    if (column<0 || column >= sourceModel()->columnCount())
        return false;
    return sourceModel()->removeColumns(column, count);
}

/*!
\reimp
*/
bool InsertProxyModel::moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent, int destinationChild)
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

void InsertProxyModel::setInsertDirection(const InsertDirections& direction)
{
    Q_D(InsertProxyModel);
    if (d->m_insertDirection == direction)
        return;
    if (sourceModel()) {
        const InsertDirections oldDirection{ std::move(d->m_insertDirection) };
        if ((oldDirection & InsertRow) && !(direction & InsertRow)) {
            const int rowToRemove = sourceModel()->rowCount();
            beginRemoveRows(QModelIndex(), rowToRemove, rowToRemove);
            d->m_insertDirection &= ~InsertRow;
            endRemoveRows();
        }
        else  if (!(oldDirection & InsertRow) && (direction & InsertRow)) {
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
    }
    else{
        d->m_insertDirection = direction;
    }
    Q_ASSERT(direction == d->m_insertDirection);
    insertDirectionChanged(direction);
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
void InsertProxyModel::setDataForCorner(const QVariant& value, int role)
{
    Q_D(InsertProxyModel);
    const auto dataIter = d->m_dataForCorner.find(role);
    if (value.isValid()) {
        if (dataIter == d->m_dataForCorner.end()) {
            d->m_dataForCorner.insert(role, value);
            dataForCornerChanged(role);
            return;
        }
        if (dataIter.value() == value)
            return;
        dataIter.value() = value;
        dataForCornerChanged(role);
    }
    else {
        if (dataIter == d->m_dataForCorner.end())
            return;
        d->m_dataForCorner.erase(dataIter);
        dataForCornerChanged(role);
    }
}

/*!
Forces the extra row to be added to the source model
*/
bool InsertProxyModel::commitRow()
{
    Q_D(InsertProxyModel);
    return d->commitRow();
}

/*!
Forces the extra row to be added to the source model
*/
bool InsertProxyModel::commitColumn()
{
    Q_D(InsertProxyModel);
    return d->commitColumn();
}

/*!
\property InsertProxy::separateEditDisplay
\accessors %separateEditDisplay(), setSeparateEditDisplay()
\brief This property determines if the Qt::DisplayRole and Qt::EditRole should be merged in the extra row/column
\details By default the two roles are one and the same you can use this property to separate them.
If there's any data in the role when you set this property to true it will be duplicated for both roles.
If there is data both in Qt::DisplayRole and Qt::EditRole when you set this property to false Qt::DisplayRole will prevail.
This property only affects the extra row/column. Data in the source model is not affected.
*/

bool InsertProxyModel::separateEditDisplay() const
{
    Q_D(const InsertProxyModel);
    return d->m_separateEditDisplay;
}

void InsertProxyModel::setSeparateEditDisplay(bool val)
{
    Q_D(InsertProxyModel);
    if (d->m_separateEditDisplay != val) {
        d->m_separateEditDisplay = val;
        if (sourceModel()) {
            const QVector<int> rolesVector = QVector<int>() << Qt::EditRole << Qt::DisplayRole;
            const int sourceCol = sourceModel()->columnCount();
            const int sourceRows = sourceModel()->rowCount();
            for (QHash<int, QVariant>& singleHash : d->m_extraHeaderData)
                d->setSeparateEditDisplayHash(singleHash);
            if (d->m_insertDirection & InsertColumn)
                headerDataChanged(Qt::Horizontal, sourceCol, sourceCol);
            if (d->m_insertDirection & InsertRow)
                headerDataChanged(Qt::Vertical, sourceRows, sourceRows);
            for (auto& sectionHash : d->m_extraData) {
                for (auto singleHashIter = sectionHash.begin(); singleHashIter != sectionHash.end(); ++singleHashIter)
                    d->setSeparateEditDisplayHash(*singleHashIter);
            }
            if (d->m_insertDirection & InsertColumn)
                dataChanged(index(0, sourceCol), index(sourceRows - 1, sourceCol), rolesVector);
            if (d->m_insertDirection & InsertRow)
                dataChanged(index(sourceRows, 0), index(sourceRows, sourceCol - 1), rolesVector);
        }
        separateEditDisplayChanged(val);
    }
}

/*!
\brief Returns true if the extra row can be merged in the main model
\details The default implementation never commits the row to the model.

If, for example, you want the row to be added to the source model as soon as there's any data in the Qt::DisplayRole of any cell you can repliement this method as:
\code{.cpp}
if (!sourceModel())
    return false;
const int sourceCols = sourceModel()->columnCount();
const int sourceRows = sourceModel()->rowCount();
for (int i = 0; i < sourceCols; ++i) {
    if (index(sourceRows,i).data().isValid())
        return true;
}
return false;
\code
*/
bool InsertProxyModel::validRow() const
{
    return false;
}

/*!
\brief Returns true if the extra column can be merged in the main model
\details The default implementation never commits the column to the model.

If, for example, you want the row to be added to the source model as soon as there's any data in the Qt::DisplayRole of any cell you can repliement this method as:
\code{.cpp}
if (!sourceModel())
    return false;
const int sourceCols = sourceModel()->columnCount();
const int sourceRows = sourceModel()->rowCount();
for (int i = 0; i < sourceRows; ++i) {
    if (index(i, sourceCols).data().isValid())
        return true;
}
return false;
\code
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
\fn void dataForCornerChanged(int role)

This signal is emitted whenever the data for the corner at the intersection of the extra row and column is changed
*/

/*!
\fn void extraDataChanged(int role)

Same as dataChanged but is emitted only for the extra row/column
*/

