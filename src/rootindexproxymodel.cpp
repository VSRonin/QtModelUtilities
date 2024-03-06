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
#include "private/rootindexproxymodel_p.h"
#include "rootindexproxymodel.h"
#include <functional>
#include <QSize>

RootIndexProxyModelPrivate::RootIndexProxyModelPrivate(RootIndexProxyModel *q)
    : q_ptr(q)
    , m_rootRowDeleted(false)
    , m_rootColumnDeleted(false)
{
    Q_ASSERT(q_ptr);
    QObject::connect(q, &RootIndexProxyModel::sourceModelChanged, std::bind(&RootIndexProxyModelPrivate::resetRootOnModelChange, this));
}

bool RootIndexProxyModelPrivate::isDescendant(QModelIndex childIdx, const QModelIndex &parentIdx) const
{
    Q_ASSERT(childIdx.isValid());
    if (!parentIdx.isValid())
        return true;
    Q_ASSERT(childIdx.model() == parentIdx.model());
    for (childIdx = childIdx.parent(); childIdx.isValid(); childIdx = childIdx.parent()) {
        if (childIdx == parentIdx)
            return true;
    }
    return false;
}

void RootIndexProxyModelPrivate::resetRootOnModelChange()
{
    m_rootIndex = QModelIndex();
}

void RootIndexProxyModelPrivate::checkRootRowRemoved(const QModelIndex &parent, int first, int last)
{
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!parent.isValid() || parent.model() == q->sourceModel());
    if (!m_rootIndex.isValid())
        return;
    if (!isDescendant(m_rootIndex, parent))
        return;
    if (m_rootIndex.row() >= first && m_rootIndex.row() <= last) {
        m_rootRowDeleted = true;
        q->setRootIndex(QModelIndex());
    }
}

void RootIndexProxyModelPrivate::checkRootColumnsRemoved(const QModelIndex &parent, int first, int last)
{
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!parent.isValid() || parent.model() == q->sourceModel());
    if (!m_rootIndex.isValid())
        return;
    if (!isDescendant(m_rootIndex, parent))
        return;
    if (m_rootIndex.column() >= first && m_rootIndex.column() <= last) {
        m_rootColumnDeleted = true;
        q->setRootIndex(QModelIndex());
    }
}

void RootIndexProxyModelPrivate::onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!parent.isValid() || parent.model() == q->sourceModel());
    if (m_rootIndex.isValid()) {
        if (isDescendant(m_rootIndex, parent))
            return;
        if (parent != m_rootIndex && !isDescendant(parent, m_rootIndex))
            return;
    }
    q->beginInsertRows(q->mapFromSource(parent), first, last);
}

void RootIndexProxyModelPrivate::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(first)
    Q_UNUSED(last)
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!parent.isValid() || parent.model() == q->sourceModel());
    if (m_rootIndex.isValid()) {
        if (isDescendant(m_rootIndex, parent))
            return;
        if (parent != m_rootIndex && !isDescendant(parent, m_rootIndex))
            return;
    }
    q->endInsertRows();
}

void RootIndexProxyModelPrivate::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!parent.isValid() || parent.model() == q->sourceModel());
    if (m_rootIndex.isValid()) {
        if (isDescendant(m_rootIndex, parent))
            return;
        if (parent != m_rootIndex && !isDescendant(parent, m_rootIndex))
            return;
    }
    q->beginRemoveRows(q->mapFromSource(parent), first, last);
}

void RootIndexProxyModelPrivate::onRowsRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(first)
    Q_UNUSED(last)
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!parent.isValid() || parent.model() == q->sourceModel());
    if (m_rootIndex.isValid()) {
        if (isDescendant(m_rootIndex, parent))
            return;
        if (parent != m_rootIndex && !isDescendant(parent, m_rootIndex))
            return;
    }
    if (m_rootRowDeleted)
        m_rootRowDeleted = false;
    else
        q->endRemoveRows();
}

bool RootIndexProxyModelPrivate::ignoreMove(const QModelIndex &sourceParent, const QModelIndex &destParent) const
{
    if (sourceParent == m_rootIndex || m_rootIndex == destParent)
        return false;
    if (sourceParent.isValid()) {
        if (isDescendant(sourceParent, m_rootIndex))
            return false;
    }
    if (destParent.isValid()) {
        if (isDescendant(destParent, m_rootIndex))
            return false;
    }
    return true;
}

