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
#include "private/genericmodel_p.h"
#include "genericmodel.h"
#include <QDateTime>
#include <functional>
#include <QMimeData>
#include <QSet>
#include <QMultiMap>
#include <QIODevice>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QPair>
#if (QT_VERSION > QT_VERSION_CHECK(5, 12, 0))
#    include <QCborValue>
#    include <QCborArray>
#    include <QCborMap>
#endif
#ifdef QT_GUI_LIB
#    include <QImage>
#    include <QBuffer>
#    include <QPixmap>
#    include <QBitmap>
#    include <QIcon>
#endif
GenericModelItem::GenericModelItem(GenericModel *model)
    : GenericModelItem(static_cast<GenericModelItem *>(nullptr))
{
    m_model = model;
}

GenericModelItem::GenericModelItem(GenericModelItem *par)
    : parent(par)
    , flags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled)
    , m_colCount(0)
    , m_rowCount(0)
    , m_row(-1)
    , m_column(-1)
    , m_rowSpan(1)
    , m_colSpan(1)
{
    if (par)
        m_model = par->m_model;
}

GenericModelItem::~GenericModelItem()
{
    qDeleteAll(children);
}

GenericModelItem *GenericModelItem::childAt(int row, int col) const
{
    const int childIndex = (row * m_colCount) + col;
    return children.at(childIndex);
}

int GenericModelItem::columnCount() const
{
    return m_colCount;
}

int GenericModelItem::rowCount() const
{
    return m_rowCount;
}

void GenericModelItem::insertColumns(int column, int count)
{
    if (m_rowCount > 0) {
        if (m_colCount == 0) {
            Q_ASSERT(column == 0);
            Q_ASSERT(children.isEmpty());
            const int childrenSize = count * m_rowCount;
            children.reserve(childrenSize);
            for (int j = 0; j < childrenSize; ++j) {
                auto newChild = new GenericModelItem(this);
                newChild->m_column = j % count;
                newChild->m_row = j / count;
                children.append(newChild);
            }
        } else {
            for (int i = column + (m_colCount * (m_rowCount - 1)); i >= 0; i -= m_colCount) {
                Q_ASSERT((i - column) % m_colCount == 0);
                const int rowIndex = (i - column) / m_colCount;
                if (rowIndex < 0)
                    continue;
                for (int j = i; j < i - column + m_colCount; ++j)
                    children.at(j)->m_column += count;
                children.insert(i, count, nullptr);
                for (int j = 0; j < count; ++j) {
                    auto newChild = new GenericModelItem(this);
                    newChild->m_column = column + j;
                    newChild->m_row = rowIndex;
                    children[i + j] = newChild;
                }
            }
        }
    }
    m_colCount += count;
#ifdef QT_DEBUG
    Q_ASSERT(m_colCount * m_rowCount == children.size());
    for (int i = 0; i < children.size(); ++i) {
        Q_ASSERT(children.at(i)->row() == i / m_colCount);
        Q_ASSERT(children.at(i)->column() == i % m_colCount);
    }
#endif
}

void GenericModelItem::removeColumns(int column, int count)
{
    if (m_rowCount > 0) {
        for (int i = column + (m_colCount * (m_rowCount - 1)); i >= 0; i -= qMax(1, m_colCount)) {
            for (int j = i + count; j < i - column + m_colCount; ++j)
                children.at(j)->m_column -= count;
            const auto endRemoveIter = children.begin() + i + count;
            const auto startRemoveIter = children.begin() + i;
            qDeleteAll(startRemoveIter, endRemoveIter);
            children.erase(startRemoveIter, endRemoveIter);
        }
    }
    m_colCount -= count;
#ifdef QT_DEBUG
    Q_ASSERT(m_colCount * m_rowCount == children.size());
    for (int i = 0; i < children.size(); ++i) {
        Q_ASSERT(children.at(i)->row() == i / m_colCount);
        Q_ASSERT(children.at(i)->column() == i % m_colCount);
    }
#endif
}

void GenericModelItem::insertRows(int row, int count)
{
    if (m_colCount > 0) {
        for (auto i = children.begin() + (row * m_colCount), iEnd = children.end(); i != iEnd; ++i)
            (*i)->m_row += count;
        children.insert(row * m_colCount, count * m_colCount, nullptr);
        for (int i = row * m_colCount; i < (row + count) * m_colCount; ++i) {
            auto newChild = new GenericModelItem(this);
            newChild->m_column = i % m_colCount;
            newChild->m_row = i / m_colCount;
            children[i] = newChild;
        }
    }
    m_rowCount += count;
#ifdef QT_DEBUG
    Q_ASSERT(m_colCount * m_rowCount == children.size());
    for (int i = 0; i < children.size(); ++i) {
        Q_ASSERT(children.at(i)->row() == i / m_colCount);
        Q_ASSERT(children.at(i)->column() == i % m_colCount);
    }
#endif
}

void GenericModelItem::removeRows(int row, int count)
{
    if (m_colCount > 0) {
        Q_ASSERT((row + count) * m_colCount <= children.size());
        const auto endRemoveIter = children.begin() + ((row + count) * m_colCount);
        const auto startRemoveIter = children.begin() + (row * m_colCount);
        for (auto i = endRemoveIter, iEnd = children.end(); i != iEnd; ++i)
            (*i)->m_row -= count;
        qDeleteAll(startRemoveIter, endRemoveIter);
        children.erase(startRemoveIter, endRemoveIter);
    }
    m_rowCount -= count;
#ifdef QT_DEBUG
    Q_ASSERT(m_colCount * m_rowCount == children.size());
    for (int i = 0; i < children.size(); ++i) {
        Q_ASSERT(children.at(i)->row() == i / m_colCount);
        Q_ASSERT(children.at(i)->column() == i % m_colCount);
    }
#endif
}

