#include "private/insertproxy_p.h"
#include "insertproxy.h"
#include <functional>
#include <QSet>
#include <QVector>
#include <QByteArray>


/*!
\internal
*/
void InsertProxyPrivate::checkExtraRowChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    Q_UNUSED(topLeft)
    Q_Q(InsertProxy);
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
void InsertProxyPrivate::commitRow()
{
    commitToSource(true);
}

void InsertProxyPrivate::commitToSource(const bool isRow)
{
    Q_Q(InsertProxy);
    if (!q->sourceModel())
        return;
    const int sourceCols = q->sourceModel()->columnCount();
    const int sourceRows = q->sourceModel()->rowCount();
    if (!(isRow ? q->sourceModel()->insertRow(sourceRows) : q->sourceModel()->insertColumn(sourceCols)))
        return;
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
        QVector<int> rolesChanged;
        rolesChanged.reserve(allRoles.size() + 1);
        bool hasDisplay = false;
        bool hasEdit = false;
        for (auto j = allRoles.constBegin(); j != endRoles; ++j) {
            q->sourceModel()->setData(currentIdx, j.value(), j.key());
            rolesChanged.append(j.key());
            if (!hasDisplay && j.key() == Qt::DisplayRole)
                hasDisplay = true;
            if (!hasDisplay && j.key() == Qt::EditRole)
                hasEdit = true;
        }
        if (hasDisplay != hasEdit && !m_separateEditDisplay){
            if (hasDisplay)
                rolesChanged.append(Qt::EditRole);
            if (hasEdit)
                rolesChanged.append(Qt::DisplayRole);
        }
        allRoles.clear();
        const QModelIndex proxyIdx = isRow ? q->index(sourceRows + 1, i) : q->index(i, sourceCols + 1);
        q->dataChanged(proxyIdx, proxyIdx, rolesChanged);
    }
}

/*!
\internal
*/
void InsertProxyPrivate::setSeparateEditDisplayHash(QHash<int, QVariant>& singleHash)
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
void InsertProxyPrivate::onInserted(bool isRow, const QModelIndex &parent, int first, int last)
{
    if (parent.isValid())
        return;
    for (; first <= last; --last)
        m_extraData[!isRow].insert(first, QHash<int, QVariant>());
    Q_Q(InsertProxy);
    isRow ? q->endInsertRows() : q->endInsertColumns();
}

/*!
\internal
*/
void InsertProxyPrivate::onMoved(bool isRow, const QModelIndex &parent, int start, int end, const QModelIndex &destination, int destIdx)
{
    if (parent.isValid())
        return;
    if (destination.isValid())
        return onRemoved(isRow, parent, start, end);
    for (; start <= end; --end)
        m_extraData[!isRow].move(end, destIdx);
    Q_Q(InsertProxy);
    isRow ? q->endMoveRows() : q->endMoveColumns();
}

/*!
\internal
*/
void InsertProxyPrivate::onRemoved(bool isRow, const QModelIndex &parent, int first, int last)
{
    if (parent.isValid())
        return;
    for (; first <= last; --last)
        m_extraData[!isRow].removeAt(last);
    Q_Q(InsertProxy);
    isRow ? q->endRemoveRows() : q->endRemoveColumns();
}

/*!
\internal
*/
void InsertProxyPrivate::commitColumn()
{
    commitToSource(false);
}

/*!
\internal
*/
InsertProxyPrivate::InsertProxyPrivate(InsertProxy* q)
    :q_ptr(q)
    , m_insertDirection(InsertProxy::NoInsert)
    , m_separateEditDisplay(false)
{
    Q_ASSERT(q_ptr);
    const QHash<int, QVariant> starHeader{ std::make_pair(Qt::DisplayRole, QVariant::fromValue(QStringLiteral("*"))) };
    m_extraHeaderData[true] = starHeader;
    m_extraHeaderData[false] = starHeader;
    QObject::connect(q_ptr, &QAbstractItemModel::dataChanged, std::bind(&InsertProxyPrivate::checkExtraRowChanged, this, std::placeholders::_1, std::placeholders::_2));
}

/*!
Constructs a new proxy model with the given \a parent.
*/
InsertProxy::InsertProxy(QObject* parent)
    : QAbstractProxyModel(parent)
    , m_dptr(new InsertProxyPrivate(this))
{}

/*!
Destructor
*/
InsertProxy::~InsertProxy()
{
    delete m_dptr;
}

