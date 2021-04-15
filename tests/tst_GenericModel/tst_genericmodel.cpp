#include "tst_genericmodel.h"
#include <genericmodel.h>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include "../modeltestmanager.h"

void tst_GenericModel::autoParent()
{
    QObject *parentObj = new QObject;
    auto testItem = new GenericModel(parentObj);
    QSignalSpy testItemDestroyedSpy(testItem, SIGNAL(destroyed(QObject *)));
    QVERIFY(testItemDestroyedSpy.isValid());
    delete parentObj;
    QCOMPARE(testItemDestroyedSpy.count(), 1);
}

void tst_GenericModel::insertRow_data()
{
    QTest::addColumn<int>("insertCol");
    QTest::newRow("No Columns") << 0;
    QTest::newRow("One Column") << 1;
    QTest::newRow("Multiple Columns") << 3;
}

void tst_GenericModel::insertRow()
{
    QFETCH(int, insertCol);
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    if (insertCol > 0)
        testModel.insertColumns(0, insertCol);
    QSignalSpy rowsAboutToBeInsertedSpy(&testModel, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(rowsAboutToBeInsertedSpy.isValid());
    QSignalSpy rowsInsertedSpy(&testModel, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(rowsInsertedSpy.isValid());

    QCOMPARE(testModel.rowCount(), 0);
    QVERIFY(!testModel.insertRow(-1));
    QCOMPARE(testModel.rowCount(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QVERIFY(!testModel.insertRow(1));
    QCOMPARE(testModel.rowCount(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QVERIFY(testModel.insertRow(0));
    QCOMPARE(testModel.rowCount(), 1);
    for (QSignalSpy *spy : {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QVERIFY(testModel.insertRow(0));
    QCOMPARE(testModel.rowCount(), 2);
    for (QSignalSpy *spy : {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QVERIFY(testModel.insertRow(1));
    QCOMPARE(testModel.rowCount(), 3);
    for (QSignalSpy *spy : {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 1);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QVERIFY(testModel.insertRow(3));
    QCOMPARE(testModel.rowCount(), 4);
    for (QSignalSpy *spy : {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(1).toInt(), 3);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
}

void tst_GenericModel::insertRows_data()
{
    QTest::addColumn<int>("insertCol");
    QTest::newRow("No Columns") << 0;
    QTest::newRow("One Column") << 1;
    QTest::newRow("Multiple Columns") << 3;
}

void tst_GenericModel::insertRows()
{
    QFETCH(int, insertCol);
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    if (insertCol > 0)
        testModel.insertColumns(0, insertCol);
    QVERIFY(testModel.insertRow(0));
    testModel.setHeaderData(0,Qt::Vertical,1);

    QSignalSpy rowsAboutToBeInsertedSpy(&testModel, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(rowsAboutToBeInsertedSpy.isValid());
    QSignalSpy rowsInsertedSpy(&testModel, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(rowsInsertedSpy.isValid());

    QVERIFY(testModel.insertRows(0, 1));
    QCOMPARE(testModel.rowCount(), 2);
    for (QSignalSpy *spy : {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(1,Qt::Vertical).toInt(),1);

    QVERIFY(testModel.insertRows(0, 2));
    QCOMPARE(testModel.rowCount(), 4);
    for (QSignalSpy *spy : {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 1);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(3,Qt::Vertical).toInt(),1);

    QVERIFY(testModel.insertRows(2, 2));
    QCOMPARE(testModel.rowCount(), 6);
    for (QSignalSpy *spy : {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(5,Qt::Vertical).toInt(),1);

    QVERIFY(!testModel.insertRows(0, 0));
    QCOMPARE(testModel.rowCount(), 6);
    for (QSignalSpy *spy : {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy})
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertRows(2, 0));
    QCOMPARE(testModel.rowCount(), 6);
    for (QSignalSpy *spy : {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy})
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertRows(-1, 2));
    QCOMPARE(testModel.rowCount(), 6);
    for (QSignalSpy *spy : {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy})
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertRows(7, 2));
    QCOMPARE(testModel.rowCount(), 6);
    for (QSignalSpy *spy : {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy})
        QCOMPARE(spy->count(), 0);
}

void tst_GenericModel::insertColumn_data()
{
    QTest::addColumn<int>("insertRow");
    QTest::newRow("No Rows") << 0;
    QTest::newRow("One Row") << 1;
    QTest::newRow("Multiple Rows") << 3;
}

void tst_GenericModel::insertColumn()
{
    QFETCH(int, insertRow);
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    if (insertRow > 0)
        testModel.insertRows(0, insertRow);
    QSignalSpy columnsAboutToBeInsertedSpy(&testModel, SIGNAL(columnsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(columnsAboutToBeInsertedSpy.isValid());
    QSignalSpy columnsInsertedSpy(&testModel, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(columnsInsertedSpy.isValid());

    QCOMPARE(testModel.columnCount(), 0);
    testModel.insertColumn(-1);
    QCOMPARE(testModel.columnCount(), 0);
    QCOMPARE(columnsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(columnsInsertedSpy.count(), 0);
    testModel.insertColumn(1);
    QCOMPARE(testModel.columnCount(), 0);
    QCOMPARE(columnsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(columnsInsertedSpy.count(), 0);
    testModel.insertColumn(0);
    QCOMPARE(testModel.columnCount(), 1);
    QCOMPARE(columnsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(columnsInsertedSpy.count(), 1);
    for (QSignalSpy *spy : {&columnsInsertedSpy, &columnsAboutToBeInsertedSpy}) {
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    testModel.insertColumn(0);
    QCOMPARE(testModel.columnCount(), 2);
    for (QSignalSpy *spy : {&columnsInsertedSpy, &columnsAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    testModel.insertColumn(1);
    QCOMPARE(testModel.columnCount(), 3);
    for (QSignalSpy *spy : {&columnsInsertedSpy, &columnsAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 1);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    testModel.insertColumn(3);
    QCOMPARE(testModel.columnCount(), 4);
    for (QSignalSpy *spy : {&columnsInsertedSpy, &columnsAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(1).toInt(), 3);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
}

void tst_GenericModel::insertColumns_data()
{
    QTest::addColumn<int>("insertRow");
    QTest::newRow("No Rows") << 0;
    QTest::newRow("One Row") << 1;
    QTest::newRow("Multiple Rows") << 3;
}

void tst_GenericModel::insertColumns()
{
    QFETCH(int, insertRow);
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    if (insertRow > 0)
        testModel.insertRows(0, insertRow);
    QVERIFY(testModel.insertColumn(0));
    testModel.setHeaderData(0,Qt::Horizontal,1);

    QSignalSpy columnsAboutToBeInsertedSpy(&testModel, SIGNAL(columnsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(columnsAboutToBeInsertedSpy.isValid());
    QSignalSpy columnsInsertedSpy(&testModel, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(columnsInsertedSpy.isValid());
    QSignalSpy* const spyArr[] = {&columnsInsertedSpy, &columnsAboutToBeInsertedSpy};

    QVERIFY(testModel.insertColumns(0, 1));
    QCOMPARE(testModel.columnCount(), 2);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(1,Qt::Horizontal).toInt(),1);

    QVERIFY(testModel.insertColumns(0, 2));
    QCOMPARE(testModel.columnCount(), 4);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 1);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(3,Qt::Horizontal).toInt(),1);

    QVERIFY(testModel.insertColumns(2, 2));
    QCOMPARE(testModel.columnCount(), 6);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(5,Qt::Horizontal).toInt(),1);

    QVERIFY(!testModel.insertColumns(0, 0));
    QCOMPARE(testModel.columnCount(), 6);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(2, 0));
    QCOMPARE(testModel.columnCount(), 6);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(-1, 2));
    QCOMPARE(testModel.columnCount(), 6);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(7, 2));
    QCOMPARE(testModel.columnCount(), 6);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
}

void tst_GenericModel::insertChildren()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    testModel.insertColumns(0, 3);
    testModel.insertRows(0, 5);

    QSignalSpy rowsAboutToBeInsertedSpy(&testModel, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(rowsAboutToBeInsertedSpy.isValid());
    QSignalSpy rowsInsertedSpy(&testModel, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(rowsInsertedSpy.isValid());
    QSignalSpy columnsAboutToBeInsertedSpy(&testModel, SIGNAL(columnsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(columnsAboutToBeInsertedSpy.isValid());
    QSignalSpy columnsInsertedSpy(&testModel, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(columnsInsertedSpy.isValid());
    QSignalSpy* const colSpyArr[] = {&columnsInsertedSpy, &columnsAboutToBeInsertedSpy};
    QSignalSpy* const rowSpyArr[] = {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy};
    QSignalSpy* const spyArr[] = {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy,&columnsInsertedSpy, &columnsAboutToBeInsertedSpy};

    QModelIndex parentIdx = testModel.index(0,0);
    QVERIFY(!testModel.insertRows(-1,1,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertRows(1,1,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertRows(0,0,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(-1,1,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(1,1,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(0,0,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(testModel.insertRows(0,1,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),1);
    QCOMPARE(testModel.rowCount(),5);
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : colSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : rowSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel.insertColumns(0,1,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),1);
    QCOMPARE(testModel.columnCount(),3);
    QCOMPARE(testModel.rowCount(parentIdx),1);
    for (QSignalSpy *spy : rowSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : colSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel.insertRows(1,2,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),3);
    QCOMPARE(testModel.rowCount(),5);
    QCOMPARE(testModel.columnCount(parentIdx),1);
    for (QSignalSpy *spy : colSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : rowSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel.insertColumns(1,3,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),4);
    QCOMPARE(testModel.columnCount(),3);
    QCOMPARE(testModel.rowCount(parentIdx),3);
    for (QSignalSpy *spy : rowSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : colSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }

    parentIdx = testModel.index(2,1);
    QVERIFY(!testModel.insertRows(-1,1,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertRows(1,1,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertRows(0,0,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(-1,1,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(1,1,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(0,0,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);


    QVERIFY(testModel.insertRows(0,1,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),1);
    QCOMPARE(testModel.rowCount(),5);
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : colSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : rowSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel.insertColumns(0,1,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),1);
    QCOMPARE(testModel.columnCount(),3);
    QCOMPARE(testModel.rowCount(parentIdx),1);
    for (QSignalSpy *spy : rowSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : colSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }

    QVERIFY(testModel.insertRows(1,2,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),3);
    QCOMPARE(testModel.rowCount(),5);
    QCOMPARE(testModel.columnCount(parentIdx),1);
    for (QSignalSpy *spy : colSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : rowSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel.insertColumns(1,3,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),4);
    QCOMPARE(testModel.columnCount(),3);
    QCOMPARE(testModel.rowCount(parentIdx),3);
    for (QSignalSpy *spy : rowSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : colSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }

    parentIdx = testModel.index(0,0,testModel.index(0,0));
    QVERIFY(!testModel.insertRows(-1,1,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertRows(1,1,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertRows(0,0,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(-1,1,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(1,1,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(0,0,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);


    QVERIFY(testModel.insertColumns(0,1,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),1);
    QCOMPARE(testModel.columnCount(),3);
    QCOMPARE(testModel.rowCount(parentIdx),0);
    for (QSignalSpy *spy : rowSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : colSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel.insertRows(0,1,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),1);
    QCOMPARE(testModel.rowCount(),5);
    QCOMPARE(testModel.columnCount(parentIdx),1);
    for (QSignalSpy *spy : colSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : rowSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }


    QVERIFY(testModel.insertRows(1,2,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),3);
    QCOMPARE(testModel.rowCount(),5);
    QCOMPARE(testModel.columnCount(parentIdx),1);
    for (QSignalSpy *spy : colSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : rowSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel.insertColumns(1,3,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),4);
    QCOMPARE(testModel.columnCount(),3);
    QCOMPARE(testModel.rowCount(parentIdx),3);
    for (QSignalSpy *spy : rowSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : colSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }

    parentIdx = testModel.index(2,1,testModel.index(0,0));
    QVERIFY(!testModel.insertRows(-1,1,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertRows(1,1,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertRows(0,0,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(-1,1,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(1,1,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.insertColumns(0,0,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);


    QVERIFY(testModel.insertRows(0,1,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),1);
    QCOMPARE(testModel.rowCount(),5);
    QCOMPARE(testModel.columnCount(parentIdx),0);
    for (QSignalSpy *spy : colSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : rowSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel.insertColumns(0,1,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),1);
    QCOMPARE(testModel.columnCount(),3);
    QCOMPARE(testModel.rowCount(parentIdx),1);
    for (QSignalSpy *spy : rowSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : colSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }

    QVERIFY(testModel.insertRows(1,2,parentIdx));
    QCOMPARE(testModel.rowCount(parentIdx),3);
    QCOMPARE(testModel.rowCount(),5);
    QCOMPARE(testModel.columnCount(parentIdx),1);
    for (QSignalSpy *spy : colSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : rowSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel.insertColumns(1,3,parentIdx));
    QCOMPARE(testModel.columnCount(parentIdx),4);
    QCOMPARE(testModel.columnCount(),3);
    QCOMPARE(testModel.rowCount(parentIdx),3);
    for (QSignalSpy *spy : rowSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : colSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
}

void tst_GenericModel::data()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    testModel.insertColumns(0, 3);
    testModel.insertRows(0, 5);
    QSignalSpy dataChangedSpy(&testModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(dataChangedSpy.isValid());

    testModel.setData(QModelIndex(), 13);
    QCOMPARE(dataChangedSpy.count(), 0);
    testModel.setData(testModel.index(0, 0), QVariant());
    QCOMPARE(dataChangedSpy.count(), 0);

    for (int r = 0; r < testModel.rowCount(); ++r) {
        for (int c = 0; c < testModel.columnCount(); ++c) {
            const QModelIndex idx = testModel.index(r, c);
            const QString displayString = QStringLiteral("%1,%2").arg(r).arg(c);
            testModel.setData(idx, displayString);
            QCOMPARE(testModel.data(idx).toString(), displayString);
            QCOMPARE(dataChangedSpy.count(), 1);
            auto args = dataChangedSpy.takeFirst();
            QCOMPARE(args.at(0).value<QModelIndex>(), idx);
            QCOMPARE(args.at(1).value<QModelIndex>(), idx);
            QVector<int> rolesVector = args.at(2).value<QVector<int>>();
            QVERIFY(rolesVector.contains(Qt::EditRole));
            QVERIFY(rolesVector.contains(Qt::DisplayRole));
            const QString tooltipString = QStringLiteral("Tooltip %1,%2").arg(r).arg(c);
            testModel.setData(idx, tooltipString, Qt::ToolTipRole);
            QCOMPARE(testModel.data(idx, Qt::ToolTipRole).toString(), tooltipString);
            QCOMPARE(dataChangedSpy.count(), 1);
            args = dataChangedSpy.takeFirst();
            QCOMPARE(args.at(0).value<QModelIndex>(), idx);
            QCOMPARE(args.at(1).value<QModelIndex>(), idx);
            QCOMPARE(args.at(2).value<QVector<int>>(), QVector<int>{Qt::ToolTipRole});
            testModel.setData(idx, r + c, Qt::UserRole + 5);
            QCOMPARE(idx.data(Qt::UserRole + 5).toInt(), r + c);
            QCOMPARE(dataChangedSpy.count(), 1);
            args = dataChangedSpy.takeFirst();
            QCOMPARE(args.at(0).value<QModelIndex>(), idx);
            QCOMPARE(args.at(1).value<QModelIndex>(), idx);
            QCOMPARE(args.at(2).value<QVector<int>>(), QVector<int>{Qt::UserRole + 5});

            QMap<int, QVariant> itemData = testModel.itemData(idx);
            QCOMPARE(itemData.size(), 4);
            QVERIFY(itemData.contains(Qt::EditRole));
            QVERIFY(itemData.contains(Qt::DisplayRole));
            QVERIFY(itemData.contains(Qt::ToolTipRole));
            QVERIFY(itemData.contains(Qt::UserRole + 5));
            QCOMPARE(itemData.value(Qt::EditRole).toString(), displayString);
            QCOMPARE(itemData.value(Qt::DisplayRole).toString(), displayString);
            QCOMPARE(itemData.value(Qt::ToolTipRole).toString(), tooltipString);
            QCOMPARE(itemData.value(Qt::UserRole + 5).toInt(), r + c);

            testModel.insertColumn(0, idx);
            testModel.insertRow(0, idx);
            const QModelIndex childIdx = testModel.index(0, 0, idx);
            testModel.setData(childIdx, displayString);
            QCOMPARE(testModel.data(childIdx).toString(), displayString);
            QCOMPARE(dataChangedSpy.count(), 1);
            args = dataChangedSpy.takeFirst();
            QCOMPARE(args.at(0).value<QModelIndex>(), childIdx);
            QCOMPARE(args.at(1).value<QModelIndex>(), childIdx);
            rolesVector = args.at(2).value<QVector<int>>();
            QVERIFY(rolesVector.contains(Qt::EditRole));
            QVERIFY(rolesVector.contains(Qt::DisplayRole));
            testModel.setData(childIdx, r + c, Qt::UserRole + 5);
            QCOMPARE(childIdx.data(Qt::UserRole + 5).toInt(), r + c);
            QCOMPARE(dataChangedSpy.count(), 1);
            args = dataChangedSpy.takeFirst();
            QCOMPARE(args.at(0).value<QModelIndex>(), childIdx);
            QCOMPARE(args.at(1).value<QModelIndex>(), childIdx);
            QCOMPARE(args.at(2).value<QVector<int>>(), QVector<int>{Qt::UserRole + 5});

            itemData = testModel.itemData(childIdx);
            QCOMPARE(itemData.size(), 3);
            QVERIFY(itemData.contains(Qt::EditRole));
            QVERIFY(itemData.contains(Qt::DisplayRole));
            QVERIFY(itemData.contains(Qt::UserRole + 5));
            QCOMPARE(itemData.value(Qt::EditRole).toString(), displayString);
            QCOMPARE(itemData.value(Qt::DisplayRole).toString(), displayString);
            QCOMPARE(itemData.value(Qt::UserRole + 5).toInt(), r + c);
        }
    }
    QVERIFY(testModel.itemData(QModelIndex()).isEmpty());
}

void tst_GenericModel::fillTable(QAbstractItemModel *model) const
{
    model->insertColumns(0, 3);
    model->insertRows(0, 5);
    for (int r = 0; r < model->rowCount(); ++r) {
        for (int c = 0; c < model->columnCount(); ++c) {
            const QModelIndex idx = model->index(r, c);
            model->setData(idx, QStringLiteral("%1,%2").arg(r).arg(c));
            model->setData(idx, r, Qt::UserRole);
            model->setData(idx, c, Qt::UserRole + 1);
        }
    }
}