QVector<GenericModelItem *> GenericModelItem::takeRows(int row, int count)
{
    Q_ASSERT(m_colCount > 0);
    Q_ASSERT(count > 0 && row >= 0 && row < m_rowCount);
    const auto takeBegin = children.begin() + (row * m_colCount);
    const auto takeEnd = children.begin() + ((row + count) * m_colCount);
    const auto childEnd = children.end();
    for (auto i = takeEnd; i != childEnd; ++i)
        (*i)->m_row -= count;
    for (auto i = takeBegin; i != takeEnd; ++i) {
        (*i)->parent = nullptr;
        (*i)->m_row = -1;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QVector<GenericModelItem *> result(takeBegin, takeEnd);
#else
    QVector<GenericModelItem *> result(m_colCount * count, nullptr);
    std::copy(takeBegin, takeEnd, result.begin());
#endif
    children.erase(takeBegin, takeEnd);
    m_rowCount -= count;
#ifdef QT_DEBUG
    Q_ASSERT(m_colCount * m_rowCount == children.size());
    for (int i = 0; i < children.size(); ++i) {
        Q_ASSERT(children.at(i)->row() == i / m_colCount);
        Q_ASSERT(children.at(i)->column() == i % m_colCount);
    }
#endif
    return result;
}

void GenericModelItem::insertRows(int row, QVector<GenericModelItem *> rows)
{
    Q_ASSERT(row >= 0 && row <= m_rowCount);
    Q_ASSERT(!rows.isEmpty());
    Q_ASSERT(rows.count() % m_colCount == 0);
    const int count = rows.count() / m_colCount;
    for (int i = row * m_colCount, maxI = children.size(); i < maxI; ++i)
        children[i]->m_row += count;
    for (int i = 0, maxI = rows.size(); i < maxI; ++i) {
        rows[i]->parent = this;
        rows[i]->m_row = row + (i / m_colCount);
    }
    children.insert(row * m_colCount, rows.size(), nullptr);
    std::copy(rows.begin(), rows.end(), children.begin() + (row * m_colCount));
    m_rowCount += count;
#ifdef QT_DEBUG
    Q_ASSERT(m_colCount * m_rowCount == children.size());
    Q_ASSERT(std::all_of(children.constBegin(), children.constEnd(), [](GenericModelItem *item) -> bool { return item != nullptr; }));
    for (int i = 0; i < children.size(); ++i) {
        Q_ASSERT(children.at(i)->row() == i / m_colCount);
        Q_ASSERT(children.at(i)->column() == i % m_colCount);
    }
#endif
}

QVector<GenericModelItem *> GenericModelItem::takeCols(int col, int count)
{
    Q_ASSERT(m_rowCount > 0);
    Q_ASSERT(count > 0 && col >= 0 && col + count - 1 < m_colCount);
    QVector<GenericModelItem *> result;
    result.reserve(count * m_rowCount);
    for (int i = children.size() - m_colCount; i >= 0; i -= m_colCount) {
        for (int j = col + count; j < m_colCount; ++j)
            children[i + j]->m_column -= count;
        const auto takeBegin = children.begin() + i + col;
        const auto takeEnd = takeBegin + count;
        for (int j = count - 1; j >= 0; --j) {
            GenericModelItem *const child = children[i + col + j];
            child->parent = nullptr;
            child->m_column = -1;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
            result.append(child);
#else
            result.prepend(child);
#endif
        }
        children.erase(takeBegin, takeEnd);
    }
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    std::reverse(result.begin(), result.end());
#endif
    Q_ASSERT(result.size() == count * m_rowCount);
    m_colCount -= count;
#ifdef QT_DEBUG
    Q_ASSERT(m_colCount * m_rowCount == children.size());
    for (int i = 0; i < children.size(); ++i) {
        Q_ASSERT(children.at(i)->row() == i / m_colCount);
        Q_ASSERT(children.at(i)->column() == i % m_colCount);
    }
#endif
    return result;
}

void GenericModelItem::insertCols(int col, QVector<GenericModelItem *> cols)
{
    Q_ASSERT(col >= 0 && col <= m_colCount);
    Q_ASSERT(!cols.isEmpty());
    Q_ASSERT(cols.count() % m_rowCount == 0);
    const int count = cols.count() / m_rowCount;
    if (m_colCount == 0) {
        for (int i = 0; i < cols.size(); ++i) {
            cols[i]->parent = this;
            cols[i]->m_column = i % count;
        }
        children = cols;
    } else {
        auto colEnd = cols.end();
        auto colBegin = colEnd - count;
        for (int i = m_colCount * (m_rowCount - 1); i >= 0; i -= m_colCount) {
            for (int j = col; j < m_colCount; ++j)
                children[i + j]->m_column += count;
            for (int j = 0; j < count; ++j) {
                (*(colBegin + j))->parent = this;
                (*(colBegin + j))->m_column = col + j;
            }
            children.insert(i + col, count, nullptr);
            std::copy(colBegin, colEnd, children.begin() + (i + col));
            colEnd = colBegin;
            colBegin = colBegin - count;
        }
    }
    m_colCount += count;
#ifdef QT_DEBUG
    Q_ASSERT(m_colCount * m_rowCount == children.size());
    Q_ASSERT(std::all_of(children.constBegin(), children.constEnd(), [](GenericModelItem *item) -> bool { return item != nullptr; }));
    for (int i = 0; i < children.size(); ++i) {
        Q_ASSERT(children.at(i)->row() == i / m_colCount);
        Q_ASSERT(children.at(i)->column() == i % m_colCount);
    }
#endif
}

int GenericModelItem::row() const
{
    return m_row;
}

int GenericModelItem::column() const
{
    return m_column;
}

void GenericModelItem::setMergeDisplayEdit(bool val)
{
    GenericModelPrivate::setMergeDisplayEdit(val, data);
    for (int i = 0, maxI = children.size(); i < maxI; ++i)
        children[i]->setMergeDisplayEdit(val);
}

QSize GenericModelItem::span() const
{
    return QSize(m_rowSpan, m_colSpan);
}

void GenericModelItem::setSpan(const QSize &sz)
{
    m_rowSpan = sz.height();
    m_colSpan = sz.width();
}

void GenericModelItem::sortChildren(int column, int role, Qt::SortOrder order, bool recursive, QVector<RolesContainer> *headersToSort)
{
    sortChildren(column, role, order, recursive, m_model->persistentIndexList(), headersToSort);
}

void GenericModelItem::moveChildRows(int sourceRow, int count, int destinationChild)
{
    const auto sourceBegin = children.begin() + (sourceRow * m_colCount);
    const auto sourceEnd = children.begin() + ((sourceRow + count) * m_colCount);
    const auto destination = children.begin() + (destinationChild * m_colCount);
    if (destinationChild < sourceRow) {
        for (auto i = destination; i != sourceBegin; ++i)
            (*i)->m_row += count;
        for (auto i = sourceBegin; i != sourceEnd; ++i)
            (*i)->m_row -= sourceRow - destinationChild;
        std::rotate(destination, sourceBegin, sourceEnd);
    } else {
        for (auto i = sourceEnd; i != destination; ++i)
            (*i)->m_row -= count;
        for (auto i = sourceBegin; i != sourceEnd; ++i)
            (*i)->m_row += destinationChild - (sourceRow + count);
        std::rotate(sourceBegin, sourceEnd, destination);
    }
#ifdef QT_DEBUG
    for (int i = 0; i < children.size(); ++i) {
        Q_ASSERT(children.at(i)->row() == i / m_colCount);
        Q_ASSERT(children.at(i)->column() == i % m_colCount);
    }
#endif
}

void GenericModelItem::moveChildColumns(int sourceCol, int count, int destinationChild)
{
    for (int i = 0; i < children.size(); i += m_colCount) {
        const auto sourceBegin = children.begin() + i + sourceCol;
        const auto sourceEnd = children.begin() + i + sourceCol + count;
        const auto destination = children.begin() + i + destinationChild;
        if (destinationChild < sourceCol) {
            for (auto i = destination; i != sourceBegin; ++i)
                (*i)->m_column += count;
            for (auto i = sourceBegin; i != sourceEnd; ++i)
                (*i)->m_column -= sourceCol - destinationChild;
            std::rotate(destination, sourceBegin, sourceEnd);
        } else {
            for (auto i = sourceEnd; i != destination; ++i)
                (*i)->m_column -= count;
            for (auto i = sourceBegin; i != sourceEnd; ++i)
                (*i)->m_column += destinationChild - (sourceCol + count);
            std::rotate(sourceBegin, sourceEnd, destination);
        }
    }
#ifdef QT_DEBUG
    for (int i = 0; i < children.size(); ++i) {
        Q_ASSERT(children.at(i)->row() == i / m_colCount);
        Q_ASSERT(children.at(i)->column() == i % m_colCount);
    }
#endif
}

void GenericModelItem::setRow(int r)
{
    m_row = r;
}

void GenericModelItem::setColumn(int c)
{
    m_column = c;
}

void GenericModelItem::sortChildren(int column, int role, Qt::SortOrder order, bool recursive, const QModelIndexList &persistentIndexes,
                                    QVector<RolesContainer> *headersToSort)
{
    Q_ASSERT(column >= 0);
    if (children.isEmpty() || column >= m_colCount)
        return;
    if (recursive) {
        for (int i = 0, maxI = children.size(); i < maxI; ++i)
            children.at(i)->sortChildren(column, role, order, recursive, persistentIndexes, nullptr);
    }
    QVector<GenericModelItem *> columnChildren;
    columnChildren.reserve(m_rowCount);
    for (int i = column, maxI = children.size(); i < maxI; i += m_colCount) {
        GenericModelItem *columnChild = children.at(i);
        columnChildren.append(columnChild);
    }
    const auto lessComparison = [role](const GenericModelItem *a, const GenericModelItem *b) -> bool {
        return GenericModelPrivate::isVariantLessThan(a->data.value(role), b->data.value(role));
    };
    const auto greaterComparison = [role](const GenericModelItem *a, const GenericModelItem *b) -> bool {
        return GenericModelPrivate::isVariantLessThan(b->data.value(role), a->data.value(role));
    };
    if (order == Qt::AscendingOrder)
        std::stable_sort(columnChildren.begin(), columnChildren.end(), lessComparison);
    else
        std::stable_sort(columnChildren.begin(), columnChildren.end(), greaterComparison);
    QVector<RolesContainer> updatedHeadersToSort;
    if (headersToSort)
        updatedHeadersToSort = QVector<RolesContainer>(headersToSort->size(), RolesContainer());
    QVector<GenericModelItem *> newChildren;
    newChildren.reserve(children.size());
    QModelIndexList changedPersistentIndexesFrom, changedPersistentIndexesTo;
    for (int toRow = 0, maxRow = columnChildren.size(); toRow < maxRow; ++toRow) {
        GenericModelItem *const columnChild = columnChildren.at(toRow);
        const int fromRow = columnChild->m_row;
        if (headersToSort)
            updatedHeadersToSort[toRow] = headersToSort->at(fromRow);
        for (int i = m_colCount * fromRow; i < m_colCount * (fromRow + 1); ++i) {
            GenericModelItem *const iChild = children.at(i);
            if (toRow != fromRow) {
                QModelIndex fromIdx = m_model->createIndex(iChild->m_row, iChild->m_column, iChild);
                if (persistentIndexes.contains(fromIdx)) {
                    changedPersistentIndexesFrom.append(fromIdx);
                    changedPersistentIndexesTo.append(m_model->createIndex(toRow, iChild->m_column, iChild));
                }
                iChild->m_row = toRow;
            }
            newChildren.append(iChild);
        }
    }
    Q_ASSERT(newChildren.size() == children.size());
    Q_ASSERT(std::all_of(newChildren.constBegin(), newChildren.constEnd(), [](const GenericModelItem *a) -> bool { return a != nullptr; }));
    children = newChildren;
    if (headersToSort)
        *headersToSort = updatedHeadersToSort;
    m_model->changePersistentIndexList(changedPersistentIndexesFrom, changedPersistentIndexesTo);
}

bool GenericModelPrivate::isVariantLessThan(const QVariant &left, const QVariant &right)
{
    if (left.userType() == QMetaType::UnknownType)
        return false;
    if (right.userType() == QMetaType::UnknownType)
        return true;
    switch (left.userType()) {
    case QMetaType::Int:
        return left.toInt() < right.toInt();
    case QMetaType::UInt:
        return left.toUInt() < right.toUInt();
    case QMetaType::LongLong:
        return left.toLongLong() < right.toLongLong();
    case QMetaType::ULongLong:
        return left.toULongLong() < right.toULongLong();
    case QMetaType::Float:
        return left.toFloat() < right.toFloat();
    case QMetaType::Double:
        return left.toDouble() < right.toDouble();
    case QMetaType::Char:
        return left.toChar() < right.toChar();
    case QMetaType::QDate:
        return left.toDate() < right.toDate();
    case QMetaType::QTime:
        return left.toTime() < right.toTime();
    case QMetaType::QDateTime:
        return left.toDateTime() < right.toDateTime();
    case QMetaType::QString:
    default:
        return left.toString().compare(right.toString()) < 0;
    }
}

GenericModelPrivate::~GenericModelPrivate()
{
    delete root;
}

QString GenericModelPrivate::mimeDataName() const
{
    return QStringLiteral("application/x-genericmodeldatalist");
}

GenericModelPrivate::GenericModelPrivate(GenericModel *q)
    : q_ptr(q)
    , root(new GenericModelItem(q))
    , m_mergeDisplayEdit(true)
    , sortRole(Qt::DisplayRole)
{
    Q_ASSERT(q_ptr);
}

GenericModelItem *GenericModelPrivate::itemForIndex(const QModelIndex &idx) const
{
    if (!idx.isValid())
        return root;
    Q_ASSERT(idx.model() == q_func());
    return static_cast<GenericModelItem *>(idx.internalPointer());
}

QModelIndex GenericModelPrivate::indexForItem(GenericModelItem *item) const
{
    Q_ASSERT(item);
    Q_Q(const GenericModel);
    if (item == root)
        return QModelIndex();
    return q->createIndex(item->row(), item->column(), item);
}

void GenericModelPrivate::insertColumns(int column, int count, const QModelIndex &parent)
{
    if (!parent.isValid())
        hHeaderData.insert(column, count, RolesContainer());
    GenericModelItem *item = itemForIndex(parent);
    item->insertColumns(column, count);
}

void GenericModelPrivate::insertRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid())
        vHeaderData.insert(row, count, RolesContainer());
    GenericModelItem *item = itemForIndex(parent);
    item->insertRows(row, count);
}