/*!
\reimp
*/
void InsertProxy::setSourceModel(QAbstractItemModel* newSourceModel)
{
    Q_D(InsertProxy);
    beginResetModel();
    if (sourceModel()) {
        for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
            QObject::disconnect(*discIter);
    }
    QAbstractProxyModel::setSourceModel(newSourceModel);
    d->m_sourceConnections.clear();
    for(auto& extraData : d->m_extraData)
        extraData.clear();
    if (sourceModel()) {
        using namespace std::placeholders;
        d->m_sourceConnections
            << QObject::connect(sourceModel(), &QAbstractItemModel::modelAboutToBeReset, this, &QAbstractItemModel::modelAboutToBeReset)
        << QObject::connect(sourceModel(), &QAbstractItemModel::modelReset, this, &QAbstractItemModel::modelReset)
        << QObject::connect(sourceModel(), &QAbstractItemModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
            dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight), roles);
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::headerDataChanged, this, &QAbstractItemModel::headerDataChanged)
        << QObject::connect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged, this, [this](const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint) {
            QList<QPersistentModelIndex> mappedParents;
            for (const auto& parent : parents)
                mappedParents << QPersistentModelIndex(mapFromSource(parent));
            layoutAboutToBeChanged(mappedParents, hint);
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::layoutChanged, this, [this](const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint) {
            QList<QPersistentModelIndex> mappedParents;
            for (auto& parent : parents)
                mappedParents << QPersistentModelIndex(mapFromSource(parent));
            layoutChanged(mappedParents, hint);
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeInserted, this, [this](const QModelIndex &parent, int first, int last) {
            if (!parent.isValid()) {
                beginInsertColumns(QModelIndex(), first, last);
            }
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeMoved, this, [this](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn) {
            if (sourceParent.isValid())
                return;
            if (destinationParent.isValid())
                beginRemoveColumns(QModelIndex(), sourceStart, sourceEnd);
            else 
                beginMoveColumns(QModelIndex(), sourceStart, sourceEnd, QModelIndex(), destinationColumn);
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeRemoved, this, [this](const QModelIndex &parent, int first, int last) {
            if (!parent.isValid()) {
                beginRemoveColumns(QModelIndex(), first, last);
            }
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsInserted, this, std::bind(&InsertProxyPrivate::onColumnsInserted,d,_1,_2,_3))
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsRemoved, this, std::bind(&InsertProxyPrivate::onColumnsRemoved, d, _1, _2, _3))
        << QObject::connect(sourceModel(), &QAbstractItemModel::columnsMoved, this, std::bind(&InsertProxyPrivate::onColumnsMoved, d, _1, _2, _3, _4,_5))
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted, this, [this](const QModelIndex &parent, int first, int last) {
            if (!parent.isValid()) {
                beginInsertRows(QModelIndex(), first, last);
            }
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeMoved, this, [this](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow) {
            if (sourceParent.isValid())
                return;
            if (destinationParent.isValid())
                beginRemoveRows(QModelIndex(), sourceStart, sourceEnd);
            else
                beginMoveRows(QModelIndex(), sourceStart, sourceEnd, QModelIndex(), destinationRow);
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, this, [this](const QModelIndex &parent, int first, int last) {
            if (!parent.isValid()) {
                beginRemoveRows(QModelIndex(), first, last);
            }
        })
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, std::bind(&InsertProxyPrivate::onRowsInserted, d, _1, _2, _3))
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, std::bind(&InsertProxyPrivate::onRowsRemoved, d, _1, _2, _3))
        << QObject::connect(sourceModel(), &QAbstractItemModel::rowsMoved, this, std::bind(&InsertProxyPrivate::onRowsMoved, d, _1, _2, _3, _4, _5))
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
QModelIndex InsertProxy::buddy(const QModelIndex &index) const
{
    if (!sourceModel())
        return QModelIndex();
    Q_D(const InsertProxy);
    const bool isExtraRow = index.row() == sourceModel()->rowCount() && d->m_insertDirection & InsertRow;
    const bool isExtraCol = index.column() == sourceModel()->columnCount() && d->m_insertDirection & InsertColumn;
    if (isExtraCol || isExtraRow)
        return index;
    return mapFromSource(sourceModel()->buddy(mapToSource(index)));
}

/*!
\reimp
*/
bool InsertProxy::hasChildren(const QModelIndex &parent) const
{
    return !parent.isValid();
}

/*!
\reimp
*/
int InsertProxy::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    if (!sourceModel())
        return 0;
    Q_D(const InsertProxy);
    const int originalCount = sourceModel()->rowCount(parent);
    if (d->m_insertDirection & InsertRow)
        return originalCount + 1;
    return originalCount;
}

/*!
\reimp
*/
int InsertProxy::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    if (!sourceModel())
        return 0;
    Q_D(const InsertProxy);
    const int originalCount = sourceModel()->columnCount(parent);
    if (d->m_insertDirection & InsertColumn)
        return originalCount + 1;
    return originalCount;
}