void RootIndexProxyModelPrivate::onRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent,
                                                      int dest)
{
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == q->sourceModel());
    Q_ASSERT(!destParent.isValid() || destParent.model() == q->sourceModel());
    if (ignoreMove(sourceParent, destParent))
        return;
    const bool isDescendantRootDestParent = isDescendant(m_rootIndex, destParent);
    if (isDescendantRootDestParent == isDescendant(m_rootIndex, sourceParent)) {
        if (isDescendantRootDestParent)
            return;
        q->beginMoveRows(q->mapFromSource(sourceParent), sourceStart, sourceEnd, q->mapFromSource(destParent), dest);
    } else if (isDescendantRootDestParent) {

        q->beginRemoveRows(q->mapFromSource(sourceParent), sourceStart, sourceEnd);
    } else {
        q->beginInsertRows(q->mapFromSource(destParent), dest, dest + sourceEnd - sourceStart);
    }
}

void RootIndexProxyModelPrivate::onRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
{
    Q_UNUSED(sourceStart)
    Q_UNUSED(sourceEnd)
    Q_UNUSED(dest)
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == q->sourceModel());
    Q_ASSERT(!destParent.isValid() || destParent.model() == q->sourceModel());
    if (ignoreMove(sourceParent, destParent))
        return;
    const bool isDescendantRootDestParent = isDescendant(m_rootIndex, destParent);
    if (isDescendantRootDestParent == isDescendant(m_rootIndex, sourceParent)) {
        if (isDescendantRootDestParent)
            return;
        q->endMoveRows();
    } else if (isDescendantRootDestParent) {
        q->endRemoveRows();
    } else {
        q->endInsertRows();
    }
}

void RootIndexProxyModelPrivate::onColumnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent,
                                                int dest)
{
    Q_UNUSED(sourceStart)
    Q_UNUSED(sourceEnd)
    Q_UNUSED(dest)
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == q->sourceModel());
    Q_ASSERT(!destParent.isValid() || destParent.model() == q->sourceModel());
    if (ignoreMove(sourceParent, destParent))
        return;
    const bool isDescendantRootDestParent = isDescendant(m_rootIndex, destParent);
    if (isDescendantRootDestParent == isDescendant(m_rootIndex, sourceParent)) {
        if (isDescendantRootDestParent)
            return;
        if (destParent != m_rootIndex && !isDescendant(destParent, m_rootIndex))
            return;
        q->endMoveColumns();
    } else if (isDescendantRootDestParent)
        q->endRemoveColumns();
    else
        q->endInsertColumns();
}

void RootIndexProxyModelPrivate::onColumnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                                                         const QModelIndex &destParent, int dest)
{
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == q->sourceModel());
    Q_ASSERT(!destParent.isValid() || destParent.model() == q->sourceModel());
    if (ignoreMove(sourceParent, destParent))
        return;
    const bool isDescendantRootDestParent = isDescendant(m_rootIndex, destParent);
    if (isDescendantRootDestParent == isDescendant(m_rootIndex, sourceParent)) {
        if (isDescendantRootDestParent)
            return;
        if (destParent != m_rootIndex && !isDescendant(destParent, m_rootIndex))
            return;
        q->beginMoveColumns(q->mapFromSource(sourceParent), sourceStart, sourceEnd, q->mapFromSource(destParent), dest);
    } else if (isDescendantRootDestParent)
        q->beginRemoveColumns(q->mapFromSource(sourceParent), sourceStart, sourceEnd);
    else
        q->beginInsertColumns(q->mapFromSource(destParent), dest, dest + sourceEnd - sourceStart);
}

void RootIndexProxyModelPrivate::onColumnsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!parent.isValid() || parent.model() == q->sourceModel());
    if (m_rootIndex.isValid()) {
        if (isDescendant(m_rootIndex, parent))
            return;
        if (parent != m_rootIndex && !isDescendant(parent, m_rootIndex))
            return;
    }
    q->beginInsertColumns(q->mapFromSource(parent), first, last);
}

void RootIndexProxyModelPrivate::onColumnsInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(first)
    Q_UNUSED(last)
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!parent.isValid() || parent.model() == q->sourceModel());
    if (m_rootIndex.isValid()) {
        if (isDescendant(m_rootIndex, parent))
            return;
        if (parent != m_rootIndex && !isDescendant(parent, m_rootIndex))
            return;
    }
    q->endInsertColumns();
}

