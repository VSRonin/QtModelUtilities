#include "transposeproxymodel.h"
#include "private/transposeproxymodel_p.h"

/*!
\internal
*/
TransposeProxyModelPrivate::TransposeProxyModelPrivate(TransposeProxyModel* q)
    :TreeMapProxyModelPrivate(q)
{}

/*!
\internal
*/
void TransposeProxyModelPrivate::onLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
    Q_Q(TransposeProxyModel);
    Q_ASSERT(q->sourceModel());
    QModelIndexList toList;
    toList.reserve(m_layoutChangePersistentIndexes.size());
    for (auto i = m_layoutChangePersistentIndexes.cbegin(), listEnd = m_layoutChangePersistentIndexes.cend(); i != listEnd; ++i) {
        toList << q->mapFromSource(*i);
    }
    q->changePersistentIndexList(m_layoutChangeProxyIndexes, toList);
    m_layoutChangeProxyIndexes.clear();
    m_layoutChangePersistentIndexes.clear();
    QList<QPersistentModelIndex> proxyParents;
    proxyParents.reserve(parents.size());
    for (auto& srcParent : parents)
        proxyParents << q->mapFromSource(srcParent);
    q->layoutChanged(proxyParents, hint);
}

/*!
\internal
*/
void TransposeProxyModelPrivate::onLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
    Q_Q(TransposeProxyModel);
    const auto proxyPersistentIndexes = q->persistentIndexList();
    m_layoutChangeProxyIndexes.clear();
    m_layoutChangePersistentIndexes.clear();
    m_layoutChangeProxyIndexes.reserve(proxyPersistentIndexes.size());
    m_layoutChangePersistentIndexes.reserve(proxyPersistentIndexes.size());
    for (const QPersistentModelIndex &proxyPersistentIndex : proxyPersistentIndexes) {
        m_layoutChangeProxyIndexes << proxyPersistentIndex;
        Q_ASSERT(proxyPersistentIndex.isValid());
        const QPersistentModelIndex srcPersistentIndex = q->mapToSource(proxyPersistentIndex);
        Q_ASSERT(srcPersistentIndex.isValid());
        m_layoutChangePersistentIndexes << srcPersistentIndex;
    }
    QList<QPersistentModelIndex> proxyParents;
    proxyParents.reserve(parents.size());
    for (auto& srcParent : parents)
        proxyParents << q->mapFromSource(srcParent);
    q->layoutAboutToBeChanged(proxyParents, hint);
}

/*!
\internal
*/
void TransposeProxyModelPrivate::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    if (!topLeft.isValid())
        return;
    if (!bottomRight.isValid())
        return;
    Q_Q(TransposeProxyModel);
    Q_ASSERT(q->sourceModel());
    Q_ASSERT(topLeft.model() == bottomRight.model());
    Q_ASSERT(topLeft.model() == q->sourceModel());
    Q_ASSERT(topLeft.parent() == bottomRight.parent());
    q->dataChanged(q->mapFromSource(topLeft), q->mapFromSource(bottomRight), roles);
}

/*!
Constructs a new proxy model with the given \a parent.
*/
TransposeProxyModel::TransposeProxyModel(QObject* parent)
    : TreeMapProxyModel(*new TransposeProxyModelPrivate(this), parent)
{}

/*!
Constructor used only while subclassing the private class.
Not part of the public API
*/
TransposeProxyModel::TransposeProxyModel(TransposeProxyModelPrivate& dptr, QObject* parent)
    : TreeMapProxyModel(dptr,parent)
{}

/*!
Destructor
*/
TransposeProxyModel::~TransposeProxyModel()
{
    Q_D(TransposeProxyModel);
    for (auto discIter = d->m_sourceConnections.cbegin(), discEnd = d->m_sourceConnections.cend(); discIter != discEnd; ++discIter)
        QObject::disconnect(*discIter);
}

