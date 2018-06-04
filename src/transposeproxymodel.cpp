#include "transposeproxymodel.h"
#include "private/transposeproxymodel_p.h"

/*!
\internal
*/
TransposeProxyModelPrivate::TransposeProxyModelPrivate(TransposeProxyModel* q)
    :q_ptr(q)
{
    Q_ASSERT(q_ptr);
}

/*!
Constructs a new proxy model with the given \a parent.
*/
TransposeProxyModel::TransposeProxyModel(QObject* parent)
    : TransposeProxyModel(*new TransposeProxyModelPrivate(this), parent)
{}

/*!
Constructor used only while subclassing the private class.
Not part of the public API
*/
TransposeProxyModel::TransposeProxyModel(TransposeProxyModelPrivate& dptr, QObject* parent)
    : QAbstractProxyModel(parent)
    , m_dptr(&dptr)
{}

/*!
Destructor
*/
TransposeProxyModel::~TransposeProxyModel()
{
    delete m_dptr;
}

/*!
\reimp
*/
void TransposeProxyModel::setSourceModel(QAbstractItemModel* newSourceModel) 
{
    Q_D(TransposeProxyModel);
    beginResetModel();
    if (sourceModel()) {
        for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
            Q_ASSUME(QObject::disconnect(*discIter));
    }
    QAbstractProxyModel::setSourceModel(newSourceModel);
    d->m_sourceConnections.clear();
    if (sourceModel()) {
        d->m_sourceConnections
            << QObject::connect(sourceModel(), &QAbstractItemModel::modelAboutToBeReset, this, [this]()->void { beginResetModel(); })
            << QObject::connect(sourceModel(), &QAbstractItemModel::modelReset, this, [this]()->void { endResetModel(); })
            << QObject::connect(sourceModel(), &QAbstractItemModel::dataChanged, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
                dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight), roles);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::headerDataChanged, this, [this](Qt::Orientation orientation, int first, int last)->void { 
                headerDataChanged(orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal, first, last); 
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeInserted, this, [this](const QModelIndex &parent, int first, int last) {
                beginInsertRows(mapFromSource(parent), first, last);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeMoved, this, [this](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn) {
                beginMoveRows(mapFromSource(sourceParent), sourceStart, sourceEnd, mapFromSource(destinationParent), destinationColumn);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeRemoved, this, [this](const QModelIndex &parent, int first, int last) {
                beginRemoveRows(mapFromSource(parent), first, last);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::columnsInserted, this, [this](const QModelIndex &parent)->void {
                endInsertRows();
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::columnsRemoved, this, [this](const QModelIndex &parent)->void {
                endRemoveRows();
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::columnsMoved, this, [this]()->void {
                endMoveRows();
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted, this, [this](const QModelIndex &parent, int first, int last) {
                beginInsertColumns(mapFromSource(parent), first, last);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeMoved, this, [this](const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow) {
                beginMoveColumns(mapFromSource(sourceParent), sourceStart, sourceEnd, mapFromSource(destinationParent), destinationRow);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, this, [this](const QModelIndex &parent, int first, int last) {
                beginRemoveColumns(mapFromSource(parent), first, last);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, [this]()->void {
                endInsertColumns();
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, [this]()->void {
                endRemoveColumns();
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::rowsMoved, this, [this]()->void {
                endMoveColumns();
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged, this, [d](const QList<QPersistentModelIndex>& parents, QAbstractItemModel::LayoutChangeHint hint){
                d->onLayoutAboutToBeChanged(parents, hint);
            })
            << QObject::connect(sourceModel(), &QAbstractItemModel::layoutChanged, this, [d](const QList<QPersistentModelIndex>& parents, QAbstractItemModel::LayoutChangeHint hint) {
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
    return index;
}

/*!
\reimp
*/
bool TransposeProxyModel::hasChildren(const QModelIndex &parent) const
{
    if (!sourceModel())
        return false;
    return sourceModel()->hasChildren(mapToSource(parent));
}

/*!
\reimp
*/
int TransposeProxyModel::rowCount(const QModelIndex &parent) const 
{
    if (!sourceModel())
        return 0;
    return sourceModel()->columnCount(mapToSource(parent));
}

/*!
\reimp
*/
int TransposeProxyModel::columnCount(const QModelIndex &parent) const
{
    if (!sourceModel())
        return 0;
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
    return sourceModel()->data(mapToSource(proxyIndex), role);
}

/*!
\reimp
*/
bool TransposeProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!sourceModel())
        return false;
    return sourceModel()->setData(mapToSource(index), value, role);
}

/*!
\reimp
*/
bool TransposeProxyModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    if (!sourceModel())
        return false;
    return sourceModel()->setItemData(mapToSource(index), roles);
}

/*!
\reimp
*/
QMap<int, QVariant> TransposeProxyModel::itemData(const QModelIndex &index) const 
{
    if (!sourceModel())
        return QMap<int, QVariant>();
    return sourceModel()->itemData(mapToSource(index));
}

/*!
\reimp
*/
QModelIndex TransposeProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid() || !sourceModel())
        return QModelIndex();
    Q_ASSERT(sourceIndex.model() == sourceModel());
    return QAbstractItemModel::createIndex(sourceIndex.column(), sourceIndex.row(), sourceIndex.internalPointer());
}

/*!
\reimp
*/
QModelIndex TransposeProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid() || !sourceModel())
        return QModelIndex();
    Q_ASSERT(proxyIndex.model() == this);
    return QAbstractItemModel::createIndex(proxyIndex.column(), proxyIndex.row(), proxyIndex.internalPointer());
}

/*!
\reimp
*/
Qt::ItemFlags TransposeProxyModel::flags(const QModelIndex &index) const 
{
    if (!sourceModel())
        Qt::NoItemFlags;
    Q_ASSERT(index.model() == this);
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
    if (!sourceModel());
        return QModelIndex();
    return mapFromSource(sourceModel()->index(column, row, mapToSource(parent)));
}

/*!
\reimp
*/
QModelIndex TransposeProxyModel::sibling(int row, int column, const QModelIndex &idx) const
{
    if (!sourceModel());
    return QModelIndex();
    return mapFromSource(sourceModel()->sibling(row, column, mapToSource(idx)));
}

/*!
\reimp
*/
bool TransposeProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (!sourceModel())
        return false;
    return sourceModel()->insertColumns(row, count, mapToSource(parent));
}

/*!
\reimp
*/
bool TransposeProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!sourceModel())
        return false;
    return sourceModel()->removeColumns(row, count, mapToSource(parent));
}

/*!
\reimp
*/
bool TransposeProxyModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (!sourceModel())
        return false;
    return sourceModel()->moveColumns(mapToSource(sourceParent), sourceRow, count, mapToSource(destinationParent), destinationChild);
}

/*!
\reimp
*/
bool TransposeProxyModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    if (!sourceModel())
        return false;
    return sourceModel()->insertRows(column, count, mapToSource(parent));
}

/*!
\reimp
*/
bool TransposeProxyModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    if (!sourceModel())
        return false;
    return sourceModel()->removeRows(column, count, mapToSource(parent));
}

/*!
\reimp
*/
bool TransposeProxyModel::moveColumns(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (!sourceModel())
        return false;
    return sourceModel()->moveRows(mapToSource(sourceParent), sourceRow, count, mapToSource(destinationParent), destinationChild);
}

/*!
\reimp
*/
void TransposeProxyModel::sort(int column, Qt::SortOrder order)
{
    // There is no method to sort column generically in QAbstractItemModel
    return;
}

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
\class TransposeProxyModel
\brief This proxy transposes the source model
\details This model will make the rows of the source model become columns of and vice-versa 
If the model is a tree the parents will be transposed as well. For example, if an index in the source model had parent `index(2,0)` it will have parent `index(2,0)` in the proxy
*/