void RootIndexProxyModelPrivate::onColumnsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!parent.isValid() || parent.model() == q->sourceModel());
    if (m_rootIndex.isValid()) {
        if (isDescendant(m_rootIndex, parent))
            return;
        if (parent != m_rootIndex && !isDescendant(parent, m_rootIndex))
            return;
    }
    q->beginRemoveColumns(q->mapFromSource(parent), first, last);
}

void RootIndexProxyModelPrivate::onColumnsRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(first)
    Q_UNUSED(last)
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(!parent.isValid() || parent.model() == q->sourceModel());
    if (m_rootIndex.isValid()) {
        if (isDescendant(m_rootIndex, parent))
            return;
        if (parent != m_rootIndex && !isDescendant(parent, m_rootIndex))
            return;
    }
    if (m_rootColumnDeleted)
        m_rootColumnDeleted = false;
    else
        q->endRemoveColumns();
}

void RootIndexProxyModelPrivate::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_Q(RootIndexProxyModel);
    Q_ASSERT(topLeft.isValid());
    Q_ASSERT(bottomRight.isValid());
    Q_ASSERT(topLeft.model() == q->sourceModel());
    Q_ASSERT(bottomRight.model() == q->sourceModel());
    Q_ASSERT(bottomRight.parent() == topLeft.parent());
    if (m_rootIndex.isValid()) {
        if (isDescendant(m_rootIndex, topLeft.parent()))
            return;
    }
    q->dataChanged(q->mapFromSource(topLeft), q->mapFromSource(bottomRight), roles);
}

void RootIndexProxyModelPrivate::onLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents,
                                                          QAbstractItemModel::LayoutChangeHint hint)
{
    Q_Q(RootIndexProxyModel);
    QList<QPersistentModelIndex> parents;
    parents.reserve(sourceParents.size());
    bool addedInvalidIndex = false;
    for (const QPersistentModelIndex &parent : sourceParents) {
        if (!parent.isValid() || isDescendant(m_rootIndex, parent)) {
            if (!addedInvalidIndex) {
                addedInvalidIndex = true;
                parents << QPersistentModelIndex();
            }
            continue;
        }
        const QModelIndex mappedParent = q->mapFromSource(parent);
        Q_ASSERT(mappedParent.isValid());
        parents << mappedParent;
    }
    q->layoutAboutToBeChanged(parents, hint);
    const auto proxyPersistentIndexes = q->persistentIndexList();
    for (const QModelIndex &proxyPersistentIndex : proxyPersistentIndexes) {
        proxyIndexes << proxyPersistentIndex;
        Q_ASSERT(proxyPersistentIndex.isValid());
        const QPersistentModelIndex srcPersistentIndex = q->mapToSource(proxyPersistentIndex);
        Q_ASSERT(srcPersistentIndex.isValid());
        layoutChangePersistentIndexes << srcPersistentIndex;
    }
}

void RootIndexProxyModelPrivate::onLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint)
{
    Q_Q(RootIndexProxyModel);
    for (int i = 0; i < proxyIndexes.size(); ++i)
        q->changePersistentIndex(proxyIndexes.at(i), q->mapFromSource(layoutChangePersistentIndexes.at(i)));
    layoutChangePersistentIndexes.clear();
    proxyIndexes.clear();
    QList<QPersistentModelIndex> parents;
    parents.reserve(sourceParents.size());
    bool addedInvalidIndex = false;
    for (const QPersistentModelIndex &parent : sourceParents) {
        if (!parent.isValid() || isDescendant(m_rootIndex, parent)) {
            if (!addedInvalidIndex) {
                addedInvalidIndex = true;
                parents << QPersistentModelIndex();
            }
            continue;
        }
        const QModelIndex mappedParent = q->mapFromSource(parent);
        Q_ASSERT(mappedParent.isValid());
        parents << mappedParent;
    }
    q->layoutChanged(parents, hint);
}

/*!
Constructs a new proxy model with the given \a parent.
*/
RootIndexProxyModel::RootIndexProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
    , m_dptr(new RootIndexProxyModelPrivate(this))
{ }

/*!
\internal
*/
RootIndexProxyModel::RootIndexProxyModel(RootIndexProxyModelPrivate &dptr, QObject *parent)
    : QIdentityProxyModel(parent)
    , m_dptr(&dptr)
{ }

/*!
Destructor
*/
RootIndexProxyModel::~RootIndexProxyModel()
{
    Q_D(RootIndexProxyModel);
    for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
        QObject::disconnect(*discIter);
    delete m_dptr;
}

