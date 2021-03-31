#include "tst_insertproxymodel.h"
#include <QStringListModel>
#include <insertproxymodel.h>
#ifdef QT_GUI_LIB
#include <QStandardItemModel>
#endif
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include "../modeltestmanager.h"
#include <QSortFilterProxyModel>

QAbstractItemModel* createNullModel(QObject* parent){
    Q_UNUSED(parent)
    return nullptr;
}

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    using StringListModel = QStringListModel;
#else
class StringListModel : public QStringListModel{
public:
    using QStringListModel::QStringListModel;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override{
        const QVariant baseData = QStringListModel::data(index, role);
        if(baseData.toString().isEmpty())
            return QVariant();
        return baseData;
    }
};
#endif

QAbstractItemModel* createListModel(QObject* parent) 
{
    return new StringListModel(QStringList() << QStringLiteral("1") << QStringLiteral("2") << QStringLiteral("3") << QStringLiteral("4") << QStringLiteral("5"), parent);
}

QAbstractItemModel* createTableModel(QObject* parent)
{
    QAbstractItemModel* result = nullptr;
#ifdef QT_GUI_LIB
    result = new QStandardItemModel(parent);
    result->insertRows(0, 5);
    result->insertColumns(0, 3);
    for (int i = 0; i < result->rowCount(); ++i) {
        for (int j = 0; j < result->columnCount(); ++j) {
            result->setData(result->index(i, j), QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            result->setData(result->index(i, j), i, Qt::UserRole);
            result->setData(result->index(i, j), j, Qt::UserRole + 1);
        }
    }
#endif
    return result;
}

QAbstractItemModel* createTreeModel(QObject* parent)
{
    QAbstractItemModel* result = nullptr;
#ifdef QT_GUI_LIB
    result = new QStandardItemModel(parent);
    result->insertRows(0, 5);
    result->insertColumns(0, 3);
    for (int i = 0; i < result->rowCount(); ++i) {
        for (int j = 0; j < result->columnCount(); ++j) {
            result->setData(result->index(i, j), QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            result->setData(result->index(i, j), i, Qt::UserRole);
            result->setData(result->index(i, j), j, Qt::UserRole + 1);
        }
        const QModelIndex parIdx = result->index(i, 0);
        result->insertRows(0, 5, parIdx);
        result->insertColumns(0, 3, parIdx);
        for (int k = 0; k < result->rowCount(parIdx); ++k) {
            for (int h = 0; h < result->columnCount(parIdx); ++h) {
                result->setData(result->index(k, h, parIdx), QStringLiteral("%1,%2,%3").arg(i).arg(k).arg(h), Qt::EditRole);
                result->setData(result->index(k, h, parIdx), h, Qt::UserRole);
                result->setData(result->index(k, h, parIdx), k, Qt::UserRole + 1);
            }
        }
    }
#endif
    return result;
}
void tst_InsertProxyModel::testCommitSubclass_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<InsertProxyModel::InsertDirections>("insertDirection");
    QTest::addColumn<bool>("canInsertColumns");

    QTest::newRow("List Row") << createListModel(this) << InsertProxyModel::InsertDirections(InsertProxyModel::InsertRow) << false;
    QTest::newRow("List Column") << createListModel(this) << InsertProxyModel::InsertDirections(InsertProxyModel::InsertColumn) << false;
    QTest::newRow("List Row Column") << createListModel(this) << (InsertProxyModel::InsertRow | InsertProxyModel::InsertColumn) << false;
    QTest::newRow("Table Row") << createTableModel(this) << InsertProxyModel::InsertDirections(InsertProxyModel::InsertRow) << true;
    QTest::newRow("Table Column") << createTableModel(this) << InsertProxyModel::InsertDirections(InsertProxyModel::InsertColumn) << true;
    QTest::newRow("Table Row Column") << createTableModel(this) << (InsertProxyModel::InsertRow | InsertProxyModel::InsertColumn) << true;
    QTest::newRow("Tree Row") << createTreeModel(this) << InsertProxyModel::InsertDirections(InsertProxyModel::InsertRow) << true;
    QTest::newRow("Tree Column") << createTreeModel(this) << InsertProxyModel::InsertDirections(InsertProxyModel::InsertColumn) << true;
    QTest::newRow("Tree Row Column") << createTreeModel(this) << (InsertProxyModel::InsertRow | InsertProxyModel::InsertColumn) << true;
}

void tst_InsertProxyModel::testCommitSlot()
{
    QFETCH(QAbstractItemModel*, baseModel);
    if (!baseModel)
        return;
    QFETCH(InsertProxyModel::InsertDirections, insertDirection);
    QFETCH(bool, canInsertColumns);
    InsertProxyModel proxy;
    new ModelTest(&proxy, baseModel);
    proxy.setSourceModel(baseModel);
    proxy.setInsertDirection(insertDirection);
    const int originalColCount = baseModel->columnCount();
    const int originalRowCount = baseModel->rowCount();
    QCOMPARE(proxy.rowCount(), originalRowCount + static_cast<bool>(insertDirection & InsertProxyModel::InsertRow));
    QCOMPARE(proxy.columnCount(), originalColCount + static_cast<bool>(insertDirection & InsertProxyModel::InsertColumn));
    for (int i = 0; i < originalRowCount; ++i) {
        for (int j = 0; j < originalColCount; ++j) {
            QCOMPARE(proxy.index(i, j).data(), baseModel->index(i, j).data());
        }
    }
    QSignalSpy baseDataChangedSpy(baseModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(baseDataChangedSpy.isValid());
    QSignalSpy proxyDataChangedSpy(&proxy, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(proxyDataChangedSpy.isValid());
    QSignalSpy proxyExtraDataChangedSpy(&proxy, SIGNAL(extraDataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(proxyExtraDataChangedSpy.isValid());
    QSignalSpy baseRowsInsertedSpy(baseModel, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(baseRowsInsertedSpy.isValid());
    QSignalSpy proxyRowsInsertedSpy(&proxy, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(proxyRowsInsertedSpy.isValid());
    QSignalSpy baseColsInsertedSpy(baseModel, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(baseColsInsertedSpy.isValid());
    QSignalSpy proxyColsInsertedSpy(&proxy, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(proxyColsInsertedSpy.isValid());
    if (insertDirection & InsertProxyModel::InsertColumn) {
        for (int i = 0; i < originalRowCount; ++i)
            QCOMPARE(proxy.index(i, originalColCount).data(), QVariant());
        QVERIFY(proxy.setData(proxy.index(0, originalColCount), QStringLiteral("Test")));
        QCOMPARE(proxyDataChangedSpy.count(), 1);
        proxyDataChangedSpy.clear();
        QCOMPARE(proxyExtraDataChangedSpy.count(), 1);
        proxyExtraDataChangedSpy.clear();
        QCOMPARE(proxy.commitColumn(), canInsertColumns);
        QCOMPARE(proxy.columnCount(), baseModel->columnCount() + 1);
        QCOMPARE(baseModel->columnCount(), originalColCount + canInsertColumns);
        if (canInsertColumns) {
            QCOMPARE(baseColsInsertedSpy.count(), 1);
            baseColsInsertedSpy.clear();
            QCOMPARE(proxyColsInsertedSpy.count(), 1);
            proxyColsInsertedSpy.clear();
            QCOMPARE(baseDataChangedSpy.count(), 1);
            baseDataChangedSpy.clear();
            QCOMPARE(baseModel->index(0, originalColCount).data().toString(), QStringLiteral("Test"));
            for (int i = 1; i < originalRowCount; ++i)
                QCOMPARE(baseModel->index(i, originalColCount).data(), QVariant());
        }
        for (int i = 0; i < originalRowCount; ++i)
            QCOMPARE(proxy.index(i, originalColCount + 1).data(), QVariant());
    }
    proxyDataChangedSpy.clear();
    proxyExtraDataChangedSpy.clear();
    baseDataChangedSpy.clear();
    if (insertDirection & InsertProxyModel::InsertRow) {
        for (int i = 0; i < originalColCount; ++i)
            QCOMPARE(proxy.index(originalRowCount, i).data(), QVariant());
        QVERIFY(proxy.setData(proxy.index(originalRowCount, 0), QStringLiteral("Test")));
        QCOMPARE(proxyDataChangedSpy.count(), 1);
        proxyDataChangedSpy.clear();
        QCOMPARE(proxyExtraDataChangedSpy.count(), 1);
        proxyExtraDataChangedSpy.clear();
        QVERIFY(proxy.commitRow());
        QVERIFY(proxyDataChangedSpy.count() >= 2); // change to QCOMPARE(proxyDataChangedSpy.count(),2) once QTBUG-67511 is fixed
        proxyDataChangedSpy.clear();
        QCOMPARE(proxyExtraDataChangedSpy.count(), 1);
        proxyExtraDataChangedSpy.clear();
        QCOMPARE(baseRowsInsertedSpy.count(), 1);
        baseRowsInsertedSpy.clear();
        QCOMPARE(proxyRowsInsertedSpy.count(), 1);
        proxyRowsInsertedSpy.clear();
        QVERIFY(baseDataChangedSpy.count()>0); // change to QCOMPARE(baseDataChangedSpy.count(),1) once QTBUG-67511 is fixed
        baseDataChangedSpy.clear();
        QCOMPARE(proxy.rowCount(), baseModel->rowCount() + 1);
        QCOMPARE(baseModel->rowCount(), originalRowCount + 1);
        QCOMPARE(baseModel->index(originalRowCount, 0).data().toString(), QStringLiteral("Test"));
        for (int i = 1; i < originalColCount; ++i)
            QCOMPARE(baseModel->index(originalRowCount, i).data(), QVariant());
        for (int i = 0; i < originalColCount; ++i)
            QCOMPARE(proxy.index(originalRowCount + 1, i).data(), QVariant());
    }
    baseModel->deleteLater();
}

void tst_InsertProxyModel::testCommitSlot_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<InsertProxyModel::InsertDirections>("insertDirection");
    QTest::addColumn<bool>("canInsertColumns");

    QTest::newRow("List Row") << createListModel(this) << InsertProxyModel::InsertDirections(InsertProxyModel::InsertRow) << false;
    QTest::newRow("List Column") << createListModel(this) << InsertProxyModel::InsertDirections(InsertProxyModel::InsertColumn) << false;
    QTest::newRow("List Row Column") << createListModel(this) << (InsertProxyModel::InsertRow | InsertProxyModel::InsertColumn) << false;
    QTest::newRow("Table Row") << createTableModel(this) << InsertProxyModel::InsertDirections(InsertProxyModel::InsertRow) << true;
    QTest::newRow("Table Column") << createTableModel(this) << InsertProxyModel::InsertDirections(InsertProxyModel::InsertColumn) << true;
    QTest::newRow("Table Row Column") << createTableModel(this) << (InsertProxyModel::InsertRow | InsertProxyModel::InsertColumn) << true;
    QTest::newRow("Tree Row") << createTreeModel(this) << InsertProxyModel::InsertDirections(InsertProxyModel::InsertRow) << true;
    QTest::newRow("Tree Column") << createTreeModel(this) << InsertProxyModel::InsertDirections(InsertProxyModel::InsertColumn) << true;
    QTest::newRow("Tree Row Column") << createTreeModel(this) << (InsertProxyModel::InsertRow | InsertProxyModel::InsertColumn) << true;
}
void tst_InsertProxyModel::testSourceInsertCol()
{
#if defined(QT_GUI_LIB)
    QFETCH(QAbstractItemModel*, baseModel);
    QFETCH(InsertProxyModel::InsertDirections, insertDirection);
    QFETCH(int, indexToInsert);
    QFETCH(bool, addViaProxy);
    InsertProxyModel proxy;
    new ModelTest(&proxy, baseModel);
    proxy.setSourceModel(baseModel);
    proxy.setInsertDirection(insertDirection);
    QSignalSpy proxyColsInsertedSpy(&proxy, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(proxyColsInsertedSpy.isValid());
    QSignalSpy baseColsInsertedSpy(baseModel, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(baseColsInsertedSpy.isValid());
    const int originalColCount = baseModel->columnCount();
    const int originalRowCount = baseModel->rowCount();
    QCOMPARE(proxy.rowCount(), originalRowCount + bool(insertDirection & InsertProxyModel::InsertRow));
    QCOMPARE(proxy.columnCount(), originalColCount + bool(insertDirection & InsertProxyModel::InsertColumn));
    if (insertDirection & InsertProxyModel::InsertRow) {
        if (indexToInsert > 0)
            QVERIFY(proxy.setData(proxy.index(originalRowCount, indexToInsert - 1), QStringLiteral("TestRowBefore")));
        if (indexToInsert<originalColCount)
            QVERIFY(proxy.setData(proxy.index(originalRowCount, indexToInsert), QStringLiteral("TestRowAfter")));
    }
    if (insertDirection & InsertProxyModel::InsertColumn) {
        QVERIFY(proxy.setData(proxy.index(0, originalColCount), QStringLiteral("TestColumn")));
        QVERIFY(!proxy.insertColumn(proxy.columnCount()));
    }
    for (int i = 0; i < originalRowCount; ++i) {
        for (int j = 0; j < originalColCount; ++j) {
            QCOMPARE(proxy.index(i, j).data(), baseModel->index(i, j).data());
        }
    }
    if (addViaProxy)
        QVERIFY(proxy.insertColumn(indexToInsert));
    else
        QVERIFY(baseModel->insertColumn(indexToInsert));
    QCOMPARE(baseColsInsertedSpy.count(), 1);
    baseColsInsertedSpy.clear();
    QCOMPARE(proxyColsInsertedSpy.count(), 1);
    const auto args = proxyColsInsertedSpy.value(0);
    proxyColsInsertedSpy.clear();
    QVERIFY(!args.value(0).value<QModelIndex>().isValid());
    QCOMPARE(args.value(1).toInt(), indexToInsert);
    QCOMPARE(args.value(2).toInt(), indexToInsert);
    QCOMPARE(baseModel->columnCount(), originalColCount + 1);
    QCOMPARE(baseModel->rowCount(), originalRowCount);
    QCOMPARE(proxy.rowCount(), originalRowCount + bool(insertDirection & InsertProxyModel::InsertRow));
    QCOMPARE(proxy.columnCount(), originalColCount +1  + bool(insertDirection & InsertProxyModel::InsertColumn));
    for (int i = 0; i < originalRowCount; ++i) {
        for (int j = 0; j < originalColCount+1; ++j) {
            QCOMPARE(proxy.index(i, j).data(), baseModel->index(i, j).data());
        }
    }
    for (int j = 0; j < originalRowCount; ++j) {
        QVERIFY(!proxy.index(j,indexToInsert).data().isValid());
        QVERIFY(!baseModel->index(j,indexToInsert).data().isValid());
    }
    if (insertDirection & InsertProxyModel::InsertRow) {
        if (indexToInsert<originalColCount)
            QCOMPARE(proxy.index(originalRowCount,indexToInsert + 1).data().toString(), QStringLiteral("TestRowAfter"));
        if (indexToInsert > 0)
            QCOMPARE(proxy.index(originalRowCount, indexToInsert - 1).data().toString(), QStringLiteral("TestRowBefore"));
    }
    QCOMPARE(proxy.commitRow(), bool(insertDirection & InsertProxyModel::InsertRow));
    QCOMPARE(proxy.commitColumn(), bool(insertDirection & InsertProxyModel::InsertColumn));
    for (int i = 0; i < originalRowCount  + bool(insertDirection & InsertProxyModel::InsertRow); ++i) {
        for (int j = 0; j < originalColCount+1 + bool(insertDirection & InsertProxyModel::InsertColumn); ++j) {
            QCOMPARE(proxy.index(i, j).data(), baseModel->index(i, j).data());
        }
    }
    if (insertDirection & InsertProxyModel::InsertColumn) {
        QCOMPARE(baseModel->index(0, originalColCount + 1).data().toString(), QStringLiteral("TestColumn"));
        for (int i = 0; i < proxy.rowCount(); ++i)
            QCOMPARE(proxy.index(i, baseModel->columnCount()).data(), QVariant());
    }
    if (insertDirection & InsertProxyModel::InsertRow) {
        if (indexToInsert<originalColCount)
            QCOMPARE(baseModel->index(originalRowCount, indexToInsert + 1).data().toString(), QStringLiteral("TestRowAfter"));
        if (indexToInsert > 0)
            QCOMPARE(baseModel->index(originalRowCount, indexToInsert - 1).data().toString(), QStringLiteral("TestRowBefore"));
        for (int i = 0; i < proxy.columnCount(); ++i)
            QCOMPARE(proxy.index(i, baseModel->columnCount()).data(), QVariant());
    }
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI module");
#endif
}
void tst_InsertProxyModel::testSourceInsertRow()
{
    QFETCH(QAbstractItemModel*, baseModel);
    QFETCH(InsertProxyModel::InsertDirections, insertDirection);
    QFETCH(int, indexToInsert);
    QFETCH(bool, addViaProxy);
    InsertProxyModel proxy;
    new ModelTest(&proxy, baseModel);
    proxy.setSourceModel(baseModel);
    proxy.setInsertDirection(insertDirection);
    QSignalSpy proxyRowsInsertedSpy(&proxy, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(proxyRowsInsertedSpy.isValid());
    QSignalSpy baseRowsInsertedSpy(baseModel, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(baseRowsInsertedSpy.isValid());
    const int originalColCount = baseModel->columnCount();
    const int originalRowCount = baseModel->rowCount();
    QCOMPARE(proxy.rowCount(), originalRowCount + bool(insertDirection & InsertProxyModel::InsertRow));
    QCOMPARE(proxy.columnCount(), originalColCount + bool(insertDirection & InsertProxyModel::InsertColumn));
    if (insertDirection & InsertProxyModel::InsertRow){
        QVERIFY(proxy.setData(proxy.index(originalRowCount, 0), QStringLiteral("TestRow")));
        QVERIFY(!proxy.insertRow(proxy.rowCount()));
    }
    if (insertDirection & InsertProxyModel::InsertColumn) {
        if (indexToInsert>0)
            QVERIFY(proxy.setData(proxy.index(indexToInsert-1, originalColCount), QStringLiteral("TestColumnBefore")));
        if (indexToInsert < originalRowCount)
            QVERIFY(proxy.setData(proxy.index(indexToInsert, originalColCount), QStringLiteral("TestColumnAfter")));
    }
    for (int i = 0; i < originalRowCount;++i){
        for (int j = 0; j < originalColCount; ++j) {
            QCOMPARE(proxy.index(i, j).data(), baseModel->index(i, j).data());
        }
    }
    if (addViaProxy)
        QVERIFY(proxy.insertRow(indexToInsert));
    else
        QVERIFY(baseModel->insertRow(indexToInsert));
    QCOMPARE(baseRowsInsertedSpy.count(), 1);
    baseRowsInsertedSpy.clear();
    QCOMPARE(proxyRowsInsertedSpy.count(), 1);
    const auto args = proxyRowsInsertedSpy.value(0);
    proxyRowsInsertedSpy.clear();
    QVERIFY(!args.value(0).value<QModelIndex>().isValid());
    QCOMPARE(args.value(1).toInt(), indexToInsert);
    QCOMPARE(args.value(2).toInt(), indexToInsert);
    QCOMPARE(baseModel->rowCount(), originalRowCount + 1);
    QCOMPARE(baseModel->columnCount(), originalColCount);
    QCOMPARE(proxy.rowCount(), originalRowCount + 1 + bool(insertDirection & InsertProxyModel::InsertRow));
    QCOMPARE(proxy.columnCount(), originalColCount + bool(insertDirection & InsertProxyModel::InsertColumn));
    for (int i = 0; i < originalRowCount+1; ++i) {
        for (int j = 0; j < originalColCount; ++j) {
            QCOMPARE(proxy.index(i, j).data(), baseModel->index(i, j).data());
        }
    }
    for (int j = 0; j < originalColCount; ++j) {
        QVERIFY(proxy.index(indexToInsert, j).data().isNull());
        QVERIFY(baseModel->index(indexToInsert, j).data().isNull());
    }
    if (insertDirection & InsertProxyModel::InsertColumn) {
        if (indexToInsert < originalRowCount)
            QCOMPARE(proxy.index(indexToInsert + 1, originalColCount).data().toString(), QStringLiteral("TestColumnAfter"));
        if (indexToInsert > 0)
            QCOMPARE(proxy.index(indexToInsert - 1, originalColCount).data().toString(), QStringLiteral("TestColumnBefore"));
    }
    QCOMPARE(proxy.commitRow(), bool(insertDirection & InsertProxyModel::InsertRow));
    QCOMPARE(proxy.commitColumn(), bool(insertDirection & InsertProxyModel::InsertColumn));
    for (int i = 0; i < originalRowCount +1+ bool(insertDirection & InsertProxyModel::InsertRow); ++i) {
        for (int j = 0; j < originalColCount + bool(insertDirection & InsertProxyModel::InsertColumn); ++j) {
            QCOMPARE(proxy.index(i, j).data(), baseModel->index(i, j).data());
        }
    }
    if (insertDirection & InsertProxyModel::InsertRow) {
        QCOMPARE(baseModel->index(originalRowCount+1, 0).data().toString(), QStringLiteral("TestRow"));
        for (int i = 0; i < proxy.columnCount();++i)
            QCOMPARE(proxy.index(baseModel->rowCount(), i).data(), QVariant());
    }
    if (insertDirection & InsertProxyModel::InsertColumn) {
        if (indexToInsert < originalRowCount)
            QCOMPARE(baseModel->index(indexToInsert + 1, originalColCount).data().toString(), QStringLiteral("TestColumnAfter"));
        if (indexToInsert > 0)
            QCOMPARE(baseModel->index(indexToInsert - 1, originalColCount).data().toString(), QStringLiteral("TestColumnBefore"));
        for (int i = 0; i < proxy.rowCount(); ++i)
            QCOMPARE(proxy.index(i, baseModel->columnCount()).data(), QVariant());
    }
    baseModel->deleteLater();
}

void tst_InsertProxyModel::testSourceInsertRow_data()
{
    
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<InsertProxyModel::InsertDirections>("insertDirection");
    QTest::addColumn<int>("indexToInsert");
    QTest::addColumn<bool>("addViaProxy");

    for (auto&& insertDirection : {
        std::make_pair<InsertProxyModel::InsertDirections, QByteArray>(InsertProxyModel::InsertRow, QByteArrayLiteral("Extra Row"))
        , std::make_pair<InsertProxyModel::InsertDirections, QByteArray>(InsertProxyModel::InsertColumn, QByteArrayLiteral("Extra Column"))
        , std::make_pair<InsertProxyModel::InsertDirections, QByteArray>(InsertProxyModel::InsertColumn | InsertProxyModel::InsertRow, QByteArrayLiteral("Extra Row and Column"))
    }) {
        for (auto&& baseModel : {
            std::make_pair(insertDirection.first & InsertProxyModel::InsertColumn ? &createNullModel : &createListModel, QByteArrayLiteral("List"))
            , std::make_pair(&createTableModel, QByteArrayLiteral("Table"))
            , std::make_pair(&createTreeModel, QByteArrayLiteral("Tree"))
        }) {
            QAbstractItemModel* const tempModel = baseModel.first(this);
            if (tempModel) {
                for (auto&& addViaProxy : {
                    std::make_pair(true, QByteArrayLiteral("via Proxy"))
                    , std::make_pair(false, QByteArrayLiteral("via Base"))
                }) {

                    for (auto&& indexToInsert : {
                        std::make_pair(0, QByteArrayLiteral("Begin"))
                        , std::make_pair(tempModel->rowCount(), QByteArrayLiteral("End"))
                        , std::make_pair(tempModel->rowCount() / 2, QByteArrayLiteral("Middle"))
                    }) {
                        QTest::newRow((baseModel.second + ' ' + insertDirection.second + ' ' + addViaProxy.second + ' ' + indexToInsert.second).constData())
                            << baseModel.first(this)
                            << insertDirection.first
                            << indexToInsert.first
                            << addViaProxy.first
                            ;
                    }
                }
                tempModel->deleteLater();
            }
        }
    }
}

void tst_InsertProxyModel::testSourceInsertCol_data()
{

    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<InsertProxyModel::InsertDirections>("insertDirection");
    QTest::addColumn<int>("indexToInsert");
    QTest::addColumn<bool>("addViaProxy");

    for (auto&& insertDirection : {
        std::make_pair<InsertProxyModel::InsertDirections, QByteArray>(InsertProxyModel::InsertRow, QByteArrayLiteral("Extra Row"))
        , std::make_pair<InsertProxyModel::InsertDirections, QByteArray>(InsertProxyModel::InsertColumn, QByteArrayLiteral("Extra Column"))
        , std::make_pair<InsertProxyModel::InsertDirections, QByteArray>(InsertProxyModel::InsertColumn | InsertProxyModel::InsertRow, QByteArrayLiteral("Extra Row and Column"))
    }) {

        for (auto&& baseModel : {
            std::make_pair(&createTableModel, QByteArrayLiteral("Table"))
            , std::make_pair(&createTreeModel, QByteArrayLiteral("Tree"))
        }) {
            QAbstractItemModel* const tempModel = baseModel.first(this);
            if (tempModel) {
                for (auto&& indexToInsert : {
                    std::make_pair(0, QByteArrayLiteral("Begin"))
                    , std::make_pair(tempModel->columnCount(), QByteArrayLiteral("End"))
                    , std::make_pair(tempModel->columnCount() / 2, QByteArrayLiteral("Middle"))
                }) {
                    for (auto&& addViaProxy : {
                        std::make_pair(true, QByteArrayLiteral("via Proxy"))
                        , std::make_pair(false, QByteArrayLiteral("via Base"))
                    }) {
                        QTest::newRow((baseModel.second + ' ' + insertDirection.second + ' ' + addViaProxy.second + ' ' + indexToInsert.second).constData())
                            << baseModel.first(this)
                            << insertDirection.first
                            << indexToInsert.first
                            << addViaProxy.first
                            ;
                    }
                }
                tempModel->deleteLater();
            }
        }
    }
}

void tst_InsertProxyModel::testNullModel()
{
    InsertProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    proxyModel.setInsertDirection(InsertProxyModel::InsertRow);
    proxyModel.setSourceModel(nullptr);
    QVERIFY(!proxyModel.setData(proxyModel.index(0, 0), QStringLiteral("1")));
    QVERIFY(!proxyModel.index(0, 0).data().isValid());
    QVERIFY(!proxyModel.commitRow());
    QStringListModel validModel(QStringList() << QStringLiteral("1"));
    proxyModel.setSourceModel(&validModel);
    QVERIFY(proxyModel.setData(proxyModel.index(1, 0), QStringLiteral("2")));
    QCOMPARE(proxyModel.index(1, 0).data().toString(), QStringLiteral("2"));
    QVERIFY(proxyModel.commitRow());
    QCOMPARE(validModel.index(1, 0).data().toString(), QStringLiteral("2"));
    proxyModel.setSourceModel(nullptr);
    QVERIFY(!proxyModel.index(0, 0).data().isValid());
    QVERIFY(!proxyModel.setData(proxyModel.index(0, 0), QStringLiteral("3")));
    QVERIFY(!proxyModel.index(0, 0).data().isValid());
    QVERIFY(!proxyModel.commitRow());
}

void tst_InsertProxyModel::testProperties()
{
    InsertProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    QVERIFY(proxyModel.setProperty("insertDirection", QVariant::fromValue(InsertProxyModel::InsertColumn | InsertProxyModel::InsertRow)));
    QCOMPARE(proxyModel.property("insertDirection").value<InsertProxyModel::InsertDirections>(), InsertProxyModel::InsertColumn | InsertProxyModel::InsertRow);
    QVERIFY(proxyModel.setProperty("mergeDisplayEdit", false));
    QCOMPARE(proxyModel.property("mergeDisplayEdit").toBool(), false);
}

void tst_InsertProxyModel::testDataForCorner()
{
    QAbstractItemModel* const baseModel = createListModel(this);
    InsertProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    QSignalSpy cornerChangeSpy(&proxyModel, SIGNAL(dataForCornerChanged(QVector<int>)));
    QSignalSpy cornerDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(cornerChangeSpy.isValid());
    QVERIFY(cornerDataChangeSpy.isValid());
    proxyModel.setInsertDirection(InsertProxyModel::InsertColumn | InsertProxyModel::InsertRow);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setDataForCorner(5689);
    QCOMPARE(proxyModel.index(baseModel->rowCount(), baseModel->columnCount()).data().toInt(), 5689);
    QCOMPARE(proxyModel.dataForCorner().toInt(), 5689);
    QCOMPARE(cornerChangeSpy.count(), 1);
    QCOMPARE(cornerDataChangeSpy.count(), 1);
    auto roles = cornerChangeSpy.value(0).value(0).value<QVector<int> >();
    cornerChangeSpy.clear();
    cornerDataChangeSpy.clear();
    QCOMPARE(roles.size(), 2);
    QVERIFY(roles.contains(Qt::EditRole));
    QVERIFY(roles.contains(Qt::DisplayRole));
    proxyModel.setMergeDisplayEdit(false);
    proxyModel.setDataForCorner(8741);
    QCOMPARE(proxyModel.index(baseModel->rowCount(), baseModel->columnCount()).data().toInt(), 5689);
    QCOMPARE(proxyModel.dataForCorner().toInt(), 5689);
    QCOMPARE(proxyModel.index(baseModel->rowCount(), baseModel->columnCount()).data(Qt::EditRole).toInt(), 8741);
    QCOMPARE(proxyModel.dataForCorner(Qt::EditRole).toInt(), 8741);
    QCOMPARE(cornerChangeSpy.count(), 1);
    QCOMPARE(cornerDataChangeSpy.count(), 1);
    roles = cornerChangeSpy.value(0).value(0).value<QVector<int> >();
    cornerChangeSpy.clear();
    cornerDataChangeSpy.clear();
    QCOMPARE(roles.size(), 1);
    QVERIFY(roles.contains(Qt::EditRole));
    baseModel->deleteLater();
}

void tst_InsertProxyModel::testSort()
{
    QFETCH(bool, sortProxy);
    QStringListModel baseModel(QStringList({ QStringLiteral("b"), QStringLiteral("a"), QStringLiteral("d"), QStringLiteral("c") }));
    InsertProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    proxyModel.setSourceModel(&baseModel);
    proxyModel.setInsertDirection(InsertProxyModel::InsertColumn);
    QVERIFY(proxyModel.setData(proxyModel.index(0, 1), 1));
    QVERIFY(proxyModel.setData(proxyModel.index(1, 1), 0));
    QVERIFY(proxyModel.setData(proxyModel.index(2, 1), 3));
    QVERIFY(proxyModel.setData(proxyModel.index(3, 1), 2));
    QPersistentModelIndex persistBase = baseModel.index(0, 0);
    std::pair<int, int> persistCode = std::make_pair(persistBase.row(), persistBase.column());
    QPersistentModelIndex persistInside = proxyModel.index(0, 0);
    persistCode = std::make_pair(persistInside.row(), persistInside.column());
    QPersistentModelIndex persistExtra = proxyModel.index(0, 1);
    persistCode = std::make_pair(persistExtra.row(), persistExtra.column());
    if (sortProxy)
        proxyModel.sort(0, Qt::AscendingOrder);
    else
        baseModel.sort(0, Qt::AscendingOrder);
    persistCode = std::make_pair(persistBase.row(), persistBase.column());
    persistCode = std::make_pair(persistInside.row(), persistInside.column());
    persistCode = std::make_pair(persistExtra.row(), persistExtra.column());
    QCOMPARE(persistExtra.data().toInt(), 1);
    QCOMPARE(persistInside.data().toString(), persistBase.data().toString());
    for (int i = 0; i < baseModel.rowCount(); ++i)
        QCOMPARE(proxyModel.index(i, 1).data().toInt(), i);

}

void tst_InsertProxyModel::testSort_data()
{
    QTest::addColumn<bool>("sortProxy");
    QTest::newRow("Sort via Proxy") << true;
    QTest::newRow("Sort via Base") << false;
}

void tst_InsertProxyModel::testInsertOnEmptyModel()
{
    QStringListModel baseModel1;
    InsertProxyModel proxyModel1;
    new ModelTest(&proxyModel1, this);
    proxyModel1.setSourceModel(&baseModel1);
    proxyModel1.setInsertDirection(InsertProxyModel::InsertRow);
    proxyModel1.setData(proxyModel1.index(0,0),QStringLiteral("London"));
    QCOMPARE(proxyModel1.rowCount(), 1);
    QCOMPARE(proxyModel1.index(0,0).data().toString(), QStringLiteral("London"));
    proxyModel1.commitRow();
    QCOMPARE(baseModel1.rowCount(), 1);
    QCOMPARE(baseModel1.index(0,0).data().toString(), QStringLiteral("London"));
#if defined(QT_GUI_LIB)
    QStandardItemModel baseModel2;
    baseModel2.insertColumns(0,2);
    InsertProxyModel proxyModel2;
    new ModelTest(&proxyModel2, this);
    proxyModel2.setSourceModel(&baseModel2);
    proxyModel2.setInsertDirection(InsertProxyModel::InsertRow);
    proxyModel2.setData(proxyModel2.index(0,1),QStringLiteral("London"));
    QCOMPARE(proxyModel2.rowCount(), 1);
    QCOMPARE(proxyModel2.index(0,1).data().toString(), QStringLiteral("London"));
    QVERIFY(!proxyModel2.index(0,0).data().isValid());
    proxyModel2.commitRow();
    QCOMPARE(baseModel2.rowCount(), 1);
    QCOMPARE(baseModel2.index(0,1).data().toString(), QStringLiteral("London"));
    QVERIFY(!baseModel2.index(0,0).data().isValid());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0) || QT_VERSION > QT_VERSION_CHECK(6, 0, 2))) //QTBUG-92220
    QStandardItemModel baseModel3;
    InsertProxyModel proxyModel3;
    new ModelTest(&proxyModel3, this);
    proxyModel3.setSourceModel(&baseModel3);
    proxyModel3.setInsertDirection(InsertProxyModel::InsertRow);
    baseModel3.insertColumns(0,2);
    proxyModel3.setData(proxyModel3.index(0,1),QStringLiteral("London"));
    QCOMPARE(proxyModel3.rowCount(), 1);
    QCOMPARE(proxyModel3.index(0,1).data().toString(), QStringLiteral("London"));
    QVERIFY(!proxyModel3.index(0,0).data().isValid());
    proxyModel3.commitRow();
    QCOMPARE(baseModel3.rowCount(), 1);
    QCOMPARE(baseModel3.index(0,1).data().toString(), QStringLiteral("London"));
    QVERIFY(!baseModel3.index(0,0).data().isValid());
#endif
#endif
}

void tst_InsertProxyModel::testResetModel()
{
    QStringListModel baseModel1(QStringList({ QStringLiteral("London"), QStringLiteral("Berlin"), QStringLiteral("Paris") }));
    QStringListModel baseModel2(QStringList({ QStringLiteral("Rome"), QStringLiteral("Madrid") }));
    QSortFilterProxyModel middleProxy;
    middleProxy.setSourceModel(&baseModel1);
    InsertProxyModel insertProxy;
    new ModelTest(&insertProxy, this);
    insertProxy.setSourceModel(&middleProxy);
    insertProxy.setInsertDirection(InsertProxyModel::InsertRow);
    insertProxy.setData(insertProxy.index(3,0),QStringLiteral("Tokyo"));
    QSignalSpy insertProxyAboutToResetSpy(&insertProxy, SIGNAL(modelAboutToBeReset()));
    QVERIFY(insertProxyAboutToResetSpy.isValid());
    QSignalSpy insertProxyResetSpy(&insertProxy, SIGNAL(modelReset()));
    QVERIFY(insertProxyResetSpy.isValid());
    middleProxy.setSourceModel(&baseModel2);
    QCOMPARE(insertProxy.rowCount(),3);
    QCOMPARE(insertProxy.index(0,0).data(0).toString(),QStringLiteral("Rome"));
    QCOMPARE(insertProxy.index(1,0).data(0).toString(),QStringLiteral("Madrid"));
    QVERIFY(!insertProxy.index(2,0).data(0).isValid());
    QCOMPARE(insertProxyAboutToResetSpy.count(),1);
    QCOMPARE(insertProxyResetSpy.count(),1);
}

void tst_InsertProxyModel::testDisconnectedModel()
{
    QStringListModel baseModel1(QStringList({ QStringLiteral("London"), QStringLiteral("Berlin"), QStringLiteral("Paris") }));
    QStringListModel baseModel2(QStringList({ QStringLiteral("Rome"), QStringLiteral("Madrid"), QStringLiteral("Prague") }));
    InsertProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    proxyModel.setSourceModel(&baseModel1);
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    baseModel1.setData(baseModel1.index(0, 0), QStringLiteral("New York"));
    QCOMPARE(proxyDataChangeSpy.count(), 1);
    proxyDataChangeSpy.clear();
    proxyModel.setSourceModel(&baseModel2);
    baseModel1.setData(baseModel1.index(1, 0), QStringLiteral("Tokyo"));
    QCOMPARE(proxyDataChangeSpy.count(), 0);
    proxyDataChangeSpy.clear();
    baseModel2.setData(baseModel2.index(0, 0), QStringLiteral("Lima"));
    QCOMPARE(proxyDataChangeSpy.count(), 1);
}

void tst_InsertProxyModel::initTestCase()
{
    qRegisterMetaType<QVector<int> >();
}

void tst_InsertProxyModel::testData()
{
    QFETCH(QAbstractItemModel*, baseModel);
    if (!baseModel)
        return;
    InsertProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setInsertDirection(InsertProxyModel::InsertColumn | InsertProxyModel::InsertRow);
    proxyModel.setSourceModel(baseModel);
    const int sourceRows = baseModel->rowCount();
    const int sourceCols = baseModel->columnCount();
    for (int i = 0; i < sourceRows;++i){
        for (int j = 0; j < sourceCols; ++j) {
            QCOMPARE(proxyModel.index(i, j).data(), baseModel->index(i, j).data());
        }
    }
    for (int i = 0; i < sourceRows; ++i) {
        QVERIFY(!proxyModel.index(i, sourceCols).data().isValid());
    }
    for (int i = 0; i < sourceCols; ++i) {
        QVERIFY(!proxyModel.index(sourceRows,i).data().isValid());
    }
    baseModel->deleteLater();
}

void tst_InsertProxyModel::testData_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Table") << createTableModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_InsertProxyModel::testSetData()
{
    QFETCH(QAbstractItemModel*, baseModel);
    if (!baseModel)
        return;
    QFETCH(int, idxRow);
    QFETCH(int, idxCol);
    InsertProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.setInsertDirection(InsertProxyModel::InsertColumn | InsertProxyModel::InsertRow);
    proxyModel.setSourceModel(baseModel);
    const QModelIndex proxyIdX = proxyModel.index(idxRow, idxCol);
    const QString idxData = QStringLiteral("Test");
    QVERIFY(proxyModel.setData(proxyIdX, idxData,Qt::DisplayRole));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::DisplayRole).toString(), idxData);
    QCOMPARE(proxyModel.data(proxyIdX, Qt::EditRole).toString(), idxData);
    baseModel->deleteLater();
}

void tst_InsertProxyModel::testSetData_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<int>("idxRow");
    QTest::addColumn<int>("idxCol");
    QTest::newRow("List Inside Base Model") << createListModel(this) << 0 << 0;
    QAbstractItemModel* baseModel = createListModel(this);
    QTest::newRow("List Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createListModel(this);
    QTest::newRow("List Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createListModel(this);
    QTest::newRow("List Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
    QTest::newRow("Table Inside Base Model") << createTableModel(this) << 0 << 0;
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
    QTest::newRow("Tree Inside Base Model") << createTreeModel(this) << 0 << 0;
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
}

void tst_InsertProxyModel::testSetItemData_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<int>("idxRow");
    QTest::addColumn<int>("idxCol");
    QAbstractItemModel* baseModel = createListModel(this);
    QTest::newRow("List Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createListModel(this);
    QTest::newRow("List Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createListModel(this);
    QTest::newRow("List Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
    QTest::newRow("Table Inside Base Model") << createTableModel(this) << 0 << 0;
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
    QTest::newRow("Tree Inside Base Model") << createTreeModel(this) << 0 << 0;
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
}

void tst_InsertProxyModel::testSetItemDataDataChanged_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<int>("idxRow");
    QTest::addColumn<int>("idxCol");
    QAbstractItemModel* baseModel = createListModel(this);
    QTest::newRow("List Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createListModel(this);
    QTest::newRow("List Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createListModel(this);
    QTest::newRow("List Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
}

void tst_InsertProxyModel::testSetItemDataDataChanged()
{
    QFETCH(QAbstractItemModel*, baseModel);
    if (!baseModel)
        return;
    QFETCH(int, idxRow);
    QFETCH(int, idxCol);
    QMap<int, QVariant> itemDataSet{ { //TextAlignmentRole
            std::make_pair<int, QVariant>(Qt::UserRole, 5)
            , std::make_pair<int, QVariant>(Qt::DisplayRole, QStringLiteral("Test"))
            , std::make_pair<int, QVariant>(Qt::ToolTipRole, QStringLiteral("ToolTip"))
        } };
    InsertProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.setInsertDirection(InsertProxyModel::InsertColumn | InsertProxyModel::InsertRow);
    proxyModel.setSourceModel(baseModel);
    const QModelIndex proxyIdX = proxyModel.index(idxRow, idxCol);
    QVERIFY(proxyModel.setData(proxyIdX, Qt::AlignRight, Qt::TextAlignmentRole));
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QCOMPARE(proxyDataChangeSpy.size(), 1);
    auto argList = proxyDataChangeSpy.takeFirst();
    QCOMPARE(argList.at(0).value<QModelIndex>(), proxyIdX);
    QCOMPARE(argList.at(1).value<QModelIndex>(), proxyIdX);
    auto rolesVector = argList.at(2).value<QVector<int> >();
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    // bug fixed by Qt commit 1382374deaa4a854aeb542e6c8f7e1841f2abb10
    QCOMPARE(rolesVector.size(), 4);
    QVERIFY(!rolesVector.contains(Qt::TextAlignmentRole));
#endif
    QVERIFY(rolesVector.contains(Qt::ToolTipRole));
    QVERIFY(rolesVector.contains(Qt::EditRole));
    QVERIFY(rolesVector.contains(Qt::DisplayRole));
    QVERIFY(rolesVector.contains(Qt::UserRole));
    itemDataSet[Qt::UserRole] = 6;
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QCOMPARE(proxyDataChangeSpy.size(), 1);
    argList = proxyDataChangeSpy.takeFirst();
    QCOMPARE(argList.at(0).value<QModelIndex>(), proxyIdX);
    QCOMPARE(argList.at(1).value<QModelIndex>(), proxyIdX);
    rolesVector = argList.at(2).value<QVector<int> >();
    QCOMPARE(rolesVector.size(), 1);
    QVERIFY(rolesVector.contains(Qt::UserRole));
    itemDataSet.clear();
    itemDataSet[Qt::UserRole] = 6;
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    // bug fixed by Qt commit 1382374deaa4a854aeb542e6c8f7e1841f2abb10
    QCOMPARE(proxyDataChangeSpy.size(), 0);
#endif
    baseModel->deleteLater();
}

void tst_InsertProxyModel::testSetItemData()
{
    QFETCH(QAbstractItemModel*, baseModel);
    if (!baseModel)
        return;
    QFETCH(int, idxRow);
    QFETCH(int, idxCol);
    const QMap<int, QVariant> itemDataSet{{
            std::make_pair<int, QVariant>(Qt::UserRole, 5)
            ,std::make_pair<int, QVariant>(Qt::DisplayRole, QStringLiteral("Test"))
            , std::make_pair<int, QVariant>(Qt::ToolTipRole, QStringLiteral("ToolTip"))
        }};
    InsertProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.setInsertDirection(InsertProxyModel::InsertColumn | InsertProxyModel::InsertRow);
    proxyModel.setSourceModel(baseModel);
    const QModelIndex proxyIdX = proxyModel.index(idxRow, idxCol);
    QVERIFY(proxyModel.setData(proxyIdX, Qt::AlignRight, Qt::TextAlignmentRole));
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::DisplayRole).toString(), QStringLiteral("Test"));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::EditRole).toString(), QStringLiteral("Test"));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::UserRole).toInt(), 5);
    QCOMPARE(proxyModel.data(proxyIdX, Qt::ToolTipRole).toString(), QStringLiteral("ToolTip"));
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    // bug fixed by Qt commit 1382374deaa4a854aeb542e6c8f7e1841f2abb10
    QCOMPARE(proxyModel.data(proxyIdX, Qt::TextAlignmentRole).toInt(), Qt::AlignRight);
#endif
    baseModel->deleteLater();
}

void tst_InsertProxyModel::testCommitSubclass()
{
    class InsertProxyModelCommit : public InsertProxyModel{
    public:
        explicit InsertProxyModelCommit(QObject* parent = Q_NULLPTR) : InsertProxyModel(parent) {}
    protected:
        bool validRow() const Q_DECL_OVERRIDE
        {
            if (!sourceModel())
                return false;
            const int sourceCols = sourceModel()->columnCount();
            const int sourceRows = sourceModel()->rowCount();
            for (int i = 0; i < sourceCols; ++i) {
                if (index(sourceRows,i).data().isValid())
                    return true;
            }
            return false;
        }
        bool validColumn() const  Q_DECL_OVERRIDE
        {
            if (!sourceModel())
                return false;
            const int sourceCols = sourceModel()->columnCount();
            const int sourceRows = sourceModel()->rowCount();
            for (int i = 0; i < sourceRows; ++i) {
                if (index(i, sourceCols).data().isValid())
                    return true;
            }
            return false;
        }
    };
    QFETCH(QAbstractItemModel*, baseModel);
    if (!baseModel)
        return;
    QFETCH(InsertProxyModel::InsertDirections, insertDirection);
    QFETCH(bool, canInsertColumns);
    InsertProxyModelCommit proxy;
    new ModelTest(&proxy, baseModel);
    proxy.setSourceModel(baseModel);
    proxy.setInsertDirection(insertDirection);
    const int originalColCount = baseModel->columnCount();
    const int originalRowCount = baseModel->rowCount();
    QCOMPARE(proxy.rowCount(), originalRowCount + static_cast<bool>(insertDirection & InsertProxyModel::InsertRow));
    QCOMPARE(proxy.columnCount(), originalColCount + static_cast<bool>(insertDirection & InsertProxyModel::InsertColumn));
    for (int i = 0; i < originalRowCount; ++i) {
        for (int j = 0; j < originalColCount; ++j) {
            QCOMPARE(proxy.index(i, j).data(), baseModel->index(i, j).data());
        }
    }
    QSignalSpy baseDataChangedSpy(baseModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(baseDataChangedSpy.isValid());
    QSignalSpy proxyDataChangedSpy(&proxy, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(proxyDataChangedSpy.isValid());
    QSignalSpy proxyExtraDataChangedSpy(&proxy, SIGNAL(extraDataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(proxyExtraDataChangedSpy.isValid());
    QSignalSpy baseRowsInsertedSpy(baseModel, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(baseRowsInsertedSpy.isValid());
    QSignalSpy proxyRowsInsertedSpy(&proxy, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(proxyRowsInsertedSpy.isValid());
    QSignalSpy baseColsInsertedSpy(baseModel, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(baseColsInsertedSpy.isValid());
    QSignalSpy proxyColsInsertedSpy(&proxy, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(proxyColsInsertedSpy.isValid());
    if (insertDirection & InsertProxyModel::InsertColumn && canInsertColumns) {
        for (int i = 0; i < originalRowCount; ++i)
            QCOMPARE(proxy.index(i, originalColCount).data(), QVariant());
        QVERIFY(proxy.setData(proxy.index(0, originalColCount), QStringLiteral("Test")));
        QCOMPARE(proxyDataChangedSpy.count(), 3);
        proxyDataChangedSpy.clear();
        QCOMPARE(proxyExtraDataChangedSpy.count(), 2);
        proxyExtraDataChangedSpy.clear();
        QCOMPARE(proxy.columnCount(), baseModel->columnCount() + 1);
        QCOMPARE(baseModel->columnCount(), originalColCount + canInsertColumns);
        QCOMPARE(baseColsInsertedSpy.count(), 1);
        baseColsInsertedSpy.clear();
        QCOMPARE(proxyColsInsertedSpy.count(), 1);
        proxyColsInsertedSpy.clear();
        QCOMPARE(baseDataChangedSpy.count(), 1);
        baseDataChangedSpy.clear();
        QCOMPARE(baseModel->index(0, originalColCount).data().toString(), QStringLiteral("Test"));
        for (int i = 1; i < originalRowCount; ++i)
            QCOMPARE(baseModel->index(i, originalColCount).data(), QVariant());
        
        for (int i = 0; i < originalRowCount; ++i)
            QCOMPARE(proxy.index(i, originalColCount+1).data(), QVariant());
    }
    proxyDataChangedSpy.clear();
    proxyExtraDataChangedSpy.clear();
    baseDataChangedSpy.clear();
    if (insertDirection & InsertProxyModel::InsertRow) {
        for (int i = 0; i < originalColCount; ++i)
            QCOMPARE(proxy.index(originalRowCount,i).data(), QVariant());
        QVERIFY(proxy.setData(proxy.index(originalRowCount,0), QStringLiteral("Test")));
        QVERIFY(proxyDataChangedSpy.count() >=3);// change to QCOMPARE(proxyDataChangedSpy.count(),3) once QTBUG-67511 is fixed
        proxyDataChangedSpy.clear();
        QCOMPARE(proxyExtraDataChangedSpy.count(), 2);
        proxyExtraDataChangedSpy.clear();
        QCOMPARE(baseRowsInsertedSpy.count(), 1);
        baseRowsInsertedSpy.clear();
        QCOMPARE(proxyRowsInsertedSpy.count(), 1);
        proxyRowsInsertedSpy.clear();
        QVERIFY(baseDataChangedSpy.count() >= 1);// change to QCOMPARE(baseDataChangedSpy.count(),1) once QTBUG-67511 is fixed
        baseDataChangedSpy.clear();
        QCOMPARE(proxy.rowCount(), baseModel->rowCount() + 1);
        QCOMPARE(baseModel->rowCount(), originalRowCount + 1);
        QCOMPARE(baseModel->index(originalRowCount,0).data().toString(), QStringLiteral("Test"));
        for (int i = 1; i < originalColCount; ++i)
            QCOMPARE(baseModel->index(originalRowCount,i).data(), QVariant());
        for (int i = 0; i < originalColCount; ++i)
            QCOMPARE(proxy.index(originalRowCount+1,i).data(), QVariant());
    }
    baseModel->deleteLater();
}