/*!
\reimp
*/
QVariant InsertProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!sourceModel())
        return QVariant();
    Q_D(const InsertProxy);
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
bool InsertProxy::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    Q_D(InsertProxy);
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
QVariant InsertProxy::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid())
        return QVariant();
    if (proxyIndex.parent().isValid())
        return QVariant();
    if (!sourceModel())
        return QVariant();
    Q_D(const InsertProxy);
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
bool InsertProxy::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if (!sourceModel())
        return false;
    const int sourceRows = sourceModel()->rowCount();
    const int sourceCols = sourceModel()->columnCount();
    Q_D(InsertProxy);
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
    return setItemData(index, QMap<int, QVariant>{{std::make_pair(role,value)}});
}

/*!
\reimp
*/
bool InsertProxy::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    if (!index.isValid())
        return false;
    if (!sourceModel())
        return false;
    const int sourceRows = sourceModel()->rowCount();
    const int sourceCols = sourceModel()->columnCount();
    Q_D(InsertProxy);
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
QModelIndex InsertProxy::mapFromSource(const QModelIndex &sourceIndex) const
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
QModelIndex InsertProxy::mapToSource(const QModelIndex &proxyIndex) const
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
InsertProxy::InsertDirections InsertProxy::insertDirection() const
{
    Q_D(const InsertProxy);
    return d->m_insertDirection;
}

/*!
\reimp
*/
Qt::ItemFlags InsertProxy::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    if (!sourceModel())
        return Qt::NoItemFlags;
    Q_D(const InsertProxy);
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
QModelIndex InsertProxy::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QModelIndex();
}

/*!
\reimp
*/
QModelIndex InsertProxy::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
        return QModelIndex();
    return createIndex(row, column);
}
/*!
\reimp
*/
bool InsertProxy::insertRows(int row, int count, const QModelIndex &parent)
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
bool InsertProxy::removeRows(int row, int count, const QModelIndex &parent)
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
bool InsertProxy::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
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
bool InsertProxy::insertColumns(int column, int count, const QModelIndex &parent)
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
bool InsertProxy::removeColumns(int column, int count, const QModelIndex &parent)
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
bool InsertProxy::moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent, int destinationChild)
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
void InsertProxy::sort(int column, Qt::SortOrder order) 
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
Qt::ItemFlags InsertProxy::flagForExtra(bool isRow, int section) const
{
    Q_UNUSED(isRow)
    Q_UNUSED(section)
    return (Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

void InsertProxy::setInsertDirection(const InsertDirections& direction)
{
    Q_D(InsertProxy);
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
QVariant InsertProxy::dataForCorner(int role) const
{
    Q_D(const InsertProxy);
    return d->m_dataForCorner.value(role);
}

/*!
Set the data for the corner cell at the intersection between the extra row and extra column
*/
void InsertProxy::setDataForCorner(const QVariant& value, int role)
{
    Q_D(InsertProxy);
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
void InsertProxy::commitRow()
{
    Q_D(InsertProxy);
    d->commitRow();
}

/*!
Forces the extra row to be added to the source model
*/
void InsertProxy::commitColumn()
{
    Q_D(InsertProxy);
    d->commitColumn();
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

bool InsertProxy::separateEditDisplay() const
{
    Q_D(const InsertProxy);
    return d->m_separateEditDisplay;
}

void InsertProxy::setSeparateEditDisplay(bool val)
{
    Q_D(InsertProxy);
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
\details The default implementation commits the row to the model as soon as there's any data in any cell
*/
bool InsertProxy::validRow() const
{
    Q_D(const InsertProxy);
    if (!sourceModel())
        return false;
    const int sourceCols = sourceModel()->columnCount();
    for (int i = 0; i < sourceCols; ++i) {
        if (!d->m_extraData[true].value(i).isEmpty())
            return true;
    }
    return false;
}

/*!
\brief Returns true if the extra column can be merged in the main model
\details The default implementation commits the column to the model as soon as there's any data in any cell
*/
bool InsertProxy::validColumn() const
{
    Q_D(const InsertProxy);
    if (!sourceModel())
        return false;
    const int sourceRows = sourceModel()->rowCount();
    for (int i = 0; i < sourceRows; ++i) {
        if (!d->m_extraData[false].value(i).isEmpty())
            return true;
    }
    return false;
}


/*!
\class InsertProxy
\brief This proxy model provides an extra row and column to handle user insertions
\details This proxy will add an extra row, column or both to allow users to insert new sections with a familiar interface.

You can use setInsertDirection to determine whether to show an extra row or column. By default, this model will behave as QIdentityProxyModel

You can either call commitRow/commitColumn or reimplement validRow/validColumn to decide when a row/column should be added to the main model.
By default the row/column is committed as soon as there is any data in any cell

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

