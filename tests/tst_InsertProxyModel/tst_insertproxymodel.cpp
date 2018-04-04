#include "tst_insertproxymodel.h"
#include <QStringListModel>
#include <insertproxymodel.h>
#ifdef QT_GUI_LIB
#include <QStandardItemModel>
#endif
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

QAbstractItemModel* tst_InsertProxyModel::createListModel() 
{
    return new QStringListModel(QStringList() << QStringLiteral("1") << QStringLiteral("2") << QStringLiteral("3") << QStringLiteral("4") << QStringLiteral("5"), this);
}

QAbstractItemModel* tst_InsertProxyModel::createTableModel()
{
    QAbstractItemModel* result = nullptr;
#ifdef QT_GUI_LIB
    result = new QStandardItemModel(this);
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

QAbstractItemModel* tst_InsertProxyModel::createTreeModel()
{
    QAbstractItemModel* result = nullptr;
#ifdef QT_GUI_LIB
    result = new QStandardItemModel(this);
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

    QTest::newRow("List Row") << createListModel() << InsertProxyModel::InsertDirections(InsertProxyModel::InsertRow) << false;
    QTest::newRow("List Column") << createListModel() << InsertProxyModel::InsertDirections(InsertProxyModel::InsertColumn) << false;
    QTest::newRow("List Row Column") << createListModel() << (InsertProxyModel::InsertRow | InsertProxyModel::InsertColumn) << false;
#ifdef QT_GUI_LIB
    QTest::newRow("Table Row") << createTableModel() << InsertProxyModel::InsertDirections(InsertProxyModel::InsertRow) << true;
    QTest::newRow("Table Column") << createTableModel() << InsertProxyModel::InsertDirections(InsertProxyModel::InsertColumn) << true;
    QTest::newRow("Table Row Column") << createTableModel() << (InsertProxyModel::InsertRow | InsertProxyModel::InsertColumn) << true;
    QTest::newRow("Tree Row") << createTreeModel() << InsertProxyModel::InsertDirections(InsertProxyModel::InsertRow) << true;
    QTest::newRow("Tree Column") << createTreeModel() << InsertProxyModel::InsertDirections(InsertProxyModel::InsertColumn) << true;
    QTest::newRow("Tree Row Column") << createTreeModel() << (InsertProxyModel::InsertRow | InsertProxyModel::InsertColumn) << true;
#endif
}

void tst_InsertProxyModel::testCommitSlot()
{
    QFETCH(QAbstractItemModel*, baseModel);
    QFETCH(InsertProxyModel::InsertDirections, insertDirection);
    QFETCH(bool, canInsertColumns);
    InsertProxyModel proxy;
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
    if (insertDirection & InsertProxyModel::InsertRow) {
        for (int i = 0; i < originalColCount; ++i)
            QCOMPARE(proxy.index(originalRowCount, i).data(), QVariant());
        QVERIFY(proxy.setData(proxy.index(originalRowCount, 0), QStringLiteral("Test")));
        QCOMPARE(proxyDataChangedSpy.count(), 1);
        proxyDataChangedSpy.clear();
        QCOMPARE(proxyExtraDataChangedSpy.count(), 1);
        proxyExtraDataChangedSpy.clear();
        QVERIFY(proxy.commitRow());
        QCOMPARE(baseRowsInsertedSpy.count(), 1);
        baseRowsInsertedSpy.clear();
        QCOMPARE(proxyRowsInsertedSpy.count(), 1);
        proxyRowsInsertedSpy.clear();
        QCOMPARE(baseDataChangedSpy.count(), 1);
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

    QTest::newRow("List Row") << createListModel() << InsertProxyModel::InsertDirections(InsertProxyModel::InsertRow) << false;
    QTest::newRow("List Column") << createListModel() << InsertProxyModel::InsertDirections(InsertProxyModel::InsertColumn) << false;
    QTest::newRow("List Row Column") << createListModel() << (InsertProxyModel::InsertRow | InsertProxyModel::InsertColumn) << false;
#ifdef QT_GUI_LIB
    QTest::newRow("Table Row") << createTableModel() << InsertProxyModel::InsertDirections(InsertProxyModel::InsertRow) << true;
    QTest::newRow("Table Column") << createTableModel() << InsertProxyModel::InsertDirections(InsertProxyModel::InsertColumn) << true;
    QTest::newRow("Table Row Column") << createTableModel() << (InsertProxyModel::InsertRow | InsertProxyModel::InsertColumn) << true;
    QTest::newRow("Tree Row") << createTreeModel() << InsertProxyModel::InsertDirections(InsertProxyModel::InsertRow) << true;
    QTest::newRow("Tree Column") << createTreeModel() << InsertProxyModel::InsertDirections(InsertProxyModel::InsertColumn) << true;
    QTest::newRow("Tree Row Column") << createTreeModel() << (InsertProxyModel::InsertRow | InsertProxyModel::InsertColumn) << true;
#endif
}

void tst_InsertProxyModel::testCommitSubclass()
{
    class InsertProxyModelCommit : public InsertProxyModel{
        Q_DISABLE_COPY(InsertProxyModelCommit)
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
    QFETCH(InsertProxyModel::InsertDirections, insertDirection);
    QFETCH(bool, canInsertColumns);
    InsertProxyModelCommit proxy;
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
            QCOMPARE(proxy.index(i, originalColCount+1).data(), QVariant());
    }
    if (insertDirection & InsertProxyModel::InsertRow) {
        for (int i = 0; i < originalColCount; ++i)
            QCOMPARE(proxy.index(originalRowCount,i).data(), QVariant());
        QVERIFY(proxy.setData(proxy.index(originalRowCount,0), QStringLiteral("Test")));
        QCOMPARE(proxyDataChangedSpy.count(), 1);
        proxyDataChangedSpy.clear();
        QCOMPARE(proxyExtraDataChangedSpy.count(), 1);
        proxyExtraDataChangedSpy.clear();
        QCOMPARE(baseRowsInsertedSpy.count(), 1);
        baseRowsInsertedSpy.clear();
        QCOMPARE(proxyRowsInsertedSpy.count(), 1);
        proxyRowsInsertedSpy.clear();
        QCOMPARE(baseDataChangedSpy.count(), 1);
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