/*!
\property RootIndexProxyModel::rootIndex
\accessors %rootIndex(), setRootIndex()
\notifier rootIndexChanged()
\brief This property holds the root index of the model
\details The root index is an index of the source model used as root by all elements of the proxy model
*/

//! Getter for rootIndex property
QModelIndex RootIndexProxyModel::rootIndex() const
{
    Q_D(const RootIndexProxyModel);
    return d->m_rootIndex;
}

//! Setter for rootIndex property
void RootIndexProxyModel::setRootIndex(const QModelIndex &root)
{
    Q_ASSERT_X(!root.isValid() || root.model() == sourceModel(), "RootIndexProxyModel::setRootIndex",
               "The root index must be an index of the source model");
    Q_D(RootIndexProxyModel);
    if (d->m_rootIndex == root)
        return;
    beginResetModel();
    d->m_rootIndex = root;
    endResetModel();
    rootIndexChanged();
}

/*!
\reimp
*/
void RootIndexProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    Q_D(RootIndexProxyModel);
    for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
        QObject::disconnect(*discIter);
    d->m_sourceConnections.clear();
    QIdentityProxyModel::setSourceModel(sourceModel);
    if (sourceModel) {
        Q_ASSUME(disconnect(sourceModel, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)), this,
                            SLOT(_q_sourceRowsAboutToBeInserted(QModelIndex, int, int))));
        Q_ASSUME(disconnect(sourceModel, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(_q_sourceRowsInserted(QModelIndex, int, int))));
        Q_ASSUME(disconnect(sourceModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this,
                            SLOT(_q_sourceRowsAboutToBeRemoved(QModelIndex, int, int))));
        Q_ASSUME(disconnect(sourceModel, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(_q_sourceRowsRemoved(QModelIndex, int, int))));
        Q_ASSUME(disconnect(sourceModel, SIGNAL(columnsAboutToBeInserted(QModelIndex, int, int)), this,
                            SLOT(_q_sourceColumnsAboutToBeInserted(QModelIndex, int, int))));
        Q_ASSUME(
                disconnect(sourceModel, SIGNAL(columnsInserted(QModelIndex, int, int)), this, SLOT(_q_sourceColumnsInserted(QModelIndex, int, int))));
        Q_ASSUME(disconnect(sourceModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)), this,
                            SLOT(_q_sourceColumnsAboutToBeRemoved(QModelIndex, int, int))));
        Q_ASSUME(disconnect(sourceModel, SIGNAL(columnsRemoved(QModelIndex, int, int)), this, SLOT(_q_sourceColumnsRemoved(QModelIndex, int, int))));
        Q_ASSUME(disconnect(sourceModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this,
                            SLOT(_q_sourceDataChanged(QModelIndex, QModelIndex, QVector<int>))));
        Q_ASSUME(disconnect(sourceModel, SIGNAL(columnsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)), this,
                            SLOT(_q_sourceColumnsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int))));
        Q_ASSUME(disconnect(sourceModel, SIGNAL(columnsMoved(QModelIndex, int, int, QModelIndex, int)), this,
                            SLOT(_q_sourceColumnsMoved(QModelIndex, int, int, QModelIndex, int))));
        Q_ASSUME(disconnect(sourceModel, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)), this,
                            SLOT(_q_sourceRowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int))));
        Q_ASSUME(disconnect(sourceModel, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)), this,
                            SLOT(_q_sourceRowsMoved(QModelIndex, int, int, QModelIndex, int))));
        Q_ASSUME(disconnect(sourceModel, SIGNAL(layoutAboutToBeChanged(QList<QPersistentModelIndex>, QAbstractItemModel::LayoutChangeHint)), this,
                            SLOT(_q_sourceLayoutAboutToBeChanged(QList<QPersistentModelIndex>, QAbstractItemModel::LayoutChangeHint))));
        Q_ASSUME(disconnect(sourceModel, SIGNAL(layoutChanged(QList<QPersistentModelIndex>, QAbstractItemModel::LayoutChangeHint)), this,
                            SLOT(_q_sourceLayoutChanged(QList<QPersistentModelIndex>, QAbstractItemModel::LayoutChangeHint))));
        using namespace std::placeholders;
        d->m_sourceConnections
                << connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved,
                           std::bind(&RootIndexProxyModelPrivate::onRowsAboutToBeRemoved, d, _1, _2, _3))
                << connect(sourceModel, &QAbstractItemModel::columnsAboutToBeRemoved,
                           std::bind(&RootIndexProxyModelPrivate::onColumnsAboutToBeRemoved, d, _1, _2, _3))
                << connect(sourceModel, &QAbstractItemModel::rowsRemoved, std::bind(&RootIndexProxyModelPrivate::onRowsRemoved, d, _1, _2, _3))
                << connect(sourceModel, &QAbstractItemModel::columnsRemoved, std::bind(&RootIndexProxyModelPrivate::onColumnsRemoved, d, _1, _2, _3))
                << connect(sourceModel, &QAbstractItemModel::rowsAboutToBeInserted,
                           std::bind(&RootIndexProxyModelPrivate::onRowsAboutToBeInserted, d, _1, _2, _3))
                << connect(sourceModel, &QAbstractItemModel::columnsAboutToBeInserted,
                           std::bind(&RootIndexProxyModelPrivate::onColumnsAboutToBeInserted, d, _1, _2, _3))
                << connect(sourceModel, &QAbstractItemModel::rowsInserted, std::bind(&RootIndexProxyModelPrivate::onRowsInserted, d, _1, _2, _3))
                << connect(sourceModel, &QAbstractItemModel::columnsInserted,
                           std::bind(&RootIndexProxyModelPrivate::onColumnsInserted, d, _1, _2, _3))
                << connect(sourceModel, &QAbstractItemModel::dataChanged, std::bind(&RootIndexProxyModelPrivate::onDataChanged, d, _1, _2, _3))
                << connect(sourceModel, &QAbstractItemModel::rowsAboutToBeMoved,
                           std::bind(&RootIndexProxyModelPrivate::onRowsAboutToBeMoved, d, _1, _2, _3, _4, _5))
                << connect(sourceModel, &QAbstractItemModel::columnsAboutToBeMoved,
                           std::bind(&RootIndexProxyModelPrivate::onColumnsAboutToBeMoved, d, _1, _2, _3, _4, _5))
                << connect(sourceModel, &QAbstractItemModel::rowsMoved, std::bind(&RootIndexProxyModelPrivate::onRowsMoved, d, _1, _2, _3, _4, _5))
                << connect(sourceModel, &QAbstractItemModel::columnsMoved,
                           std::bind(&RootIndexProxyModelPrivate::onColumnsMoved, d, _1, _2, _3, _4, _5))
                << connect(sourceModel, &QAbstractItemModel::layoutAboutToBeChanged,
                           std::bind(&RootIndexProxyModelPrivate::onLayoutAboutToBeChanged, d, _1, _2))
                << connect(sourceModel, &QAbstractItemModel::layoutChanged, std::bind(&RootIndexProxyModelPrivate::onLayoutChanged, d, _1, _2))

                << connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved,
                           std::bind(&RootIndexProxyModelPrivate::checkRootRowRemoved, d, _1, _2, _3))
                << connect(sourceModel, &QAbstractItemModel::columnsAboutToBeRemoved,
                           std::bind(&RootIndexProxyModelPrivate::checkRootColumnsRemoved, d, _1, _2, _3));
    }
}