void GenericModelPrivate::removeColumns(int column, int count, const QModelIndex &parent)
{
    if (!parent.isValid())
        hHeaderData.erase(hHeaderData.begin() + column, hHeaderData.begin() + column + count);
    GenericModelItem *item = itemForIndex(parent);
    item->removeColumns(column, count);
}

void GenericModelPrivate::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid())
        vHeaderData.erase(vHeaderData.begin() + row, vHeaderData.begin() + row + count);
    GenericModelItem *item = itemForIndex(parent);
    item->removeRows(row, count);
}

void GenericModelPrivate::moveRowsSameParent(const QModelIndex &sourceParent, int sourceRow, int count, int destinationChild)
{
    GenericModelItem *item = itemForIndex(sourceParent);
    item->moveChildRows(sourceRow, count, destinationChild);
    if (item == root) {
        const auto sourceBegin = vHeaderData.begin() + sourceRow;
        const auto sourceEnd = vHeaderData.begin() + sourceRow + count;
        const auto destination = vHeaderData.begin() + destinationChild;
        if (destinationChild < sourceRow)
            std::rotate(destination, sourceBegin, sourceEnd);
        else
            std::rotate(sourceBegin, sourceEnd, destination);
    }
}

void GenericModelPrivate::moveRowsDifferentParent(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent,
                                                  int destinationChild)
{
    Q_Q(GenericModel);
    GenericModelItem *sourceItem = itemForIndex(sourceParent);
    GenericModelItem *destinationItem = itemForIndex(destinationParent);
    QVector<GenericModelItem *> takenRows = sourceItem->takeRows(sourceRow, count);
    Q_ASSERT(!takenRows.isEmpty());
    if (sourceItem->columnCount() < destinationItem->columnCount()) {
        const int colsToInsert = destinationItem->columnCount() - sourceItem->columnCount();
        for (int i = takenRows.size(); i > 0; i -= sourceItem->columnCount()) {
            for (int j = 0; j < colsToInsert; ++j) {
                GenericModelItem *padding = new GenericModelItem(q);
                padding->setRow(-1);
                padding->setColumn(sourceItem->columnCount() + j);
                takenRows.insert(i + j, padding);
            }
        }
    }
    Q_ASSERT(takenRows.size() == count * destinationItem->columnCount());
    destinationItem->insertRows(destinationChild, takenRows);
    if (sourceItem == root)
        vHeaderData.remove(sourceRow, count);
    if (destinationItem == root)
        vHeaderData.insert(destinationChild, count, RolesContainer());
}

