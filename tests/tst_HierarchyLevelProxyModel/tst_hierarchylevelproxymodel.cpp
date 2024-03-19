#include "tst_hierarchylevelproxymodel.h"

#include <hierarchylevelproxymodel.h>
#include <QtTest/QTest>
#include <QSignalSpy>
#include "../modeltestmanager.h"
#include <QSortFilterProxyModel>

QAbstractItemModel *createNullModel(QObject *parent)
{
    Q_UNUSED(parent)
    return nullptr;
}

QAbstractItemModel *createListModel(QObject *parent)
{
    return new SimpleModel(
            QStringList() << QStringLiteral("1") << QStringLiteral("2") << QStringLiteral("3") << QStringLiteral("4") << QStringLiteral("5"), parent);
}

void fillTreeModel(QAbstractItemModel *result, int rows = 5, int cols = 3, bool rowFirst = true)
{
#ifdef COMPLEX_MODEL_SUPPORT
    if (rowFirst)
        result->insertRows(0, rows);
    result->insertColumns(0, cols);
    if (!rowFirst)
        result->insertRows(0, rows);
    for (int i = 0; i < result->rowCount(); ++i) {
        for (int j = 0; j < result->columnCount(); ++j) {
            result->setData(result->index(i, j), QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            result->setData(result->index(i, j), i, Qt::UserRole);
            result->setData(result->index(i, j), j, Qt::UserRole + 1);
        }
        const QModelIndex parIdx = result->index(i, 0);
        if (rowFirst)
            result->insertRows(0, rows, parIdx);
        result->insertColumns(0, cols, parIdx);
        if (!rowFirst)
            result->insertRows(0, rows, parIdx);
        for (int k = 0; k < result->rowCount(parIdx); ++k) {
            for (int h = 0; h < result->columnCount(parIdx); ++h) {
                result->setData(result->index(k, h, parIdx), QStringLiteral("%1,%2,%3").arg(i).arg(k).arg(h), Qt::EditRole);
                result->setData(result->index(k, h, parIdx), h, Qt::UserRole);
                result->setData(result->index(k, h, parIdx), k, Qt::UserRole + 1);
            }
            const QModelIndex childIdx = result->index(k, 0, parIdx);
            if (rowFirst)
                result->insertRows(0, rows, childIdx);
            result->insertColumns(0, cols, childIdx);
            if (!rowFirst)
                result->insertRows(0, rows, childIdx);
            for (int j = 0; j < result->rowCount(childIdx); ++j) {
                for (int h = 0; h < result->columnCount(childIdx); ++h) {
                    result->setData(result->index(j, h, childIdx), QStringLiteral("%1,%2,%3,%4").arg(i).arg(k).arg(j).arg(h), Qt::EditRole);
                    result->setData(result->index(j, h, childIdx), h, Qt::UserRole);
                    result->setData(result->index(j, h, childIdx), k, Qt::UserRole + 1);
                }
            }
        }
    }
#endif
}

QAbstractItemModel *createTreeModel(QObject *parent, int rows = 5, int cols = 3)
{
    QAbstractItemModel *result = nullptr;
#ifdef COMPLEX_MODEL_SUPPORT
    result = new ComplexModel(parent);
#endif
    fillTreeModel(result, rows, cols);
    return result;
}

void tst_HierarchyLevelProxyModel::autoParent()
{
    QObject *parentObj = new QObject;
    auto testItem = new HierarchyLevelProxyModel(parentObj);
    QSignalSpy testItemDestroyedSpy(testItem, &HierarchyLevelProxyModel::destroyed);
    QVERIFY(testItemDestroyedSpy.isValid());
    delete parentObj;
    QCOMPARE(testItemDestroyedSpy.count(), 1);
}

void tst_HierarchyLevelProxyModel::testProperties()
{
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    QCOMPARE(proxyModel.property("hierarchyLevel").toInt(), 0);
    QVERIFY(proxyModel.setProperty("hierarchyLevel", 1));
    QCOMPARE(proxyModel.property("hierarchyLevel").toInt(), 1);
    QCOMPARE(proxyModel.property("insertBehaviour").value<HierarchyLevelProxyModel::InsertBehaviour>(), HierarchyLevelProxyModel::InsertToNext);
    QVERIFY(proxyModel.setProperty("insertBehaviour", HierarchyLevelProxyModel::InsertToPrevious));
    QCOMPARE(proxyModel.property("insertBehaviour").value<HierarchyLevelProxyModel::InsertBehaviour>(), HierarchyLevelProxyModel::InsertToPrevious);
}

void tst_HierarchyLevelProxyModel::testSort()
{
#ifdef COMPLEX_MODEL_SUPPORT

#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_HierarchyLevelProxyModel::testSort_data()
{
#ifdef COMPLEX_MODEL_SUPPORT
#endif
}

void tst_HierarchyLevelProxyModel::testResetModel()
{
#ifdef COMPLEX_MODEL_SUPPORT
    class ResettableModel : public ComplexModel
    {
    public:
        using ComplexModel::ComplexModel;
        void triggerReset()
        {
            beginResetModel();
            endResetModel();
        }
    };
    ResettableModel baseModel;
    baseModel.insertColumn(0);
    baseModel.insertRows(0, 3);
    baseModel.insertColumn(0, baseModel.index(0, 0));
    baseModel.insertRows(0, 4, baseModel.index(0, 0));
    baseModel.insertColumn(0, baseModel.index(1, 0));
    baseModel.insertRows(0, 5, baseModel.index(1, 0));
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    proxyModel.setSourceModel(&baseModel);
    proxyModel.setHierarchyLevel(1);
    QSignalSpy proxyModelAboutToBeResetSpy(&proxyModel, &HierarchyLevelProxyModel::modelAboutToBeReset);
    QSignalSpy proxyModelResetSpy(&proxyModel, &HierarchyLevelProxyModel::modelReset);
    QCOMPARE(proxyModel.rowCount(), 9);
    baseModel.triggerReset();
    QCOMPARE(proxyModelAboutToBeResetSpy.count(), 1);
    QCOMPARE(proxyModelResetSpy.count(), 1);
    QCOMPARE(proxyModel.rowCount(), 9);
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_HierarchyLevelProxyModel::testDisconnectedModel()
{
    SimpleModel baseModel1(QStringList({QStringLiteral("London"), QStringLiteral("Berlin"), QStringLiteral("Paris")}));
    SimpleModel baseModel2(QStringList({QStringLiteral("Rome"), QStringLiteral("Madrid"), QStringLiteral("Prague")}));
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    proxyModel.setSourceModel(&baseModel1);
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(proxyDataChangeSpy.isValid());
    baseModel1.setData(baseModel1.index(0, 0), QStringLiteral("New York"));
    QCOMPARE(proxyDataChangeSpy.count(), 1);
    proxyDataChangeSpy.clear();
    proxyModel.setSourceModel(&baseModel2);
    baseModel1.setData(baseModel1.index(1, 0), QStringLiteral("Tokyo"));
    QCOMPARE(proxyDataChangeSpy.count(), 0);
    baseModel2.setData(baseModel2.index(0, 0), QStringLiteral("Lima"));
    QCOMPARE(proxyDataChangeSpy.count(), 1);
}

void tst_HierarchyLevelProxyModel::testData()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    HierarchyLevelProxyModel proxyModel;
    proxyModel.setSourceModel(baseModel);
    new ModelTest(&proxyModel, baseModel);
    const int sourceRows = baseModel->rowCount();
    const int sourceCols = baseModel->columnCount();
    for (int i = 0; i < sourceRows; ++i) {
        for (int j = 0; j < sourceCols; ++j) {
            QCOMPARE(proxyModel.index(i, j).data(), baseModel->index(i, j).data());
        }
    }

    proxyModel.setHierarchyLevel(1);
    int rootRowIter = 0;
    for (int i = 0; i < sourceRows; ++i) {
        for (int j = 0; j < sourceCols; ++j) {
            const QModelIndex sourceParent = baseModel->index(i, j);
            if (baseModel->hasChildren(sourceParent)) {
                for (int h = 0, hEnd = baseModel->rowCount(sourceParent); h < hEnd; ++h) {
                    for (int k = 0, kEnd = baseModel->columnCount(sourceParent); k < kEnd; ++k) {
                        const QModelIndex sourceIdx = baseModel->index(h, k, sourceParent);
                        const QModelIndex proxyIdx = proxyModel.index(rootRowIter, k);
                        QCOMPARE(proxyIdx.data(), sourceIdx.data());
                        if (baseModel->hasChildren(sourceIdx)) {
                            for (int ii = 0, iiEnd = baseModel->rowCount(sourceIdx); ii < iiEnd; ++ii) {
                                for (int jj = 0, jjEnd = baseModel->columnCount(sourceIdx); jj < jjEnd; ++jj) {
                                    QCOMPARE(proxyModel.index(ii, jj, proxyIdx).data(), baseModel->index(ii, jj, sourceIdx).data());
                                }
                            }
                        }
                    }
                    ++rootRowIter;
                }
            }
        }
    }

    baseModel->deleteLater();
}

void tst_HierarchyLevelProxyModel::testData_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_HierarchyLevelProxyModel::testSetData()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyDataChangeSpy(&proxyModel, &QAbstractItemModel::dataChanged);
    QSignalSpy sourceDataChangeSpy(baseModel, &QAbstractItemModel::dataChanged);
    const QModelIndex proxyIdX = proxyModel.index(0, 0);
    QString idxData = QStringLiteral("Test");
    QVERIFY(proxyModel.setData(proxyIdX, idxData));
    QCOMPARE(proxyModel.data(proxyIdX).toString(), idxData);
    QCOMPARE(baseModel->index(0, 0).data().toString(), idxData);
    QCOMPARE(proxyDataChangeSpy.count(), 1);
    QCOMPARE(sourceDataChangeSpy.count(), 1);
    auto spyArgs = proxyDataChangeSpy.takeFirst();
    QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyIdX);
    QCOMPARE(spyArgs.at(1).value<QModelIndex>(), proxyIdX);
    spyArgs = sourceDataChangeSpy.takeFirst();
    QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(0, 0));
    QCOMPARE(spyArgs.at(1).value<QModelIndex>(), baseModel->index(0, 0));
    if (baseModel->hasChildren(baseModel->index(0, 0))) {
        const QModelIndex proxyChildIdX = proxyModel.index(0, 0, proxyIdX);
        QVERIFY(proxyModel.setData(proxyChildIdX, idxData));
        QCOMPARE(proxyModel.data(proxyChildIdX).toString(), idxData);
        QCOMPARE(baseModel->index(0, 0, baseModel->index(0, 0)).data().toString(), idxData);
        QCOMPARE(proxyDataChangeSpy.count(), 1);
        QCOMPARE(sourceDataChangeSpy.count(), 1);
        auto spyArgs = proxyDataChangeSpy.takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyChildIdX);
        QCOMPARE(spyArgs.at(1).value<QModelIndex>(), proxyChildIdX);
        spyArgs = sourceDataChangeSpy.takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(0, 0, baseModel->index(0, 0)));
        QCOMPARE(spyArgs.at(1).value<QModelIndex>(), baseModel->index(0, 0, baseModel->index(0, 0)));
    }
    proxyModel.setHierarchyLevel(1);
    idxData += QStringLiteral("Test2");
    if (baseModel->hasChildren(baseModel->index(0, 0))) {
        QVERIFY(proxyModel.setData(proxyModel.index(0, 0), idxData));
        QCOMPARE(proxyModel.data(proxyModel.index(0, 0)).toString(), idxData);
        QCOMPARE(baseModel->index(0, 0, baseModel->index(0, 0)).data().toString(), idxData);
        QCOMPARE(proxyDataChangeSpy.count(), 1);
        QCOMPARE(sourceDataChangeSpy.count(), 1);
        auto spyArgs = proxyDataChangeSpy.takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(0, 0));
        QCOMPARE(spyArgs.at(1).value<QModelIndex>(), proxyModel.index(0, 0));
        spyArgs = sourceDataChangeSpy.takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(0, 0, baseModel->index(0, 0)));
        QCOMPARE(spyArgs.at(1).value<QModelIndex>(), baseModel->index(0, 0, baseModel->index(0, 0)));
    } else {
        QVERIFY(!proxyModel.setData(proxyModel.index(0, 0), idxData));
    }
    baseModel->deleteLater();
}

