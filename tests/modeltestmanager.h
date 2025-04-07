#ifndef MODELTESTMANAGER_H
#define MODELTESTMANAGER_H

#include <QStringListModel>
#include <QSize>
template<class T>
class GuardedModel : public T
{
    static_assert(std::is_base_of<QAbstractItemModel, T>::value, "Template argument must be a QAbstractItemModel");
    bool verifyIndex(const QModelIndex &index) const
    {
        if (!index.isValid())
            return true;
        if (index.model() != this)
            return false;
        return true;
    }

public:
    GuardedModel(QObject *parent = nullptr)
        : T(parent)
    { }
    QModelIndex buddy(const QModelIndex &index) const override
    {
        Q_ASSERT(verifyIndex(index));
        return T::buddy(index);
    }
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override
    {
        Q_ASSERT(verifyIndex(parent));
        return T::canDropMimeData(data, action, row, column, parent);
    }
    bool canFetchMore(const QModelIndex &parent) const override
    {
        Q_ASSERT(verifyIndex(parent));
        return T::canFetchMore(parent);
    }
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_ASSERT(verifyIndex(parent));
        return T::columnCount(parent);
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        Q_ASSERT(verifyIndex(index));
        return T::data(index, role);
    }
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override
    {
        Q_ASSERT(verifyIndex(parent));
        return T::dropMimeData(data, action, row, column, parent);
    }
    void fetchMore(const QModelIndex &parent) override
    {
        Q_ASSERT(verifyIndex(parent));
        return T::fetchMore(parent);
    }
    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        Q_ASSERT(verifyIndex(index));
        return T::flags(index);
    }
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_ASSERT(verifyIndex(parent));
        return T::hasChildren(parent);
    }
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
    {
        Q_ASSERT(verifyIndex(parent));
        return T::index(row, column, parent);
    }
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override
    {
        Q_ASSERT(verifyIndex(parent));
        return T::insertColumns(column, count, parent);
    }
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override
    {
        Q_ASSERT(verifyIndex(parent));
        return T::insertRows(row, count, parent);
    }
    QMap<int, QVariant> itemData(const QModelIndex &index) const override
    {
        Q_ASSERT(verifyIndex(index));
        return T::itemData(index);
    }
    QModelIndexList match(const QModelIndex &start, int role, const QVariant &value, int hits = 1,
                          Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith | Qt::MatchWrap)) const override
    {
        Q_ASSERT(verifyIndex(start));
        return T::match(start, role, value, hits, flags);
    }
    QMimeData *mimeData(const QModelIndexList &indexes) const override
    {
        for (const QModelIndex &idx : indexes)
            Q_ASSERT(verifyIndex(idx));
        return T::mimeData(indexes);
    }
    bool moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent,
                     int destinationChild) override
    {
        Q_ASSERT(verifyIndex(sourceParent));
        Q_ASSERT(verifyIndex(destinationParent));
        return T::moveColumns(sourceParent, sourceColumn, count, destinationParent, destinationChild);
    }
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override
    {
        Q_ASSERT(verifyIndex(sourceParent));
        Q_ASSERT(verifyIndex(destinationParent));
        return T::moveRows(sourceParent, sourceRow, count, destinationParent, destinationChild);
    }
    QModelIndex parent(const QModelIndex &index) const override
    {
        Q_ASSERT(verifyIndex(index));
        return T::parent(index);
    }
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override
    {
        Q_ASSERT(verifyIndex(parent));
        return T::removeColumns(column, count, parent);
    }
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override
    {
        Q_ASSERT(verifyIndex(parent));
        return T::removeRows(row, count, parent);
    }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_ASSERT(verifyIndex(parent));
        return T::rowCount(parent);
    }
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override
    {
        Q_ASSERT(verifyIndex(index));
        return T::setData(index, value, role);
    }
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override
    {
        Q_ASSERT(verifyIndex(index));
        return T::setItemData(index, roles);
    }
    QModelIndex sibling(int row, int column, const QModelIndex &index) const override
    {
        Q_ASSERT(verifyIndex(index));
        return T::sibling(row, column, index);
    }
    QSize span(const QModelIndex &index) const override
    {
        Q_ASSERT(verifyIndex(index));
        return T::span(index);
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool clearItemData(const QModelIndex &index) override
    {
        Q_ASSERT(verifyIndex(index));
        return T::clearItemData(index);
    }
    void multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const override
    {
        Q_ASSERT(verifyIndex(index));
        return T::multiData(index, roleDataSpan);
    }
#endif
};

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
class StringListModel : public QStringListModel
{
public:
    using QStringListModel::QStringListModel;
    QModelIndex parent(const QModelIndex &child) const override { return QModelIndex(); }
    int columnCount(const QModelIndex &parent) const override { return parent.isValid() ? 0 : 1; }
    bool hasChildren(const QModelIndex &parent) const override { return parent.isValid() ? false : (rowCount() > 0); }
};
#else
class StringListModel : public QStringListModel
{
public:
    using QStringListModel::QStringListModel;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        const QVariant baseData = QStringListModel::data(index, role);
        if (baseData.toString().isEmpty())
            return QVariant();
        return baseData;
    }
    QModelIndex parent(const QModelIndex &child) const override { return QModelIndex(); }
    int columnCount(const QModelIndex &parent) const override { return parent.isValid() ? 0 : 1; }
    bool hasChildren(const QModelIndex &parent) const override { return parent.isValid() ? false : (rowCount() > 0); }
};
#endif