void GenericModelPrivate::moveColumnsSameParent(const QModelIndex &sourceParent, int sourceCol, int count, int destinationChild)
{
    GenericModelItem *item = itemForIndex(sourceParent);
    item->moveChildColumns(sourceCol, count, destinationChild);
    if (item == root) {
        const auto sourceBegin = hHeaderData.begin() + sourceCol;
        const auto sourceEnd = hHeaderData.begin() + sourceCol + count;
        const auto destination = hHeaderData.begin() + destinationChild;
        if (destinationChild < sourceCol)
            std::rotate(destination, sourceBegin, sourceEnd);
        else
            std::rotate(sourceBegin, sourceEnd, destination);
    }
}

void GenericModelPrivate::moveColumnsDifferentParent(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent,
                                                     int destinationChild)
{
    Q_Q(GenericModel);
    GenericModelItem *sourceItem = itemForIndex(sourceParent);
    GenericModelItem *destinationItem = itemForIndex(destinationParent);
    QVector<GenericModelItem *> takenCols = sourceItem->takeCols(sourceRow, count);
    Q_ASSERT(!takenCols.isEmpty());
    if (sourceItem->rowCount() < destinationItem->rowCount()) {
        const int rowsToInsert = destinationItem->rowCount() - sourceItem->rowCount();
        for (int j = 0; j < rowsToInsert; ++j) {
            for (int i = 0; i < count; ++i) {
                GenericModelItem *padding = new GenericModelItem(q);
                padding->setRow(sourceItem->rowCount() + j);
                padding->setColumn(-1);
                takenCols.append(padding);
            }
        }
    }
    Q_ASSERT(takenCols.size() == count * destinationItem->rowCount());
    destinationItem->insertCols(destinationChild, takenCols);
    if (sourceItem == root)
        hHeaderData.remove(sourceRow, count);
    if (destinationItem == root)
        hHeaderData.insert(destinationChild, count, RolesContainer());
}

/*!
Constructs a new model with the given \a parent.
*/
GenericModel::GenericModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_dptr(new GenericModelPrivate(this))
{ }

/*!
\internal
*/
GenericModel::GenericModel(GenericModelPrivate &dptr, QObject *parent)
    : QAbstractItemModel(parent)
    , m_dptr(&dptr)
{ }

/*!
 * \brief Stores the /a value in the appropriate mime type of /a data
 * \details Returns true if the data has been stored. Reimplement to support custom types.
 */