void tst_HierarchyLevelProxyModel::testSetData_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_HierarchyLevelProxyModel::testSetDataIrrelevantIndex()
{
    QAbstractItemModel* baseModel=createTreeModel(this);
    if (!baseModel)
        return;
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setHierarchyLevel(1);
    QSignalSpy proxyDataChangeSpy(&proxyModel, &QAbstractItemModel::dataChanged);
    QSignalSpy sourceDataChangeSpy(baseModel, &QAbstractItemModel::dataChanged);
    QVERIFY(baseModel->setData(baseModel->index(0,0),1234));
    QVERIFY(proxyDataChangeSpy.isEmpty());
    QCOMPARE(sourceDataChangeSpy.count(),1);
}

void tst_HierarchyLevelProxyModel::testInsertRowSource_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_HierarchyLevelProxyModel::testInsertRowSource()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyRowAboutToInsertSpy(&proxyModel, &QAbstractItemModel::rowsAboutToBeInserted);
    QSignalSpy proxyRowInsertSpy(&proxyModel, &QAbstractItemModel::rowsInserted);

    int beforeInsertRowCount = baseModel->rowCount();
    QVERIFY(baseModel->insertRow(2));
    QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount + 1);
    for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(2).toInt(), 2);
        QCOMPARE(spyArgs.at(1).toInt(), 2);
    }
    if (baseModel->hasChildren(baseModel->index(0, 0))) {
        beforeInsertRowCount = 0;
        for (int i = 0, iEnd = baseModel->rowCount(); i < iEnd; ++i)
            beforeInsertRowCount += baseModel->rowCount(baseModel->index(i, 0));
        proxyModel.setHierarchyLevel(1);
        // insert at level 0
        QVERIFY(baseModel->insertRow(2));
        QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount);
        for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy})
            QCOMPARE(spy->count(), 0);
        // insert at level 2
        QVERIFY(baseModel->insertRow(1, baseModel->index(1, 0, baseModel->index(1, 0))));
        QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount);
        for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(baseModel->rowCount(baseModel->index(0, 0)) + 1, 0));
            QCOMPARE(spyArgs.at(2).toInt(), 1);
            QCOMPARE(spyArgs.at(1).toInt(), 1);
        }
        // insert at level 1
        QVERIFY(baseModel->insertRow(1, baseModel->index(1, 0)));
        QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount + 1);
        for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
            QCOMPARE(spyArgs.at(2).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + 1);
            QCOMPARE(spyArgs.at(1).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + 1);
        }
        QCOMPARE(baseModel->rowCount(baseModel->index(2, 0)), 0);
        QVERIFY(baseModel->insertColumn(0, baseModel->index(2, 0)));
        // insert at level 1 on empty root
        QVERIFY(baseModel->insertRow(0, baseModel->index(2, 0)));
        QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount + 2);
        for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
            QCOMPARE(spyArgs.at(2).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + baseModel->rowCount(baseModel->index(1, 0)));
            QCOMPARE(spyArgs.at(1).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + baseModel->rowCount(baseModel->index(1, 0)));
        }
    }
    baseModel->deleteLater();
}