/*!
\reimp
*/
QModelIndex RootIndexProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!sourceModel())
        return QModelIndex();
    return QIdentityProxyModel::mapToSource(proxyIndex);
}

/*!
\reimp
*/
QModelIndex RootIndexProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceModel())
        return QModelIndex();
    if (!sourceIndex.isValid())
        return QModelIndex();
    Q_D(const RootIndexProxyModel);
    if (d->m_rootIndex.isValid()) {
        if (!d->isDescendant(sourceIndex, d->m_rootIndex))
            return QModelIndex();
    }
    return QIdentityProxyModel::mapFromSource(sourceIndex);
}

/*!
\reimp
*/
QModelIndex RootIndexProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return QModelIndex();
    Q_D(const RootIndexProxyModel);
    QModelIndex sourceParent;
    if (parent.isValid())
        sourceParent = mapToSource(parent);
    else
        sourceParent = d->m_rootIndex;
    const QModelIndex sourceIndex = sourceModel()->index(row, column, sourceParent);
    return mapFromSource(sourceIndex);
}

/*!
\reimp
*/
int RootIndexProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return 0;
    Q_D(const RootIndexProxyModel);
    QModelIndex sourceParent;
    if (parent.isValid())
        sourceParent = mapToSource(parent);
    else
        sourceParent = d->m_rootIndex;
    return sourceModel()->columnCount(sourceParent);
}

/*!
\reimp
*/
int RootIndexProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return 0;
    Q_D(const RootIndexProxyModel);
    QModelIndex sourceParent;
    if (parent.isValid())
        sourceParent = mapToSource(parent);
    else
        sourceParent = d->m_rootIndex;
    return sourceModel()->rowCount(sourceParent);
}