class SimpleModel : public GuardedModel<StringListModel>
{
public:
    SimpleModel(const QStringList &strList, QObject *parent = nullptr)
        : GuardedModel<StringListModel>(parent)
    {
        setStringList(strList);
    }
    SimpleModel(QObject *parent = nullptr)
        : GuardedModel<StringListModel>(parent)
    { }
};

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
#    ifdef MOC_MODEL_TEST
class ModelTest : public QObject
{
    Q_DISABLE_COPY(ModelTest)
public:
    ModelTest(QAbstractItemModel *model, QObject *parent = Q_NULLPTR)
        : QObject(parent)
    {
        Q_UNUSED(model)
    }
};
#    else
#        include "modeltest.h"
#    endif
#else
#    include <QAbstractItemModelTester>
class ModelTest : public QAbstractItemModelTester
{
    Q_DISABLE_COPY(ModelTest)
public:
    ModelTest(QAbstractItemModel *model, QObject *parent)
        : QAbstractItemModelTester(model, QAbstractItemModelTester::FailureReportingMode::QtTest, parent)
    { }
};
#endif

#ifndef COMPLEX_MODEL_SUPPORT
#    if defined(QT_GUI_LIB)
#        define COMPLEX_MODEL_SUPPORT
#        ifndef SKIP_QTBUG_45114
#            if (QT_VERSION < QT_VERSION_CHECK(5, 11, 0))
#                define SKIP_QTBUG_45114
#            endif
#        endif
#        include <QStandardItemModel>
using ComplexModel = GuardedModel<QStandardItemModel>;
#    elif defined(QTMODELUTILITIES_GENERICMODEL)
#        define COMPLEX_MODEL_SUPPORT
#        include <genericmodel.h>
using ComplexModel = GuardedModel<GenericModel>;
#    endif
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
namespace ModelTestManager {
inline int generateRandomNumber()
{
    return std::rand();
}
}
#else
#    include <QRandomGenerator>
namespace ModelTestManager {
inline int generateRandomNumber()
{
    return QRandomGenerator::global()->generate();
}
}
#endif

#ifndef SKIP_QTBUG_92220
#    if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0) || (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) && QT_VERSION <= QT_VERSION_CHECK(6, 0, 3)))
#        define SKIP_QTBUG_92220
#    endif
#endif

#ifndef SKIP_QTBUG_67511
#    if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
#        define SKIP_QTBUG_67511
#    endif
#endif

#ifndef SKIP_QTBUG_92886
#    if (QT_VERSION < QT_VERSION_CHECK(6, 0, 4) || QT_VERSION == QT_VERSION_CHECK(6, 1, 0))
#        define SKIP_QTBUG_92886
#    endif
#endif

#endif