void tst_HierarchyLevelProxyModel::testInsertRowsSource_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_HierarchyLevelProxyModel::testInsertRowsSource()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyRowAboutToInsertSpy(&proxyModel, &QAbstractItemModel::rowsAboutToBeInserted);
    QSignalSpy proxyRowInsertSpy(&proxyModel, &QAbstractItemModel::rowsInserted);

    int beforeInsertRowCount = baseModel->rowCount();
    // insert at level 0
    QVERIFY(baseModel->insertRows(2, 2));
    QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount + 2);
    for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(2).toInt(), 3);
        QCOMPARE(spyArgs.at(1).toInt(), 2);
    }
    if (baseModel->hasChildren(baseModel->index(0, 0))) {
        beforeInsertRowCount = 0;
        for (int i = 0, iEnd = baseModel->rowCount(); i < iEnd; ++i)
            beforeInsertRowCount += baseModel->rowCount(baseModel->index(i, 0));
        proxyModel.setHierarchyLevel(1);
        // insert at level 0
        QVERIFY(baseModel->insertRows(2, 2));
        QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount);
        for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy})
            QCOMPARE(spy->count(), 0);
        // insert at level 2
        QVERIFY(baseModel->insertRows(1, 2, baseModel->index(1, 0, baseModel->index(1, 0))));
        QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount);
        for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(baseModel->rowCount(baseModel->index(0, 0)) + 1, 0));
            QCOMPARE(spyArgs.at(2).toInt(), 2);
            QCOMPARE(spyArgs.at(1).toInt(), 1);
        }
        // insert at level 1
        QVERIFY(baseModel->insertRows(1, 2, baseModel->index(1, 0)));
        QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount + 2);
        for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
            QCOMPARE(spyArgs.at(2).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + 2);
            QCOMPARE(spyArgs.at(1).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + 1);
        }
        QCOMPARE(baseModel->rowCount(baseModel->index(2, 0)), 0);
        QVERIFY(baseModel->insertColumn(0, baseModel->index(2, 0)));
        // insert at level 1 on empty root
        QVERIFY(baseModel->insertRows(0, 2, baseModel->index(2, 0)));
        QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount + 4);
        for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
            QCOMPARE(spyArgs.at(2).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + baseModel->rowCount(baseModel->index(1, 0)) + 1);
            QCOMPARE(spyArgs.at(1).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + baseModel->rowCount(baseModel->index(1, 0)));
        }
    }
    baseModel->deleteLater();
}

void tst_HierarchyLevelProxyModel::testInsertRowProxy_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_HierarchyLevelProxyModel::testInsertRowProxy()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyRowAboutToInsertSpy(&proxyModel, &QAbstractItemModel::rowsAboutToBeInserted);
    QSignalSpy proxyRowInsertSpy(&proxyModel, &QAbstractItemModel::rowsInserted);
    QSignalSpy sourceRowAboutToInsertSpy(baseModel, &QAbstractItemModel::rowsAboutToBeInserted);
    QSignalSpy sourceRowInsertSpy(baseModel, &QAbstractItemModel::rowsInserted);
    // insert at invalid position
    QVERIFY(!proxyModel.insertRow(proxyModel.rowCount() + 1));
    for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy, &sourceRowAboutToInsertSpy, &sourceRowInsertSpy})
        QVERIFY(spy->isEmpty());
    // insert at invalid position
    QVERIFY(!proxyModel.insertRow(-1));
    for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy, &sourceRowAboutToInsertSpy, &sourceRowInsertSpy})
        QVERIFY(spy->isEmpty());
    int beforeInsertRowCount = baseModel->rowCount();
    QVERIFY(proxyModel.insertRow(2));
    QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount + 1);
    QCOMPARE(baseModel->rowCount(), beforeInsertRowCount + 1);
    for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy, &sourceRowAboutToInsertSpy, &sourceRowInsertSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(2).toInt(), 2);
        QCOMPARE(spyArgs.at(1).toInt(), 2);
    }
    if (baseModel->hasChildren(baseModel->index(0, 0))) {
        proxyModel.setHierarchyLevel(1);
        beforeInsertRowCount = proxyModel.rowCount(proxyModel.index(1, 0));
        // insert at level 2
        QVERIFY(proxyModel.insertRow(1, proxyModel.index(1, 0)));
        QCOMPARE(proxyModel.rowCount(proxyModel.index(1, 0)), beforeInsertRowCount + 1);
        for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(1, 0));
            QCOMPARE(spyArgs.at(2).toInt(), 1);
            QCOMPARE(spyArgs.at(1).toInt(), 1);
        }
        for (QSignalSpy *spy : {&sourceRowAboutToInsertSpy, &sourceRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(1, 0, baseModel->index(0, 0)));
            QCOMPARE(spyArgs.at(2).toInt(), 1);
            QCOMPARE(spyArgs.at(1).toInt(), 1);
        }
        beforeInsertRowCount = 0;
        for (int i = 0, iEnd = baseModel->rowCount(); i < iEnd; ++i)
            beforeInsertRowCount += baseModel->rowCount(baseModel->index(i, 0));
        // insert at level 1
        QVERIFY(proxyModel.insertRow(baseModel->rowCount(baseModel->index(0, 0)) + 1)); // equivalent to baseModel->insertRow(1,baseModel->index(1,0))
        QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount + 1);
        for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
            QCOMPARE(spyArgs.at(2).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + 1);
            QCOMPARE(spyArgs.at(1).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + 1);
        }
        for (QSignalSpy *spy : {&sourceRowAboutToInsertSpy, &sourceRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(1, 0));
            QCOMPARE(spyArgs.at(2).toInt(), 1);
            QCOMPARE(spyArgs.at(1).toInt(), 1);
        }
    }
    baseModel->deleteLater();
}