/*!
\reimp
*/
void TransposeProxyModel::setSourceModel(QAbstractItemModel* newSourceModel) 
{
    if (newSourceModel == sourceModel())
        return;
    Q_D(TransposeProxyModel);
    beginResetModel();
    if (sourceModel()) {
        for (auto discIter = d->m_sourceConnections.cbegin(), discEnd = d->m_sourceConnections.cend(); discIter != discEnd; ++discIter)
            QObject::disconnect(*discIter);
    }
    d->m_sourceConnections.clear();
    TreeMapProxyModel::setSourceModel(newSourceModel);
    if (sourceModel()) {
        d->m_sourceConnections
            << QObject::connect(sourceModel(), &QAbstractItemModel::destroyed, [this]()->void { setSourceModel(Q_NULLPTR); })
            << QObject::connect(sourceModel(), &QAbstractItemModel::modelAboutToBeReset, this, &TransposeProxyModel::beginResetModel)
            << QObject::connect(sourceModel(), &QAbstractItemModel::modelReset, this, &TransposeProxyModel::endResetModel)
            << QObject::connect(sourceModel(), &QAbstractItemModel::dataChanged, [d](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
                d->onDataChanged(topLeft, bottomRight, roles);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::headerDataChanged, [this](Qt::Orientation orientation, int first, int last)->void { 
                headerDataChanged(orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal, first, last); 
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeInserted, [this](const QModelIndex &parent, int first, int last) {
                beginInsertRows(mapFromSource(parent), first, last);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeMoved, [this](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn) {
                beginMoveRows(mapFromSource(sourceParent), sourceStart, sourceEnd, mapFromSource(destinationParent), destinationColumn);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeRemoved, [this](const QModelIndex &parent, int first, int last) {
                beginRemoveRows(mapFromSource(parent), first, last);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::columnsInserted, this, &TransposeProxyModel::endInsertRows)
            << QObject::connect(sourceModel(), &QAbstractItemModel::columnsRemoved, this, &TransposeProxyModel::endRemoveRows)
            << QObject::connect(sourceModel(), &QAbstractItemModel::columnsMoved, this, &TransposeProxyModel::endMoveRows)
            << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted, [this](const QModelIndex &parent, int first, int last) {
                beginInsertColumns(mapFromSource(parent), first, last);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeMoved, [this](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow) {
                beginMoveColumns(mapFromSource(sourceParent), sourceStart, sourceEnd, mapFromSource(destinationParent), destinationRow);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, [this](const QModelIndex &parent, int first, int last) {
                beginRemoveColumns(mapFromSource(parent), first, last);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &TransposeProxyModel::endInsertColumns)
            << QObject::connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &TransposeProxyModel::endRemoveColumns)
            << QObject::connect(sourceModel(), &QAbstractItemModel::rowsMoved, this, &TransposeProxyModel::endMoveColumns)
            << QObject::connect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged, [d](const QList<QPersistentModelIndex>& parents, QAbstractItemModel::LayoutChangeHint hint){
                d->onLayoutAboutToBeChanged(parents, hint);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::layoutChanged, [d](const QList<QPersistentModelIndex>& parents, QAbstractItemModel::LayoutChangeHint hint) {
                d->onLayoutChanged(parents, hint);
            })
        ;
    }
    endResetModel();
}

/*!
\reimp
*/
QModelIndex TransposeProxyModel::buddy(const QModelIndex &index) const 
{
    Q_ASSERT(!index.isValid() || index.model() == this);
    if (!sourceModel())
        return QModelIndex();
    return mapFromSource(sourceModel()->buddy(mapToSource(index)));
}

/*!
\reimp
*/
bool TransposeProxyModel::hasChildren(const QModelIndex &parent) const
{
    if (!sourceModel())
        return false;
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    return sourceModel()->hasChildren(mapToSource(parent));
}

/*!
\reimp
*/
int TransposeProxyModel::rowCount(const QModelIndex &parent) const 
{
    if (!sourceModel())
        return 0;
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    return sourceModel()->columnCount(mapToSource(parent));
}

/*!
\reimp
*/
int TransposeProxyModel::columnCount(const QModelIndex &parent) const
{
    if (!sourceModel())
        return 0;
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    return sourceModel()->rowCount(mapToSource(parent));
}

/*!
\reimp
*/
QVariant TransposeProxyModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
    if (!sourceModel())
        return QVariant();
    return sourceModel()->headerData(section, orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal, role);
}

/*!
\reimp
*/
bool TransposeProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (!sourceModel())
        return false;
    return sourceModel()->setHeaderData(section, orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal, value, role);
}

/*!
\reimp
*/
QVariant TransposeProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!sourceModel())
        return QVariant();
    Q_ASSERT(!proxyIndex.isValid() || proxyIndex.model() == this);
    return sourceModel()->data(mapToSource(proxyIndex), role);
}

/*!
\reimp
*/
bool TransposeProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!sourceModel() || !index.isValid())
        return false;
    Q_ASSERT(index.model() == this);
    return sourceModel()->setData(mapToSource(index), value, role);
}

/*!
\reimp
*/
bool TransposeProxyModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    if (!sourceModel() || !index.isValid())
        return false;
    Q_ASSERT(index.model() == this);
    return sourceModel()->setItemData(mapToSource(index), roles);
}