/*!
\reimp
*/
bool RootIndexProxyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return 0;
    Q_D(const RootIndexProxyModel);
    QModelIndex sourceParent;
    if (parent.isValid())
        sourceParent = mapToSource(parent);
    else
        sourceParent = d->m_rootIndex;
    return sourceModel()->dropMimeData(data, action, row, column, sourceParent);
}

/*!
\reimp
*/
bool RootIndexProxyModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return 0;
    Q_D(RootIndexProxyModel);
    QModelIndex sourceParent;
    if (parent.isValid())
        sourceParent = mapToSource(parent);
    else
        sourceParent = d->m_rootIndex;
    return sourceModel()->insertColumns(column, count, sourceParent);
}
/*!
\reimp
*/
bool RootIndexProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return 0;
    Q_D(RootIndexProxyModel);
    QModelIndex sourceParent;
    if (parent.isValid())
        sourceParent = mapToSource(parent);
    else
        sourceParent = d->m_rootIndex;
    return sourceModel()->insertRows(row, count, sourceParent);
}

/*!
\reimp
*/
bool RootIndexProxyModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(RootIndexProxyModel);
    QModelIndex sourceParent;
    if (parent.isValid())
        sourceParent = mapToSource(parent);
    else
        sourceParent = d->m_rootIndex;
    return sourceModel()->removeColumns(column, count, sourceParent);
}

/*!
\reimp
*/
bool RootIndexProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(RootIndexProxyModel);
    QModelIndex sourceParent;
    if (parent.isValid())
        sourceParent = mapToSource(parent);
    else
        sourceParent = d->m_rootIndex;
    return sourceModel()->removeRows(row, count, sourceParent);
}

/*!
\reimp
*/
bool RootIndexProxyModel::hasChildren(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(const RootIndexProxyModel);
    QModelIndex sourceParent;
    if (parent.isValid())
        sourceParent = mapToSource(parent);
    else
        sourceParent = d->m_rootIndex;
    return sourceModel()->hasChildren(sourceParent);
}

/*!
\reimp
*/
bool RootIndexProxyModel::canFetchMore(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(const RootIndexProxyModel);
    QModelIndex sourceParent;
    if (parent.isValid())
        sourceParent = mapToSource(parent);
    else
        sourceParent = d->m_rootIndex;
    return sourceModel()->canFetchMore(sourceParent);
}

/*!
\reimp
*/
void RootIndexProxyModel::fetchMore(const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (!sourceModel())
        return;
    Q_D(RootIndexProxyModel);
    QModelIndex sourceParent;
    if (parent.isValid())
        sourceParent = mapToSource(parent);
    else
        sourceParent = d->m_rootIndex;
    sourceModel()->fetchMore(sourceParent);
}

/*!
\reimp
*/
bool RootIndexProxyModel::moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent,
                                      int destinationChild)
{
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == this);
    Q_ASSERT(!destinationParent.isValid() || destinationParent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(RootIndexProxyModel);
    QModelIndex srcParent;
    if (sourceParent.isValid())
        srcParent = mapToSource(sourceParent);
    else
        srcParent = d->m_rootIndex;
    QModelIndex destParent;
    if (destinationParent.isValid())
        destParent = mapToSource(destinationParent);
    else
        destParent = d->m_rootIndex;
    return sourceModel()->moveColumns(srcParent, sourceColumn, count, destParent, destinationChild);
}

/*!
\reimp
*/
bool RootIndexProxyModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent,
                                   int destinationChild)
{
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == this);
    Q_ASSERT(!destinationParent.isValid() || destinationParent.model() == this);
    if (!sourceModel())
        return false;
    Q_D(RootIndexProxyModel);
    QModelIndex srcParent;
    if (sourceParent.isValid())
        srcParent = mapToSource(sourceParent);
    else
        srcParent = d->m_rootIndex;
    QModelIndex destParent;
    if (destinationParent.isValid())
        destParent = mapToSource(destinationParent);
    else
        destParent = d->m_rootIndex;
    return sourceModel()->moveColumns(srcParent, sourceRow, count, destParent, destinationChild);
}

/*!
\class RootIndexProxyModel
\brief This proxy model will display only the portion of a tree model with the common ancestor \a rootIndex

The functionality is similar to QAbstractItemView::setRootIndex but on the model side
*/