void tst_HierarchyLevelProxyModel::testInsertRowsProxy_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_HierarchyLevelProxyModel::testInsertRowsProxy()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyRowAboutToInsertSpy(&proxyModel, &QAbstractItemModel::rowsAboutToBeInserted);
    QSignalSpy proxyRowInsertSpy(&proxyModel, &QAbstractItemModel::rowsInserted);
    QSignalSpy sourceRowAboutToInsertSpy(baseModel, &QAbstractItemModel::rowsAboutToBeInserted);
    QSignalSpy sourceRowInsertSpy(baseModel, &QAbstractItemModel::rowsInserted);
    QVERIFY(!proxyModel.insertRows(proxyModel.rowCount() + 1, 2));
    for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy, &sourceRowAboutToInsertSpy, &sourceRowInsertSpy})
        QVERIFY(spy->isEmpty());
    QVERIFY(!proxyModel.insertRows(-1, 2));
    for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy, &sourceRowAboutToInsertSpy, &sourceRowInsertSpy})
        QVERIFY(spy->isEmpty());
    int beforeInsertRowCount = baseModel->rowCount();
    QVERIFY(proxyModel.insertRows(2, 2));
    QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount + 2);
    QCOMPARE(baseModel->rowCount(), beforeInsertRowCount + 2);
    for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy, &sourceRowAboutToInsertSpy, &sourceRowInsertSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(1).toInt(), 2);
        QCOMPARE(spyArgs.at(2).toInt(), 3);
    }
    if (baseModel->hasChildren(baseModel->index(0, 0))) {
        proxyModel.setHierarchyLevel(1);
        beforeInsertRowCount = proxyModel.rowCount(proxyModel.index(1, 0));
        // insert at level 2
        QVERIFY(proxyModel.insertRows(1, 3, proxyModel.index(1, 0)));
        QCOMPARE(proxyModel.rowCount(proxyModel.index(1, 0)), beforeInsertRowCount + 3);
        for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(1, 0));
            QCOMPARE(spyArgs.at(2).toInt(), 3);
            QCOMPARE(spyArgs.at(1).toInt(), 1);
        }
        for (QSignalSpy *spy : {&sourceRowAboutToInsertSpy, &sourceRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(1, 0, baseModel->index(0, 0)));
            QCOMPARE(spyArgs.at(2).toInt(), 3);
            QCOMPARE(spyArgs.at(1).toInt(), 1);
        }
        beforeInsertRowCount = 0;
        for (int i = 0, iEnd = baseModel->rowCount(); i < iEnd; ++i)
            beforeInsertRowCount += baseModel->rowCount(baseModel->index(i, 0));

        // equivalent to baseModel->insertRow(1,baseModel->index(1,0))
        QVERIFY(proxyModel.insertRows(baseModel->rowCount(baseModel->index(0, 0)) + 1, 2));
        QCOMPARE(proxyModel.rowCount(), beforeInsertRowCount + 2);
        for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
            QCOMPARE(spyArgs.at(1).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + 1);
            QCOMPARE(spyArgs.at(2).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + 2);
        }
        for (QSignalSpy *spy : {&sourceRowAboutToInsertSpy, &sourceRowInsertSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(1, 0));
            QCOMPARE(spyArgs.at(1).toInt(), 1);
            QCOMPARE(spyArgs.at(2).toInt(), 2);
        }
    }
    baseModel->deleteLater();
}

