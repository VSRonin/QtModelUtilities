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
#ifndef SKIP_QTBUG_92220
    QTest::newRow("No Columns") << 0;
#endif
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
#ifndef SKIP_QTBUG_92220
    QTest::newRow("No Columns") << 0;
#endif
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
    testModel.setHeaderData(0, Qt::Vertical, 1);

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
    QCOMPARE(testModel.headerData(1, Qt::Vertical).toInt(), 1);

    QVERIFY(testModel.insertRows(0, 2));
    QCOMPARE(testModel.rowCount(), 4);
    for (QSignalSpy *spy : {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 1);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(3, Qt::Vertical).toInt(), 1);

    QVERIFY(testModel.insertRows(2, 2));
    QCOMPARE(testModel.rowCount(), 6);
    for (QSignalSpy *spy : {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(5, Qt::Vertical).toInt(), 1);

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
#ifndef SKIP_QTBUG_92220
    QTest::newRow("One Row") << 1;
    QTest::newRow("Multiple Rows") << 3;
#endif
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
#ifndef SKIP_QTBUG_92220
    QTest::newRow("One Row") << 1;
    QTest::newRow("Multiple Rows") << 3;
#endif
}

void tst_GenericModel::insertColumns()
{
    QFETCH(int, insertRow);
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    if (insertRow > 0)
        testModel.insertRows(0, insertRow);
    QVERIFY(testModel.insertColumn(0));
    testModel.setHeaderData(0, Qt::Horizontal, 1);

    QSignalSpy columnsAboutToBeInsertedSpy(&testModel, SIGNAL(columnsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(columnsAboutToBeInsertedSpy.isValid());
    QSignalSpy columnsInsertedSpy(&testModel, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(columnsInsertedSpy.isValid());
    QSignalSpy *const spyArr[] = {&columnsInsertedSpy, &columnsAboutToBeInsertedSpy};

    QVERIFY(testModel.insertColumns(0, 1));
    QCOMPARE(testModel.columnCount(), 2);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(1, Qt::Horizontal).toInt(), 1);

    QVERIFY(testModel.insertColumns(0, 2));
    QCOMPARE(testModel.columnCount(), 4);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 1);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(3, Qt::Horizontal).toInt(), 1);

    QVERIFY(testModel.insertColumns(2, 2));
    QCOMPARE(testModel.columnCount(), 6);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(5, Qt::Horizontal).toInt(), 1);

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

void tst_GenericModel::removeColumn_data()
{
    QTest::addColumn<int>("insertRow");
    QTest::newRow("No Columns") << 0;
#ifndef SKIP_QTBUG_92220
    QTest::newRow("One Column") << 1;
    QTest::newRow("Multiple Columns") << 3;
#endif
}

void tst_GenericModel::removeColumn()
{
    QFETCH(int, insertRow);
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    if (insertRow > 0)
        testModel.insertRows(0, insertRow);
    testModel.insertColumns(0, 13);
    QSignalSpy columnsAboutToBeRemovedSpy(&testModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(columnsAboutToBeRemovedSpy.isValid());
    QSignalSpy columnsRemovedSpy(&testModel, SIGNAL(columnsRemoved(QModelIndex, int, int)));
    QVERIFY(columnsRemovedSpy.isValid());
    QSignalSpy *const spyArr[] = {&columnsRemovedSpy, &columnsAboutToBeRemovedSpy};

    QCOMPARE(testModel.columnCount(), 13);
    QVERIFY(!testModel.removeColumn(-1));
    QCOMPARE(testModel.columnCount(), 13);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.removeColumn(13));
    QCOMPARE(testModel.columnCount(), 13);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(testModel.removeColumn(0));
    QCOMPARE(testModel.columnCount(), 12);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QVERIFY(testModel.removeColumn(0));
    QCOMPARE(testModel.columnCount(), 11);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QVERIFY(testModel.removeColumn(1));
    QCOMPARE(testModel.columnCount(), 10);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 1);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QVERIFY(testModel.removeColumn(9));
    QCOMPARE(testModel.columnCount(), 9);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 9);
        QCOMPARE(args.at(1).toInt(), 9);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
}

void tst_GenericModel::removeColumns_data()
{
    QTest::addColumn<int>("insertRow");
    QTest::newRow("No Rows") << 0;
#ifndef SKIP_QTBUG_92220
    QTest::newRow("One Row") << 1;
    QTest::newRow("Multiple Rows") << 3;
#endif
}
void tst_GenericModel::removeColumns()
{
    QFETCH(int, insertRow);
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    if (insertRow > 0)
        testModel.insertRows(0, insertRow);
    QVERIFY(testModel.insertColumns(0, 13));
    testModel.setHeaderData(10, Qt::Horizontal, 1);

    QSignalSpy columnsAboutToBeRemovedSpy(&testModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(columnsAboutToBeRemovedSpy.isValid());
    QSignalSpy columnsRemovedSpy(&testModel, SIGNAL(columnsRemoved(QModelIndex, int, int)));
    QVERIFY(columnsRemovedSpy.isValid());
    QSignalSpy *const spyArr[] = {&columnsRemovedSpy, &columnsAboutToBeRemovedSpy};

    QVERIFY(testModel.removeColumns(0, 1));
    QCOMPARE(testModel.columnCount(), 12);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(9, Qt::Horizontal).toInt(), 1);
    QVERIFY(testModel.removeColumns(0, 2));
    QCOMPARE(testModel.columnCount(), 10);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 1);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(7, Qt::Horizontal).toInt(), 1);

    QVERIFY(testModel.removeColumns(2, 2));
    QCOMPARE(testModel.columnCount(), 8);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(5, Qt::Horizontal).toInt(), 1);

    QVERIFY(!testModel.removeColumns(0, 0));
    QCOMPARE(testModel.columnCount(), 8);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.removeColumns(2, 0));
    QCOMPARE(testModel.columnCount(), 8);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.removeColumns(-1, 2));
    QCOMPARE(testModel.columnCount(), 8);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.removeColumns(9, 2));
    QCOMPARE(testModel.columnCount(), 8);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
}

void tst_GenericModel::removeRow_data()
{
    QTest::addColumn<int>("insertCol");
#ifndef SKIP_QTBUG_92886
    QTest::newRow("No Columns") << 0;
#endif
    QTest::newRow("One Column") << 1;
    QTest::newRow("Multiple Columns") << 3;
}

void tst_GenericModel::removeRow()
{
    QFETCH(int, insertCol);
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    if (insertCol > 0)
        testModel.insertColumns(0, insertCol);
    testModel.insertRows(0, 13);
    QSignalSpy rowsAboutToBeRemovedSpy(&testModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(rowsAboutToBeRemovedSpy.isValid());
    QSignalSpy rowsRemovedSpy(&testModel, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    QVERIFY(rowsRemovedSpy.isValid());
    QSignalSpy *const spyArr[] = {&rowsAboutToBeRemovedSpy, &rowsRemovedSpy};

    QCOMPARE(testModel.rowCount(), 13);
    QVERIFY(!testModel.removeRow(-1));
    QCOMPARE(testModel.rowCount(), 13);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.removeRow(13));
    QCOMPARE(testModel.rowCount(), 13);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(testModel.removeRow(0));
    QCOMPARE(testModel.rowCount(), 12);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QVERIFY(testModel.removeRow(0));
    QCOMPARE(testModel.rowCount(), 11);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QVERIFY(testModel.removeRow(1));
    QCOMPARE(testModel.rowCount(), 10);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 1);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QVERIFY(testModel.removeRow(9));
    QCOMPARE(testModel.rowCount(), 9);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 9);
        QCOMPARE(args.at(1).toInt(), 9);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
}

void tst_GenericModel::removeRows_data()
{
    QTest::addColumn<int>("insertCol");
#ifndef SKIP_QTBUG_92886
    QTest::newRow("No Columns") << 0;
#endif
    QTest::newRow("One Column") << 1;
    QTest::newRow("Multiple Columns") << 3;
}

void tst_GenericModel::removeRows()
{
    QFETCH(int, insertCol);
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    if (insertCol > 0)
        testModel.insertColumns(0, insertCol);
    QVERIFY(testModel.insertRows(0, 13));
    testModel.setHeaderData(10, Qt::Vertical, 1);

    QSignalSpy rowsAboutToBeRemovedSpy(&testModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(rowsAboutToBeRemovedSpy.isValid());
    QSignalSpy rowsRemovedSpy(&testModel, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    QVERIFY(rowsRemovedSpy.isValid());
    QSignalSpy *const spyArr[] = {&rowsRemovedSpy, &rowsAboutToBeRemovedSpy};

    QVERIFY(testModel.removeRows(0, 1));
    QCOMPARE(testModel.rowCount(), 12);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(9, Qt::Vertical).toInt(), 1);

    QVERIFY(testModel.removeRows(0, 2));
    QCOMPARE(testModel.rowCount(), 10);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 1);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(7, Qt::Vertical).toInt(), 1);

    QVERIFY(testModel.removeRows(2, 2));
    QCOMPARE(testModel.rowCount(), 8);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
    }
    QCOMPARE(testModel.headerData(5, Qt::Vertical).toInt(), 1);

    QVERIFY(!testModel.removeRows(0, 0));
    QCOMPARE(testModel.rowCount(), 8);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.removeRows(2, 0));
    QCOMPARE(testModel.rowCount(), 8);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.removeRows(-1, 2));
    QCOMPARE(testModel.rowCount(), 8);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.removeRows(9, 2));
    QCOMPARE(testModel.rowCount(), 8);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
}

void tst_GenericModel::insertChildren_data()
{
    QTest::addColumn<QAbstractItemModel *>("testModel");
    QTest::addColumn<QModelIndex>("parentIdx");

    QAbstractItemModel *testModel = new GenericModel;
    fillTable(testModel, 5, 3);
    QTest::newRow("Child First Column") << testModel << testModel->index(0, 0);
    testModel = new GenericModel;
    fillTable(testModel, 5, 3);
    QTest::newRow("Child Middle Column") << testModel << testModel->index(2, 1);
    testModel = new GenericModel;
    fillTable(testModel, 5, 3);
    testModel->insertColumns(0, 3, testModel->index(0, 0));
    testModel->insertRows(0, 5, testModel->index(0, 0));
    QTest::newRow("Grandchild First Column") << testModel << testModel->index(0, 0, testModel->index(0, 0));
    testModel = new GenericModel;
    fillTable(testModel, 5, 3);
    testModel->insertColumns(0, 3, testModel->index(0, 0));
    testModel->insertRows(0, 5, testModel->index(0, 0));
    QTest::newRow("Grandchild Middle Column") << testModel << testModel->index(2, 1, testModel->index(0, 0));
}