bool GenericModel::mimeForValue(QMimeData *data, const QVariant &value) const
{
    if (!data)
        return false;
    switch (value.userType()) {
    case QMetaType::Bool:
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::Double:
    case QMetaType::QChar:
    case QMetaType::QString:
    case QMetaType::Long:
    case QMetaType::LongLong:
    case QMetaType::Short:
    case QMetaType::Char:
    case QMetaType::ULong:
    case QMetaType::ULongLong:
    case QMetaType::UShort:
    case QMetaType::SChar:
    case QMetaType::UChar:
    case QMetaType::Float:
    case QMetaType::QBitArray:
    case QMetaType::QUuid:
        data->setData(QStringLiteral("text/plain"), value.toString().toUtf8());
        return true;
    case QMetaType::QByteArray:
        data->setData(QStringLiteral("text/plain"), value.toByteArray());
        return true;
    case QMetaType::QDate:
        data->setData(QStringLiteral("text/plain"), value.toDate().toString(Qt::ISODate).toUtf8());
        return true;
    case QMetaType::QTime:
        data->setData(QStringLiteral("text/plain"),
                      value.toTime()
                              .toString(
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
                                      Qt::ISODateWithMs
#else
                                      Qt::ISODate
#endif
                                      )
                              .toUtf8());
        return true;
    case QMetaType::QDateTime:
        data->setData(QStringLiteral("text/plain"),
                      value.toDateTime()
                              .toString(
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
                                      Qt::ISODateWithMs
#else
                                      Qt::ISODate
#endif
                                      )
                              .toUtf8());
        return true;
    case QMetaType::QStringList:
        data->setData(QStringLiteral("text/plain"), value.toStringList().join(QChar(QLatin1Char(','))).toUtf8());
        return true;
    case QMetaType::QUrl:
        data->setData(QStringLiteral("text/plain"), value.value<QUrl>().toEncoded());
        return true;
    case QMetaType::QJsonValue: {
        QJsonObject jObj;
        jObj[QLatin1String("value")] = value.value<QJsonValue>();
        data->setData(QStringLiteral("application/json"), QJsonDocument(jObj).toJson());
        return true;
    }
    case QMetaType::QJsonObject:
        data->setData(QStringLiteral("application/json"), QJsonDocument(value.value<QJsonObject>()).toJson());
        return true;
    case QMetaType::QJsonArray:
        data->setData(QStringLiteral("application/json"), QJsonDocument(value.value<QJsonArray>()).toJson());
        return true;
    case QMetaType::QJsonDocument:
        data->setData(QStringLiteral("application/json"), value.value<QJsonDocument>().toJson());
        return true;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    case QMetaType::QByteArrayList:
        data->setData(QStringLiteral("text/plain"), value.value<QByteArrayList>().join(','));
        return true;
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    case QMetaType::QCborValue:
        data->setData(QStringLiteral("application/cbor"), value.value<QCborValue>().toCbor());
        return true;
    case QMetaType::QCborArray:
        data->setData(QStringLiteral("application/cbor"), QCborValue(value.value<QCborArray>()).toCbor());
        return true;
    case QMetaType::QCborMap:
        data->setData(QStringLiteral("application/cbor"), QCborValue(value.value<QCborMap>()).toCbor());
        return true;
#endif
#ifdef QT_GUI_LIB
    case QMetaType::QImage: {
        QByteArray imageData;
        QBuffer imageDevice(&imageData);
        value.value<QImage>().save(&imageDevice, "PNG");
        data->setData(QStringLiteral("image/png"), imageData);
        return true;
    }
    case QMetaType::QPixmap: {
        QByteArray imageData;
        QBuffer imageDevice(&imageData);
        value.value<QPixmap>().save(&imageDevice, "PNG");
        data->setData(QStringLiteral("image/png"), imageData);
        return true;
    }
    case QMetaType::QBitmap: {
        QByteArray imageData;
        QBuffer imageDevice(&imageData);
        value.value<QBitmap>().save(&imageDevice, "PNG");
        data->setData(QStringLiteral("image/png"), imageData);
        return true;
    }
    case QMetaType::QIcon: {
        const QIcon icon = value.value<QIcon>();
        const auto sizes = icon.availableSizes();
        QSize maxSize =
                sizes.isEmpty() ? QSize(20, 20) : *std::max_element(sizes.constBegin(), sizes.constEnd(), [](const QSize &a, const QSize &b) -> bool {
                    return (a.width() * a.height()) < (b.width() * b.height());
                });
        QByteArray imageData;
        QBuffer imageDevice(&imageData);
        icon.pixmap(maxSize).save(&imageDevice, "PNG");
        data->setData(QStringLiteral("image/png"), imageData);
        return true;
    }
#endif
    default:
        return false;
    }
}

/*!
Destructor
*/
GenericModel::~GenericModel()
{
    delete m_dptr;
}

/*!
\brief Sets the role names
\details This method is a setter for roleNames() allowing you to customise the
role names without subclassing the model.

Pass an empty \a rNames to revert to the default role names
*/
void GenericModel::setRoleNames(const QHash<int, QByteArray> &rNames)
{
    Q_D(GenericModel);
    if (d->m_roleNames != rNames) {
        std::function<void(const QModelIndex &)> sendDataChanged;
        sendDataChanged = [this, &sendDataChanged](const QModelIndex &parent) {
            const int rowC = rowCount(parent);
            const int colC = columnCount(parent);
            Q_ASSERT(rowC > 0 && colC > 0);
            for (int r = 0; r < rowC; ++r) {
                for (int c = 0; c < colC; ++c) {
                    const QModelIndex idx = index(r, c, parent);
                    if (hasChildren(idx))
                        sendDataChanged(idx);
                }
            }
            dataChanged(index(0, 0, parent), index(rowC - 1, colC - 1, parent));
        };
        d->m_roleNames = rNames;
        if (d->root->rowCount() > 0 && d->root->columnCount() > 0)
            sendDataChanged(QModelIndex());
    }
}

/*!
\reimp
*/
QHash<int, QByteArray> GenericModel::roleNames() const
{
    Q_D(const GenericModel);
    if (d->m_roleNames.isEmpty())
        return QAbstractItemModel::roleNames();
    return d->m_roleNames;
}

/*!
\reimp
*/
bool GenericModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (count <= 0 || column < 0 || column > columnCount(parent))
        return false;
    beginInsertColumns(parent, column, column + count - 1);
    Q_D(GenericModel);
    d->insertColumns(column, count, parent);
    endInsertColumns();
    return true;
}

/*!
\reimp
*/
bool GenericModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (count <= 0 || row < 0 || row > rowCount(parent))
        return false;
    beginInsertRows(parent, row, row + count - 1);
    Q_D(GenericModel);
    d->insertRows(row, count, parent);
    endInsertRows();
    return true;
}

/*!
\reimp
*/
bool GenericModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (count <= 0 || column < 0 || column + count - 1 >= columnCount(parent))
        return false;
    beginRemoveColumns(parent, column, column + count - 1);
    Q_D(GenericModel);
    d->removeColumns(column, count, parent);
    endRemoveColumns();
    return true;
}

/*!
\reimp
*/
bool GenericModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (count <= 0 || row < 0 || row + count - 1 >= rowCount(parent))
        return false;
    beginRemoveRows(parent, row, row + count - 1);
    Q_D(GenericModel);
    d->removeRows(row, count, parent);
    endRemoveRows();
    return true;
}

/*!
\reimp
*/
QMap<int, QVariant> GenericModel::itemData(const QModelIndex &index) const
{
    if (!index.isValid())
        return QMap<int, QVariant>();
    Q_ASSERT(index.model() == this);
    Q_D(const GenericModel);
    if (!d->m_mergeDisplayEdit)
        return convertFromContainer<QMap<int, QVariant>>(d->itemForIndex(index)->data);
    RolesContainer rawData = d->itemForIndex(index)->data;
    const auto displayIter = rawData.constFind(Qt::DisplayRole);
    if (displayIter != rawData.constEnd())
        rawData[Qt::EditRole] = displayIter.value();
    return convertFromContainer<QMap<int, QVariant>>(rawData);
}

/*!
\reimp
*/
QModelIndex GenericModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    if (row < 0 || column < 0)
        return QModelIndex();
    Q_D(const GenericModel);
    GenericModelItem *parentItem = d->itemForIndex(parent);
    if (row >= parentItem->rowCount() || column >= parentItem->columnCount())
        return QModelIndex();
    return createIndex(row, column, parentItem->childAt(row, column));
}

/*!
\reimp
*/
int GenericModel::columnCount(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    Q_D(const GenericModel);
    return d->itemForIndex(parent)->columnCount();
}

/*!
\reimp
*/
int GenericModel::rowCount(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    Q_D(const GenericModel);
    return d->itemForIndex(parent)->rowCount();
}