void tst_HierarchyLevelProxyModel::testInsertBehaviour()
{
#ifdef COMPLEX_MODEL_SUPPORT
    ComplexModel baseModel;
    baseModel.insertColumn(0);
    baseModel.insertRows(0, 4);
    QModelIndex sourceIdx = baseModel.index(0, 0);
    baseModel.insertColumn(0, sourceIdx);
    baseModel.insertRows(0, 3, sourceIdx);
    sourceIdx = baseModel.index(3, 0);
    baseModel.insertColumn(0, sourceIdx);
    baseModel.insertRows(0, 2, sourceIdx);
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, &baseModel);
    QSignalSpy proxyRowAboutToInsertSpy(&proxyModel, &QAbstractItemModel::rowsAboutToBeInserted);
    QSignalSpy proxyRowInsertSpy(&proxyModel, &QAbstractItemModel::rowsInserted);
    QSignalSpy sourceRowAboutToInsertSpy(&baseModel, &QAbstractItemModel::rowsAboutToBeInserted);
    QSignalSpy sourceRowInsertSpy(&baseModel, &QAbstractItemModel::rowsInserted);
    proxyModel.setHierarchyLevel(1);
    proxyModel.setSourceModel(&baseModel);

    proxyModel.setInsertBehaviour(HierarchyLevelProxyModel::InsertToNext);
    QVERIFY(proxyModel.insertRow(3));
    QCOMPARE(proxyModel.rowCount(), 6);
    QCOMPARE(baseModel.rowCount(sourceIdx), 3);
    for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(2).toInt(), 3);
        QCOMPARE(spyArgs.at(1).toInt(), 3);
    }
    for (QSignalSpy *spy : {&sourceRowAboutToInsertSpy, &sourceRowInsertSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), sourceIdx);
        QCOMPARE(spyArgs.at(2).toInt(), 0);
        QCOMPARE(spyArgs.at(1).toInt(), 0);
    }
    sourceIdx = baseModel.index(0, 0);
    QCOMPARE(baseModel.rowCount(sourceIdx), 3);

    proxyModel.setInsertBehaviour(HierarchyLevelProxyModel::InsertToPrevious);
    QVERIFY(proxyModel.insertRow(3));
    QCOMPARE(proxyModel.rowCount(), 7);
    QCOMPARE(baseModel.rowCount(sourceIdx), 3);
    sourceIdx = baseModel.index(3, 0);
    QCOMPARE(baseModel.rowCount(sourceIdx), 3);
    sourceIdx = baseModel.index(2, 0);
    QCOMPARE(baseModel.rowCount(sourceIdx), 1);
    for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(2).toInt(), 3);
        QCOMPARE(spyArgs.at(1).toInt(), 3);
    }
    for (QSignalSpy *spy : {&sourceRowAboutToInsertSpy, &sourceRowInsertSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), sourceIdx);
        QCOMPARE(spyArgs.at(2).toInt(), 0);
        QCOMPARE(spyArgs.at(1).toInt(), 0);
    }

    proxyModel.setInsertBehaviour(HierarchyLevelProxyModel::InsertToPreviousNonEmpty);
    QVERIFY(proxyModel.insertRow(3));
    QCOMPARE(proxyModel.rowCount(), 8);
    QCOMPARE(baseModel.rowCount(sourceIdx), 1);
    sourceIdx = baseModel.index(3, 0);
    QCOMPARE(baseModel.rowCount(sourceIdx), 3);
    sourceIdx = baseModel.index(0, 0);
    QCOMPARE(baseModel.rowCount(sourceIdx), 4);
    for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(2).toInt(), 3);
        QCOMPARE(spyArgs.at(1).toInt(), 3);
    }
    for (QSignalSpy *spy : {&sourceRowAboutToInsertSpy, &sourceRowInsertSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), sourceIdx);
        QCOMPARE(spyArgs.at(2).toInt(), 3);
        QCOMPARE(spyArgs.at(1).toInt(), 3);
    }
    QCOMPARE(baseModel.rowCount(baseModel.index(1, 0)), 0);

    sourceIdx = baseModel.index(0, 0);
    QVERIFY(baseModel.removeRows(0, baseModel.rowCount(sourceIdx), sourceIdx));
    QCOMPARE(proxyModel.rowCount(), 4);
    QVERIFY(proxyModel.insertRow(0));
    QCOMPARE(proxyModel.rowCount(), 5);
    QCOMPARE(baseModel.rowCount(sourceIdx), 1);
    QCOMPARE(baseModel.rowCount(baseModel.index(1, 0)), 0);
    QCOMPARE(baseModel.rowCount(baseModel.index(2, 0)), 1);
    QCOMPARE(baseModel.rowCount(baseModel.index(3, 0)), 3);
    for (QSignalSpy *spy : {&proxyRowAboutToInsertSpy, &proxyRowInsertSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(2).toInt(), 0);
        QCOMPARE(spyArgs.at(1).toInt(), 0);
    }
    for (QSignalSpy *spy : {&sourceRowAboutToInsertSpy, &sourceRowInsertSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), sourceIdx);
        QCOMPARE(spyArgs.at(2).toInt(), 0);
        QCOMPARE(spyArgs.at(1).toInt(), 0);
    }
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_HierarchyLevelProxyModel::testRemoveRowSource_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_HierarchyLevelProxyModel::testRemoveRowSource()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyRowAboutToRemovedSpy(&proxyModel, &QAbstractItemModel::rowsAboutToBeRemoved);
    QSignalSpy proxyRowRemovedSpy(&proxyModel, &QAbstractItemModel::rowsRemoved);

    int beforeRemovedRowCount = baseModel->rowCount();
    QVERIFY(baseModel->removeRow(2));
    QCOMPARE(proxyModel.rowCount(), beforeRemovedRowCount - 1);
    for (QSignalSpy *spy : {&proxyRowAboutToRemovedSpy, &proxyRowRemovedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(2).toInt(), 2);
        QCOMPARE(spyArgs.at(1).toInt(), 2);
    }
    if (baseModel->hasChildren(baseModel->index(0, 0))) {
        beforeRemovedRowCount = 0;
        for (int i = 0, iEnd = baseModel->rowCount(); i < iEnd; ++i)
            beforeRemovedRowCount += baseModel->rowCount(baseModel->index(i, 0));
        proxyModel.setHierarchyLevel(1);
        // remove at level 2
        QVERIFY(baseModel->removeRow(1, baseModel->index(1, 0, baseModel->index(1, 0))));
        QCOMPARE(proxyModel.rowCount(), beforeRemovedRowCount);
        for (QSignalSpy *spy : {&proxyRowAboutToRemovedSpy, &proxyRowRemovedSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(baseModel->rowCount(baseModel->index(0, 0)) + 1, 0));
            QCOMPARE(spyArgs.at(2).toInt(), 1);
            QCOMPARE(spyArgs.at(1).toInt(), 1);
        }
        // remove at level 1
        QVERIFY(baseModel->removeRow(1, baseModel->index(1, 0)));
        QCOMPARE(proxyModel.rowCount(), beforeRemovedRowCount - 1);
        for (QSignalSpy *spy : {&proxyRowAboutToRemovedSpy, &proxyRowRemovedSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
            QCOMPARE(spyArgs.at(2).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + 1);
            QCOMPARE(spyArgs.at(1).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + 1);
        }
        // remove at level 0
        beforeRemovedRowCount = proxyModel.rowCount();
        const int childRowsRemoved = baseModel->rowCount(baseModel->index(1, 0));
        Q_ASSERT(childRowsRemoved > 0);
        QVERIFY(baseModel->removeRow(1));
        QCOMPARE(proxyModel.rowCount(), beforeRemovedRowCount - childRowsRemoved);
        for (QSignalSpy *spy : {&proxyRowAboutToRemovedSpy, &proxyRowRemovedSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
            QCOMPARE(spyArgs.at(2).toInt(), baseModel->rowCount(baseModel->index(0, 0)) + childRowsRemoved - 1);
            QCOMPARE(spyArgs.at(1).toInt(), baseModel->rowCount(baseModel->index(0, 0)));
        }
        // remove at level 0 on empty root
        beforeRemovedRowCount = proxyModel.rowCount();
        QVERIFY(baseModel->insertRow(1));
        QVERIFY(baseModel->removeRow(1));
        QCOMPARE(proxyModel.rowCount(), beforeRemovedRowCount);
        for (QSignalSpy *spy : {&proxyRowAboutToRemovedSpy, &proxyRowRemovedSpy})
            QCOMPARE(spy->count(), 0);
    }
    baseModel->deleteLater();
}
void tst_HierarchyLevelProxyModel::testRemoveRowProxy_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_HierarchyLevelProxyModel::testRemoveRowProxy()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyRowAboutToRemovedSpy(&proxyModel, &QAbstractItemModel::rowsAboutToBeRemoved);
    QSignalSpy proxyRowRemovedSpy(&proxyModel, &QAbstractItemModel::rowsRemoved);
    QSignalSpy sourceRowAboutToRemovedSpy(baseModel, &QAbstractItemModel::rowsAboutToBeRemoved);
    QSignalSpy sourceRowRemovedSpy(baseModel, &QAbstractItemModel::rowsRemoved);

    int beforeRemovedRowCount = baseModel->rowCount();
    QVERIFY(proxyModel.removeRow(2));
    QCOMPARE(proxyModel.rowCount(), beforeRemovedRowCount - 1);
    QCOMPARE(baseModel->rowCount(), beforeRemovedRowCount - 1);
    for (QSignalSpy *spy : {&proxyRowAboutToRemovedSpy, &proxyRowRemovedSpy, &sourceRowAboutToRemovedSpy,&sourceRowRemovedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(2).toInt(), 2);
        QCOMPARE(spyArgs.at(1).toInt(), 2);
    }
    if (baseModel->hasChildren(baseModel->index(0, 0))) {
        beforeRemovedRowCount = 0;
        for (int i = 0, iEnd = baseModel->rowCount(); i < iEnd; ++i)
            beforeRemovedRowCount += baseModel->rowCount(baseModel->index(i, 0));
        proxyModel.setHierarchyLevel(1);
        // remove at level 2
        QVERIFY(proxyModel.removeRow(1, proxyModel.index(1, 0)));
        QCOMPARE(proxyModel.rowCount(), beforeRemovedRowCount);
        for (QSignalSpy *spy : {&proxyRowAboutToRemovedSpy, &proxyRowRemovedSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(1, 0));
            QCOMPARE(spyArgs.at(2).toInt(), 1);
            QCOMPARE(spyArgs.at(1).toInt(), 1);
        }
        for (QSignalSpy *spy : {&sourceRowAboutToRemovedSpy, &sourceRowRemovedSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(1,0,baseModel->index(0, 0)));
            QCOMPARE(spyArgs.at(2).toInt(), 1);
            QCOMPARE(spyArgs.at(1).toInt(), 1);
        }

        // remove at level 1
        QVERIFY(proxyModel.removeRow(1));
        QCOMPARE(proxyModel.rowCount(), beforeRemovedRowCount - 1);
        for (QSignalSpy *spy : {&proxyRowAboutToRemovedSpy, &proxyRowRemovedSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
            QCOMPARE(spyArgs.at(2).toInt(), 1);
            QCOMPARE(spyArgs.at(1).toInt(), 1);
        }
        for (QSignalSpy *spy : {&sourceRowAboutToRemovedSpy, &sourceRowRemovedSpy}) {
            QCOMPARE(spy->count(), 1);
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(0, 0));
            QCOMPARE(spyArgs.at(2).toInt(), 1);
            QCOMPARE(spyArgs.at(1).toInt(), 1);
        }
    }
    baseModel->deleteLater();
}