void tst_GenericModel::insertChildren()
{
    QFETCH(QAbstractItemModel *, testModel);
    QFETCH(QModelIndex, parentIdx);
    ModelTest probe(testModel, nullptr);

    QSignalSpy rowsAboutToBeInsertedSpy(testModel, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(rowsAboutToBeInsertedSpy.isValid());
    QSignalSpy rowsInsertedSpy(testModel, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(rowsInsertedSpy.isValid());
    QSignalSpy columnsAboutToBeInsertedSpy(testModel, SIGNAL(columnsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(columnsAboutToBeInsertedSpy.isValid());
    QSignalSpy columnsInsertedSpy(testModel, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(columnsInsertedSpy.isValid());
    QSignalSpy *const colSpyArr[] = {&columnsInsertedSpy, &columnsAboutToBeInsertedSpy};
    QSignalSpy *const rowSpyArr[] = {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy};
    QSignalSpy *const spyArr[] = {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy, &columnsInsertedSpy, &columnsAboutToBeInsertedSpy};

    QVERIFY(!testModel->insertRows(-1, 1, parentIdx));
    QCOMPARE(testModel->rowCount(parentIdx), 0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel->insertRows(1, 1, parentIdx));
    QCOMPARE(testModel->rowCount(parentIdx), 0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel->insertRows(0, 0, parentIdx));
    QCOMPARE(testModel->rowCount(parentIdx), 0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel->insertColumns(-1, 1, parentIdx));
    QCOMPARE(testModel->columnCount(parentIdx), 0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel->insertColumns(1, 1, parentIdx));
    QCOMPARE(testModel->columnCount(parentIdx), 0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel->insertColumns(0, 0, parentIdx));
    QCOMPARE(testModel->columnCount(parentIdx), 0);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(testModel->insertColumns(0, 1, parentIdx));
    QCOMPARE(testModel->columnCount(parentIdx), 1);
    QCOMPARE(testModel->columnCount(), 3);
    QCOMPARE(testModel->rowCount(parentIdx), 0);
    for (QSignalSpy *spy : rowSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : colSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel->insertRows(0, 1, parentIdx));
    QCOMPARE(testModel->rowCount(parentIdx), 1);
    QCOMPARE(testModel->rowCount(), 5);
    QCOMPARE(testModel->columnCount(parentIdx), 1);
    for (QSignalSpy *spy : colSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : rowSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel->insertRows(1, 2, parentIdx));
    QCOMPARE(testModel->rowCount(parentIdx), 3);
    QCOMPARE(testModel->rowCount(), 5);
    QCOMPARE(testModel->columnCount(parentIdx), 1);
    for (QSignalSpy *spy : colSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : rowSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel->insertColumns(1, 3, parentIdx));
    QCOMPARE(testModel->columnCount(parentIdx), 4);
    QCOMPARE(testModel->columnCount(), 3);
    QCOMPARE(testModel->rowCount(parentIdx), 3);
    for (QSignalSpy *spy : rowSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : colSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    testModel->deleteLater();
}
void tst_GenericModel::removeChildren_data()
{
    QTest::addColumn<QAbstractItemModel *>("testModel");
    QTest::addColumn<QModelIndex>("parentIdx");

    QAbstractItemModel *testModel = new GenericModel;
    fillTable(testModel, 5, 3);
    QTest::newRow("Child First Column") << testModel << testModel->index(0, 0);
    testModel = new GenericModel;
    fillTable(testModel, 5, 3);
    QTest::newRow("Child Middle Column") << testModel << testModel->index(2, 1);
    testModel = new GenericModel;
    fillTable(testModel, 5, 3);
    testModel->insertColumns(0, 3, testModel->index(0, 0));
    testModel->insertRows(0, 5, testModel->index(0, 0));
    QTest::newRow("Grandchild First Column") << testModel << testModel->index(0, 0, testModel->index(0, 0));
    testModel = new GenericModel;
    fillTable(testModel, 5, 3);
    testModel->insertColumns(0, 3, testModel->index(0, 0));
    testModel->insertRows(0, 5, testModel->index(0, 0));
    QTest::newRow("Grandchild Middle Column") << testModel << testModel->index(2, 1, testModel->index(0, 0));
}

void tst_GenericModel::removeChildren()
{
    QFETCH(QAbstractItemModel *, testModel);
    QFETCH(QModelIndex, parentIdx);
    ModelTest probe(testModel, nullptr);

    QSignalSpy rowsAboutToBeRemovedSpy(testModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(rowsAboutToBeRemovedSpy.isValid());
    QSignalSpy rowsRemovedSpy(testModel, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    QVERIFY(rowsRemovedSpy.isValid());
    QSignalSpy columnsAboutToBeRemovedSpy(testModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(columnsAboutToBeRemovedSpy.isValid());
    QSignalSpy columnsRemovedSpy(testModel, SIGNAL(columnsRemoved(QModelIndex, int, int)));
    QVERIFY(columnsRemovedSpy.isValid());
    QSignalSpy *const colSpyArr[] = {&columnsRemovedSpy, &columnsAboutToBeRemovedSpy};
    QSignalSpy *const rowSpyArr[] = {&rowsRemovedSpy, &rowsAboutToBeRemovedSpy};
    QSignalSpy *const spyArr[] = {&rowsRemovedSpy, &rowsAboutToBeRemovedSpy, &columnsRemovedSpy, &columnsAboutToBeRemovedSpy};

    testModel->insertColumns(0, 7, parentIdx);
    testModel->insertRows(0, 20, parentIdx);
    QVERIFY(!testModel->removeRows(-1, 1, parentIdx));
    QCOMPARE(testModel->rowCount(parentIdx), 20);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel->removeRows(20, 1, parentIdx));
    QCOMPARE(testModel->rowCount(parentIdx), 20);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel->removeRows(0, 0, parentIdx));
    QCOMPARE(testModel->rowCount(parentIdx), 20);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel->removeColumns(-1, 1, parentIdx));
    QCOMPARE(testModel->columnCount(parentIdx), 7);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel->removeColumns(8, 1, parentIdx));
    QCOMPARE(testModel->columnCount(parentIdx), 7);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel->removeColumns(0, 0, parentIdx));
    QCOMPARE(testModel->columnCount(parentIdx), 7);
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(testModel->removeRows(0, 1, parentIdx));
    QCOMPARE(testModel->rowCount(parentIdx), 19);
    QCOMPARE(testModel->rowCount(), 5);
    QCOMPARE(testModel->columnCount(parentIdx), 7);
    for (QSignalSpy *spy : colSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : rowSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel->removeColumns(0, 1, parentIdx));
    QCOMPARE(testModel->columnCount(parentIdx), 6);
    QCOMPARE(testModel->columnCount(), 3);
    QCOMPARE(testModel->rowCount(parentIdx), 19);
    for (QSignalSpy *spy : rowSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : colSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel->removeRows(1, 2, parentIdx));
    QCOMPARE(testModel->rowCount(parentIdx), 17);
    QCOMPARE(testModel->rowCount(), 5);
    QCOMPARE(testModel->columnCount(parentIdx), 6);
    for (QSignalSpy *spy : colSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : rowSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    QVERIFY(testModel->removeColumns(1, 4, parentIdx));
    QCOMPARE(testModel->columnCount(parentIdx), 2);
    QCOMPARE(testModel->columnCount(), 3);
    QCOMPARE(testModel->rowCount(parentIdx), 17);
    for (QSignalSpy *spy : rowSpyArr)
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : colSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(2).toInt(), 4);
        QCOMPARE(args.at(1).toInt(), 1);
        QCOMPARE(args.at(0).value<QModelIndex>(), parentIdx);
    }
    testModel->deleteLater();
}

void tst_GenericModel::data()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    testModel.insertColumns(0, 3);
    testModel.insertRows(0, 5);
    QSignalSpy dataChangedSpy(&testModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(dataChangedSpy.isValid());

    QVERIFY(!testModel.setData(QModelIndex(), 13));
    QCOMPARE(dataChangedSpy.count(), 0);
    QVERIFY(testModel.setData(testModel.index(0, 0), QVariant()));
    QCOMPARE(dataChangedSpy.count(), 0);

    for (int r = 0; r < testModel.rowCount(); ++r) {
        for (int c = 0; c < testModel.columnCount(); ++c) {
            const QModelIndex idx = testModel.index(r, c);
            const QString displayString = QStringLiteral("%1,%2").arg(r).arg(c);
            QVERIFY(testModel.setData(idx, displayString));
            QCOMPARE(testModel.data(idx).toString(), displayString);
            QCOMPARE(dataChangedSpy.count(), 1);
            auto args = dataChangedSpy.takeFirst();
            QCOMPARE(args.at(0).value<QModelIndex>(), idx);
            QCOMPARE(args.at(1).value<QModelIndex>(), idx);
            QVector<int> rolesVector = args.at(2).value<QVector<int>>();
            QVERIFY(rolesVector.contains(Qt::EditRole));
            QVERIFY(rolesVector.contains(Qt::DisplayRole));
            QVERIFY(testModel.setData(idx, displayString));
            QCOMPARE(dataChangedSpy.count(), 0);
            const QString tooltipString = QStringLiteral("Tooltip %1,%2").arg(r).arg(c);
            QVERIFY(testModel.setData(idx, tooltipString, Qt::ToolTipRole));
            QCOMPARE(testModel.data(idx, Qt::ToolTipRole).toString(), tooltipString);
            QCOMPARE(dataChangedSpy.count(), 1);
            args = dataChangedSpy.takeFirst();
            QCOMPARE(args.at(0).value<QModelIndex>(), idx);
            QCOMPARE(args.at(1).value<QModelIndex>(), idx);
            QCOMPARE(args.at(2).value<QVector<int>>(), QVector<int>{Qt::ToolTipRole});
            QVERIFY(testModel.setData(idx, r + c, Qt::UserRole + 5));
            QCOMPARE(idx.data(Qt::UserRole + 5).toInt(), r + c);
            QCOMPARE(dataChangedSpy.count(), 1);
            args = dataChangedSpy.takeFirst();
            QCOMPARE(args.at(0).value<QModelIndex>(), idx);
            QCOMPARE(args.at(1).value<QModelIndex>(), idx);
            QCOMPARE(args.at(2).value<QVector<int>>(), QVector<int>{Qt::UserRole + 5});
            QVERIFY(testModel.setData(idx, r + c, Qt::UserRole + 5));
            QCOMPARE(dataChangedSpy.count(), 0);

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

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
            QModelRoleData rolesData[] = {QModelRoleData(Qt::EditRole), QModelRoleData(Qt::DisplayRole), QModelRoleData(Qt::ToolTipRole),
                                          QModelRoleData(Qt::UserRole + 5), QModelRoleData(Qt::DecorationRole)};
            QModelRoleDataSpan span(rolesData);
            testModel.multiData(idx, span);
            QCOMPARE(span.dataForRole(Qt::EditRole)->toString(), displayString);
            QCOMPARE(span.dataForRole(Qt::DisplayRole)->toString(), displayString);
            QCOMPARE(span.dataForRole(Qt::ToolTipRole)->toString(), tooltipString);
            QCOMPARE(span.dataForRole(Qt::UserRole + 5)->toInt(), r + c);
            QVERIFY(!span.dataForRole(Qt::DecorationRole)->isValid());
#endif

            testModel.insertColumn(0, idx);
            testModel.insertRow(0, idx);
            const QModelIndex childIdx = testModel.index(0, 0, idx);
            QVERIFY(testModel.setData(childIdx, displayString));
            QCOMPARE(testModel.data(childIdx).toString(), displayString);
            QCOMPARE(dataChangedSpy.count(), 1);
            args = dataChangedSpy.takeFirst();
            QCOMPARE(args.at(0).value<QModelIndex>(), childIdx);
            QCOMPARE(args.at(1).value<QModelIndex>(), childIdx);
            rolesVector = args.at(2).value<QVector<int>>();
            QVERIFY(rolesVector.contains(Qt::EditRole));
            QVERIFY(rolesVector.contains(Qt::DisplayRole));
            QVERIFY(testModel.setData(childIdx, r + c, Qt::UserRole + 5));
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

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
            testModel.multiData(childIdx, span);
            QCOMPARE(span.dataForRole(Qt::EditRole)->toString(), displayString);
            QCOMPARE(span.dataForRole(Qt::DisplayRole)->toString(), displayString);
            QVERIFY(!span.dataForRole(Qt::ToolTipRole)->isValid());
            QCOMPARE(span.dataForRole(Qt::UserRole + 5)->toInt(), r + c);
            QVERIFY(!span.dataForRole(Qt::DecorationRole)->isValid());
#endif
        }
    }
    QVERIFY(testModel.itemData(QModelIndex()).isEmpty());
}

void tst_GenericModel::clearData()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    fillTable(&testModel, 5, 3);
    QSignalSpy dataChangedSpy(&testModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(dataChangedSpy.isValid());
    QVERIFY(!testModel.clearItemData(QModelIndex()));
    QCOMPARE(dataChangedSpy.count(), 0);

    QVERIFY(testModel.clearItemData(testModel.index(0, 0)));
    QVERIFY(!testModel.index(0, 0).data().isValid());
    QVERIFY(testModel.itemData(testModel.index(0, 0)).isEmpty());
    QCOMPARE(dataChangedSpy.count(), 1);
    auto args = dataChangedSpy.takeFirst();
    QCOMPARE(args.at(0).value<QModelIndex>(), testModel.index(0, 0));
    QCOMPARE(args.at(1).value<QModelIndex>(), testModel.index(0, 0));
    QVERIFY(testModel.clearItemData(testModel.index(0, 0)));
    QCOMPARE(dataChangedSpy.count(), 0);
}

void tst_GenericModel::headerData_data()
{
    QTest::addColumn<Qt::Orientation>("orientation");
    QTest::newRow("Vertical") << Qt::Vertical;
    QTest::newRow("Horizontal") << Qt::Horizontal;
}

void tst_GenericModel::headerData()
{
    QFETCH(Qt::Orientation, orientation);
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    fillTable(&testModel, 5, 3);
    const int endIter = orientation == Qt::Vertical ? testModel.rowCount() : testModel.columnCount();
    QSignalSpy headerDataChangedSpy(&testModel, SIGNAL(headerDataChanged(Qt::Orientation, int, int)));
    QVERIFY(headerDataChangedSpy.isValid());
    QSignalSpy dataChangedSpy(&testModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(dataChangedSpy.isValid());
    for (int i = 0; i < endIter; ++i) {
        const QString headString = QStringLiteral("Header %1").arg(i);
        QVERIFY(testModel.setHeaderData(i, orientation, headString));
        QCOMPARE(testModel.headerData(i, orientation).toString(), headString);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(headerDataChangedSpy.count(), 1);
        auto args = headerDataChangedSpy.takeFirst();
        QCOMPARE(args.at(0).value<Qt::Orientation>(), orientation);
        QCOMPARE(args.at(1).toInt(), i);
        QCOMPARE(args.at(2).toInt(), i);
        QVERIFY(testModel.setHeaderData(i, orientation, headString));
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(headerDataChangedSpy.count(), 0);

        QVERIFY(testModel.setHeaderData(i, orientation, i, Qt::UserRole));
        QCOMPARE(testModel.headerData(i, orientation, Qt::UserRole).toInt(), i);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(headerDataChangedSpy.count(), 1);
        args = headerDataChangedSpy.takeFirst();
        QCOMPARE(args.at(0).value<Qt::Orientation>(), orientation);
        QCOMPARE(args.at(1).toInt(), i);
        QCOMPARE(args.at(2).toInt(), i);
        QVERIFY(testModel.setHeaderData(i, orientation, i, Qt::UserRole));
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(headerDataChangedSpy.count(), 0);
    }
    QVERIFY(!testModel.headerData(endIter, orientation).isValid());
    QVERIFY(!testModel.headerData(-1, orientation).isValid());
    QVERIFY(!testModel.setHeaderData(endIter, orientation, 13));
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(headerDataChangedSpy.count(), 0);
    QVERIFY(!testModel.setHeaderData(-1, orientation, 13));
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(headerDataChangedSpy.count(), 0);
    QVERIFY(!testModel.headerData(endIter, orientation).isValid());
    QVERIFY(!testModel.headerData(-1, orientation).isValid());
}

void tst_GenericModel::sortList()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    QSignalSpy layoutAboutToBeChangedSpy(&testModel, SIGNAL(layoutAboutToBeChanged()));
    QVERIFY(layoutAboutToBeChangedSpy.isValid());
    QSignalSpy layoutChangedSpy(&testModel, SIGNAL(layoutChanged()));
    QVERIFY(layoutChangedSpy.isValid());

    testModel.insertColumn(0);
    testModel.insertRows(0, 5);
    testModel.setData(testModel.index(0, 0), 3);
    testModel.setData(testModel.index(1, 0), 1);
    testModel.setData(testModel.index(2, 0), 4);
    testModel.setData(testModel.index(3, 0), 2);
    testModel.setData(testModel.index(4, 0), 5);
    testModel.setHeaderData(0, Qt::Vertical, 3);
    testModel.setHeaderData(1, Qt::Vertical, 1);
    testModel.setHeaderData(2, Qt::Vertical, 4);
    testModel.setHeaderData(3, Qt::Vertical, 2);
    testModel.setHeaderData(4, Qt::Vertical, 5);
    QPersistentModelIndex oneIndex(testModel.index(1, 0));
    QPersistentModelIndex fiveIndex(testModel.index(4, 0));
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);

    testModel.sort(0, Qt::AscendingOrder);
    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);
    for (int i = 0; i < 5; ++i) {
        QCOMPARE(testModel.data(testModel.index(i, 0)).toInt(), i + 1);
        QCOMPARE(testModel.headerData(i, Qt::Vertical).toInt(), i + 1);
    }
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(oneIndex.row(), 0);
    QCOMPARE(fiveIndex.row(), 4);

    testModel.sort(0, Qt::AscendingOrder);
    QCOMPARE(layoutAboutToBeChangedSpy.count(), 2);
    QCOMPARE(layoutChangedSpy.count(), 2);
    for (int i = 0; i < 5; ++i) {
        QCOMPARE(testModel.data(testModel.index(i, 0)).toInt(), i + 1);
        QCOMPARE(testModel.headerData(i, Qt::Vertical).toInt(), i + 1);
    }
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(oneIndex.row(), 0);
    QCOMPARE(fiveIndex.row(), 4);

    testModel.sort(0, Qt::DescendingOrder);
    QCOMPARE(layoutAboutToBeChangedSpy.count(), 3);
    QCOMPARE(layoutChangedSpy.count(), 3);
    for (int i = 0; i < 5; ++i) {
        QCOMPARE(testModel.data(testModel.index(i, 0)).toInt(), 5 - i);
        QCOMPARE(testModel.headerData(i, Qt::Vertical).toInt(), 5 - i);
    }
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(oneIndex.row(), 4);
    QCOMPARE(fiveIndex.row(), 0);
}

void tst_GenericModel::sortTable()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    QSignalSpy layoutAboutToBeChangedSpy(&testModel, SIGNAL(layoutAboutToBeChanged()));
    QVERIFY(layoutAboutToBeChangedSpy.isValid());
    QSignalSpy layoutChangedSpy(&testModel, SIGNAL(layoutChanged()));
    QVERIFY(layoutChangedSpy.isValid());

    testModel.insertColumns(0, 2);
    testModel.insertRows(0, 5);
    testModel.setData(testModel.index(0, 1), 3);
    testModel.setData(testModel.index(1, 1), 1);
    testModel.setData(testModel.index(2, 1), 4);
    testModel.setData(testModel.index(3, 1), 2);
    testModel.setData(testModel.index(4, 1), 5);

    testModel.setData(testModel.index(0, 0), QChar(QLatin1Char('e')));
    testModel.setData(testModel.index(1, 0), QChar(QLatin1Char('b')));
    testModel.setData(testModel.index(2, 0), QChar(QLatin1Char('d')));
    testModel.setData(testModel.index(3, 0), QChar(QLatin1Char('a')));
    testModel.setData(testModel.index(4, 0), QChar(QLatin1Char('c')));
    QPersistentModelIndex oneIndex(testModel.index(1, 1));
    QPersistentModelIndex fiveIndex(testModel.index(4, 1));
    QPersistentModelIndex bIndex(testModel.index(1, 0));
    QPersistentModelIndex cIndex(testModel.index(4, 0));
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(bIndex.data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(cIndex.data().value<QChar>(), QChar(QLatin1Char('c')));

    testModel.sort(1, Qt::AscendingOrder);
    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);
    for (int i = 0; i < 5; ++i)
        QCOMPARE(testModel.data(testModel.index(i, 1)).toInt(), i + 1);
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(bIndex.data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(cIndex.data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(oneIndex.row(), 0);
    QCOMPARE(fiveIndex.row(), 4);
    QCOMPARE(bIndex.row(), 0);
    QCOMPARE(cIndex.row(), 4);

    testModel.sort(1, Qt::AscendingOrder);
    QCOMPARE(layoutAboutToBeChangedSpy.count(), 2);
    QCOMPARE(layoutChangedSpy.count(), 2);
    for (int i = 0; i < 5; ++i)
        QCOMPARE(testModel.data(testModel.index(i, 1)).toInt(), i + 1);
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(bIndex.data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(cIndex.data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(oneIndex.row(), 0);
    QCOMPARE(fiveIndex.row(), 4);
    QCOMPARE(bIndex.row(), 0);
    QCOMPARE(cIndex.row(), 4);

    testModel.sort(1, Qt::DescendingOrder);
    QCOMPARE(layoutAboutToBeChangedSpy.count(), 3);
    QCOMPARE(layoutChangedSpy.count(), 3);
    for (int i = 0; i < 5; ++i)
        QCOMPARE(testModel.data(testModel.index(i, 1)).toInt(), 5 - i);
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(bIndex.data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(cIndex.data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(oneIndex.row(), 4);
    QCOMPARE(fiveIndex.row(), 0);
    QCOMPARE(bIndex.row(), 4);
    QCOMPARE(cIndex.row(), 0);

    testModel.sort(0, Qt::AscendingOrder);
    QCOMPARE(layoutAboutToBeChangedSpy.count(), 4);
    QCOMPARE(layoutChangedSpy.count(), 4);
    for (int i = 0; i < 5; ++i)
        QCOMPARE(testModel.data(testModel.index(i, 0)).value<QChar>(), QChar('a' + i));
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(bIndex.data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(cIndex.data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(oneIndex.row(), 1);
    QCOMPARE(fiveIndex.row(), 2);
    QCOMPARE(bIndex.row(), 1);
    QCOMPARE(cIndex.row(), 2);
}

void tst_GenericModel::sortByRole()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    testModel.setSortRole(Qt::UserRole);
    QSignalSpy layoutAboutToBeChangedSpy(&testModel, SIGNAL(layoutAboutToBeChanged()));
    QVERIFY(layoutAboutToBeChangedSpy.isValid());
    QSignalSpy layoutChangedSpy(&testModel, SIGNAL(layoutChanged()));
    QVERIFY(layoutChangedSpy.isValid());

    testModel.insertColumn(0);
    testModel.insertRows(0, 5);
    testModel.setData(testModel.index(0, 0), 3);
    testModel.setData(testModel.index(1, 0), 1);
    testModel.setData(testModel.index(2, 0), 4);
    testModel.setData(testModel.index(3, 0), 2);
    testModel.setData(testModel.index(4, 0), 5);
    testModel.setData(testModel.index(0, 0), QStringLiteral("Charles"), Qt::UserRole);
    testModel.setData(testModel.index(1, 0), QStringLiteral("Emma"), Qt::UserRole);
    testModel.setData(testModel.index(2, 0), QStringLiteral("Bob"), Qt::UserRole);
    testModel.setData(testModel.index(3, 0), QStringLiteral("Daniel"), Qt::UserRole);
    testModel.setData(testModel.index(4, 0), QStringLiteral("Alice"), Qt::UserRole);
    QPersistentModelIndex oneIndex(testModel.index(1, 0));
    QPersistentModelIndex fiveIndex(testModel.index(4, 0));
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);

    testModel.sort(0, Qt::AscendingOrder);
    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);
    for (int i = 0; i < 5; ++i)
        QCOMPARE(testModel.data(testModel.index(i, 0)).toInt(), 5 - i);
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(oneIndex.row(), 4);
    QCOMPARE(fiveIndex.row(), 0);
}

void tst_GenericModel::sortTree()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    QSignalSpy layoutAboutToBeChangedSpy(&testModel, SIGNAL(layoutAboutToBeChanged()));
    QVERIFY(layoutAboutToBeChangedSpy.isValid());
    QSignalSpy layoutChangedSpy(&testModel, SIGNAL(layoutChanged()));
    QVERIFY(layoutChangedSpy.isValid());

    testModel.insertColumn(0);
    testModel.insertRows(0, 5);
    testModel.setData(testModel.index(0, 0), 3);
    testModel.setData(testModel.index(1, 0), 1);
    testModel.setData(testModel.index(2, 0), 4);
    testModel.setData(testModel.index(3, 0), 2);
    testModel.setData(testModel.index(4, 0), 5);
    const QModelIndex parIdx = testModel.index(1, 0);
    testModel.insertColumn(0, parIdx);
    testModel.insertRows(0, 3, parIdx);
    testModel.setData(testModel.index(0, 0, parIdx), 3);
    testModel.setData(testModel.index(1, 0, parIdx), 4);
    testModel.setData(testModel.index(2, 0, parIdx), 1);
    QPersistentModelIndex oneIndex(testModel.index(1, 0));
    QPersistentModelIndex fiveIndex(testModel.index(4, 0));
    QPersistentModelIndex childFourIndex(testModel.index(1, 0, parIdx));
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(childFourIndex.data().toInt(), 4);

    testModel.sort(0, QModelIndex(), Qt::AscendingOrder, false);
    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);
    for (int i = 0; i < 5; ++i)
        QCOMPARE(testModel.data(testModel.index(i, 0)).toInt(), i + 1);
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(childFourIndex.data().toInt(), 4);
    QCOMPARE(oneIndex.row(), 0);
    QCOMPARE(fiveIndex.row(), 4);
    QCOMPARE(childFourIndex.row(), 1);
}

void tst_GenericModel::sortTreeChildren()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    QSignalSpy layoutAboutToBeChangedSpy(&testModel, SIGNAL(layoutAboutToBeChanged()));
    QVERIFY(layoutAboutToBeChangedSpy.isValid());
    QSignalSpy layoutChangedSpy(&testModel, SIGNAL(layoutChanged()));
    QVERIFY(layoutChangedSpy.isValid());

    testModel.insertColumn(0);
    testModel.insertRows(0, 5);
    testModel.setData(testModel.index(0, 0), 3);
    testModel.setData(testModel.index(1, 0), 1);
    testModel.setData(testModel.index(2, 0), 4);
    testModel.setData(testModel.index(3, 0), 2);
    testModel.setData(testModel.index(4, 0), 5);
    const QModelIndex parIdx = testModel.index(1, 0);
    testModel.insertColumn(0, parIdx);
    testModel.insertRows(0, 3, parIdx);
    testModel.setData(testModel.index(0, 0, parIdx), 3);
    testModel.setData(testModel.index(1, 0, parIdx), 4);
    testModel.setData(testModel.index(2, 0, parIdx), 1);
    QPersistentModelIndex oneIndex(testModel.index(1, 0));
    QPersistentModelIndex fiveIndex(testModel.index(4, 0));
    QPersistentModelIndex childFourIndex(testModel.index(1, 0, parIdx));
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(childFourIndex.data().toInt(), 4);

    testModel.sort(0, parIdx, Qt::AscendingOrder, false);
    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);
    QCOMPARE(testModel.data(testModel.index(0, 0, parIdx)).toInt(), 1);
    QCOMPARE(testModel.data(testModel.index(1, 0, parIdx)).toInt(), 3);
    QCOMPARE(testModel.data(testModel.index(2, 0, parIdx)).toInt(), 4);
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(childFourIndex.data().toInt(), 4);
    QCOMPARE(oneIndex.row(), 1);
    QCOMPARE(fiveIndex.row(), 4);
    QCOMPARE(childFourIndex.row(), 2);
}

void tst_GenericModel::sortTreeRecursive()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    QSignalSpy layoutAboutToBeChangedSpy(&testModel, SIGNAL(layoutAboutToBeChanged()));
    QVERIFY(layoutAboutToBeChangedSpy.isValid());
    QSignalSpy layoutChangedSpy(&testModel, SIGNAL(layoutChanged()));
    QVERIFY(layoutChangedSpy.isValid());

    testModel.insertColumn(0);
    testModel.insertRows(0, 5);
    testModel.setData(testModel.index(0, 0), 3);
    testModel.setData(testModel.index(1, 0), 1);
    testModel.setData(testModel.index(2, 0), 4);
    testModel.setData(testModel.index(3, 0), 2);
    testModel.setData(testModel.index(4, 0), 5);
    const QModelIndex parIdx = testModel.index(1, 0);
    testModel.insertColumn(0, parIdx);
    testModel.insertRows(0, 3, parIdx);
    testModel.setData(testModel.index(0, 0, parIdx), 3);
    testModel.setData(testModel.index(1, 0, parIdx), 4);
    testModel.setData(testModel.index(2, 0, parIdx), 1);
    QPersistentModelIndex oneIndex(testModel.index(1, 0));
    QPersistentModelIndex fiveIndex(testModel.index(4, 0));
    QPersistentModelIndex childFourIndex(testModel.index(1, 0, parIdx));
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(childFourIndex.data().toInt(), 4);

    testModel.sort(0, Qt::AscendingOrder);
    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);
    for (int i = 0; i < 5; ++i)
        QCOMPARE(testModel.data(testModel.index(i, 0)).toInt(), i + 1);
    QCOMPARE(testModel.data(testModel.index(0, 0, parIdx)).toInt(), 1);
    QCOMPARE(testModel.data(testModel.index(1, 0, parIdx)).toInt(), 3);
    QCOMPARE(testModel.data(testModel.index(2, 0, parIdx)).toInt(), 4);
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(fiveIndex.data().toInt(), 5);
    QCOMPARE(childFourIndex.data().toInt(), 4);
    QCOMPARE(oneIndex.row(), 0);
    QCOMPARE(fiveIndex.row(), 4);
    QCOMPARE(childFourIndex.row(), 2);
}