/*!
\reimp
*/
QVariant GenericModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    Q_ASSERT(index.model() == this);
    Q_D(const GenericModel);
    const GenericModelItem *const item = d->itemForIndex(index);
    if (d->m_mergeDisplayEdit && role == Qt::EditRole)
        role = Qt::DisplayRole;
    return item->data.value(role);
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
/*!
\reimp
*/
void GenericModel::multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const
{
    Q_ASSERT(index.isValid() && index.model() == this);
    Q_D(const GenericModel);
    const GenericModelItem *const item = d->itemForIndex(index);
    for (QModelRoleData &roleData : roleDataSpan) {
        int role = roleData.role();
        if (d->m_mergeDisplayEdit && role == Qt::EditRole)
            role = Qt::DisplayRole;
        roleData.setData(item->data.value(role));
    }
}
#endif

/*!
\reimp
*/
bool GenericModel::clearItemData(const QModelIndex &index)
{
    if (!index.isValid())
        return false;
    Q_ASSERT(index.model() == this);
    Q_D(GenericModel);
    GenericModelItem *const item = d->itemForIndex(index);
    if (!item->data.isEmpty()) {
        item->data.clear();
        dataChanged(index, index);
    }
    return true;
}

/*!
\reimp
*/
Qt::ItemFlags GenericModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsDropEnabled;
    Q_ASSERT(index.model() == this);
    Q_D(const GenericModel);
    return d->itemForIndex(index)->flags;
}

/*!
Set the item flags for a specific index
*/
bool GenericModel::setFlags(const QModelIndex &index, Qt::ItemFlags flags)
{
    if (!index.isValid())
        return false;
    Q_ASSERT(index.model() == this);
    Q_D(GenericModel);
    GenericModelItem *const item = d->itemForIndex(index);
    if (item->flags != flags) {
        item->flags = flags;
        dataChanged(index, index);
    }
    return true;
}

/*!
\reimp
*/
bool GenericModel::hasChildren(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid() || parent.model() == this);
    Q_D(const GenericModel);
    GenericModelItem *const item = d->itemForIndex(parent);
    return item->rowCount() > 0 && item->columnCount() > 0;
}

/*!
\reimp
*/
QVariant GenericModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section < 0)
        return QVariant();
    Q_D(const GenericModel);
    if (d->m_mergeDisplayEdit && role == Qt::EditRole)
        role = Qt::DisplayRole;
    if (orientation == Qt::Horizontal) {
        if (section >= columnCount())
            return QVariant();
        return d->hHeaderData.at(section).value(role);
    }
    if (section >= rowCount())
        return QVariant();
    return d->vHeaderData.at(section).value(role);
}

/*!
\reimp
*/
QStringList GenericModel::mimeTypes() const
{
    Q_D(const GenericModel);
    return QAbstractItemModel::mimeTypes() << d->mimeDataName();
}

void GenericModelPrivate::encodeMime(QMimeData *data, const QModelIndexList &indexes) const
{
    Q_ASSERT(data);
    Q_Q(const GenericModel);
    QSet<GenericModelItem *> itemsToSave;
    for (auto &&idx : indexes) {
        if (!idx.isValid())
            continue;
        Q_ASSERT(idx.model() == q);
        itemsToSave << itemForIndex(idx);
    }
    for (auto i = itemsToSave.begin(); i != itemsToSave.end();) {
        bool iToErase = false;
        auto j = i;
        for (++j; j != itemsToSave.end();) {
            if (GenericModelItem::isAnchestor(*i, *j)) {
                j = itemsToSave.erase(j);
                continue;
            }
            if (GenericModelItem::isAnchestor(*j, *i)) {
                iToErase = true;
                break;
            }
            ++j;
        }
        if (iToErase)
            i = itemsToSave.erase(i);
        else
            ++i;
    }
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    stream << qint32(itemsToSave.size());
    for (auto i = itemsToSave.begin(); i != itemsToSave.end(); ++i)
        stream << **i;
    data->setData(mimeDataName(), encoded);
}

bool GenericModelPrivate::decodeMime(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_ASSERT(data);
    if (!data->hasFormat(mimeDataName()))
        return false;
    Q_Q(GenericModel);
    const QByteArray encoded = data->data(mimeDataName());
    QDataStream stream(encoded);
    qint32 itemsCount = 0;
    stream >> itemsCount;
    if (itemsCount == 0)
        return true;
    QMultiMap<int, GenericModelItem *> items;
    for (qint32 i = 0; i < itemsCount; ++i) {
        GenericModelItem *tempItem = new GenericModelItem(q);
        stream >> *tempItem;
        items.insert(tempItem->row(), tempItem);
    }
    int minCol = std::numeric_limits<int>::max();
    int maxCol = 0;
    for (auto i = items.constBegin(), iEnd = items.constEnd(); i != iEnd; ++i) {
        if ((*i)->column() < minCol)
            minCol = (*i)->column();
        if ((*i)->column() > maxCol)
            maxCol = (*i)->column();
    }
    int cCount = q->columnCount(parent);
    int rCount = q->rowCount(parent);
    row = qMin(row, rCount);
    column = qMin(column, cCount);
    if (cCount < maxCol - minCol + 1 + column) {
        Q_ASSUME(q->insertColumns(cCount, maxCol - minCol + 1 + column - cCount, parent));
        cCount = q->columnCount(parent);
    }
    const auto newRowIdxes = items.uniqueKeys();
    rCount = newRowIdxes.size();
    Q_ASSERT(rCount * cCount > 0);
    QVector<GenericModelItem *> rowsToInsert;
    rowsToInsert.reserve(rCount * cCount);
    for (auto i = newRowIdxes.constBegin(), iEnd = newRowIdxes.constEnd(); i != iEnd; ++i) {
        const auto colItems = items.values(*i);
        for (int j = 0; j < cCount; ++j) {
            GenericModelItem *itemToAppend = nullptr;
            if (j < column) {
                itemToAppend = new GenericModelItem(q);
            } else {
                const auto itemIter = std::find_if(colItems.constBegin(), colItems.constEnd(),
                                                   [=](GenericModelItem *item) -> bool { return item->column() == minCol + j - column; });
                if (itemIter == colItems.constEnd())
                    itemToAppend = new GenericModelItem(q);
                else
                    itemToAppend = *itemIter;
            }
            Q_ASSERT(itemToAppend);
            rowsToInsert.append(itemToAppend);
        }
    }
    Q_ASSERT(rowsToInsert.size() == rCount * cCount);
    for (int i = 0; i < rCount * cCount; ++i)
        rowsToInsert[i]->m_column = i % cCount;
    q->beginInsertRows(parent, row, row + rCount - 1);
    itemForIndex(parent)->insertRows(row, rowsToInsert);
    q->endInsertRows();
    return true;
}

/*!
\reimp
*/
QMimeData *GenericModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *data = QAbstractItemModel::mimeData(indexes);
    if (!data)
        return nullptr;
    Q_D(const GenericModel);
    d->encodeMime(data, indexes);
    if (indexes.size() == 1)
        mimeForValue(data, indexes.first().data());
    return data;
}