void tst_HierarchyLevelProxyModel::testInsertColumnSource()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyColumnAboutToBeInsertedSpy(&proxyModel, &QAbstractItemModel::columnsAboutToBeInserted);
    QSignalSpy proxyColumnInsertedSpy(&proxyModel, &QAbstractItemModel::columnsInserted);
    proxyModel.setHierarchyLevel(1);

    // insert at level 2
    QModelIndex sourceParent = baseModel->index(0, 0, baseModel->index(0, 0));
    int oldSourceColCount = baseModel->columnCount(sourceParent);
    QCOMPARE(proxyModel.columnCount(proxyModel.index(0, 0)), oldSourceColCount);
    QVERIFY(baseModel->insertColumn(1, sourceParent));
    QCOMPARE(proxyModel.columnCount(proxyModel.index(0, 0)), oldSourceColCount + 1);
    for (QSignalSpy *spy : {&proxyColumnAboutToBeInsertedSpy, &proxyColumnInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(0, 0));
        QCOMPARE(spyArgs.at(1).toInt(), 1);
        QCOMPARE(spyArgs.at(2).toInt(), 1);
    }

    // insert at level 0
    oldSourceColCount = baseModel->columnCount(baseModel->index(0, 0));
    QVERIFY(baseModel->insertColumn(1));
    QCOMPARE(proxyModel.columnCount(), oldSourceColCount);
    for (QSignalSpy *spy : {&proxyColumnAboutToBeInsertedSpy, &proxyColumnInsertedSpy})
        QCOMPARE(spy->count(), 0);

    // insert at level 1
    QSignalSpy proxyDataChangedSpy(&proxyModel, &QAbstractItemModel::dataChanged);
    QVERIFY(baseModel->insertColumn(1, baseModel->index(0, 0)));
    QCOMPARE(proxyModel.columnCount(), oldSourceColCount + 1);
    for (QSignalSpy *spy : {&proxyColumnAboutToBeInsertedSpy, &proxyColumnInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(1).toInt(), oldSourceColCount);
        QCOMPARE(spyArgs.at(2).toInt(), oldSourceColCount);
    }
    {
        QCOMPARE(proxyDataChangedSpy.count(), 1);
        const auto spyArgs = proxyDataChangedSpy.takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(0, 1));
        QCOMPARE(spyArgs.at(1).value<QModelIndex>(), proxyModel.index(baseModel->rowCount(baseModel->index(0, 0)) - 1, oldSourceColCount));
        QVERIFY(spyArgs.at(2).value<QVector<int>>().isEmpty());
    }
    // insert at level 1 of dominant branch
    QVERIFY(baseModel->insertRows(0, 1000, baseModel->index(1, 0)));
    QVERIFY(baseModel->insertColumns(1, 2, baseModel->index(1, 0)));
    QCOMPARE(proxyModel.columnCount(), oldSourceColCount + 2);
    for (QSignalSpy *spy : {&proxyColumnAboutToBeInsertedSpy, &proxyColumnInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(1).toInt(), 1);
        QCOMPARE(spyArgs.at(2).toInt(), 2);
    }
    {
        QCOMPARE(proxyDataChangedSpy.count(), 2);
        auto spyArgs = proxyDataChangedSpy.takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(0, 1));
        QCOMPARE(spyArgs.at(1).value<QModelIndex>(), proxyModel.index(baseModel->rowCount(baseModel->index(0, 0)) - 1, oldSourceColCount + 1));
        QVERIFY(spyArgs.at(2).value<QVector<int>>().isEmpty());
        spyArgs = proxyDataChangedSpy.takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(),
                 proxyModel.index(baseModel->rowCount(baseModel->index(0, 0)) + baseModel->rowCount(baseModel->index(1, 0)), 1));
        QCOMPARE(spyArgs.at(1).value<QModelIndex>(), proxyModel.index(proxyModel.rowCount() - 1, oldSourceColCount + 1));
        QVERIFY(spyArgs.at(2).value<QVector<int>>().isEmpty());
    }
    // insert at level 1 no new column
    QVERIFY(baseModel->insertColumn(1, baseModel->index(0, 0)));
    QCOMPARE(proxyModel.columnCount(), oldSourceColCount + 2);
    for (QSignalSpy *spy : {&proxyColumnAboutToBeInsertedSpy, &proxyColumnInsertedSpy})
        QCOMPARE(spy->count(), 0);
    {
        QCOMPARE(proxyDataChangedSpy.count(), 1);
        auto spyArgs = proxyDataChangedSpy.takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(0, 1));
        QCOMPARE(spyArgs.at(1).value<QModelIndex>(), proxyModel.index(baseModel->rowCount(baseModel->index(0, 0)) - 1, baseModel->columnCount(baseModel->index(0, 0))-1));
        QVERIFY(spyArgs.at(2).value<QVector<int>>().isEmpty());
    }

    // insert first column at level 0
    QSignalSpy proxyModelResetSpy(&proxyModel, &QAbstractItemModel::modelReset);
    QSignalSpy proxyModelAboutToBeResetSpy(&proxyModel, &QAbstractItemModel::modelAboutToBeReset);
    QVERIFY(baseModel->insertColumn(0));
    QCOMPARE(proxyModel.columnCount(), 0);
    QCOMPARE(proxyModel.rowCount(), 0);
    for (QSignalSpy *spy : {&proxyColumnAboutToBeInsertedSpy, &proxyColumnInsertedSpy})
        QCOMPARE(spy->count(), 0);
    for (QSignalSpy *spy : {&proxyModelAboutToBeResetSpy, &proxyModelResetSpy})
        QCOMPARE(spy->count(), 1);
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_HierarchyLevelProxyModel::testInsertColumnProxy()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyColumnAboutToBeInsertedSpy(&proxyModel, &QAbstractItemModel::columnsAboutToBeInserted);
    QSignalSpy proxyColumnInsertedSpy(&proxyModel, &QAbstractItemModel::columnsInserted);
    QSignalSpy sourceColumnAboutToBeInsertedSpy(baseModel, &QAbstractItemModel::columnsAboutToBeInserted);
    QSignalSpy sourceColumnInsertedSpy(baseModel, &QAbstractItemModel::columnsInserted);
    proxyModel.setHierarchyLevel(1);
    // insert at level 2
    QModelIndex sourceParent = baseModel->index(0, 0, baseModel->index(0, 0));
    int oldSourceColCount = baseModel->columnCount(sourceParent);
    QCOMPARE(proxyModel.columnCount(proxyModel.index(0, 0)), oldSourceColCount);
    QVERIFY(proxyModel.insertColumn(1, proxyModel.index(0, 0)));
    QCOMPARE(proxyModel.columnCount(proxyModel.index(0, 0)), oldSourceColCount + 1);
    QCOMPARE(baseModel->columnCount(sourceParent), oldSourceColCount + 1);
    for (QSignalSpy *spy : {&proxyColumnAboutToBeInsertedSpy, &proxyColumnInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(0, 0));
        QCOMPARE(spyArgs.at(1).toInt(), 1);
        QCOMPARE(spyArgs.at(2).toInt(), 1);
    }
    for (QSignalSpy *spy : {&sourceColumnInsertedSpy, &sourceColumnAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), sourceParent);
        QCOMPARE(spyArgs.at(1).toInt(), 1);
        QCOMPARE(spyArgs.at(2).toInt(), 1);
    }
    // insert at level 1
    oldSourceColCount = baseModel->columnCount(baseModel->index(0, 0));
    const int SourceRowCount = baseModel->rowCount();
    QSignalSpy proxyDataChangedSpy(&proxyModel, &QAbstractItemModel::dataChanged);
    QVERIFY(proxyModel.insertColumn(1));
    for (QSignalSpy *spy : {&proxyColumnAboutToBeInsertedSpy, &proxyColumnInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        // branch 0 is not dominant so column appended at the end
        QCOMPARE(spyArgs.at(1).toInt(), oldSourceColCount);
        QCOMPARE(spyArgs.at(2).toInt(), oldSourceColCount);
    }
    {
        QCOMPARE(proxyDataChangedSpy.count(), SourceRowCount);
        const int branchRowCount = baseModel->rowCount(baseModel->index(0, 0));
        for (int i = 0; i < SourceRowCount; ++i) {
            const auto spyArgs = proxyDataChangedSpy.takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(branchRowCount * i, 1));
            QCOMPARE(spyArgs.at(1).value<QModelIndex>(), proxyModel.index((branchRowCount * (i + 1)) - 1, oldSourceColCount));
            QVERIFY(spyArgs.at(2).value<QVector<int>>().isEmpty());
        }
    }
    for (QSignalSpy *spy : {&sourceColumnInsertedSpy, &sourceColumnAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), SourceRowCount);
        for (int i = 0; i < SourceRowCount; ++i) {
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(i, 0));
            QCOMPARE(spyArgs.at(1).toInt(), 1);
            QCOMPARE(spyArgs.at(2).toInt(), 1);
        }
    }
    // insert at level 1 dominant branch
    baseModel->insertRows(0, 1000, baseModel->index(0, 0));
    oldSourceColCount = baseModel->columnCount(baseModel->index(0, 0));
    QVERIFY(proxyModel.insertColumn(1));
    for (QSignalSpy *spy : {&proxyColumnAboutToBeInsertedSpy, &proxyColumnInsertedSpy}) {
        QCOMPARE(spy->count(), 1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(1).toInt(), 1);
        QCOMPARE(spyArgs.at(2).toInt(), 1);
    }
    {
        QCOMPARE(proxyDataChangedSpy.count(), SourceRowCount);
        const int firstBranchRowCount = baseModel->rowCount(baseModel->index(0, 0));
        const int branchRowCount = baseModel->rowCount(baseModel->index(1, 0));
        const auto spyArgs = proxyDataChangedSpy.takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(firstBranchRowCount, 1));
        QCOMPARE(spyArgs.at(1).value<QModelIndex>().row(), proxyModel.rowCount() - 1);
        QCOMPARE(spyArgs.at(1).value<QModelIndex>().column(), oldSourceColCount);
        QCOMPARE(spyArgs.at(1).value<QModelIndex>().model(), &proxyModel);
        QVERIFY(spyArgs.at(2).value<QVector<int>>().isEmpty());
        for (int i = 0; i < SourceRowCount - 1; ++i) {
            const auto spyArgs = proxyDataChangedSpy.takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(firstBranchRowCount + branchRowCount * i, 1));
            QCOMPARE(spyArgs.at(1).value<QModelIndex>(), proxyModel.index(firstBranchRowCount + (branchRowCount * (i + 1)) - 1, oldSourceColCount));
            QVERIFY(spyArgs.at(2).value<QVector<int>>().isEmpty());
        }
    }
    for (QSignalSpy *spy : {&sourceColumnInsertedSpy, &sourceColumnAboutToBeInsertedSpy}) {
        QCOMPARE(spy->count(), SourceRowCount);
        for (int i = 0; i < SourceRowCount; ++i) {
            const auto spyArgs = spy->takeFirst();
            QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(i, 0));
            QCOMPARE(spyArgs.at(1).toInt(), 1);
            QCOMPARE(spyArgs.at(2).toInt(), 1);
        }
    }
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_HierarchyLevelProxyModel::testRemoveColumnSource()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyColumnAboutToBeRemovedSpy(&proxyModel, &QAbstractItemModel::columnsAboutToBeRemoved);
    QSignalSpy proxyColumnRemovedSpy(&proxyModel, &QAbstractItemModel::columnsRemoved);

    // #TODO
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_HierarchyLevelProxyModel::testSetItemDataDataChanged()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QMap<int, QVariant> itemDataSet{{std::make_pair<int, QVariant>(Qt::UserRole, 5),
                                           std::make_pair<int, QVariant>(Qt::DisplayRole, QStringLiteral("Test")),
                                           std::make_pair<int, QVariant>(Qt::ToolTipRole, QStringLiteral("ToolTip"))}};
    proxyModel.setHierarchyLevel(1);
    const QModelIndex proxyIdX = proxyModel.index(1, 1);
    const QModelIndex baseIdX = baseModel->index(1, 1,baseModel->index(0,0));
    QVERIFY(proxyModel.setData(proxyIdX, Qt::AlignRight, Qt::TextAlignmentRole));
    QSignalSpy proxyDataChangeSpy(&proxyModel, &QAbstractItemModel::dataChanged);
    QSignalSpy baseDataChangeSpy(baseModel, &QAbstractItemModel::dataChanged);

    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QCOMPARE(proxyDataChangeSpy.size(), 1);
    auto argList = proxyDataChangeSpy.takeFirst();
    QCOMPARE(argList.at(0).value<QModelIndex>(), proxyIdX);
    QCOMPARE(argList.at(1).value<QModelIndex>(), proxyIdX);
    auto rolesVector = argList.at(2).value<QVector<int>>();