/*!
\reimp
*/
QMap<int, QVariant> TransposeProxyModel::itemData(const QModelIndex &index) const 
{
    if (!sourceModel())
        return QMap<int, QVariant>();
    Q_ASSERT(!index.isValid() || index.model() == this);
    return sourceModel()->itemData(mapToSource(index));
}

/*!
\reimp
*/
QModelIndex TransposeProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    Q_D(const TransposeProxyModel);
    if (!sourceIndex.isValid() || !sourceModel())
        return QModelIndex();
    Q_ASSERT(sourceIndex.model() == sourceModel());
    const auto foundNode = indexToInternalPoiner(sourceIndex.parent());
    return createIndex(sourceIndex.column(), sourceIndex.row(), foundNode);
}

/*!
\reimp
*/
QModelIndex TransposeProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    Q_D(const TransposeProxyModel);
    if (!proxyIndex.isValid() || !sourceModel())
        return QModelIndex();
    Q_ASSERT(proxyIndex.model() == this);
    if (proxyIndex.internalPointer())
        return sourceModel()->index(proxyIndex.column(), proxyIndex.row(), internalPointerToIndex(proxyIndex.internalPointer()));
    return sourceModel()->index(proxyIndex.column(), proxyIndex.row());
}

/*!
\reimp
*/
Qt::ItemFlags TransposeProxyModel::flags(const QModelIndex &index) const 
{
    if (!sourceModel())
        return Qt::NoItemFlags;
    Q_ASSERT(!index.isValid() || index.model() == this);
    return sourceModel()->flags(mapToSource(index));
}

/*!
\reimp
*/
QModelIndex TransposeProxyModel::parent(const QModelIndex &index) const
{
    if (!sourceModel() || !index.isValid())
        return QModelIndex();
    Q_ASSERT(index.model() == this);
    return mapFromSource(mapToSource(index).parent());
}

/*!
\reimp
*/
QModelIndex TransposeProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!sourceModel())
        return QModelIndex();
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    return mapFromSource(sourceModel()->index(column, row, mapToSource(parent)));
}

/*!
\reimp
*/
QModelIndex TransposeProxyModel::sibling(int row, int column, const QModelIndex &idx) const
{
    if (!sourceModel())
        return QModelIndex();
    Q_ASSERT(!idx.isValid() || idx.model() == this);
    return mapFromSource(sourceModel()->sibling(row, column, mapToSource(idx)));
}

/*!
\reimp
*/
bool TransposeProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (!sourceModel())
        return false;
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    return sourceModel()->insertColumns(row, count, mapToSource(parent));
}

/*!
\reimp
*/
bool TransposeProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!sourceModel())
        return false;
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    return sourceModel()->removeColumns(row, count, mapToSource(parent));
}

/*!
\reimp
*/
bool TransposeProxyModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (!sourceModel())
        return false;
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == this);
    Q_ASSERT(!destinationParent.isValid() || destinationParent.model() == this);
    return sourceModel()->moveColumns(mapToSource(sourceParent), sourceRow, count, mapToSource(destinationParent), destinationChild);
}

/*!
\reimp
*/
bool TransposeProxyModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    if (!sourceModel())
        return false;
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    return sourceModel()->insertRows(column, count, mapToSource(parent));
}

/*!
\reimp
*/
bool TransposeProxyModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    if (!sourceModel())
        return false;
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    return sourceModel()->removeRows(column, count, mapToSource(parent));
}

/*!
\reimp
*/
bool TransposeProxyModel::moveColumns(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (!sourceModel())
        return false;
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == this);
    Q_ASSERT(!destinationParent.isValid() || destinationParent.model() == this);
    return sourceModel()->moveRows(mapToSource(sourceParent), sourceRow, count, mapToSource(destinationParent), destinationChild);
}

/*!
\brief This method will perform no action
\brief There is no method to sort columns generically in QAbstractItemModel so the action can't be performed. 
Use a QSortFilterProxyModel on top of this one if you require sorting
*/
void TransposeProxyModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)
    return;
}

/*!
\reimp
*/
bool TransposeProxyModel::canFetchMore(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return false;
    return sourceModel()->canFetchMore(mapToSource(parent));
}

/*!
\reimp
*/
void TransposeProxyModel::fetchMore(const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return;
    sourceModel()->fetchMore(mapToSource(parent));
}


/*!
\class TransposeProxyModel
\brief This proxy inverts the rows and columns of the source model
\details This model will make the rows of the source model become columns of and vice-versa 
If the model is a tree the parents will be transposed as well. For example, if an index in the source model had parent `index(2,0)` it will have parent `index(0,2)` in the proxy
*/