/*!
\reimp
*/
bool GenericModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (row < 0)
        row = rowCount(parent);
    if (column < 0)
        column = 0;
    if (parent.isValid() && parent.model() != this)
        return false;
    if (!data)
        return false;
    if (!(action & supportedDropActions()))
        return false;
    Q_D(GenericModel);
    if (d->decodeMime(data, action, row, column, parent))
        return true;
    return QAbstractItemModel::dropMimeData(data, action, row, column, parent);
}

bool GenericModelItem::isAnchestor(GenericModelItem *ancestor, GenericModelItem *descendent)
{
    if (!descendent)
        return false;
    if (!ancestor)
        return true;
    if (ancestor == descendent)
        return false;
    for (GenericModelItem *par = descendent->parent; par; par = par->parent) {
        if (par == ancestor)
            return true;
    }
    return false;
}

QDataStream &operator<<(QDataStream &stream, const GenericModelItem &item)
{
    stream << qint32(item.m_colCount) << qint32(item.m_rowCount) << qint32(item.m_row) << qint32(item.m_column) << qint32(item.m_rowSpan)
           << qint32(item.m_colSpan) << item.data << qint32(item.flags) << qint32(item.children.size());
    for (GenericModelItem *child : item.children)
        stream << *child;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, GenericModelItem &item)
{
    qint32 temp;
    stream >> temp;
    item.m_colCount = temp;
    stream >> temp;
    item.m_rowCount = temp;
    stream >> temp;
    item.m_row = temp;
    stream >> temp;
    item.m_column = temp;
    stream >> temp;
    item.m_rowSpan = temp;
    stream >> temp;
    item.m_colSpan = temp;
    stream >> item.data >> temp;
    item.flags = (Qt::ItemFlags)temp;
    stream >> temp;
    if (item.children.size() > temp) {
        qDeleteAll(item.children.begin() + temp, item.children.end());
        item.children.erase(item.children.begin() + temp, item.children.end());
    }
    const int oldChildSize = item.children.size();
    for (int i = 0; i < temp; ++i) {
        if (i < oldChildSize) {
            stream >> *item.children.at(i);
        } else {
            GenericModelItem *newItem = new GenericModelItem(&item);
            stream >> *newItem;
            item.children.append(newItem);
        }
    }
    return stream;
}

/*!
\reimp
*/
bool GenericModel::moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent,
                               int destinationChild)
{
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == this);
    Q_ASSERT(!destinationParent.isValid() || destinationParent.model() == this);
    const int colCnt = columnCount(sourceParent);
    if (destinationChild < 0 || sourceColumn < 0 || sourceColumn + count - 1 >= colCnt)
        return false;
    if (sourceParent != destinationParent) {
        if (destinationChild > columnCount(destinationParent))
            return false;
        const int sourceRowCount = rowCount(sourceParent);
        const int destRowCount = rowCount(destinationParent);
        if (sourceRowCount > destRowCount)
            insertRows(destRowCount, sourceRowCount - destRowCount, destinationParent);
    } else if (destinationChild > colCnt)
        return false;
    if (!beginMoveColumns(sourceParent, sourceColumn, sourceColumn + count - 1, destinationParent, destinationChild))
        return false;
    Q_D(GenericModel);
    if (sourceParent != destinationParent)
        d->moveColumnsDifferentParent(sourceParent, sourceColumn, count, destinationParent, destinationChild);
    else
        d->moveColumnsSameParent(sourceParent, sourceColumn, count, destinationChild);
    endMoveColumns();
    return true;
}

/*!
\reimp
*/
bool GenericModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    Q_ASSERT(!sourceParent.isValid() || sourceParent.model() == this);
    Q_ASSERT(!destinationParent.isValid() || destinationParent.model() == this);
    const int rowCnt = rowCount(sourceParent);
    if (destinationChild < 0 || sourceRow < 0 || sourceRow + count - 1 >= rowCnt)
        return false;
    if (sourceParent != destinationParent) {
        if (destinationChild > rowCount(destinationParent))
            return false;
        const int sourceColCount = columnCount(sourceParent);
        const int destColCount = columnCount(destinationParent);
        if (sourceColCount > destColCount)
            insertColumns(destColCount, sourceColCount - destColCount, destinationParent);
    } else if (destinationChild > rowCnt)
        return false;
    if (!beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild))
        return false;
    Q_D(GenericModel);
    if (sourceParent != destinationParent)
        d->moveRowsDifferentParent(sourceParent, sourceRow, count, destinationParent, destinationChild);
    else
        d->moveRowsSameParent(sourceParent, sourceRow, count, destinationChild);
    endMoveRows();
    return true;
}

/*!
\reimp
*/
bool GenericModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (section < 0)
        return false;
    RolesContainer *sectionContainer = nullptr;
    Q_D(GenericModel);
    if (orientation == Qt::Horizontal) {
        if (section >= columnCount())
            return false;
        sectionContainer = &(d->hHeaderData[section]);

    } else {
        if (section >= rowCount())
            return false;
        sectionContainer = &(d->vHeaderData[section]);
    }
    Q_ASSERT(sectionContainer);
    if (d->m_mergeDisplayEdit && role == Qt::EditRole)
        role = Qt::DisplayRole;
    const auto roleIter = sectionContainer->find(role);
    if (roleIter == sectionContainer->end()) {
        if (value.isValid()) {
            sectionContainer->insert(role, value);
            headerDataChanged(orientation, section, section);
        }
        return true;
    }
    if (!value.isValid()) {
        sectionContainer->erase(roleIter);
        headerDataChanged(orientation, section, section);
        return true;
    }
    if (roleIter.value() != value) {
        roleIter.value() = value;
        headerDataChanged(orientation, section, section);
    }
    return true;
}

/*!
\reimp
*/
QModelIndex GenericModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();
    Q_ASSERT(index.model() == this);
    Q_D(const GenericModel);
    return d->indexForItem(d->itemForIndex(index)->parent);
}

/*!
\reimp
*/
bool GenericModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    Q_ASSERT(index.model() == this);
    Q_D(GenericModel);
    GenericModelItem *const item = d->itemForIndex(index);
    if (d->m_mergeDisplayEdit && role == Qt::EditRole)
        role = Qt::DisplayRole;
    QVector<int> rolesToEmit{role};
    if (d->m_mergeDisplayEdit && role == Qt::DisplayRole)
        rolesToEmit.append(Qt::EditRole);
    const auto roleIter = item->data.find(role);
    if (roleIter == item->data.end()) {
        if (value.isValid()) {
            item->data.insert(role, value);
            dataChanged(index, index, rolesToEmit);
        }
        return true;
    }
    if (!value.isValid()) {
        item->data.erase(roleIter);
        dataChanged(index, index, rolesToEmit);
        return true;
    }
    if (value != roleIter.value()) {
        roleIter.value() = value;
        dataChanged(index, index, rolesToEmit);
    }
    return true;
}