void tst_GenericModel::moveRowsList()
{

    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    testModel.insertColumn(0);
    testModel.insertRows(0, 6);
    QSignalSpy rowsAboutToBeMovedSpy(&testModel, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsAboutToBeMovedSpy.isValid());
    QSignalSpy rowsMovedSpy(&testModel, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsMovedSpy.isValid());
    QSignalSpy *spyArr[] = {&rowsAboutToBeMovedSpy, &rowsMovedSpy};
    const auto resetData = [&testModel]() {
        for (int i = 0; i < 6; ++i) {
            testModel.setData(testModel.index(i, 0), i);
            testModel.setHeaderData(i, Qt::Vertical, i + 1);
        }
    };
    resetData();
    QVERIFY(!testModel.moveRows(QModelIndex(), 2, 1, QModelIndex(), -1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(QModelIndex(), 2, 1, QModelIndex(), 7));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(QModelIndex(), -1, 1, QModelIndex(), 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(QModelIndex(), 6, 1, QModelIndex(), 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(QModelIndex(), 1, 2, QModelIndex(), 2));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(QModelIndex(), 1, 2, QModelIndex(), 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);

    QPersistentModelIndex twoIndex = testModel.index(2, 0);
    QVERIFY(testModel.moveRows(QModelIndex(), 2, 1, QModelIndex(), 4));
    QCOMPARE(testModel.index(0, 0).data().toInt(), 0);
    QCOMPARE(testModel.index(1, 0).data().toInt(), 1);
    QCOMPARE(testModel.index(2, 0).data().toInt(), 3);
    QCOMPARE(testModel.index(3, 0).data().toInt(), 2);
    QCOMPARE(testModel.index(4, 0).data().toInt(), 4);
    QCOMPARE(testModel.index(5, 0).data().toInt(), 5);
    QCOMPARE(testModel.headerData(0, Qt::Vertical).toInt(), 1);
    QCOMPARE(testModel.headerData(1, Qt::Vertical).toInt(), 2);
    QCOMPARE(testModel.headerData(2, Qt::Vertical).toInt(), 4);
    QCOMPARE(testModel.headerData(3, Qt::Vertical).toInt(), 3);
    QCOMPARE(testModel.headerData(4, Qt::Vertical).toInt(), 5);
    QCOMPARE(testModel.headerData(5, Qt::Vertical).toInt(), 6);
    QCOMPARE(testModel.index(1, 0).data().toInt(), 1);
    QCOMPARE(testModel.index(2, 0).data().toInt(), 3);
    QCOMPARE(testModel.index(3, 0).data().toInt(), 2);
    QCOMPARE(testModel.index(4, 0).data().toInt(), 4);
    QCOMPARE(testModel.index(5, 0).data().toInt(), 5);
    QCOMPARE(twoIndex.data().toInt(), 2);
    QCOMPARE(twoIndex.row(), 3);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QVERIFY(!args.at(0).value<QModelIndex>().isValid());
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QVERIFY(!args.at(3).value<QModelIndex>().isValid());
        QCOMPARE(args.at(4).toInt(), 4);
    }

    resetData();
    twoIndex = testModel.index(2, 0);
    QVERIFY(testModel.moveRows(QModelIndex(), 2, 1, QModelIndex(), 0));
    QCOMPARE(testModel.index(0, 0).data().toInt(), 2);
    QCOMPARE(testModel.index(1, 0).data().toInt(), 0);
    QCOMPARE(testModel.index(2, 0).data().toInt(), 1);
    QCOMPARE(testModel.index(3, 0).data().toInt(), 3);
    QCOMPARE(testModel.index(4, 0).data().toInt(), 4);
    QCOMPARE(testModel.index(5, 0).data().toInt(), 5);
    QCOMPARE(testModel.headerData(0, Qt::Vertical).toInt(), 3);
    QCOMPARE(testModel.headerData(1, Qt::Vertical).toInt(), 1);
    QCOMPARE(testModel.headerData(2, Qt::Vertical).toInt(), 2);
    QCOMPARE(testModel.headerData(3, Qt::Vertical).toInt(), 4);
    QCOMPARE(testModel.headerData(4, Qt::Vertical).toInt(), 5);
    QCOMPARE(testModel.headerData(5, Qt::Vertical).toInt(), 6);
    QCOMPARE(twoIndex.data().toInt(), 2);
    QCOMPARE(twoIndex.row(), 0);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QVERIFY(!args.at(0).value<QModelIndex>().isValid());
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QVERIFY(!args.at(3).value<QModelIndex>().isValid());
        QCOMPARE(args.at(4).toInt(), 0);
    }

    resetData();
    twoIndex = testModel.index(2, 0);
    QVERIFY(testModel.moveRows(QModelIndex(), 2, 1, QModelIndex(), 6));
    QCOMPARE(testModel.index(0, 0).data().toInt(), 0);
    QCOMPARE(testModel.index(1, 0).data().toInt(), 1);
    QCOMPARE(testModel.index(2, 0).data().toInt(), 3);
    QCOMPARE(testModel.index(3, 0).data().toInt(), 4);
    QCOMPARE(testModel.index(4, 0).data().toInt(), 5);
    QCOMPARE(testModel.index(5, 0).data().toInt(), 2);
    QCOMPARE(testModel.headerData(0, Qt::Vertical).toInt(), 1);
    QCOMPARE(testModel.headerData(1, Qt::Vertical).toInt(), 2);
    QCOMPARE(testModel.headerData(2, Qt::Vertical).toInt(), 4);
    QCOMPARE(testModel.headerData(3, Qt::Vertical).toInt(), 5);
    QCOMPARE(testModel.headerData(4, Qt::Vertical).toInt(), 6);
    QCOMPARE(testModel.headerData(5, Qt::Vertical).toInt(), 3);
    QCOMPARE(twoIndex.data().toInt(), 2);
    QCOMPARE(twoIndex.row(), 5);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QVERIFY(!args.at(0).value<QModelIndex>().isValid());
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QVERIFY(!args.at(3).value<QModelIndex>().isValid());
        QCOMPARE(args.at(4).toInt(), 6);
    }

    resetData();
    QPersistentModelIndex oneIndex = testModel.index(1, 0);
    QVERIFY(testModel.moveRows(QModelIndex(), 0, 2, QModelIndex(), 3));
    QCOMPARE(testModel.index(0, 0).data().toInt(), 2);
    QCOMPARE(testModel.index(1, 0).data().toInt(), 0);
    QCOMPARE(testModel.index(2, 0).data().toInt(), 1);
    QCOMPARE(testModel.index(3, 0).data().toInt(), 3);
    QCOMPARE(testModel.index(4, 0).data().toInt(), 4);
    QCOMPARE(testModel.index(5, 0).data().toInt(), 5);
    QCOMPARE(testModel.headerData(0, Qt::Vertical).toInt(), 3);
    QCOMPARE(testModel.headerData(1, Qt::Vertical).toInt(), 1);
    QCOMPARE(testModel.headerData(2, Qt::Vertical).toInt(), 2);
    QCOMPARE(testModel.headerData(3, Qt::Vertical).toInt(), 4);
    QCOMPARE(testModel.headerData(4, Qt::Vertical).toInt(), 5);
    QCOMPARE(testModel.headerData(5, Qt::Vertical).toInt(), 6);
    QCOMPARE(oneIndex.data().toInt(), 1);
    QCOMPARE(oneIndex.row(), 2);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QVERIFY(!args.at(0).value<QModelIndex>().isValid());
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(2).toInt(), 1);
        QVERIFY(!args.at(3).value<QModelIndex>().isValid());
        QCOMPARE(args.at(4).toInt(), 3);
    }
}