#ifndef SKIP_QTBUG_45114
    QCOMPARE(rolesVector.size(), 4);
    QVERIFY(!rolesVector.contains(Qt::TextAlignmentRole));
#endif
    QVERIFY(rolesVector.contains(Qt::ToolTipRole));
    QVERIFY(rolesVector.contains(Qt::EditRole));
    QVERIFY(rolesVector.contains(Qt::DisplayRole));
    QVERIFY(rolesVector.contains(Qt::UserRole));
    QCOMPARE(baseDataChangeSpy.size(), 1);
    argList = baseDataChangeSpy.takeFirst();
    QCOMPARE(argList.at(0).value<QModelIndex>(), baseIdX);
    QCOMPARE(argList.at(1).value<QModelIndex>(), baseIdX);
    rolesVector = argList.at(2).value<QVector<int>>();
#ifndef SKIP_QTBUG_45114
    QCOMPARE(rolesVector.size(), 4);
    QVERIFY(!rolesVector.contains(Qt::TextAlignmentRole));
#endif
    QVERIFY(rolesVector.contains(Qt::ToolTipRole));
    QVERIFY(rolesVector.contains(Qt::EditRole));
    QVERIFY(rolesVector.contains(Qt::DisplayRole));
    QVERIFY(rolesVector.contains(Qt::UserRole));
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}