/*!
\reimp
\details If mergeDisplayEdit is true (the default) and roles contains valuer for both Qt::EditRole and Qt::DisplayRole the latter will prevail
*/
bool GenericModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    if (!index.isValid())
        return false;
    Q_ASSERT(index.model() == this);
    Q_D(GenericModel);
    GenericModelItem *const item = d->itemForIndex(index);
    RolesContainer newData = convertToContainer(roles);
    if (d->m_mergeDisplayEdit) {
        const auto editIter = newData.find(Qt::EditRole);
        const auto displayIter = newData.find(Qt::DisplayRole);
        if (displayIter == newData.end()) {
            if (editIter != newData.end()) {
                newData.insert(Qt::DisplayRole, editIter.value());
                newData.erase(editIter);
            }
        } else if (editIter != newData.end()) {
            newData.erase(editIter);
        }
    }
    QVector<int> changedRoles = listToVector(roles.keys());
    if (d->m_mergeDisplayEdit && newData.contains(Qt::DisplayRole))
        changedRoles.append(Qt::EditRole);
    for (auto i = item->data.constBegin(), iEnd = item->data.constEnd(); i != iEnd; ++i) {
        if (newData.contains(i.key()))
            continue;
        Q_ASSERT(!(d->m_mergeDisplayEdit && i.key() == Qt::EditRole));
        newData.insert(i.key(), i.value());
    }
    if (item->data != newData) {
        item->data = std::move(newData);
        dataChanged(index, index, changedRoles);
    }
    return true;
}

/*!
\reimp
*/
void GenericModel::sort(int column, Qt::SortOrder order)
{
    sort(column, QModelIndex(), order);
}

/*!
\brief Sorts all children of \a parent by \a column in the given \a order.
\details The role used for the sorting is determined by sortRole.
If \a recursive is set to true the sorting will propagate down the hierarchy of the model
*/
void GenericModel::sort(int column, const QModelIndex &parent, Qt::SortOrder order, bool recursive)
{
    if (column < 0 || column >= columnCount(parent) || rowCount(parent) == 0)
        return;
    QList<QPersistentModelIndex> parents;
    if (parent.isValid())
        parents.append(parent);
    layoutAboutToBeChanged(parents, QAbstractItemModel::VerticalSortHint);
    Q_D(GenericModel);
    d->itemForIndex(parent)->sortChildren(column, d->sortRole, order, recursive, &(d->vHeaderData));
    layoutChanged(parents, QAbstractItemModel::VerticalSortHint);
}

/*!
\reimp
\details At the moment no view provided by Qt supports this
*/
QSize GenericModel::span(const QModelIndex &index) const
{
    if (!index.isValid())
        return QSize();
    Q_ASSERT(index.model() == this);
    Q_D(const GenericModel);
    return d->itemForIndex(index)->span();
}

/*!
\brief Set the amount of rows and columns an item should occupy.
\details At the moment no view provided by Qt supports this
*/
bool GenericModel::setSpan(const QModelIndex &index, const QSize &size)
{
    if (!index.isValid() || size.isEmpty())
        return false;
    Q_ASSERT(index.model() == this);
    Q_D(GenericModel);
    d->itemForIndex(index)->setSpan(size);
    const QModelIndex parIdx = index.parent();
    const QModelIndex bottomRight = this->index(qMin(index.row() + size.height(), rowCount(parIdx) - 1),
                                                qMin(index.column() + size.width(), columnCount(parIdx) - 1), parIdx);
    dataChanged(index, bottomRight);
    return true;
}

/*!
\reimp
*/
Qt::DropActions GenericModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

/*!
\reimp
*/
Qt::DropActions GenericModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

/*!
\property GenericModel::mergeDisplayEdit
\accessors %mergeDisplayEdit(), setMergeDisplayEdit()
\notifier mergeDisplayEditChanged()
\brief This property determines if the Qt::DisplayRole and Qt::EditRole should be merged in the extra row/column
\details By default the two roles are one and the same you can use this property to separate them.
If there's any data in the role when you set this property to true it will be duplicated for both roles.
If there is data both in Qt::DisplayRole and Qt::EditRole when you set this property to false Qt::DisplayRole will prevail.
*/
bool GenericModel::mergeDisplayEdit() const
{
    Q_D(const GenericModel);
    return d->m_mergeDisplayEdit;
}

void GenericModel::setMergeDisplayEdit(bool val)
{
    Q_D(GenericModel);
    if (d->m_mergeDisplayEdit == val)
        return;
    d->setMergeDisplayEdit(val);
    d->m_mergeDisplayEdit = val;
    mergeDisplayEditChanged(d->m_mergeDisplayEdit);
    d->signalAllChanged({Qt::DisplayRole, Qt::EditRole});
}

/*!
\property GenericModel::sortRole
\accessors %sortRole(), setSortRole()
\notifier sortRoleChanged()
\brief the item role that is used to query the source model's data when sorting items

The default value is Qt::DisplayRole.
*/
int GenericModel::sortRole() const
{
    Q_D(const GenericModel);
    return d->sortRole;
}
void GenericModel::setSortRole(int role)
{
    Q_D(GenericModel);
    if (d->sortRole == role)
        return;
    d->sortRole = role;
    sortRoleChanged(role);
}

void GenericModelPrivate::signalAllChanged(const QVector<int> &roles, const QModelIndex &parent)
{
    Q_Q(GenericModel);
    const int rowCnt = q->rowCount(parent);
    const int colCnt = q->columnCount(parent);
    for (int i = 0; i < rowCnt; ++i) {
        for (int j = 0; j < colCnt; ++j) {
            const QModelIndex currPar = q->index(i, j, parent);
            if (q->hasChildren(currPar))
                signalAllChanged(roles, currPar);
        }
    }
    q->dataChanged(q->index(0, 0, parent), q->index(rowCnt - 1, colCnt - 1, parent), roles);
}

void GenericModelPrivate::setMergeDisplayEdit(bool val)
{
    root->setMergeDisplayEdit(val);
    for (auto i = vHeaderData.begin(), iEnd = vHeaderData.end(); i != iEnd; ++i)
        setMergeDisplayEdit(val, *i);
    for (auto i = hHeaderData.begin(), iEnd = hHeaderData.end(); i != iEnd; ++i)
        setMergeDisplayEdit(val, *i);
}

void GenericModelPrivate::setMergeDisplayEdit(bool val, RolesContainer &container)
{
    const auto displayIter = container.constFind(Qt::DisplayRole);
    if (val) {
        if (displayIter != container.constEnd()) {
            container.remove(Qt::EditRole);
            return;
        }
        const auto editIter = container.find(Qt::EditRole);
        if (editIter != container.constEnd()) {
            container.insert(Qt::DisplayRole, editIter.value());
            container.erase(editIter);
        }
    } else if (displayIter != container.constEnd())
        container.insert(Qt::EditRole, displayIter.value());
}

/*!
\class GenericModel
\brief This is a full implementation for generic use of the `QAbstractItemModel` interface.
\details The model can replace `QStandardItemModel` depending only on Qt Core.
*/