void tst_GenericModel::moveRowsTable()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    testModel.insertColumns(0, 2);
    testModel.insertRows(0, 6);
    QSignalSpy rowsAboutToBeMovedSpy(&testModel, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsAboutToBeMovedSpy.isValid());
    QSignalSpy rowsMovedSpy(&testModel, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsMovedSpy.isValid());
    QSignalSpy *spyArr[] = {&rowsAboutToBeMovedSpy, &rowsMovedSpy};
    const auto resetData = [&testModel]() {
        for (int i = 0; i < 6; ++i) {
            testModel.setData(testModel.index(i, 0), i);
            testModel.setData(testModel.index(i, 1), QChar('a' + i));
        }
    };
    resetData();
    QVERIFY(!testModel.moveRows(QModelIndex(), 2, 1, QModelIndex(), -1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(QModelIndex(), 2, 1, QModelIndex(), 7));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(QModelIndex(), -1, 1, QModelIndex(), 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(QModelIndex(), 6, 1, QModelIndex(), 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(QModelIndex(), 1, 2, QModelIndex(), 2));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(QModelIndex(), 1, 2, QModelIndex(), 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);

    QPersistentModelIndex twoIndex = testModel.index(2, 1);
    QVERIFY(testModel.moveRows(QModelIndex(), 2, 1, QModelIndex(), 4));
    QCOMPARE(testModel.index(0, 0).data().toInt(), 0);
    QCOMPARE(testModel.index(1, 0).data().toInt(), 1);
    QCOMPARE(testModel.index(2, 0).data().toInt(), 3);
    QCOMPARE(testModel.index(3, 0).data().toInt(), 2);
    QCOMPARE(testModel.index(4, 0).data().toInt(), 4);
    QCOMPARE(testModel.index(5, 0).data().toInt(), 5);
    QCOMPARE(testModel.index(0, 1).data().value<QChar>(), QChar(QLatin1Char('a')));
    QCOMPARE(testModel.index(1, 1).data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(testModel.index(2, 1).data().value<QChar>(), QChar(QLatin1Char('d')));
    QCOMPARE(testModel.index(3, 1).data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(testModel.index(4, 1).data().value<QChar>(), QChar(QLatin1Char('e')));
    QCOMPARE(testModel.index(5, 1).data().value<QChar>(), QChar(QLatin1Char('f')));
    QCOMPARE(twoIndex.data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(twoIndex.row(), 3);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QVERIFY(!args.at(0).value<QModelIndex>().isValid());
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QVERIFY(!args.at(3).value<QModelIndex>().isValid());
        QCOMPARE(args.at(4).toInt(), 4);
    }

    resetData();
    twoIndex = testModel.index(2, 1);
    QVERIFY(testModel.moveRows(QModelIndex(), 2, 1, QModelIndex(), 0));
    QCOMPARE(testModel.index(0, 0).data().toInt(), 2);
    QCOMPARE(testModel.index(1, 0).data().toInt(), 0);
    QCOMPARE(testModel.index(2, 0).data().toInt(), 1);
    QCOMPARE(testModel.index(3, 0).data().toInt(), 3);
    QCOMPARE(testModel.index(4, 0).data().toInt(), 4);
    QCOMPARE(testModel.index(5, 0).data().toInt(), 5);
    QCOMPARE(testModel.index(0, 1).data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(testModel.index(1, 1).data().value<QChar>(), QChar(QLatin1Char('a')));
    QCOMPARE(testModel.index(2, 1).data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(testModel.index(3, 1).data().value<QChar>(), QChar(QLatin1Char('d')));
    QCOMPARE(testModel.index(4, 1).data().value<QChar>(), QChar(QLatin1Char('e')));
    QCOMPARE(testModel.index(5, 1).data().value<QChar>(), QChar(QLatin1Char('f')));
    QCOMPARE(twoIndex.data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(twoIndex.row(), 0);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QVERIFY(!args.at(0).value<QModelIndex>().isValid());
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QVERIFY(!args.at(3).value<QModelIndex>().isValid());
        QCOMPARE(args.at(4).toInt(), 0);
    }

    resetData();
    twoIndex = testModel.index(2, 1);
    QVERIFY(testModel.moveRows(QModelIndex(), 2, 1, QModelIndex(), 6));
    QCOMPARE(testModel.index(0, 0).data().toInt(), 0);
    QCOMPARE(testModel.index(1, 0).data().toInt(), 1);
    QCOMPARE(testModel.index(2, 0).data().toInt(), 3);
    QCOMPARE(testModel.index(3, 0).data().toInt(), 4);
    QCOMPARE(testModel.index(4, 0).data().toInt(), 5);
    QCOMPARE(testModel.index(5, 0).data().toInt(), 2);
    QCOMPARE(testModel.index(0, 1).data().value<QChar>(), QChar(QLatin1Char('a')));
    QCOMPARE(testModel.index(1, 1).data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(testModel.index(2, 1).data().value<QChar>(), QChar(QLatin1Char('d')));
    QCOMPARE(testModel.index(3, 1).data().value<QChar>(), QChar(QLatin1Char('e')));
    QCOMPARE(testModel.index(4, 1).data().value<QChar>(), QChar(QLatin1Char('f')));
    QCOMPARE(testModel.index(5, 1).data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(twoIndex.data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(twoIndex.row(), 5);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QVERIFY(!args.at(0).value<QModelIndex>().isValid());
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QVERIFY(!args.at(3).value<QModelIndex>().isValid());
        QCOMPARE(args.at(4).toInt(), 6);
    }

    resetData();
    QPersistentModelIndex oneIndex = testModel.index(1, 1);
    QVERIFY(testModel.moveRows(QModelIndex(), 0, 2, QModelIndex(), 3));
    QCOMPARE(testModel.index(0, 0).data().toInt(), 2);
    QCOMPARE(testModel.index(1, 0).data().toInt(), 0);
    QCOMPARE(testModel.index(2, 0).data().toInt(), 1);
    QCOMPARE(testModel.index(3, 0).data().toInt(), 3);
    QCOMPARE(testModel.index(4, 0).data().toInt(), 4);
    QCOMPARE(testModel.index(5, 0).data().toInt(), 5);
    QCOMPARE(testModel.index(0, 1).data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(testModel.index(1, 1).data().value<QChar>(), QChar(QLatin1Char('a')));
    QCOMPARE(testModel.index(2, 1).data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(testModel.index(3, 1).data().value<QChar>(), QChar(QLatin1Char('d')));
    QCOMPARE(testModel.index(4, 1).data().value<QChar>(), QChar(QLatin1Char('e')));
    QCOMPARE(testModel.index(5, 1).data().value<QChar>(), QChar(QLatin1Char('f')));
    QCOMPARE(oneIndex.data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(oneIndex.row(), 2);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QVERIFY(!args.at(0).value<QModelIndex>().isValid());
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(2).toInt(), 1);
        QVERIFY(!args.at(3).value<QModelIndex>().isValid());
        QCOMPARE(args.at(4).toInt(), 3);
    }
}

void tst_GenericModel::moveRowsTreeSameBranch()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    testModel.insertColumns(0, 2);
    testModel.insertRows(0, 3);
    const QModelIndex parIndex = testModel.index(1, 0);
    testModel.insertColumns(0, 2, parIndex);
    testModel.insertRows(0, 6, parIndex);
    QSignalSpy rowsAboutToBeMovedSpy(&testModel, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsAboutToBeMovedSpy.isValid());
    QSignalSpy rowsMovedSpy(&testModel, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsMovedSpy.isValid());
    QSignalSpy *spyArr[] = {&rowsAboutToBeMovedSpy, &rowsMovedSpy};
    const auto resetData = [&testModel, &parIndex]() {
        for (int i = 0; i < 6; ++i) {
            testModel.setData(testModel.index(i, 0, parIndex), i);
            testModel.setData(testModel.index(i, 1, parIndex), QChar('a' + i));
        }
    };

    resetData();
    QVERIFY(!testModel.moveRows(parIndex, 2, 1, parIndex, -1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, 2, 1, parIndex, 7));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, -1, 1, parIndex, 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, 6, 1, parIndex, 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, 1, 2, parIndex, 2));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, 1, 2, parIndex, 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);

    QPersistentModelIndex twoIndex = testModel.index(2, 1, parIndex);
    QVERIFY(testModel.moveRows(parIndex, 2, 1, parIndex, 4));
    QCOMPARE(testModel.index(0, 0, parIndex).data().toInt(), 0);
    QCOMPARE(testModel.index(1, 0, parIndex).data().toInt(), 1);
    QCOMPARE(testModel.index(2, 0, parIndex).data().toInt(), 3);
    QCOMPARE(testModel.index(3, 0, parIndex).data().toInt(), 2);
    QCOMPARE(testModel.index(4, 0, parIndex).data().toInt(), 4);
    QCOMPARE(testModel.index(5, 0, parIndex).data().toInt(), 5);
    QCOMPARE(testModel.index(0, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('a')));
    QCOMPARE(testModel.index(1, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(testModel.index(2, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('d')));
    QCOMPARE(testModel.index(3, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(testModel.index(4, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('e')));
    QCOMPARE(testModel.index(5, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('f')));
    QCOMPARE(twoIndex.data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(twoIndex.row(), 3);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(3).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(4).toInt(), 4);
    }

    resetData();
    twoIndex = testModel.index(2, 1, parIndex);
    QVERIFY(testModel.moveRows(parIndex, 2, 1, parIndex, 0));
    QCOMPARE(testModel.index(0, 0, parIndex).data().toInt(), 2);
    QCOMPARE(testModel.index(1, 0, parIndex).data().toInt(), 0);
    QCOMPARE(testModel.index(2, 0, parIndex).data().toInt(), 1);
    QCOMPARE(testModel.index(3, 0, parIndex).data().toInt(), 3);
    QCOMPARE(testModel.index(4, 0, parIndex).data().toInt(), 4);
    QCOMPARE(testModel.index(5, 0, parIndex).data().toInt(), 5);
    QCOMPARE(testModel.index(0, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(testModel.index(1, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('a')));
    QCOMPARE(testModel.index(2, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(testModel.index(3, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('d')));
    QCOMPARE(testModel.index(4, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('e')));
    QCOMPARE(testModel.index(5, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('f')));
    QCOMPARE(twoIndex.data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(twoIndex.row(), 0);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(3).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(4).toInt(), 0);
    }

    resetData();
    twoIndex = testModel.index(2, 1, parIndex);
    QVERIFY(testModel.moveRows(parIndex, 2, 1, parIndex, 6));
    QCOMPARE(testModel.index(0, 0, parIndex).data().toInt(), 0);
    QCOMPARE(testModel.index(1, 0, parIndex).data().toInt(), 1);
    QCOMPARE(testModel.index(2, 0, parIndex).data().toInt(), 3);
    QCOMPARE(testModel.index(3, 0, parIndex).data().toInt(), 4);
    QCOMPARE(testModel.index(4, 0, parIndex).data().toInt(), 5);
    QCOMPARE(testModel.index(5, 0, parIndex).data().toInt(), 2);
    QCOMPARE(testModel.index(0, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('a')));
    QCOMPARE(testModel.index(1, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(testModel.index(2, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('d')));
    QCOMPARE(testModel.index(3, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('e')));
    QCOMPARE(testModel.index(4, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('f')));
    QCOMPARE(testModel.index(5, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(twoIndex.data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(twoIndex.row(), 5);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(3).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(4).toInt(), 6);
    }

    resetData();
    QPersistentModelIndex oneIndex = testModel.index(1, 1, parIndex);
    QVERIFY(testModel.moveRows(parIndex, 0, 2, parIndex, 3));
    QCOMPARE(testModel.index(0, 0, parIndex).data().toInt(), 2);
    QCOMPARE(testModel.index(1, 0, parIndex).data().toInt(), 0);
    QCOMPARE(testModel.index(2, 0, parIndex).data().toInt(), 1);
    QCOMPARE(testModel.index(3, 0, parIndex).data().toInt(), 3);
    QCOMPARE(testModel.index(4, 0, parIndex).data().toInt(), 4);
    QCOMPARE(testModel.index(5, 0, parIndex).data().toInt(), 5);
    QCOMPARE(testModel.index(0, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(testModel.index(1, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('a')));
    QCOMPARE(testModel.index(2, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(testModel.index(3, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('d')));
    QCOMPARE(testModel.index(4, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('e')));
    QCOMPARE(testModel.index(5, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('f')));
    QCOMPARE(oneIndex.data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(oneIndex.row(), 2);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(2).toInt(), 1);
        QCOMPARE(args.at(3).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(4).toInt(), 3);
    }
}

void tst_GenericModel::moveRowsNewBranch()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    testModel.insertColumns(0, 2);
    testModel.insertRows(0, 3);
    const QModelIndex parIndex = testModel.index(1, 0);
    const QModelIndex destIndex = testModel.index(2, 0);
    testModel.insertColumns(0, 2, parIndex);
    testModel.insertRows(0, 6, parIndex);
    QSignalSpy rowsAboutToBeMovedSpy(&testModel, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsAboutToBeMovedSpy.isValid());
    QSignalSpy rowsMovedSpy(&testModel, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsMovedSpy.isValid());
    QSignalSpy *spyArr[] = {&rowsAboutToBeMovedSpy, &rowsMovedSpy};
    for (int i = 0; i < 6; ++i) {
        testModel.setData(testModel.index(i, 0, parIndex), i);
        testModel.setData(testModel.index(i, 1, parIndex), QChar('a' + i));
    }
    QVERIFY(!testModel.moveRows(parIndex, 2, 1, destIndex, -1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, 2, 1, destIndex, 1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, -1, 1, destIndex, 0));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, 6, 1, destIndex, 0));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);

    QPersistentModelIndex twoIndex = testModel.index(2, 1, parIndex);
    QVERIFY(testModel.moveRows(parIndex, 2, 1, destIndex, 0));
    QCOMPARE(testModel.rowCount(destIndex), 1);
    QCOMPARE(testModel.columnCount(destIndex), 2);
    QCOMPARE(testModel.rowCount(parIndex), 5);
    QCOMPARE(testModel.index(0, 0, destIndex).data().toInt(), 2);
    QCOMPARE(testModel.index(0, 0, parIndex).data().toInt(), 0);
    QCOMPARE(testModel.index(1, 0, parIndex).data().toInt(), 1);
    QCOMPARE(testModel.index(2, 0, parIndex).data().toInt(), 3);
    QCOMPARE(testModel.index(3, 0, parIndex).data().toInt(), 4);
    QCOMPARE(testModel.index(4, 0, parIndex).data().toInt(), 5);
    QCOMPARE(testModel.index(0, 1, destIndex).data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(testModel.index(0, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('a')));
    QCOMPARE(testModel.index(1, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(testModel.index(2, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('d')));
    QCOMPARE(testModel.index(3, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('e')));
    QCOMPARE(testModel.index(4, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('f')));
    QCOMPARE(twoIndex.data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(twoIndex.row(), 0);
    QCOMPARE(twoIndex.parent(), destIndex);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(3).value<QModelIndex>(), destIndex);
        QCOMPARE(args.at(4).toInt(), 0);
    }
}

void tst_GenericModel::moveRowsExistingBranch()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    testModel.insertColumns(0, 2);
    testModel.insertRows(0, 3);
    const QModelIndex parIndex = testModel.index(1, 0);
    const QModelIndex destIndex = testModel.index(2, 0);
    testModel.insertColumns(0, 2, parIndex);
    testModel.insertRows(0, 6, parIndex);
    testModel.insertColumns(0, 2, destIndex);
    testModel.insertRows(0, 6, destIndex);
    QSignalSpy rowsAboutToBeMovedSpy(&testModel, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsAboutToBeMovedSpy.isValid());
    QSignalSpy rowsMovedSpy(&testModel, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsMovedSpy.isValid());
    QSignalSpy *spyArr[] = {&rowsAboutToBeMovedSpy, &rowsMovedSpy};
    for (int i = 0; i < 6; ++i) {
        testModel.setData(testModel.index(i, 0, parIndex), i);
        testModel.setData(testModel.index(i, 1, parIndex), QChar('a' + i));
        testModel.setData(testModel.index(i, 0, destIndex), 10 + i);
        testModel.setData(testModel.index(i, 1, destIndex), QChar('a' + i + 10));
    }

    QVERIFY(!testModel.moveRows(parIndex, 2, 1, destIndex, -1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, 2, 1, destIndex, 7));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, -1, 1, destIndex, 1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, 6, 1, destIndex, 1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);

    QPersistentModelIndex twoIndex = testModel.index(2, 1, parIndex);
    QVERIFY(testModel.moveRows(parIndex, 2, 2, destIndex, 1));
    QCOMPARE(testModel.rowCount(destIndex), 8);
    QCOMPARE(testModel.columnCount(destIndex), 2);
    QCOMPARE(testModel.rowCount(parIndex), 4);
    QCOMPARE(testModel.index(0, 0, parIndex).data().toInt(), 0);
    QCOMPARE(testModel.index(1, 0, parIndex).data().toInt(), 1);
    QCOMPARE(testModel.index(2, 0, parIndex).data().toInt(), 4);
    QCOMPARE(testModel.index(3, 0, parIndex).data().toInt(), 5);
    QCOMPARE(testModel.index(0, 0, destIndex).data().toInt(), 10);
    QCOMPARE(testModel.index(1, 0, destIndex).data().toInt(), 2);
    QCOMPARE(testModel.index(2, 0, destIndex).data().toInt(), 3);
    QCOMPARE(testModel.index(3, 0, destIndex).data().toInt(), 11);
    QCOMPARE(testModel.index(4, 0, destIndex).data().toInt(), 12);
    QCOMPARE(testModel.index(5, 0, destIndex).data().toInt(), 13);
    QCOMPARE(testModel.index(6, 0, destIndex).data().toInt(), 14);
    QCOMPARE(testModel.index(7, 0, destIndex).data().toInt(), 15);
    QCOMPARE(testModel.index(0, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('a')));
    QCOMPARE(testModel.index(1, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('b')));
    QCOMPARE(testModel.index(2, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('e')));
    QCOMPARE(testModel.index(3, 1, parIndex).data().value<QChar>(), QChar(QLatin1Char('f')));
    QCOMPARE(testModel.index(0, 1, destIndex).data().value<QChar>(), QChar('a' + 10));
    QCOMPARE(testModel.index(1, 1, destIndex).data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(testModel.index(2, 1, destIndex).data().value<QChar>(), QChar(QLatin1Char('d')));
    QCOMPARE(testModel.index(3, 1, destIndex).data().value<QChar>(), QChar('b' + 10));
    QCOMPARE(testModel.index(4, 1, destIndex).data().value<QChar>(), QChar('c' + 10));
    QCOMPARE(testModel.index(5, 1, destIndex).data().value<QChar>(), QChar('d' + 10));
    QCOMPARE(testModel.index(6, 1, destIndex).data().value<QChar>(), QChar('e' + 10));
    QCOMPARE(testModel.index(7, 1, destIndex).data().value<QChar>(), QChar('f' + 10));

    QCOMPARE(twoIndex.data().value<QChar>(), QChar(QLatin1Char('c')));
    QCOMPARE(twoIndex.row(), 1);
    QCOMPARE(twoIndex.parent(), destIndex);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(3).value<QModelIndex>(), destIndex);
        QCOMPARE(args.at(4).toInt(), 1);
    }
}

void tst_GenericModel::moveRowsFromRootToBranch()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    fillTable(&testModel, 5, 3);
    const QModelIndex parIndex = testModel.index(1, 0);
    testModel.insertColumns(0, 4, parIndex);
    testModel.insertRow(0, parIndex);

    QVERIFY(testModel.moveRows(QModelIndex(), 2, 2, parIndex, 1));
    QCOMPARE(testModel.rowCount(parIndex), 3);
    QCOMPARE(testModel.columnCount(parIndex), 4);
    QCOMPARE(testModel.rowCount(), 3);
    QCOMPARE(testModel.columnCount(), 3);

    QCOMPARE(testModel.index(1, 0, parIndex).data().toString(), QStringLiteral("2,0"));
    QCOMPARE(testModel.index(2, 0, parIndex).data().toString(), QStringLiteral("3,0"));
    QCOMPARE(testModel.index(1, 2, parIndex).data().toString(), QStringLiteral("2,2"));
    QCOMPARE(testModel.index(2, 2, parIndex).data().toString(), QStringLiteral("3,2"));
    QVERIFY(testModel.index(1, 3, parIndex).isValid());
    QVERIFY(testModel.index(2, 3, parIndex).isValid());
    QVERIFY(!testModel.index(1, 3, parIndex).data().isValid());
    QVERIFY(!testModel.index(2, 3, parIndex).data().isValid());
    QCOMPARE(testModel.index(0, 0).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(1, 0).data().toString(), QStringLiteral("1,0"));
    QCOMPARE(testModel.index(2, 0).data().toString(), QStringLiteral("4,0"));
    QCOMPARE(testModel.headerData(0, Qt::Vertical).toInt(), 0);
    QCOMPARE(testModel.headerData(1, Qt::Vertical).toInt(), 1);
    QCOMPARE(testModel.headerData(2, Qt::Vertical).toInt(), 4);
}

void tst_GenericModel::moveRowsFromBranchToRoot()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    fillTable(&testModel, 5, 3);
    const QModelIndex parIndex = testModel.index(1, 0);
    testModel.insertColumns(0, 4, parIndex);
    testModel.insertRows(0, 5, parIndex);
    for (int r = 0; r < testModel.rowCount(parIndex); ++r) {
        for (int c = 0; c < testModel.columnCount(parIndex); ++c)
            testModel.setData(testModel.index(r, c, parIndex), QStringLiteral("r%1,c%2").arg(r).arg(c));
    }

    QVERIFY(testModel.moveRows(parIndex, 1, 2, QModelIndex(), 0));
    QCOMPARE(testModel.rowCount(parIndex), 3);
    QCOMPARE(testModel.columnCount(parIndex), 4);
    QCOMPARE(testModel.rowCount(), 7);
    QCOMPARE(testModel.columnCount(), 4);
    QCOMPARE(testModel.index(0, 0).data().toString(), QStringLiteral("r1,c0"));
    QCOMPARE(testModel.index(1, 0).data().toString(), QStringLiteral("r2,c0"));
    QCOMPARE(testModel.index(0, 3).data().toString(), QStringLiteral("r1,c3"));
    QCOMPARE(testModel.index(1, 3).data().toString(), QStringLiteral("r2,c3"));
    QCOMPARE(testModel.index(2, 0).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(3, 0).data().toString(), QStringLiteral("1,0"));
    QCOMPARE(testModel.index(4, 0).data().toString(), QStringLiteral("2,0"));
    QCOMPARE(testModel.index(5, 0).data().toString(), QStringLiteral("3,0"));
    QCOMPARE(testModel.index(6, 0).data().toString(), QStringLiteral("4,0"));
    for (int i = 2; i < 7; ++i)
        QCOMPARE(testModel.headerData(i, Qt::Vertical).toInt(), i - 2);
    QCOMPARE(testModel.index(0, 0, parIndex).data().toString(), QStringLiteral("r0,c0"));
    QCOMPARE(testModel.index(1, 0, parIndex).data().toString(), QStringLiteral("r3,c0"));
    QCOMPARE(testModel.index(2, 0, parIndex).data().toString(), QStringLiteral("r4,c0"));
}

void tst_GenericModel::moveColumnsTable()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    fillTable(&testModel, 2, 6);

    QSignalSpy rowsAboutToBeMovedSpy(&testModel, SIGNAL(columnsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsAboutToBeMovedSpy.isValid());
    QSignalSpy columnsMovedSpy(&testModel, SIGNAL(columnsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(columnsMovedSpy.isValid());
    QSignalSpy *spyArr[] = {&rowsAboutToBeMovedSpy, &columnsMovedSpy};

    QVERIFY(!testModel.moveColumns(QModelIndex(), 2, 1, QModelIndex(), -1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveColumns(QModelIndex(), 2, 1, QModelIndex(), 7));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveColumns(QModelIndex(), -1, 1, QModelIndex(), 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveColumns(QModelIndex(), 6, 1, QModelIndex(), 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveColumns(QModelIndex(), 1, 2, QModelIndex(), 2));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveColumns(QModelIndex(), 1, 2, QModelIndex(), 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);

    QPersistentModelIndex oneTwoIndex = testModel.index(1, 2);
    QVERIFY(testModel.moveColumns(QModelIndex(), 2, 1, QModelIndex(), 4));
    QCOMPARE(testModel.index(0, 0).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(0, 1).data().toString(), QStringLiteral("0,1"));
    QCOMPARE(testModel.index(0, 2).data().toString(), QStringLiteral("0,3"));
    QCOMPARE(testModel.index(0, 3).data().toString(), QStringLiteral("0,2"));
    QCOMPARE(testModel.index(0, 4).data().toString(), QStringLiteral("0,4"));
    QCOMPARE(testModel.index(0, 5).data().toString(), QStringLiteral("0,5"));
    QCOMPARE(testModel.index(1, 0).data().toString(), QStringLiteral("1,0"));
    QCOMPARE(testModel.index(1, 1).data().toString(), QStringLiteral("1,1"));
    QCOMPARE(testModel.index(1, 2).data().toString(), QStringLiteral("1,3"));
    QCOMPARE(testModel.index(1, 3).data().toString(), QStringLiteral("1,2"));
    QCOMPARE(testModel.index(1, 4).data().toString(), QStringLiteral("1,4"));
    QCOMPARE(testModel.index(1, 5).data().toString(), QStringLiteral("1,5"));
    QCOMPARE(testModel.headerData(0, Qt::Horizontal).toInt(), 0);
    QCOMPARE(testModel.headerData(1, Qt::Horizontal).toInt(), 1);
    QCOMPARE(testModel.headerData(2, Qt::Horizontal).toInt(), 3);
    QCOMPARE(testModel.headerData(3, Qt::Horizontal).toInt(), 2);
    QCOMPARE(testModel.headerData(4, Qt::Horizontal).toInt(), 4);
    QCOMPARE(testModel.headerData(5, Qt::Horizontal).toInt(), 5);
    QCOMPARE(oneTwoIndex.data().toString(), QStringLiteral("1,2"));
    QCOMPARE(oneTwoIndex.column(), 3);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QVERIFY(!args.at(0).value<QModelIndex>().isValid());
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QVERIFY(!args.at(3).value<QModelIndex>().isValid());
        QCOMPARE(args.at(4).toInt(), 4);
    }

    fillTable(&testModel, 2, 6);
    oneTwoIndex = testModel.index(1, 2);
    QVERIFY(testModel.moveColumns(QModelIndex(), 2, 1, QModelIndex(), 0));
    QCOMPARE(testModel.index(0, 0).data().toString(), QStringLiteral("0,2"));
    QCOMPARE(testModel.index(0, 1).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(0, 2).data().toString(), QStringLiteral("0,1"));
    QCOMPARE(testModel.index(0, 3).data().toString(), QStringLiteral("0,3"));
    QCOMPARE(testModel.index(0, 4).data().toString(), QStringLiteral("0,4"));
    QCOMPARE(testModel.index(0, 5).data().toString(), QStringLiteral("0,5"));
    QCOMPARE(testModel.index(1, 0).data().toString(), QStringLiteral("1,2"));
    QCOMPARE(testModel.index(1, 1).data().toString(), QStringLiteral("1,0"));
    QCOMPARE(testModel.index(1, 2).data().toString(), QStringLiteral("1,1"));
    QCOMPARE(testModel.index(1, 3).data().toString(), QStringLiteral("1,3"));
    QCOMPARE(testModel.index(1, 4).data().toString(), QStringLiteral("1,4"));
    QCOMPARE(testModel.index(1, 5).data().toString(), QStringLiteral("1,5"));
    QCOMPARE(testModel.headerData(0, Qt::Horizontal).toInt(), 2);
    QCOMPARE(testModel.headerData(1, Qt::Horizontal).toInt(), 0);
    QCOMPARE(testModel.headerData(2, Qt::Horizontal).toInt(), 1);
    QCOMPARE(testModel.headerData(3, Qt::Horizontal).toInt(), 3);
    QCOMPARE(testModel.headerData(4, Qt::Horizontal).toInt(), 4);
    QCOMPARE(testModel.headerData(5, Qt::Horizontal).toInt(), 5);
    QCOMPARE(oneTwoIndex.data().toString(), QStringLiteral("1,2"));
    QCOMPARE(oneTwoIndex.column(), 0);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QVERIFY(!args.at(0).value<QModelIndex>().isValid());
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QVERIFY(!args.at(3).value<QModelIndex>().isValid());
        QCOMPARE(args.at(4).toInt(), 0);
    }

    fillTable(&testModel, 2, 6);
    oneTwoIndex = testModel.index(1, 2);
    QVERIFY(testModel.moveColumns(QModelIndex(), 2, 1, QModelIndex(), 6));
    QCOMPARE(testModel.index(0, 0).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(0, 1).data().toString(), QStringLiteral("0,1"));
    QCOMPARE(testModel.index(0, 2).data().toString(), QStringLiteral("0,3"));
    QCOMPARE(testModel.index(0, 3).data().toString(), QStringLiteral("0,4"));
    QCOMPARE(testModel.index(0, 4).data().toString(), QStringLiteral("0,5"));
    QCOMPARE(testModel.index(0, 5).data().toString(), QStringLiteral("0,2"));
    QCOMPARE(testModel.index(1, 0).data().toString(), QStringLiteral("1,0"));
    QCOMPARE(testModel.index(1, 1).data().toString(), QStringLiteral("1,1"));
    QCOMPARE(testModel.index(1, 2).data().toString(), QStringLiteral("1,3"));
    QCOMPARE(testModel.index(1, 3).data().toString(), QStringLiteral("1,4"));
    QCOMPARE(testModel.index(1, 4).data().toString(), QStringLiteral("1,5"));
    QCOMPARE(testModel.index(1, 5).data().toString(), QStringLiteral("1,2"));
    QCOMPARE(testModel.headerData(0, Qt::Horizontal).toInt(), 0);
    QCOMPARE(testModel.headerData(1, Qt::Horizontal).toInt(), 1);
    QCOMPARE(testModel.headerData(2, Qt::Horizontal).toInt(), 3);
    QCOMPARE(testModel.headerData(3, Qt::Horizontal).toInt(), 4);
    QCOMPARE(testModel.headerData(4, Qt::Horizontal).toInt(), 5);
    QCOMPARE(testModel.headerData(5, Qt::Horizontal).toInt(), 2);
    QCOMPARE(oneTwoIndex.data().toString(), QStringLiteral("1,2"));
    QCOMPARE(oneTwoIndex.column(), 5);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QVERIFY(!args.at(0).value<QModelIndex>().isValid());
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QVERIFY(!args.at(3).value<QModelIndex>().isValid());
        QCOMPARE(args.at(4).toInt(), 6);
    }

    fillTable(&testModel, 2, 6);
    QPersistentModelIndex oneOneIndex = testModel.index(1, 1);
    QVERIFY(testModel.moveColumns(QModelIndex(), 0, 2, QModelIndex(), 3));
    QCOMPARE(testModel.index(0, 0).data().toString(), QStringLiteral("0,2"));
    QCOMPARE(testModel.index(0, 1).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(0, 2).data().toString(), QStringLiteral("0,1"));
    QCOMPARE(testModel.index(0, 3).data().toString(), QStringLiteral("0,3"));
    QCOMPARE(testModel.index(0, 4).data().toString(), QStringLiteral("0,4"));
    QCOMPARE(testModel.index(0, 5).data().toString(), QStringLiteral("0,5"));
    QCOMPARE(testModel.index(1, 0).data().toString(), QStringLiteral("1,2"));
    QCOMPARE(testModel.index(1, 1).data().toString(), QStringLiteral("1,0"));
    QCOMPARE(testModel.index(1, 2).data().toString(), QStringLiteral("1,1"));
    QCOMPARE(testModel.index(1, 3).data().toString(), QStringLiteral("1,3"));
    QCOMPARE(testModel.index(1, 4).data().toString(), QStringLiteral("1,4"));
    QCOMPARE(testModel.index(1, 5).data().toString(), QStringLiteral("1,5"));
    QCOMPARE(testModel.headerData(0, Qt::Horizontal).toInt(), 2);
    QCOMPARE(testModel.headerData(1, Qt::Horizontal).toInt(), 0);
    QCOMPARE(testModel.headerData(2, Qt::Horizontal).toInt(), 1);
    QCOMPARE(testModel.headerData(3, Qt::Horizontal).toInt(), 3);
    QCOMPARE(testModel.headerData(4, Qt::Horizontal).toInt(), 4);
    QCOMPARE(testModel.headerData(5, Qt::Horizontal).toInt(), 5);
    QCOMPARE(oneOneIndex.data().toString(), QStringLiteral("1,1"));
    QCOMPARE(oneOneIndex.column(), 2);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QVERIFY(!args.at(0).value<QModelIndex>().isValid());
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(2).toInt(), 1);
        QVERIFY(!args.at(3).value<QModelIndex>().isValid());
        QCOMPARE(args.at(4).toInt(), 3);
    }
}

void tst_GenericModel::moveColumnsTreeSameBranch()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    fillTable(&testModel, 2, 6);
    const QModelIndex parIndex = testModel.index(1, 0);
    fillTable(&testModel, 2, 6, parIndex);
    QSignalSpy rowsAboutToBeMovedSpy(&testModel, SIGNAL(columnsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsAboutToBeMovedSpy.isValid());
    QSignalSpy columnsMovedSpy(&testModel, SIGNAL(columnsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(columnsMovedSpy.isValid());
    QSignalSpy *spyArr[] = {&rowsAboutToBeMovedSpy, &columnsMovedSpy};

    QVERIFY(!testModel.moveColumns(parIndex, 2, 1, parIndex, -1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveColumns(parIndex, 2, 1, parIndex, 7));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveColumns(parIndex, -1, 1, parIndex, 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveColumns(parIndex, 6, 1, parIndex, 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveColumns(parIndex, 1, 2, parIndex, 2));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveColumns(parIndex, 1, 2, parIndex, 3));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);

    QPersistentModelIndex oneTwoIndex = testModel.index(1, 2, parIndex);
    QVERIFY(testModel.moveColumns(parIndex, 2, 1, parIndex, 4));
    QCOMPARE(testModel.index(0, 0, parIndex).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(0, 1, parIndex).data().toString(), QStringLiteral("0,1"));
    QCOMPARE(testModel.index(0, 2, parIndex).data().toString(), QStringLiteral("0,3"));
    QCOMPARE(testModel.index(0, 3, parIndex).data().toString(), QStringLiteral("0,2"));
    QCOMPARE(testModel.index(0, 4, parIndex).data().toString(), QStringLiteral("0,4"));
    QCOMPARE(testModel.index(0, 5, parIndex).data().toString(), QStringLiteral("0,5"));
    QCOMPARE(testModel.index(1, 0, parIndex).data().toString(), QStringLiteral("1,0"));
    QCOMPARE(testModel.index(1, 1, parIndex).data().toString(), QStringLiteral("1,1"));
    QCOMPARE(testModel.index(1, 2, parIndex).data().toString(), QStringLiteral("1,3"));
    QCOMPARE(testModel.index(1, 3, parIndex).data().toString(), QStringLiteral("1,2"));
    QCOMPARE(testModel.index(1, 4, parIndex).data().toString(), QStringLiteral("1,4"));
    QCOMPARE(testModel.index(1, 5, parIndex).data().toString(), QStringLiteral("1,5"));
    QCOMPARE(oneTwoIndex.data().toString(), QStringLiteral("1,2"));
    QCOMPARE(oneTwoIndex.column(), 3);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(3).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(4).toInt(), 4);
    }

    fillTable(&testModel, 2, 6, parIndex);
    oneTwoIndex = testModel.index(1, 2, parIndex);
    QVERIFY(testModel.moveColumns(parIndex, 2, 1, parIndex, 0));
    QCOMPARE(testModel.index(0, 0, parIndex).data().toString(), QStringLiteral("0,2"));
    QCOMPARE(testModel.index(0, 1, parIndex).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(0, 2, parIndex).data().toString(), QStringLiteral("0,1"));
    QCOMPARE(testModel.index(0, 3, parIndex).data().toString(), QStringLiteral("0,3"));
    QCOMPARE(testModel.index(0, 4, parIndex).data().toString(), QStringLiteral("0,4"));
    QCOMPARE(testModel.index(0, 5, parIndex).data().toString(), QStringLiteral("0,5"));
    QCOMPARE(testModel.index(1, 0, parIndex).data().toString(), QStringLiteral("1,2"));
    QCOMPARE(testModel.index(1, 1, parIndex).data().toString(), QStringLiteral("1,0"));
    QCOMPARE(testModel.index(1, 2, parIndex).data().toString(), QStringLiteral("1,1"));
    QCOMPARE(testModel.index(1, 3, parIndex).data().toString(), QStringLiteral("1,3"));
    QCOMPARE(testModel.index(1, 4, parIndex).data().toString(), QStringLiteral("1,4"));
    QCOMPARE(testModel.index(1, 5, parIndex).data().toString(), QStringLiteral("1,5"));
    QCOMPARE(oneTwoIndex.data().toString(), QStringLiteral("1,2"));
    QCOMPARE(oneTwoIndex.column(), 0);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(3).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(4).toInt(), 0);
    }

    fillTable(&testModel, 2, 6, parIndex);
    oneTwoIndex = testModel.index(1, 2, parIndex);
    QVERIFY(testModel.moveColumns(parIndex, 2, 1, parIndex, 6));
    QCOMPARE(testModel.index(0, 0, parIndex).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(0, 1, parIndex).data().toString(), QStringLiteral("0,1"));
    QCOMPARE(testModel.index(0, 2, parIndex).data().toString(), QStringLiteral("0,3"));
    QCOMPARE(testModel.index(0, 3, parIndex).data().toString(), QStringLiteral("0,4"));
    QCOMPARE(testModel.index(0, 4, parIndex).data().toString(), QStringLiteral("0,5"));
    QCOMPARE(testModel.index(0, 5, parIndex).data().toString(), QStringLiteral("0,2"));
    QCOMPARE(testModel.index(1, 0, parIndex).data().toString(), QStringLiteral("1,0"));
    QCOMPARE(testModel.index(1, 1, parIndex).data().toString(), QStringLiteral("1,1"));
    QCOMPARE(testModel.index(1, 2, parIndex).data().toString(), QStringLiteral("1,3"));
    QCOMPARE(testModel.index(1, 3, parIndex).data().toString(), QStringLiteral("1,4"));
    QCOMPARE(testModel.index(1, 4, parIndex).data().toString(), QStringLiteral("1,5"));
    QCOMPARE(testModel.index(1, 5, parIndex).data().toString(), QStringLiteral("1,2"));
    QCOMPARE(oneTwoIndex.data().toString(), QStringLiteral("1,2"));
    QCOMPARE(oneTwoIndex.column(), 5);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(3).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(4).toInt(), 6);
    }

    fillTable(&testModel, 2, 6, parIndex);
    QPersistentModelIndex oneOneIndex = testModel.index(1, 1, parIndex);
    QVERIFY(testModel.moveColumns(parIndex, 0, 2, parIndex, 3));
    QCOMPARE(testModel.index(0, 0, parIndex).data().toString(), QStringLiteral("0,2"));
    QCOMPARE(testModel.index(0, 1, parIndex).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(0, 2, parIndex).data().toString(), QStringLiteral("0,1"));
    QCOMPARE(testModel.index(0, 3, parIndex).data().toString(), QStringLiteral("0,3"));
    QCOMPARE(testModel.index(0, 4, parIndex).data().toString(), QStringLiteral("0,4"));
    QCOMPARE(testModel.index(0, 5, parIndex).data().toString(), QStringLiteral("0,5"));
    QCOMPARE(testModel.index(1, 0, parIndex).data().toString(), QStringLiteral("1,2"));
    QCOMPARE(testModel.index(1, 1, parIndex).data().toString(), QStringLiteral("1,0"));
    QCOMPARE(testModel.index(1, 2, parIndex).data().toString(), QStringLiteral("1,1"));
    QCOMPARE(testModel.index(1, 3, parIndex).data().toString(), QStringLiteral("1,3"));
    QCOMPARE(testModel.index(1, 4, parIndex).data().toString(), QStringLiteral("1,4"));
    QCOMPARE(testModel.index(1, 5, parIndex).data().toString(), QStringLiteral("1,5"));
    QCOMPARE(oneOneIndex.data().toString(), QStringLiteral("1,1"));
    QCOMPARE(oneOneIndex.column(), 2);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(2).toInt(), 1);
        QCOMPARE(args.at(3).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(4).toInt(), 3);
    }
}

void tst_GenericModel::moveColumnsNewBranch()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    fillTable(&testModel, 5, 5);
    const QModelIndex parIndex = testModel.index(1, 0);
    const QModelIndex destIndex = testModel.index(2, 0);
    fillTable(&testModel, 2, 6, parIndex);

    QSignalSpy columnsAboutToBeMovedSpy(&testModel, SIGNAL(columnsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(columnsAboutToBeMovedSpy.isValid());
    QSignalSpy columnsMovedSpy(&testModel, SIGNAL(columnsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(columnsMovedSpy.isValid());
    QSignalSpy *spyArr[] = {&columnsAboutToBeMovedSpy, &columnsMovedSpy};

    QVERIFY(!testModel.moveColumns(parIndex, 2, 1, destIndex, -1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveColumns(parIndex, 2, 1, destIndex, 1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveColumns(parIndex, -1, 1, destIndex, 0));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveColumns(parIndex, 6, 1, destIndex, 0));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);

    QPersistentModelIndex oneTwoIndex = testModel.index(1, 2, parIndex);
    QCOMPARE(oneTwoIndex.data().toString(), QStringLiteral("1,2"));
    QVERIFY(testModel.moveColumns(parIndex, 2, 1, destIndex, 0));
    QCOMPARE(testModel.columnCount(destIndex), 1);
    QCOMPARE(testModel.rowCount(destIndex), 2);
    QCOMPARE(testModel.columnCount(parIndex), 5);
    QCOMPARE(testModel.index(0, 0, destIndex).data().toString(), QStringLiteral("0,2"));
    QCOMPARE(testModel.index(1, 0, destIndex).data().toString(), QStringLiteral("1,2"));
    QCOMPARE(testModel.index(0, 0, parIndex).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(0, 1, parIndex).data().toString(), QStringLiteral("0,1"));
    QCOMPARE(testModel.index(0, 2, parIndex).data().toString(), QStringLiteral("0,3"));
    QCOMPARE(testModel.index(0, 3, parIndex).data().toString(), QStringLiteral("0,4"));
    QCOMPARE(testModel.index(0, 4, parIndex).data().toString(), QStringLiteral("0,5"));
    QCOMPARE(testModel.index(1, 0, parIndex).data().toString(), QStringLiteral("1,0"));
    QCOMPARE(testModel.index(1, 1, parIndex).data().toString(), QStringLiteral("1,1"));
    QCOMPARE(testModel.index(1, 2, parIndex).data().toString(), QStringLiteral("1,3"));
    QCOMPARE(testModel.index(1, 3, parIndex).data().toString(), QStringLiteral("1,4"));
    QCOMPARE(testModel.index(1, 4, parIndex).data().toString(), QStringLiteral("1,5"));
    QCOMPARE(oneTwoIndex.data().toString(), QStringLiteral("1,2"));
    QCOMPARE(oneTwoIndex.column(), 0);
    QCOMPARE(oneTwoIndex.parent(), destIndex);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 2);
        QCOMPARE(args.at(3).value<QModelIndex>(), destIndex);
        QCOMPARE(args.at(4).toInt(), 0);
    }
}

void tst_GenericModel::moveColumnsExistingBranch()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    fillTable(&testModel, 4, 4);
    const QModelIndex parIndex = testModel.index(1, 0);
    const QModelIndex destIndex = testModel.index(2, 0);
    fillTable(&testModel, 2, 6, parIndex);
    fillTable(&testModel, 2, 6, destIndex, 10);
    QSignalSpy columnsAboutToBeMovedSpy(&testModel, SIGNAL(columnsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(columnsAboutToBeMovedSpy.isValid());
    QSignalSpy columnsMovedSpy(&testModel, SIGNAL(columnsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(columnsMovedSpy.isValid());
    QSignalSpy *spyArr[] = {&columnsAboutToBeMovedSpy, &columnsMovedSpy};

    QVERIFY(!testModel.moveRows(parIndex, 2, 1, destIndex, -1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, 2, 1, destIndex, 7));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, -1, 1, destIndex, 1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(!testModel.moveRows(parIndex, 6, 1, destIndex, 1));
    for (QSignalSpy *spy : spyArr)
        QCOMPARE(spy->count(), 0);

    QPersistentModelIndex oneTwoIndex = testModel.index(1, 2, parIndex);
    QVERIFY(testModel.moveColumns(parIndex, 2, 2, destIndex, 1));
    QCOMPARE(testModel.columnCount(destIndex), 8);
    QCOMPARE(testModel.rowCount(destIndex), 2);
    QCOMPARE(testModel.columnCount(parIndex), 4);
    QCOMPARE(testModel.index(0, 0, parIndex).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(0, 1, parIndex).data().toString(), QStringLiteral("0,1"));
    QCOMPARE(testModel.index(0, 2, parIndex).data().toString(), QStringLiteral("0,4"));
    QCOMPARE(testModel.index(0, 3, parIndex).data().toString(), QStringLiteral("0,5"));
    QCOMPARE(testModel.index(1, 0, parIndex).data().toString(), QStringLiteral("1,0"));
    QCOMPARE(testModel.index(1, 1, parIndex).data().toString(), QStringLiteral("1,1"));
    QCOMPARE(testModel.index(1, 2, parIndex).data().toString(), QStringLiteral("1,4"));
    QCOMPARE(testModel.index(1, 3, parIndex).data().toString(), QStringLiteral("1,5"));

    QCOMPARE(testModel.index(0, 0, destIndex).data().toString(), QStringLiteral("10,10"));
    QCOMPARE(testModel.index(0, 1, destIndex).data().toString(), QStringLiteral("0,2"));
    QCOMPARE(testModel.index(0, 2, destIndex).data().toString(), QStringLiteral("0,3"));
    QCOMPARE(testModel.index(0, 3, destIndex).data().toString(), QStringLiteral("10,11"));
    QCOMPARE(testModel.index(0, 4, destIndex).data().toString(), QStringLiteral("10,12"));
    QCOMPARE(testModel.index(0, 5, destIndex).data().toString(), QStringLiteral("10,13"));
    QCOMPARE(testModel.index(0, 6, destIndex).data().toString(), QStringLiteral("10,14"));
    QCOMPARE(testModel.index(0, 7, destIndex).data().toString(), QStringLiteral("10,15"));
    QCOMPARE(testModel.index(1, 0, destIndex).data().toString(), QStringLiteral("11,10"));
    QCOMPARE(testModel.index(1, 1, destIndex).data().toString(), QStringLiteral("1,2"));
    QCOMPARE(testModel.index(1, 2, destIndex).data().toString(), QStringLiteral("1,3"));
    QCOMPARE(testModel.index(1, 3, destIndex).data().toString(), QStringLiteral("11,11"));
    QCOMPARE(testModel.index(1, 4, destIndex).data().toString(), QStringLiteral("11,12"));
    QCOMPARE(testModel.index(1, 5, destIndex).data().toString(), QStringLiteral("11,13"));
    QCOMPARE(testModel.index(1, 6, destIndex).data().toString(), QStringLiteral("11,14"));
    QCOMPARE(testModel.index(1, 7, destIndex).data().toString(), QStringLiteral("11,15"));
    QCOMPARE(oneTwoIndex.data().toString(), QStringLiteral("1,2"));
    QCOMPARE(oneTwoIndex.column(), 1);
    QCOMPARE(oneTwoIndex.parent(), destIndex);
    for (QSignalSpy *spy : spyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), parIndex);
        QCOMPARE(args.at(1).toInt(), 2);
        QCOMPARE(args.at(2).toInt(), 3);
        QCOMPARE(args.at(3).value<QModelIndex>(), destIndex);
        QCOMPARE(args.at(4).toInt(), 1);
    }
}

void tst_GenericModel::moveColumnsFromRootToBranch()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    fillTable(&testModel, 2, 5);
    const QModelIndex parIndex = testModel.index(1, 0);
    fillTable(&testModel, 4, 2, parIndex, 10);

    QVERIFY(testModel.moveColumns(QModelIndex(), 2, 2, parIndex, 1));
    QCOMPARE(testModel.columnCount(parIndex), 4);
    QCOMPARE(testModel.rowCount(parIndex), 4);
    QCOMPARE(testModel.columnCount(), 3);
    QCOMPARE(testModel.rowCount(), 2);

    QCOMPARE(testModel.index(0, 0).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(0, 1).data().toString(), QStringLiteral("0,1"));
    QCOMPARE(testModel.index(0, 2).data().toString(), QStringLiteral("0,4"));
    QCOMPARE(testModel.index(1, 0).data().toString(), QStringLiteral("1,0"));
    QCOMPARE(testModel.index(1, 1).data().toString(), QStringLiteral("1,1"));
    QCOMPARE(testModel.index(1, 2).data().toString(), QStringLiteral("1,4"));

    QCOMPARE(testModel.index(0, 0, parIndex).data().toString(), QStringLiteral("10,10"));
    QCOMPARE(testModel.index(0, 1, parIndex).data().toString(), QStringLiteral("0,2"));
    QCOMPARE(testModel.index(0, 2, parIndex).data().toString(), QStringLiteral("0,3"));
    QCOMPARE(testModel.index(0, 3, parIndex).data().toString(), QStringLiteral("10,11"));
    QCOMPARE(testModel.index(1, 0, parIndex).data().toString(), QStringLiteral("11,10"));
    QCOMPARE(testModel.index(1, 1, parIndex).data().toString(), QStringLiteral("1,2"));
    QCOMPARE(testModel.index(1, 2, parIndex).data().toString(), QStringLiteral("1,3"));
    QCOMPARE(testModel.index(1, 3, parIndex).data().toString(), QStringLiteral("11,11"));
    QVERIFY(testModel.index(2, 1, parIndex).isValid());
    QVERIFY(!testModel.index(2, 1, parIndex).data().isValid());
    QVERIFY(testModel.index(3, 1, parIndex).isValid());
    QVERIFY(!testModel.index(3, 1, parIndex).data().isValid());
    QCOMPARE(testModel.headerData(0, Qt::Horizontal).toInt(), 0);
    QCOMPARE(testModel.headerData(1, Qt::Horizontal).toInt(), 1);
    QCOMPARE(testModel.headerData(2, Qt::Horizontal).toInt(), 4);
}

void tst_GenericModel::moveColumnsFromBranchToRoot()
{
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    fillTable(&testModel, 3, 5);
    const QModelIndex parIndex = testModel.index(1, 0);
    fillTable(&testModel, 4, 5, parIndex, 10);

    QVERIFY(testModel.moveColumns(parIndex, 1, 2, QModelIndex(), 0));
    QCOMPARE(testModel.columnCount(parIndex), 3);
    QCOMPARE(testModel.rowCount(parIndex), 4);
    QCOMPARE(testModel.columnCount(), 7);
    QCOMPARE(testModel.rowCount(), 4);

    QCOMPARE(testModel.index(0, 0).data().toString(), QStringLiteral("10,11"));
    QCOMPARE(testModel.index(0, 1).data().toString(), QStringLiteral("10,12"));
    QCOMPARE(testModel.index(0, 2).data().toString(), QStringLiteral("0,0"));
    QCOMPARE(testModel.index(0, 3).data().toString(), QStringLiteral("0,1"));
    QCOMPARE(testModel.index(0, 4).data().toString(), QStringLiteral("0,2"));
    QCOMPARE(testModel.index(0, 5).data().toString(), QStringLiteral("0,3"));
    QCOMPARE(testModel.index(0, 6).data().toString(), QStringLiteral("0,4"));
    QCOMPARE(testModel.index(3, 0).data().toString(), QStringLiteral("13,11"));
    QCOMPARE(testModel.index(3, 1).data().toString(), QStringLiteral("13,12"));
    QVERIFY(testModel.index(3, 2).isValid());
    QVERIFY(!testModel.index(3, 2).data().isValid());
    QVERIFY(testModel.index(3, 6).isValid());
    QVERIFY(!testModel.index(3, 6).data().isValid());
    QCOMPARE(testModel.index(0, 0, parIndex).data().toString(), QStringLiteral("10,10"));
    QCOMPARE(testModel.index(0, 1, parIndex).data().toString(), QStringLiteral("10,13"));
    QCOMPARE(testModel.index(0, 2, parIndex).data().toString(), QStringLiteral("10,14"));
    QCOMPARE(testModel.index(2, 0, parIndex).data().toString(), QStringLiteral("12,10"));
    QCOMPARE(testModel.index(2, 1, parIndex).data().toString(), QStringLiteral("12,13"));
    QCOMPARE(testModel.index(2, 2, parIndex).data().toString(), QStringLiteral("12,14"));
    QVERIFY(!testModel.headerData(0, Qt::Horizontal).isValid());
    QVERIFY(!testModel.headerData(1, Qt::Horizontal).isValid());
    QCOMPARE(testModel.headerData(2, Qt::Horizontal).toInt(), 0);
    QCOMPARE(testModel.headerData(3, Qt::Horizontal).toInt(), 1);
    QCOMPARE(testModel.headerData(4, Qt::Horizontal).toInt(), 2);
    QCOMPARE(testModel.headerData(5, Qt::Horizontal).toInt(), 3);
    QCOMPARE(testModel.headerData(6, Qt::Horizontal).toInt(), 4);
}

void tst_GenericModel::roleNames()
{
    class FakeItemModel : public QAbstractItemModel
    {
    public:
        using QAbstractItemModel::QAbstractItemModel;
        int columnCount(const QModelIndex & = QModelIndex()) const override { return 0; }
        int rowCount(const QModelIndex & = QModelIndex()) const override { return 0; }
        QModelIndex index(int, int, const QModelIndex & = QModelIndex()) const override { return QModelIndex(); }
        QVariant data(const QModelIndex &, int = Qt::DisplayRole) const override { return QVariant(); }
        QModelIndex parent(const QModelIndex &) const override { return QModelIndex(); }
    };
    FakeItemModel dafaultModel;
    GenericModel testModel;
    ModelTest probe(&testModel, nullptr);
    fillTable(&testModel, 3, 5);
    fillTable(&testModel, 3, 5, testModel.index(0, 0));
    QSignalSpy dataChangedSpy(&testModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(dataChangedSpy.isValid());

    QCOMPARE(testModel.roleNames(), dafaultModel.roleNames());
    testModel.setRoleNames(QHash<int, QByteArray>{std::make_pair<int>(Qt::UserRole, QByteArrayLiteral("myrole"))});
    QCOMPARE(dataChangedSpy.count(), 2);
    auto args = dataChangedSpy.takeFirst();
    QCOMPARE(args.at(0).value<QModelIndex>(), testModel.index(0, 0, testModel.index(0, 0)));
    QCOMPARE(args.at(1).value<QModelIndex>(), testModel.index(2, 4, testModel.index(0, 0)));
    args = dataChangedSpy.takeFirst();
    QCOMPARE(args.at(0).value<QModelIndex>(), testModel.index(0, 0));
    QCOMPARE(args.at(1).value<QModelIndex>(), testModel.index(2, 4));
    const auto roleNs = testModel.roleNames();
    QCOMPARE(roleNs.size(), 1);
    QCOMPARE(roleNs.begin().key(), static_cast<int>(Qt::UserRole));
    QCOMPARE(roleNs.begin().value(), QByteArrayLiteral("myrole"));
    testModel.setRoleNames(QHash<int, QByteArray>());
    QCOMPARE(dataChangedSpy.count(), 2);
    args = dataChangedSpy.takeFirst();
    QCOMPARE(args.at(0).value<QModelIndex>(), testModel.index(0, 0, testModel.index(0, 0)));
    QCOMPARE(args.at(1).value<QModelIndex>(), testModel.index(2, 4, testModel.index(0, 0)));
    args = dataChangedSpy.takeFirst();
    QCOMPARE(args.at(0).value<QModelIndex>(), testModel.index(0, 0));
    QCOMPARE(args.at(1).value<QModelIndex>(), testModel.index(2, 4));
    QCOMPARE(testModel.roleNames(), dafaultModel.roleNames());
}

void tst_GenericModel::fillTable(QAbstractItemModel *model, int rows, int cols, const QModelIndex &parent, int shift) const
{
    model->removeRows(0, model->rowCount(parent), parent);
    model->removeColumns(0, model->columnCount(parent), parent);
    model->insertColumns(0, cols, parent);
    model->insertRows(0, rows, parent);
    for (int r = 0; r < model->rowCount(parent); ++r) {
        if (!parent.isValid())
            model->setHeaderData(r, Qt::Vertical, r + shift);
        for (int c = 0; c < model->columnCount(parent); ++c) {
            const QModelIndex idx = model->index(r, c, parent);
            model->setData(idx, QStringLiteral("%1,%2").arg(r + shift).arg(c + shift));
            model->setData(idx, r + shift, Qt::UserRole);
            model->setData(idx, c + shift, Qt::UserRole + 1);
        }
    }
    for (int c = 0; !parent.isValid() && c < model->columnCount(parent); ++c)
        model->setHeaderData(c, Qt::Horizontal, c + shift);
}