void tst_HierarchyLevelProxyModel::testSetItemData()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    const QMap<int, QVariant> itemDataSet{{std::make_pair<int, QVariant>(Qt::UserRole, 5),
                                           std::make_pair<int, QVariant>(Qt::DisplayRole, QStringLiteral("Test")),
                                           std::make_pair<int, QVariant>(Qt::ToolTipRole, QStringLiteral("ToolTip"))}};

    QModelIndex proxyIdX = proxyModel.index(1,1);
    QModelIndex baseIndex = baseModel->index(1,1);
    QVERIFY(proxyModel.setData(proxyIdX, Qt::AlignRight, Qt::TextAlignmentRole));
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::DisplayRole).toString(), QStringLiteral("Test"));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::UserRole).toInt(), 5);
    QCOMPARE(proxyModel.data(proxyIdX, Qt::ToolTipRole).toString(), QStringLiteral("ToolTip"));
    QCOMPARE(baseIndex.data(Qt::DisplayRole).toString(), QStringLiteral("Test"));
    QCOMPARE(baseIndex.data(Qt::UserRole).toInt(), 5);
    QCOMPARE(baseIndex.data(Qt::ToolTipRole).toString(), QStringLiteral("ToolTip"));
#ifndef SKIP_QTBUG_45114
    QCOMPARE(proxyModel.data(proxyIdX, Qt::TextAlignmentRole).toInt(), Qt::AlignRight);
    QCOMPARE(baseIndex.data(Qt::TextAlignmentRole).toInt(), Qt::AlignRight);
#endif
    proxyModel.setHierarchyLevel(1);
    proxyIdX = proxyModel.index(1,1);
    baseIndex = baseModel->index(1,1,baseModel->index(0,0));
    QVERIFY(proxyModel.setData(proxyIdX, Qt::AlignRight, Qt::TextAlignmentRole));
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::DisplayRole).toString(), QStringLiteral("Test"));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::UserRole).toInt(), 5);
    QCOMPARE(proxyModel.data(proxyIdX, Qt::ToolTipRole).toString(), QStringLiteral("ToolTip"));
    QCOMPARE(baseIndex.data(Qt::DisplayRole).toString(), QStringLiteral("Test"));
    QCOMPARE(baseIndex.data(Qt::UserRole).toInt(), 5);
    QCOMPARE(baseIndex.data(Qt::ToolTipRole).toString(), QStringLiteral("ToolTip"));
#ifndef SKIP_QTBUG_45114
    QCOMPARE(proxyModel.data(proxyIdX, Qt::TextAlignmentRole).toInt(), Qt::AlignRight);
    QCOMPARE(baseIndex.data(Qt::TextAlignmentRole).toInt(), Qt::AlignRight);
#endif
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_HierarchyLevelProxyModel::createPersistentOnLayoutAboutToBeChanged()
{
    SimpleModel model(QStringList{QStringLiteral("1"), QStringLiteral("2"), QStringLiteral("3")});
    HierarchyLevelProxyModel proxy;
    new ModelTest(&proxy, &proxy);
    proxy.setSourceModel(&model);
    /* #TODO
    QList<QPersistentModelIndex> idxList;
    QSignalSpy layoutAboutToBeChangedSpy(&proxy, SIGNAL(layoutAboutToBeChanged()));
    QVERIFY(layoutAboutToBeChangedSpy.isValid());
    QSignalSpy layoutChangedSpy(&proxy, SIGNAL(layoutChanged()));
    QVERIFY(layoutChangedSpy.isValid());
    connect(&proxy, &QAbstractItemModel::layoutAboutToBeChanged, [&idxList, &proxy]() {
        idxList.clear();
        for (int row = 0; row < 4; ++row)
            idxList << QPersistentModelIndex(proxy.index(row, 0));
    });
    connect(&proxy, &QAbstractItemModel::layoutChanged, [&idxList]() {
        QCOMPARE(idxList.size(), 4);
        QCOMPARE(idxList.at(0).row(), 1);
        QCOMPARE(idxList.at(0).column(), 0);
        QCOMPARE(idxList.at(0).data().toString(), QStringLiteral("1"));
        QCOMPARE(idxList.at(1).row(), 0);
        QCOMPARE(idxList.at(1).column(), 0);
        QCOMPARE(idxList.at(1).data().toString(), QStringLiteral("0"));
        QCOMPARE(idxList.at(2).row(), 2);
        QCOMPARE(idxList.at(2).column(), 0);
        QCOMPARE(idxList.at(2).data().toString(), QStringLiteral("3"));
        QCOMPARE(idxList.at(3).row(), 3);
        QCOMPARE(idxList.at(3).column(), 0);
        QCOMPARE(idxList.at(3).data().toString(), QStringLiteral("4"));
    });
    model.setData(model.index(1, 0), QStringLiteral("0"));
    model.sort(0);
    QCOMPARE(layoutAboutToBeChangedSpy.size(), 1);
    QCOMPARE(layoutChangedSpy.size(), 1);
    */
}
void tst_HierarchyLevelProxyModel::testBuildFromScratch_data()
{
    QTest::addColumn<bool>("insertBy");
    QTest::newRow("Row First") << true;
    QTest::newRow("Column First") << false;
}

void tst_HierarchyLevelProxyModel::testBuildFromScratch()
{
    QFETCH(bool, insertBy);
#ifdef COMPLEX_MODEL_SUPPORT
    ComplexModel model;
    HierarchyLevelProxyModel proxy;
    new ModelTest(&proxy, &proxy);
    proxy.setSourceModel(&model);
    proxy.setHierarchyLevel(1);
    fillTreeModel(&model, 3, 2, insertBy);
    QCOMPARE(proxy.rowCount(), 9);
    QCOMPARE(proxy.columnCount(), 2);
    QCOMPARE(proxy.rowCount(proxy.index(0, 0)), 3);
    QCOMPARE(proxy.columnCount(proxy.index(0, 0)), 2);
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}
