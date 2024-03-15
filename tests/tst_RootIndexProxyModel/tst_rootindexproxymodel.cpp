#include "tst_rootindexproxymodel.h"
#include <QStringListModel>
#include <rootindexproxymodel.h>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include "../modeltestmanager.h"
#ifdef QTMODELUTILITIES_GENERICMODEL
#    include <genericmodel.h>
#endif

QAbstractItemModel *createTreeModel(QObject *parent)
{
    QAbstractItemModel *result = nullptr;
#ifdef COMPLEX_MODEL_SUPPORT
    result = new ComplexModel(parent);
    result->insertColumns(0, 3);
    result->insertRows(0, 3);
    for (int i = 0; i < result->rowCount(); ++i) {
        for (int j = 0; j < result->columnCount(); ++j)
            result->setData(result->index(i, j), QStringLiteral("%1,%2").arg(i).arg(j));
        const QModelIndex parIdx = result->index(i, 0);
        result->insertColumns(0, 2, parIdx);
        result->insertRows(0, 4, parIdx);
        for (int k = 0; k < result->rowCount(parIdx); ++k) {
            for (int h = 0; h < result->columnCount(parIdx); ++h)
                result->setData(result->index(k, h, parIdx), QStringLiteral("%1,%2,%3").arg(i).arg(k).arg(h));
            const QModelIndex grandParIdx = result->index(i, 0, parIdx);
            result->insertColumns(0, 2, grandParIdx);
            result->insertRows(0, 6, grandParIdx);
            for (int j = 0; j < result->rowCount(grandParIdx); ++j) {
                for (int h = 0; h < result->columnCount(grandParIdx); ++h)
                    result->setData(result->index(j, h, grandParIdx), QStringLiteral("%1,%2,%3,%4").arg(i).arg(k).arg(j).arg(h));
            }
        }
    }
#endif
    return result;
}

void tst_RootIndexProxyModel::autoParent()
{
    QObject *parentObj = new QObject;
    auto testItem = new RootIndexProxyModel(parentObj);
    QSignalSpy testItemDestroyedSpy(testItem, SIGNAL(destroyed(QObject *)));
    QVERIFY(testItemDestroyedSpy.isValid());
    delete parentObj;
    QCOMPARE(testItemDestroyedSpy.count(), 1);
}

void tst_RootIndexProxyModel::showRoot()
{

    QFETCH(QAbstractItemModel *, baseModel);
    QFETCH(QModelIndex, root);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(root);
    QCOMPARE(proxyModel.rootIndex(), root);
    compareModels(baseModel, &proxyModel, root, QModelIndex());
    baseModel->deleteLater();
}

void tst_RootIndexProxyModel::showRoot_data()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<QModelIndex>("root");
    QTest::newRow("Parent") << createTreeModel(nullptr) << QModelIndex();
    QAbstractItemModel *baseModel = createTreeModel(nullptr);
    QTest::newRow("Child") << baseModel << baseModel->index(1, 0);
    baseModel = createTreeModel(nullptr);
    QTest::newRow("Grandchild") << baseModel << baseModel->index(2, 0, baseModel->index(1, 0));
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}
void tst_RootIndexProxyModel::replaceModel()
{

    QFETCH(QAbstractItemModel *, baseModel);
    QFETCH(QModelIndex, baseRoot);
    QFETCH(QAbstractItemModel *, replaceModel);
    QFETCH(QModelIndex, replaceRoot);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseRoot);
    QCOMPARE(proxyModel.rootIndex(), baseRoot);
    proxyModel.setSourceModel(replaceModel);
    QCOMPARE(proxyModel.rootIndex(), QModelIndex());
    compareModels(replaceModel, &proxyModel, QModelIndex(), QModelIndex());
    proxyModel.setRootIndex(replaceRoot);
    QCOMPARE(proxyModel.rootIndex(), replaceRoot);
    baseModel->deleteLater();
    replaceModel->deleteLater();
}
void tst_RootIndexProxyModel::replaceModel_data()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<QModelIndex>("baseRoot");
    QTest::addColumn<QAbstractItemModel *>("replaceModel");
    QTest::addColumn<QModelIndex>("replaceRoot");

    QAbstractItemModel *baseModel = createTreeModel(this);
    QAbstractItemModel *replaceModel = createTreeModel(this);
    QTest::newRow("Child") << baseModel << baseModel->index(1, 0) << replaceModel << replaceModel->index(0, 0);
    baseModel = createTreeModel(this);
    replaceModel = createTreeModel(this);
    QTest::newRow("Grandchild") << baseModel << baseModel->index(1, 0) << replaceModel << replaceModel->index(2, 0, replaceModel->index(1, 0));
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RootIndexProxyModel::sourceDataChanged()
{

    QFETCH(QAbstractItemModel *, baseModel);
    QFETCH(QModelIndex, changeIndex);
    QFETCH(int, expectedSignals);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseModel->index(1, 0, baseModel->index(1, 0)));
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(proxyDataChangeSpy.isValid());
    baseModel->setData(changeIndex, QStringLiteral("Hello"));
    QCOMPARE(proxyDataChangeSpy.count(), expectedSignals);
}

void tst_RootIndexProxyModel::sourceDataChanged_data()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<QModelIndex>("changeIndex");
    QTest::addColumn<int>("expectedSignals");
    QAbstractItemModel *baseModel = createTreeModel(this);
    QTest::newRow("Sibling of Root") << baseModel << baseModel->index(0, 0, baseModel->index(1, 0)) << 0;
    baseModel = createTreeModel(this);
    QTest::newRow("Ancestor of Root") << baseModel << baseModel->index(0, 0) << 0;
    baseModel = createTreeModel(this);
    QTest::newRow("Root") << baseModel << baseModel->index(1, 0, baseModel->index(1, 0)) << 0;
    baseModel = createTreeModel(this);
    QTest::newRow("Descendant of Root") << baseModel << baseModel->index(0, 0, baseModel->index(1, 0, baseModel->index(1, 0))) << 1;
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RootIndexProxyModel::insertRow()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseModel->index(1, 0));
    QSignalSpy proxyRowsAboutToBeInsertedSpy(&proxyModel, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(proxyRowsAboutToBeInsertedSpy.isValid());
    QSignalSpy proxyRowsInsertedSpy(&proxyModel, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(proxyRowsInsertedSpy.isValid());
    QSignalSpy sourceRowsAboutToBeInsertedSpy(baseModel, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(sourceRowsAboutToBeInsertedSpy.isValid());
    QSignalSpy sourceRowsInsertedSpy(baseModel, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(sourceRowsInsertedSpy.isValid());
    int oldRowCount = baseModel->rowCount(baseModel->index(1, 0));
    QCOMPARE(oldRowCount, proxyModel.rowCount());
    proxyModel.insertRows(1, 1);
    QCOMPARE(oldRowCount + 1, proxyModel.rowCount());
    QCOMPARE(oldRowCount + 1, baseModel->rowCount(baseModel->index(1, 0)));
    QCOMPARE(proxyRowsAboutToBeInsertedSpy.count(), 1);
    QList<QVariant> arguments = proxyRowsAboutToBeInsertedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    QCOMPARE(proxyRowsInsertedSpy.count(), 1);
    arguments = proxyRowsInsertedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    QCOMPARE(sourceRowsAboutToBeInsertedSpy.count(), 1);
    arguments = sourceRowsAboutToBeInsertedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), baseModel->index(1, 0));
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    QCOMPARE(sourceRowsInsertedSpy.count(), 1);
    arguments = sourceRowsInsertedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), baseModel->index(1, 0));
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RootIndexProxyModel::insertColumn()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseModel->index(1, 0));
    QSignalSpy proxyColumnsAboutToBeInsertedSpy(&proxyModel, SIGNAL(columnsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(proxyColumnsAboutToBeInsertedSpy.isValid());
    QSignalSpy proxyColumnsInsertedSpy(&proxyModel, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(proxyColumnsInsertedSpy.isValid());
    QSignalSpy sourceColumnsAboutToBeInsertedSpy(baseModel, SIGNAL(columnsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(sourceColumnsAboutToBeInsertedSpy.isValid());
    QSignalSpy sourceColumnsInsertedSpy(baseModel, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(sourceColumnsInsertedSpy.isValid());
    int oldColCount = baseModel->columnCount(baseModel->index(1, 0));
    QCOMPARE(oldColCount, proxyModel.columnCount());
    proxyModel.insertColumns(1, 1);
    QCOMPARE(oldColCount + 1, proxyModel.columnCount());
    QCOMPARE(oldColCount + 1, baseModel->columnCount(baseModel->index(1, 0)));
    QCOMPARE(proxyColumnsAboutToBeInsertedSpy.count(), 1);
    QList<QVariant> arguments = proxyColumnsAboutToBeInsertedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    QCOMPARE(proxyColumnsInsertedSpy.count(), 1);
    arguments = proxyColumnsInsertedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    QCOMPARE(sourceColumnsAboutToBeInsertedSpy.count(), 1);
    arguments = sourceColumnsAboutToBeInsertedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), baseModel->index(1, 0));
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    QCOMPARE(sourceColumnsInsertedSpy.count(), 1);
    arguments = sourceColumnsInsertedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), baseModel->index(1, 0));
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RootIndexProxyModel::removeRow()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseModel->index(1, 0));
    QSignalSpy proxyRowsAboutToBeRemovedSpy(&proxyModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(proxyRowsAboutToBeRemovedSpy.isValid());
    QSignalSpy proxyRowsRemovedSpy(&proxyModel, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    QVERIFY(proxyRowsRemovedSpy.isValid());
    QSignalSpy sourceRowsAboutToBeRemovedSpy(baseModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(sourceRowsAboutToBeRemovedSpy.isValid());
    QSignalSpy sourceRowsRemovedSpy(baseModel, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    QVERIFY(sourceRowsRemovedSpy.isValid());
    int oldRowCount = baseModel->rowCount(baseModel->index(1, 0));
    QCOMPARE(oldRowCount, proxyModel.rowCount());
    proxyModel.removeRows(1, 1);
    QCOMPARE(oldRowCount - 1, proxyModel.rowCount());
    QCOMPARE(oldRowCount - 1, baseModel->rowCount(baseModel->index(1, 0)));
    QCOMPARE(proxyRowsAboutToBeRemovedSpy.count(), 1);
    QList<QVariant> arguments = proxyRowsAboutToBeRemovedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    QCOMPARE(proxyRowsRemovedSpy.count(), 1);
    arguments = proxyRowsRemovedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    QCOMPARE(sourceRowsAboutToBeRemovedSpy.count(), 1);
    arguments = sourceRowsAboutToBeRemovedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), baseModel->index(1, 0));
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    QCOMPARE(sourceRowsRemovedSpy.count(), 1);
    arguments = sourceRowsRemovedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), baseModel->index(1, 0));
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RootIndexProxyModel::removeColumn()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseModel->index(1, 0));
    QSignalSpy proxyColumnsAboutToBeRemovedSpy(&proxyModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(proxyColumnsAboutToBeRemovedSpy.isValid());
    QSignalSpy proxyColumnsRemovedSpy(&proxyModel, SIGNAL(columnsRemoved(QModelIndex, int, int)));
    QVERIFY(proxyColumnsRemovedSpy.isValid());
    QSignalSpy sourceColumnsAboutToBeRemovedSpy(baseModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(sourceColumnsAboutToBeRemovedSpy.isValid());
    QSignalSpy sourceColumnsRemovedSpy(baseModel, SIGNAL(columnsRemoved(QModelIndex, int, int)));
    QVERIFY(sourceColumnsRemovedSpy.isValid());
    int oldColCount = baseModel->columnCount(baseModel->index(1, 0));
    QCOMPARE(oldColCount, proxyModel.columnCount());
    proxyModel.removeColumns(1, 1);
    QCOMPARE(oldColCount - 1, proxyModel.columnCount());
    QCOMPARE(oldColCount - 1, baseModel->columnCount(baseModel->index(1, 0)));
    QCOMPARE(proxyColumnsAboutToBeRemovedSpy.count(), 1);
    QList<QVariant> arguments = proxyColumnsAboutToBeRemovedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    QCOMPARE(proxyColumnsRemovedSpy.count(), 1);
    arguments = proxyColumnsRemovedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    QCOMPARE(sourceColumnsAboutToBeRemovedSpy.count(), 1);
    arguments = sourceColumnsAboutToBeRemovedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), baseModel->index(1, 0));
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    QCOMPARE(sourceColumnsRemovedSpy.count(), 1);
    arguments = sourceColumnsRemovedSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), baseModel->index(1, 0));
    QCOMPARE(arguments.at(1).value<int>(), 1);
    QCOMPARE(arguments.at(2).value<int>(), 1);
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RootIndexProxyModel::bug60Row()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    QSignalSpy baseRowsAboutToBeRemovedSpy(baseModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(baseRowsAboutToBeRemovedSpy.isValid());
    QSignalSpy baseRowsRemovedSpy(baseModel, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    QVERIFY(baseRowsRemovedSpy.isValid());
    QSignalSpy proxyResetSpy(&proxyModel, SIGNAL(modelReset()));
    QVERIFY(proxyResetSpy.isValid());
    QSignalSpy proxyAboutToBeResetSpy(&proxyModel, SIGNAL(modelAboutToBeReset()));
    QVERIFY(proxyAboutToBeResetSpy.isValid());
    QSignalSpy proxyRootChangedSpy(&proxyModel, SIGNAL(rootIndexChanged()));
    QVERIFY(proxyRootChangedSpy.isValid());
    connect(baseModel,&QAbstractItemModel::rowsAboutToBeRemoved,[&](){
        QCOMPARE(baseRowsAboutToBeRemovedSpy.count(), 1);
        QCOMPARE(baseRowsRemovedSpy.count(), 0);
        QCOMPARE(proxyResetSpy.count(), 0);
        QCOMPARE(proxyAboutToBeResetSpy.count(), 0);
    });
    connect(&proxyModel,&QAbstractItemModel::modelAboutToBeReset,[&](){
        QCOMPARE(baseRowsAboutToBeRemovedSpy.count(), 1);
        QCOMPARE(baseRowsRemovedSpy.count(), 0);
        QCOMPARE(proxyResetSpy.count(), 0);
        QCOMPARE(proxyAboutToBeResetSpy.count(), 1);
    });
    connect(baseModel,&QAbstractItemModel::rowsRemoved,[&](){
        QCOMPARE(baseRowsAboutToBeRemovedSpy.count(), 1);
        QCOMPARE(baseRowsRemovedSpy.count(), 1);
        QCOMPARE(proxyResetSpy.count(), 0);
        QCOMPARE(proxyAboutToBeResetSpy.count(), 1);
    });
    connect(&proxyModel,&QAbstractItemModel::modelReset,[&](){
        QCOMPARE(baseRowsAboutToBeRemovedSpy.count(), 1);
        QCOMPARE(baseRowsRemovedSpy.count(), 1);
        QCOMPARE(proxyResetSpy.count(), 1);
        QCOMPARE(proxyAboutToBeResetSpy.count(), 1);
    });
    proxyModel.blockSignals(true);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseModel->index(0,0));
    proxyModel.blockSignals(false);
    QVERIFY(baseModel->removeRow(0));
    QCOMPARE(proxyModel.rootIndex(), QModelIndex());
    QCOMPARE(baseRowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(baseRowsRemovedSpy.count(), 1);
    QCOMPARE(proxyResetSpy.count(), 1);
    QCOMPARE(proxyAboutToBeResetSpy.count(), 1);
    QCOMPARE(proxyRootChangedSpy.count(), 1);
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}
void tst_RootIndexProxyModel::bug60Col()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    QSignalSpy baseColsAboutToBeRemovedSpy(baseModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(baseColsAboutToBeRemovedSpy.isValid());
    QSignalSpy baseColsRemovedSpy(baseModel, SIGNAL(columnsRemoved(QModelIndex, int, int)));
    QVERIFY(baseColsRemovedSpy.isValid());
    QSignalSpy proxyResetSpy(&proxyModel, SIGNAL(modelReset()));
    QVERIFY(proxyResetSpy.isValid());
    QSignalSpy proxyAboutToBeResetSpy(&proxyModel, SIGNAL(modelAboutToBeReset()));
    QVERIFY(proxyAboutToBeResetSpy.isValid());
    QSignalSpy proxyRootChangedSpy(&proxyModel, SIGNAL(rootIndexChanged()));
    QVERIFY(proxyRootChangedSpy.isValid());
    connect(baseModel,&QAbstractItemModel::rowsAboutToBeRemoved,[&](){
        QCOMPARE(baseColsAboutToBeRemovedSpy.count(), 1);
        QCOMPARE(baseColsRemovedSpy.count(), 0);
        QCOMPARE(proxyResetSpy.count(), 0);
        QCOMPARE(proxyAboutToBeResetSpy.count(), 0);
    });
    connect(&proxyModel,&QAbstractItemModel::modelAboutToBeReset,[&](){
        QCOMPARE(baseColsAboutToBeRemovedSpy.count(), 1);
        QCOMPARE(baseColsRemovedSpy.count(), 0);
        QCOMPARE(proxyResetSpy.count(), 0);
        QCOMPARE(proxyAboutToBeResetSpy.count(), 1);
    });
    connect(baseModel,&QAbstractItemModel::rowsRemoved,[&](){
        QCOMPARE(baseColsAboutToBeRemovedSpy.count(), 1);
        QCOMPARE(baseColsRemovedSpy.count(), 1);
        QCOMPARE(proxyResetSpy.count(), 0);
        QCOMPARE(proxyAboutToBeResetSpy.count(), 1);
    });
    connect(&proxyModel,&QAbstractItemModel::modelReset,[&](){
        QCOMPARE(baseColsAboutToBeRemovedSpy.count(), 1);
        QCOMPARE(baseColsRemovedSpy.count(), 1);
        QCOMPARE(proxyResetSpy.count(), 1);
        QCOMPARE(proxyAboutToBeResetSpy.count(), 1);
    });
    proxyModel.blockSignals(true);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseModel->index(0,0));
    proxyModel.blockSignals(false);
    QVERIFY(baseModel->removeColumn(0));
    QCOMPARE(proxyModel.rootIndex(), QModelIndex());
    QCOMPARE(baseColsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(baseColsRemovedSpy.count(), 1);
    QCOMPARE(proxyResetSpy.count(), 1);
    QCOMPARE(proxyAboutToBeResetSpy.count(), 1);
    QCOMPARE(proxyRootChangedSpy.count(), 1);
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RootIndexProxyModel::bug53Rows()
{
#ifdef COMPLEX_MODEL_SUPPORT
    ComplexModel baseModel;
    QVERIFY(baseModel.insertColumn(0));
    QVERIFY(baseModel.insertRows(0, 2));
    QVERIFY(baseModel.insertColumn(0, baseModel.index(0, 0)));
    QVERIFY(baseModel.insertRows(0, 2, baseModel.index(0, 0)));
    QVERIFY(baseModel.insertColumn(0, baseModel.index(1, 0)));
    QVERIFY(baseModel.insertRows(0, 1, baseModel.index(1, 0)));
    QVERIFY(baseModel.setData(baseModel.index(0, 0), QChar(QLatin1Char('A'))));
    QVERIFY(baseModel.setData(baseModel.index(0, 0, baseModel.index(0, 0)), QChar(QLatin1Char('C'))));
    QVERIFY(baseModel.setData(baseModel.index(1, 0, baseModel.index(0, 0)), QChar(QLatin1Char('D'))));
    QVERIFY(baseModel.setData(baseModel.index(1, 0), QChar(QLatin1Char('B'))));
    QVERIFY(baseModel.setData(baseModel.index(0, 0, baseModel.index(1, 0)), QChar(QLatin1Char('E'))));
    RootIndexProxyModel proxyModel1;
    new ModelTest(&proxyModel1, &baseModel);
    proxyModel1.setSourceModel(&baseModel);
    proxyModel1.setRootIndex(baseModel.index(0, 0));
    RootIndexProxyModel proxyModel2;
    new ModelTest(&proxyModel2, &baseModel);
    proxyModel2.setSourceModel(&baseModel);
    proxyModel2.setRootIndex(baseModel.index(1, 0));
    QSignalSpy proxy1RowsAboutToBeInsertedSpy(&proxyModel1, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(proxy1RowsAboutToBeInsertedSpy.isValid());
    QSignalSpy proxy1RowsInsertedSpy(&proxyModel1, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(proxy1RowsInsertedSpy.isValid());
    QSignalSpy proxy2RowsAboutToBeInsertedSpy(&proxyModel2, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(proxy2RowsAboutToBeInsertedSpy.isValid());
    QSignalSpy proxy2RowsInsertedSpy(&proxyModel2, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(proxy2RowsInsertedSpy.isValid());
    QSignalSpy proxy1RowsAboutToBeRemovedSpy(&proxyModel1, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(proxy1RowsAboutToBeRemovedSpy.isValid());
    QSignalSpy proxy1RowsRemovedSpy(&proxyModel1, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    QVERIFY(proxy1RowsRemovedSpy.isValid());
    QSignalSpy proxy2RowsAboutToBeRemovedSpy(&proxyModel2, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(proxy2RowsAboutToBeRemovedSpy.isValid());
    QSignalSpy proxy2RowsRemovedSpy(&proxyModel2, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    QVERIFY(proxy2RowsRemovedSpy.isValid());
    QSignalSpy proxy1rowsMovedSpy(&proxyModel1, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy1rowsMovedSpy.isValid());
    QSignalSpy proxy1rowsAboutToBeMovedSpy(&proxyModel1, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy1rowsAboutToBeMovedSpy.isValid());
    QSignalSpy proxy2rowsMovedSpy(&proxyModel2, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy2rowsMovedSpy.isValid());
    QSignalSpy proxy2rowsAboutToBeMovedSpy(&proxyModel2, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy2rowsAboutToBeMovedSpy.isValid());
    QVERIFY(baseModel.insertRow(2, baseModel.index(0, 0)));
    QCOMPARE(proxyModel1.rowCount(), 3);
    QCOMPARE(proxyModel2.rowCount(), 1);
    QVERIFY(proxy2RowsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy2RowsInsertedSpy.isEmpty());
    QVERIFY(proxy1RowsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy1RowsRemovedSpy.isEmpty());
    QVERIFY(proxy2RowsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy2RowsRemovedSpy.isEmpty());
    QVERIFY(proxy1rowsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy1rowsMovedSpy.isEmpty());
    QVERIFY(proxy2rowsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy2rowsMovedSpy.isEmpty());
    QCOMPARE(proxy1RowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(proxy1RowsInsertedSpy.count(), 1);
    proxy1RowsAboutToBeInsertedSpy.clear();
    proxy1RowsInsertedSpy.clear();

    QVERIFY(baseModel.removeRow(2, baseModel.index(0, 0)));
    QCOMPARE(proxyModel1.rowCount(), 2);
    QCOMPARE(proxyModel2.rowCount(), 1);
    QCOMPARE(proxy1RowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(proxy1RowsAboutToBeRemovedSpy.count(), 1);
    QVERIFY(proxy1RowsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy1RowsInsertedSpy.isEmpty());
    QVERIFY(proxy2RowsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy2RowsInsertedSpy.isEmpty());
    QVERIFY(proxy2RowsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy2RowsRemovedSpy.isEmpty());
    QVERIFY(proxy1rowsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy1rowsMovedSpy.isEmpty());
    QVERIFY(proxy2rowsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy2rowsMovedSpy.isEmpty());
    proxy1RowsAboutToBeRemovedSpy.clear();
    proxy1RowsAboutToBeRemovedSpy.clear();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RootIndexProxyModel::bug53Cols()
{
#ifdef COMPLEX_MODEL_SUPPORT
    ComplexModel baseModel;
    QVERIFY(baseModel.insertColumn(0));
    QVERIFY(baseModel.insertRows(0, 2));
    QVERIFY(baseModel.insertColumn(0, baseModel.index(0, 0)));
    QVERIFY(baseModel.insertRows(0, 2, baseModel.index(0, 0)));
    QVERIFY(baseModel.insertColumn(0, baseModel.index(1, 0)));
    QVERIFY(baseModel.insertRows(0, 1, baseModel.index(1, 0)));
    QVERIFY(baseModel.setData(baseModel.index(0, 0), QChar(QLatin1Char('A'))));
    QVERIFY(baseModel.setData(baseModel.index(0, 0, baseModel.index(0, 0)), QChar(QLatin1Char('C'))));
    QVERIFY(baseModel.setData(baseModel.index(1, 0, baseModel.index(0, 0)), QChar(QLatin1Char('D'))));
    QVERIFY(baseModel.setData(baseModel.index(1, 0), QChar(QLatin1Char('B'))));
    QVERIFY(baseModel.setData(baseModel.index(0, 0, baseModel.index(1, 0)), QChar(QLatin1Char('E'))));
    RootIndexProxyModel proxyModel1;
    new ModelTest(&proxyModel1, &baseModel);
    proxyModel1.setSourceModel(&baseModel);
    proxyModel1.setRootIndex(baseModel.index(0, 0));
    RootIndexProxyModel proxyModel2;
    new ModelTest(&proxyModel2, &baseModel);
    proxyModel2.setSourceModel(&baseModel);
    proxyModel2.setRootIndex(baseModel.index(1, 0));
    QSignalSpy proxy1ColsAboutToBeInsertedSpy(&proxyModel1, SIGNAL(columnsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(proxy1ColsAboutToBeInsertedSpy.isValid());
    QSignalSpy proxy1ColsInsertedSpy(&proxyModel1, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(proxy1ColsInsertedSpy.isValid());
    QSignalSpy proxy2ColsAboutToBeInsertedSpy(&proxyModel2, SIGNAL(columnsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(proxy2ColsAboutToBeInsertedSpy.isValid());
    QSignalSpy proxy2ColsInsertedSpy(&proxyModel2, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(proxy2ColsInsertedSpy.isValid());
    QSignalSpy proxy1ColsAboutToBeRemovedSpy(&proxyModel1, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(proxy1ColsAboutToBeRemovedSpy.isValid());
    QSignalSpy proxy1ColsRemovedSpy(&proxyModel1, SIGNAL(columnsRemoved(QModelIndex, int, int)));
    QVERIFY(proxy1ColsRemovedSpy.isValid());
    QSignalSpy proxy2ColsAboutToBeRemovedSpy(&proxyModel2, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(proxy2ColsAboutToBeRemovedSpy.isValid());
    QSignalSpy proxy2ColsRemovedSpy(&proxyModel2, SIGNAL(columnsRemoved(QModelIndex, int, int)));
    QVERIFY(proxy2ColsRemovedSpy.isValid());
    QSignalSpy proxy1ColsMovedSpy(&proxyModel1, SIGNAL(columnsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy1ColsMovedSpy.isValid());
    QSignalSpy proxy1ColsAboutToBeMovedSpy(&proxyModel1, SIGNAL(columnsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy1ColsAboutToBeMovedSpy.isValid());
    QSignalSpy proxy2ColsMovedSpy(&proxyModel2, SIGNAL(columnsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy2ColsMovedSpy.isValid());
    QSignalSpy proxy2ColsAboutToBeMovedSpy(&proxyModel2, SIGNAL(columnsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy2ColsAboutToBeMovedSpy.isValid());
    QVERIFY(baseModel.insertColumn(1, baseModel.index(0, 0)));
    QCOMPARE(proxyModel1.columnCount(), 2);
    QCOMPARE(proxyModel2.columnCount(), 1);
    QVERIFY(proxy2ColsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy2ColsInsertedSpy.isEmpty());
    QVERIFY(proxy1ColsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy1ColsRemovedSpy.isEmpty());
    QVERIFY(proxy2ColsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy2ColsRemovedSpy.isEmpty());
    QVERIFY(proxy1ColsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy1ColsMovedSpy.isEmpty());
    QVERIFY(proxy2ColsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy2ColsMovedSpy.isEmpty());
    QCOMPARE(proxy1ColsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(proxy1ColsInsertedSpy.count(), 1);
    proxy1ColsAboutToBeInsertedSpy.clear();
    proxy1ColsInsertedSpy.clear();

    QVERIFY(baseModel.removeColumn(1, baseModel.index(0, 0)));
    QCOMPARE(proxyModel1.columnCount(), 1);
    QCOMPARE(proxyModel2.columnCount(), 1);
    QCOMPARE(proxy1ColsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(proxy1ColsAboutToBeRemovedSpy.count(), 1);
    QVERIFY(proxy1ColsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy1ColsInsertedSpy.isEmpty());
    QVERIFY(proxy2ColsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy2ColsInsertedSpy.isEmpty());
    QVERIFY(proxy2ColsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy2ColsRemovedSpy.isEmpty());
    QVERIFY(proxy1ColsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy1ColsMovedSpy.isEmpty());
    QVERIFY(proxy2ColsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy2ColsMovedSpy.isEmpty());
    proxy1ColsAboutToBeRemovedSpy.clear();
    proxy1ColsAboutToBeRemovedSpy.clear();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

#ifdef QTMODELUTILITIES_GENERICMODEL
#    include <genericmodel.h>
#endif
void tst_RootIndexProxyModel::bug53MoveCols()
{
#ifdef QTMODELUTILITIES_GENERICMODEL
    GenericModel baseModel;
    QVERIFY(baseModel.insertColumn(0));
    QVERIFY(baseModel.insertRows(0, 2));
    QVERIFY(baseModel.insertColumn(0, baseModel.index(0, 0)));
    QVERIFY(baseModel.insertRows(0, 2, baseModel.index(0, 0)));
    QVERIFY(baseModel.insertColumn(0, baseModel.index(1, 0)));
    QVERIFY(baseModel.insertRows(0, 1, baseModel.index(1, 0)));
    QVERIFY(baseModel.setData(baseModel.index(0, 0), QChar(QLatin1Char('A'))));
    QVERIFY(baseModel.setData(baseModel.index(0, 0, baseModel.index(0, 0)), QChar(QLatin1Char('C'))));
    QVERIFY(baseModel.setData(baseModel.index(1, 0, baseModel.index(0, 0)), QChar(QLatin1Char('D'))));
    QVERIFY(baseModel.setData(baseModel.index(1, 0), QChar(QLatin1Char('B'))));
    QVERIFY(baseModel.setData(baseModel.index(0, 0, baseModel.index(1, 0)), QChar(QLatin1Char('E'))));
    RootIndexProxyModel proxyModel1;
    new ModelTest(&proxyModel1, &baseModel);
    proxyModel1.setSourceModel(&baseModel);
    proxyModel1.setRootIndex(baseModel.index(0, 0));
    RootIndexProxyModel proxyModel2;
    new ModelTest(&proxyModel2, &baseModel);
    proxyModel2.setSourceModel(&baseModel);
    proxyModel2.setRootIndex(baseModel.index(1, 0));
    QSignalSpy proxy1ColumnsAboutToBeInsertedSpy(&proxyModel1, SIGNAL(columnsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(proxy1ColumnsAboutToBeInsertedSpy.isValid());
    QSignalSpy proxy1ColumnsInsertedSpy(&proxyModel1, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(proxy1ColumnsInsertedSpy.isValid());
    QSignalSpy proxy2ColumnsAboutToBeInsertedSpy(&proxyModel2, SIGNAL(columnsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(proxy2ColumnsAboutToBeInsertedSpy.isValid());
    QSignalSpy proxy2ColumnsInsertedSpy(&proxyModel2, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(proxy2ColumnsInsertedSpy.isValid());
    QSignalSpy proxy1ColumnsAboutToBeRemovedSpy(&proxyModel1, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(proxy1ColumnsAboutToBeRemovedSpy.isValid());
    QSignalSpy proxy1ColumnsRemovedSpy(&proxyModel1, SIGNAL(columnsRemoved(QModelIndex, int, int)));
    QVERIFY(proxy1ColumnsRemovedSpy.isValid());
    QSignalSpy proxy2ColumnsAboutToBeRemovedSpy(&proxyModel2, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(proxy2ColumnsAboutToBeRemovedSpy.isValid());
    QSignalSpy proxy2ColumnsRemovedSpy(&proxyModel2, SIGNAL(columnsRemoved(QModelIndex, int, int)));
    QVERIFY(proxy2ColumnsRemovedSpy.isValid());
    QSignalSpy proxy1ColumnsMovedSpy(&proxyModel1, SIGNAL(columnsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy1ColumnsMovedSpy.isValid());
    QSignalSpy proxy1ColumnsAboutToBeMovedSpy(&proxyModel1, SIGNAL(columnsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy1ColumnsAboutToBeMovedSpy.isValid());
    QSignalSpy proxy2ColumnsMovedSpy(&proxyModel2, SIGNAL(columnsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy2ColumnsMovedSpy.isValid());
    QSignalSpy proxy2ColumnsAboutToBeMovedSpy(&proxyModel2, SIGNAL(columnsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy2ColumnsAboutToBeMovedSpy.isValid());
    QVERIFY(baseModel.insertColumn(1, baseModel.index(0, 0)));
    QCOMPARE(proxyModel1.columnCount(), 2);
    QCOMPARE(proxyModel2.columnCount(), 1);
    QVERIFY(proxy2ColumnsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy2ColumnsInsertedSpy.isEmpty());
    QVERIFY(proxy1ColumnsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy1ColumnsRemovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsRemovedSpy.isEmpty());
    QVERIFY(proxy1ColumnsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy1ColumnsMovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsMovedSpy.isEmpty());
    QCOMPARE(proxy1ColumnsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(proxy1ColumnsInsertedSpy.count(), 1);
    proxy1ColumnsAboutToBeInsertedSpy.clear();
    proxy1ColumnsInsertedSpy.clear();

    QVERIFY(baseModel.moveColumn(baseModel.index(0, 0), 1, baseModel.index(0, 0), 0));
    QCOMPARE(proxyModel1.columnCount(), 2);
    QCOMPARE(proxyModel2.columnCount(), 1);
    QCOMPARE(proxy1ColumnsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(proxy1ColumnsMovedSpy.count(), 1);
    QVERIFY(proxy2ColumnsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsMovedSpy.isEmpty());
    QVERIFY(proxy1ColumnsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy1ColumnsInsertedSpy.isEmpty());
    QVERIFY(proxy2ColumnsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy2ColumnsInsertedSpy.isEmpty());
    QVERIFY(proxy1ColumnsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy1ColumnsRemovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsRemovedSpy.isEmpty());
    proxy1ColumnsAboutToBeMovedSpy.clear();
    proxy1ColumnsMovedSpy.clear();

    QVERIFY(baseModel.moveColumn(baseModel.index(0, 0), 0, QModelIndex(), 0));
    QCOMPARE(proxyModel1.columnCount(), 1);
    QCOMPARE(proxyModel2.columnCount(), 1);
    QCOMPARE(proxy1ColumnsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(proxy1ColumnsRemovedSpy.count(), 1);
    QVERIFY(proxy2ColumnsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsMovedSpy.isEmpty());
    QVERIFY(proxy1ColumnsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy1ColumnsInsertedSpy.isEmpty());
    QVERIFY(proxy2ColumnsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy2ColumnsInsertedSpy.isEmpty());
    QVERIFY(proxy1ColumnsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy1ColumnsMovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsRemovedSpy.isEmpty());
    proxy1ColumnsAboutToBeRemovedSpy.clear();
    proxy1ColumnsRemovedSpy.clear();

    QVERIFY(baseModel.moveColumn(QModelIndex(), 0, baseModel.index(0, 1), 0));
    QCOMPARE(proxyModel1.columnCount(), 2);
    QCOMPARE(proxyModel2.columnCount(), 1);
    QVERIFY(proxy1ColumnsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy1ColumnsMovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsMovedSpy.isEmpty());
    QCOMPARE(proxy1ColumnsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(proxy1ColumnsInsertedSpy.count(), 1);
    QVERIFY(proxy2ColumnsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy2ColumnsInsertedSpy.isEmpty());
    QVERIFY(proxy1ColumnsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy1ColumnsRemovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy2ColumnsRemovedSpy.isEmpty());
#else
    QSKIP("This test requires the GenericModel module");
#endif
}


void tst_RootIndexProxyModel::bug53MoveRows()
{
#ifdef QTMODELUTILITIES_GENERICMODEL
    GenericModel baseModel;
    QVERIFY(baseModel.insertColumn(0));
    QVERIFY(baseModel.insertRows(0, 2));
    QVERIFY(baseModel.insertColumn(0, baseModel.index(0, 0)));
    QVERIFY(baseModel.insertRows(0, 2, baseModel.index(0, 0)));
    QVERIFY(baseModel.insertColumn(0, baseModel.index(1, 0)));
    QVERIFY(baseModel.insertRows(0, 1, baseModel.index(1, 0)));
    QVERIFY(baseModel.setData(baseModel.index(0, 0), QChar(QLatin1Char('A'))));
    QVERIFY(baseModel.setData(baseModel.index(0, 0, baseModel.index(0, 0)), QChar(QLatin1Char('C'))));
    QVERIFY(baseModel.setData(baseModel.index(1, 0, baseModel.index(0, 0)), QChar(QLatin1Char('D'))));
    QVERIFY(baseModel.setData(baseModel.index(1, 0), QChar(QLatin1Char('B'))));
    QVERIFY(baseModel.setData(baseModel.index(0, 0, baseModel.index(1, 0)), QChar(QLatin1Char('E'))));
    RootIndexProxyModel proxyModel1;
    new ModelTest(&proxyModel1, &baseModel);
    proxyModel1.setSourceModel(&baseModel);
    proxyModel1.setRootIndex(baseModel.index(0, 0));
    RootIndexProxyModel proxyModel2;
    new ModelTest(&proxyModel2, &baseModel);
    proxyModel2.setSourceModel(&baseModel);
    proxyModel2.setRootIndex(baseModel.index(1, 0));
    QSignalSpy proxy1RowsAboutToBeInsertedSpy(&proxyModel1, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(proxy1RowsAboutToBeInsertedSpy.isValid());
    QSignalSpy proxy1RowsInsertedSpy(&proxyModel1, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(proxy1RowsInsertedSpy.isValid());
    QSignalSpy proxy2RowsAboutToBeInsertedSpy(&proxyModel2, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(proxy2RowsAboutToBeInsertedSpy.isValid());
    QSignalSpy proxy2RowsInsertedSpy(&proxyModel2, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(proxy2RowsInsertedSpy.isValid());
    QSignalSpy proxy1RowsAboutToBeRemovedSpy(&proxyModel1, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(proxy1RowsAboutToBeRemovedSpy.isValid());
    QSignalSpy proxy1RowsRemovedSpy(&proxyModel1, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    QVERIFY(proxy1RowsRemovedSpy.isValid());
    QSignalSpy proxy2RowsAboutToBeRemovedSpy(&proxyModel2, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(proxy2RowsAboutToBeRemovedSpy.isValid());
    QSignalSpy proxy2RowsRemovedSpy(&proxyModel2, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    QVERIFY(proxy2RowsRemovedSpy.isValid());
    QSignalSpy proxy1rowsMovedSpy(&proxyModel1, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy1rowsMovedSpy.isValid());
    QSignalSpy proxy1rowsAboutToBeMovedSpy(&proxyModel1, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy1rowsAboutToBeMovedSpy.isValid());
    QSignalSpy proxy2rowsMovedSpy(&proxyModel2, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy2rowsMovedSpy.isValid());
    QSignalSpy proxy2rowsAboutToBeMovedSpy(&proxyModel2, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(proxy2rowsAboutToBeMovedSpy.isValid());
    QVERIFY(baseModel.insertRow(2, baseModel.index(0, 0)));
    QCOMPARE(proxyModel1.rowCount(), 3);
    QCOMPARE(proxyModel2.rowCount(), 1);
    QVERIFY(proxy2RowsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy2RowsInsertedSpy.isEmpty());
    QVERIFY(proxy1RowsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy1RowsRemovedSpy.isEmpty());
    QVERIFY(proxy2RowsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy2RowsRemovedSpy.isEmpty());
    QVERIFY(proxy1rowsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy1rowsMovedSpy.isEmpty());
    QVERIFY(proxy2rowsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy2rowsMovedSpy.isEmpty());
    QCOMPARE(proxy1RowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(proxy1RowsInsertedSpy.count(), 1);
    proxy1RowsAboutToBeInsertedSpy.clear();
    proxy1RowsInsertedSpy.clear();

    QVERIFY(baseModel.moveRow(baseModel.index(0, 0), 2, baseModel.index(0, 0), 0));
    QCOMPARE(proxyModel1.rowCount(), 3);
    QCOMPARE(proxyModel2.rowCount(), 1);
    QCOMPARE(proxy1rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(proxy1rowsMovedSpy.count(), 1);
    QVERIFY(proxy2rowsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy2rowsMovedSpy.isEmpty());
    QVERIFY(proxy1RowsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy1RowsInsertedSpy.isEmpty());
    QVERIFY(proxy2RowsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy2RowsInsertedSpy.isEmpty());
    QVERIFY(proxy1RowsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy1RowsRemovedSpy.isEmpty());
    QVERIFY(proxy2RowsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy2RowsRemovedSpy.isEmpty());
    proxy1rowsAboutToBeMovedSpy.clear();
    proxy1rowsMovedSpy.clear();

    QVERIFY(baseModel.moveRow(baseModel.index(0, 0), 2, QModelIndex(), 0));
    QCOMPARE(proxyModel1.rowCount(), 2);
    QCOMPARE(proxyModel2.rowCount(), 1);
    QCOMPARE(proxy1RowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(proxy1RowsRemovedSpy.count(), 1);
    QVERIFY(proxy2rowsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy2rowsMovedSpy.isEmpty());
    QVERIFY(proxy1RowsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy1RowsInsertedSpy.isEmpty());
    QVERIFY(proxy2RowsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy2RowsInsertedSpy.isEmpty());
    QVERIFY(proxy1rowsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy1rowsMovedSpy.isEmpty());
    QVERIFY(proxy2RowsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy2RowsRemovedSpy.isEmpty());
    proxy1RowsAboutToBeRemovedSpy.clear();
    proxy1RowsRemovedSpy.clear();

    QVERIFY(baseModel.moveRow(QModelIndex(), 0, baseModel.index(1, 0), 0));
    QCOMPARE(proxyModel1.rowCount(), 3);
    QCOMPARE(proxyModel2.rowCount(), 1);
    QVERIFY(proxy1rowsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy1rowsMovedSpy.isEmpty());
    QVERIFY(proxy2rowsAboutToBeMovedSpy.isEmpty());
    QVERIFY(proxy2rowsMovedSpy.isEmpty());
    QCOMPARE(proxy1RowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(proxy1RowsInsertedSpy.count(), 1);
    QVERIFY(proxy2RowsAboutToBeInsertedSpy.isEmpty());
    QVERIFY(proxy2RowsInsertedSpy.isEmpty());
    QVERIFY(proxy1RowsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy1RowsRemovedSpy.isEmpty());
    QVERIFY(proxy2RowsAboutToBeRemovedSpy.isEmpty());
    QVERIFY(proxy2RowsRemovedSpy.isEmpty());
#else
    QSKIP("This test requires the GenericModel module");
#endif
}

void tst_RootIndexProxyModel::switchRoot()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyRootChangeSpy(&proxyModel, SIGNAL(rootIndexChanged()));
    QVERIFY(proxyRootChangeSpy.isValid());
    QSignalSpy proxyResetSpy(&proxyModel, SIGNAL(modelReset()));
    QVERIFY(proxyResetSpy.isValid());
    proxyModel.setRootIndex(baseModel->index(1, 0));
    QCOMPARE(proxyRootChangeSpy.count(), 1);
    QCOMPARE(proxyResetSpy.count(), 1);
    proxyModel.setRootIndex(baseModel->index(1, 0));
    QCOMPARE(proxyRootChangeSpy.count(), 1);
    QCOMPARE(proxyResetSpy.count(), 1);
    proxyModel.setRootIndex(baseModel->index(2, 0, baseModel->index(1, 0)));
    QCOMPARE(proxyModel.rootIndex(), baseModel->index(2, 0, baseModel->index(1, 0)));
    QCOMPARE(proxyRootChangeSpy.count(), 2);
    QCOMPARE(proxyResetSpy.count(), 2);
    compareModels(baseModel, &proxyModel, baseModel->index(2, 0, baseModel->index(1, 0)), QModelIndex());
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RootIndexProxyModel::rootMoves()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseModel->index(1, 0));
    QCOMPARE(proxyModel.rootIndex(), baseModel->index(1, 0));
    baseModel->insertRow(0);
    QCOMPARE(proxyModel.rootIndex(), baseModel->index(2, 0));
    compareModels(baseModel, &proxyModel, baseModel->index(2, 0), QModelIndex());
    baseModel->insertRow(3);
    QCOMPARE(proxyModel.rootIndex(), baseModel->index(2, 0));
    compareModels(baseModel, &proxyModel, baseModel->index(2, 0), QModelIndex());
    baseModel->insertColumn(0);
    QCOMPARE(proxyModel.rootIndex(), baseModel->index(2, 1));
    compareModels(baseModel, &proxyModel, baseModel->index(2, 1), QModelIndex());
    baseModel->insertColumn(2);
    QCOMPARE(proxyModel.rootIndex(), baseModel->index(2, 1));
    compareModels(baseModel, &proxyModel, baseModel->index(2, 1), QModelIndex());
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RootIndexProxyModel::rootRemoved()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = createTreeModel(this);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseModel->index(1, 0));
    QSignalSpy proxyRootChangeSpy(&proxyModel, SIGNAL(rootIndexChanged()));
    QVERIFY(proxyRootChangeSpy.isValid());
    QSignalSpy proxyResetSpy(&proxyModel, SIGNAL(modelReset()));
    QVERIFY(proxyResetSpy.isValid());
    baseModel->removeRow(1);
    QCOMPARE(proxyModel.rootIndex(), QModelIndex());
    QCOMPARE(proxyRootChangeSpy.count(), 1);
    QCOMPARE(proxyResetSpy.count(), 1);
    compareModels(baseModel, &proxyModel, QModelIndex(), QModelIndex());
    proxyModel.setRootIndex(baseModel->index(1, 0));
    QCOMPARE(proxyRootChangeSpy.count(), 2);
    QCOMPARE(proxyResetSpy.count(), 2);
    compareModels(baseModel, &proxyModel, baseModel->index(1, 0), QModelIndex());
    baseModel->removeColumn(0);
    QCOMPARE(proxyModel.rootIndex(), QModelIndex());
    QCOMPARE(proxyRootChangeSpy.count(), 3);
    QCOMPARE(proxyResetSpy.count(), 3);
    compareModels(baseModel, &proxyModel, QModelIndex(), QModelIndex());
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RootIndexProxyModel::sourceMoveRows()
{
#ifdef QTMODELUTILITIES_GENERICMODEL
    const auto fillData = [](QAbstractItemModel *model, const QModelIndex &parent = QModelIndex(), int shift = 0) {
        for (int i = 0, maxI = model->rowCount(parent); i < maxI; ++i) {
            for (int j = 0, maxJ = model->columnCount(parent); j < maxJ; ++j) {
                model->setData(model->index(i, j, parent), i + shift, Qt::UserRole);
                model->setData(model->index(i, j, parent), j + shift, Qt::UserRole + 1);
            }
        }
    };
    GenericModel baseModel;
    baseModel.insertColumns(0, 2);
    baseModel.insertRows(0, 5);
    fillData(&baseModel);
    const QPersistentModelIndex grandParent = baseModel.index(1, 0);
    baseModel.insertColumns(0, 2, grandParent);
    baseModel.insertRows(0, 5, grandParent);
    fillData(&baseModel, grandParent, 50);
    const QPersistentModelIndex parent = baseModel.index(1, 0, grandParent);
    baseModel.insertColumns(0, 2, parent);
    baseModel.insertRows(0, 5, parent);
    fillData(&baseModel, parent, 100);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, &baseModel);
    proxyModel.setSourceModel(&baseModel);
    QSignalSpy rowsMovedSpy(&proxyModel, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsMovedSpy.isValid());
    QSignalSpy rowsAboutToBeMovedSpy(&proxyModel, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(rowsAboutToBeMovedSpy.isValid());
    QSignalSpy *moveSpyArr[] = {&rowsMovedSpy, &rowsAboutToBeMovedSpy};
    QSignalSpy rowsInsertedSpy(&proxyModel, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QVERIFY(rowsInsertedSpy.isValid());
    QSignalSpy rowsAboutToBeInsertedSpy(&proxyModel, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(rowsAboutToBeInsertedSpy.isValid());
    QSignalSpy *insertSpyArr[] = {&rowsInsertedSpy, &rowsAboutToBeInsertedSpy};
    QSignalSpy rowsRemovedSpy(&proxyModel, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    QVERIFY(rowsRemovedSpy.isValid());
    QSignalSpy rowsAboutToBeRemovedSpy(&proxyModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(rowsAboutToBeRemovedSpy.isValid());
    QSignalSpy *removeSpyArr[] = {&rowsRemovedSpy, &rowsAboutToBeRemovedSpy};

    proxyModel.setRootIndex(grandParent);
    const QPersistentModelIndex proxyParent = proxyModel.index(1, 0);
    QVERIFY(baseModel.moveRows(QModelIndex(), 0, 1, QModelIndex(), 5));
    QCOMPARE(proxyModel.rowCount(), 5);
    for (auto &&spy : moveSpyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(baseModel.moveRows(grandParent, 0, 1, grandParent, 5));
    QCOMPARE(proxyModel.rowCount(), 5);
    QCOMPARE(proxyModel.index(4, 0).data(Qt::UserRole).toInt(), 50);
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 51);
    QCOMPARE(proxyParent.data(Qt::UserRole).toInt(), 51);
    for (auto &&spy : moveSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(3).value<QModelIndex>(), QModelIndex());
        QCOMPARE(args.at(4).toInt(), 5);
    }
    QVERIFY(baseModel.moveRows(parent, 0, 1, parent, 5));
    QCOMPARE(proxyModel.rowCount(proxyParent), 5);
    QCOMPARE(proxyModel.index(4, 0, proxyParent).data(Qt::UserRole).toInt(), 100);
    QCOMPARE(proxyModel.index(0, 0, proxyParent).data(Qt::UserRole).toInt(), 101);
    for (auto &&spy : moveSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex(proxyParent));
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(3).value<QModelIndex>(), QModelIndex(proxyParent));
        QCOMPARE(args.at(4).toInt(), 5);
    }
    QVERIFY(baseModel.moveRows(parent, 0, 1, grandParent, 5));
    QCOMPARE(proxyModel.rowCount(proxyParent), 4);
    QCOMPARE(proxyModel.rowCount(), 6);
    QCOMPARE(proxyModel.index(4, 0).data(Qt::UserRole).toInt(), 50);
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 51);
    QCOMPARE(proxyModel.index(5, 0).data(Qt::UserRole).toInt(), 101);
    for (auto &&spy : moveSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex(proxyParent));
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(3).value<QModelIndex>(), QModelIndex());
        QCOMPARE(args.at(4).toInt(), 5);
    }
    QVERIFY(baseModel.moveRows(parent, 0, 1, QModelIndex(), 5));
    QCOMPARE(proxyModel.rowCount(proxyParent), 3);
    QCOMPARE(proxyModel.rowCount(), 6);
    for (auto &&spy : moveSpyArr)
        QCOMPARE(spy->count(), 0);
    for (auto &&spy : removeSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex(proxyParent));
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(2).toInt(), 0);
    }
    QVERIFY(baseModel.moveRows(QModelIndex(), 2, 1, parent, 3));
    QCOMPARE(proxyModel.rowCount(proxyParent), 4);
    QCOMPARE(proxyModel.index(3, 0, proxyParent).data(Qt::UserRole).toInt(), 3);
    QCOMPARE(proxyModel.rowCount(), 6);
    for (auto &&spy : moveSpyArr)
        QCOMPARE(spy->count(), 0);
    for (auto &&spy : insertSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex(proxyParent));
        QCOMPARE(args.at(1).toInt(), 3);
        QCOMPARE(args.at(2).toInt(), 3);
    }
    QVERIFY(baseModel.moveRows(QModelIndex(), 0, 1, QModelIndex(), 5));
    QCOMPARE(proxyModel.rowCount(), 6);
    for (auto &&spy : moveSpyArr)
        QCOMPARE(spy->count(), 0);
#else
    QSKIP("This test requires the GenericModel module");
#endif
}

void tst_RootIndexProxyModel::sourceMoveColumns()
{
#ifdef QTMODELUTILITIES_GENERICMODEL
    const auto fillData = [](QAbstractItemModel *model, const QModelIndex &parent = QModelIndex(), int shift = 0) {
        for (int i = 0, maxI = model->rowCount(parent); i < maxI; ++i) {
            for (int j = 0, maxJ = model->columnCount(parent); j < maxJ; ++j) {
                model->setData(model->index(i, j, parent), i + shift, Qt::UserRole);
                model->setData(model->index(i, j, parent), j + shift, Qt::UserRole + 1);
            }
        }
    };
    GenericModel baseModel;
    baseModel.insertColumns(0, 5);
    baseModel.insertRows(0, 2);
    fillData(&baseModel);
    const QPersistentModelIndex grandParent = baseModel.index(1, 0);
    baseModel.insertColumns(0, 5, grandParent);
    baseModel.insertRows(0, 2, grandParent);
    fillData(&baseModel, grandParent, 50);
    const QPersistentModelIndex parent = baseModel.index(1, 0, grandParent);
    baseModel.insertColumns(0, 5, parent);
    baseModel.insertRows(0, 2, parent);
    fillData(&baseModel, parent, 100);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, &baseModel);
    proxyModel.setSourceModel(&baseModel);
    QSignalSpy columnsMovedSpy(&proxyModel, SIGNAL(columnsMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(columnsMovedSpy.isValid());
    QSignalSpy columnsAboutToBeMovedSpy(&proxyModel, SIGNAL(columnsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    QVERIFY(columnsAboutToBeMovedSpy.isValid());
    QSignalSpy *moveSpyArr[] = {&columnsMovedSpy, &columnsAboutToBeMovedSpy};
    QSignalSpy columnsInsertedSpy(&proxyModel, SIGNAL(columnsInserted(QModelIndex, int, int)));
    QVERIFY(columnsInsertedSpy.isValid());
    QSignalSpy columnsAboutToBeInsertedSpy(&proxyModel, SIGNAL(columnsAboutToBeInserted(QModelIndex, int, int)));
    QVERIFY(columnsAboutToBeInsertedSpy.isValid());
    QSignalSpy *insertSpyArr[] = {&columnsInsertedSpy, &columnsAboutToBeInsertedSpy};
    QSignalSpy columnsRemovedSpy(&proxyModel, SIGNAL(columnsRemoved(QModelIndex, int, int)));
    QVERIFY(columnsRemovedSpy.isValid());
    QSignalSpy columnsAboutToBeRemovedSpy(&proxyModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)));
    QVERIFY(columnsAboutToBeRemovedSpy.isValid());
    QSignalSpy *removeSpyArr[] = {&columnsRemovedSpy, &columnsAboutToBeRemovedSpy};

    proxyModel.setRootIndex(grandParent);
    const QPersistentModelIndex proxyParent = proxyModel.index(1, 0);
    QVERIFY(baseModel.moveColumns(QModelIndex(), 0, 1, QModelIndex(), 5));
    QCOMPARE(proxyModel.columnCount(), 5);
    for (auto &&spy : moveSpyArr)
        QCOMPARE(spy->count(), 0);
    QVERIFY(baseModel.moveColumns(grandParent, 0, 1, grandParent, 5));
    QCOMPARE(proxyModel.columnCount(), 5);
    QCOMPARE(proxyModel.index(0, 4).data(Qt::UserRole + 1).toInt(), 50);
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole + 1).toInt(), 51);
    QCOMPARE(proxyParent.data(Qt::UserRole + 1).toInt(), 50);
    for (auto &&spy : moveSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex());
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(3).value<QModelIndex>(), QModelIndex());
        QCOMPARE(args.at(4).toInt(), 5);
    }
    QVERIFY(baseModel.moveColumns(parent, 0, 1, parent, 5));
    QCOMPARE(proxyModel.columnCount(proxyParent), 5);
    QCOMPARE(proxyModel.index(0, 4, proxyParent).data(Qt::UserRole + 1).toInt(), 100);
    QCOMPARE(proxyModel.index(0, 0, proxyParent).data(Qt::UserRole + 1).toInt(), 101);
    for (auto &&spy : moveSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex(proxyParent));
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(3).value<QModelIndex>(), QModelIndex(proxyParent));
        QCOMPARE(args.at(4).toInt(), 5);
    }
    QVERIFY(baseModel.moveColumns(parent, 0, 1, grandParent, 5));
    QCOMPARE(proxyModel.columnCount(proxyParent), 4);
    QCOMPARE(proxyModel.columnCount(), 6);
    QCOMPARE(proxyModel.index(0, 4).data(Qt::UserRole + 1).toInt(), 50);
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole + 1).toInt(), 51);
    QCOMPARE(proxyModel.index(0, 5).data(Qt::UserRole + 1).toInt(), 101);
    for (auto &&spy : moveSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex(proxyParent));
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(2).toInt(), 0);
        QCOMPARE(args.at(3).value<QModelIndex>(), QModelIndex());
        QCOMPARE(args.at(4).toInt(), 5);
    }
    QVERIFY(baseModel.moveColumns(parent, 0, 1, QModelIndex(), 5));
    QCOMPARE(proxyModel.columnCount(proxyParent), 3);
    QCOMPARE(proxyModel.columnCount(), 6);
    for (auto &&spy : moveSpyArr)
        QCOMPARE(spy->count(), 0);
    for (auto &&spy : removeSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex(proxyParent));
        QCOMPARE(args.at(1).toInt(), 0);
        QCOMPARE(args.at(2).toInt(), 0);
    }
    QVERIFY(baseModel.moveColumns(QModelIndex(), 2, 1, parent, 3));
    QCOMPARE(proxyModel.columnCount(proxyParent), 4);
    QCOMPARE(proxyModel.index(0, 3, proxyParent).data(Qt::UserRole + 1).toInt(), 3);
    QCOMPARE(proxyModel.columnCount(), 6);
    for (auto &&spy : moveSpyArr)
        QCOMPARE(spy->count(), 0);
    for (auto &&spy : insertSpyArr) {
        QCOMPARE(spy->count(), 1);
        const auto args = spy->takeFirst();
        QCOMPARE(args.at(0).value<QModelIndex>(), QModelIndex(proxyParent));
        QCOMPARE(args.at(1).toInt(), 3);
        QCOMPARE(args.at(2).toInt(), 3);
    }
    QVERIFY(baseModel.moveColumns(QModelIndex(), 0, 1, QModelIndex(), 5));
    QCOMPARE(proxyModel.columnCount(), 6);
    for (auto &&spy : moveSpyArr)
        QCOMPARE(spy->count(), 0);
#else
    QSKIP("This test requires the GenericModel module");
#endif
}

void tst_RootIndexProxyModel::sourceSortDescendantOfRoot()
{
#ifdef COMPLEX_MODEL_SUPPORT
    ComplexModel baseModel;
    baseModel.insertColumn(0);
    baseModel.insertRows(0, 3);
    for (int i = 0; i < baseModel.rowCount(); ++i) {
        const QModelIndex grandparent = baseModel.index(i, 0);
        baseModel.setData(grandparent, QChar('A' + i));
        baseModel.insertColumn(0, grandparent);
        baseModel.insertRows(0, 3, grandparent);
        for (int j = 0; j < baseModel.rowCount(grandparent); ++j) {
            const QModelIndex parent = baseModel.index(j, 0, grandparent);
            baseModel.setData(parent, QChar('A' + i) + QString::number(j + 1));
            baseModel.insertColumn(0, parent);
            baseModel.insertRows(0, 3, parent);
            for (int h = 0; h < baseModel.rowCount(parent); ++h)
                baseModel.setData(baseModel.index(h, 0, parent), QChar('A' + i) + QString::number(j + 1) + QChar('z' - h));
        }
    }
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, &baseModel);
    proxyModel.setSourceModel(&baseModel);
    proxyModel.setRootIndex(baseModel.index(1, 0));
    const QPersistentModelIndex B3zIdx = proxyModel.index(0, 0, proxyModel.index(2, 0));
    QVERIFY(B3zIdx.isValid());
    QCOMPARE(B3zIdx.data().toString(), QStringLiteral("B3z"));
    QSignalSpy proxyLayChangeSpy(&proxyModel, SIGNAL(layoutChanged()));
    QVERIFY(proxyLayChangeSpy.isValid());
    baseModel.sort(0);
    QCOMPARE(B3zIdx.row(), 2);
    QCOMPARE(B3zIdx.data().toString(), QStringLiteral("B3z"));
    QCOMPARE(proxyLayChangeSpy.count(), 1);
    QModelIndex B1Idx = proxyModel.index(0, 0);
    QCOMPARE(proxyModel.index(0, 0, B1Idx).data().toString(), QStringLiteral("B1x"));
    QCOMPARE(proxyModel.index(1, 0, B1Idx).data().toString(), QStringLiteral("B1y"));
    QCOMPARE(proxyModel.index(2, 0, B1Idx).data().toString(), QStringLiteral("B1z"));
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RootIndexProxyModel::sourceSortAncestorOfRoot()
{
#ifdef COMPLEX_MODEL_SUPPORT
    ComplexModel baseModel;
    baseModel.insertColumn(0);
    baseModel.insertRows(0, 3);
    for (int i = 0; i < baseModel.rowCount(); ++i) {
        const QModelIndex grandparent = baseModel.index(i, 0);
        baseModel.setData(grandparent, QChar('Z' - i));
        baseModel.insertColumn(0, grandparent);
        baseModel.insertRows(0, 3, grandparent);
        for (int j = 0; j < baseModel.rowCount(grandparent); ++j) {
            const QModelIndex parent = baseModel.index(j, 0, grandparent);
            baseModel.setData(parent, QChar('Z' - i) + QString::number(j + 1));
            baseModel.insertColumn(0, parent);
            baseModel.insertRows(0, 3, parent);
            for (int h = 0; h < baseModel.rowCount(parent); ++h)
                baseModel.setData(baseModel.index(h, 0, parent), QChar('Z' - i) + QString::number(j + 1) + QChar('a' + h));
        }
    }
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, &baseModel);
    proxyModel.setSourceModel(&baseModel);
    proxyModel.setRootIndex(baseModel.index(0, 0));
    const QPersistentModelIndex Z3aIdx = proxyModel.index(0, 0, proxyModel.index(2, 0));
    QVERIFY(Z3aIdx.isValid());
    QCOMPARE(Z3aIdx.data().toString(), QStringLiteral("Z3a"));
    QSignalSpy proxyLayChangeSpy(&proxyModel, SIGNAL(layoutChanged()));
    QVERIFY(proxyLayChangeSpy.isValid());
    baseModel.sort(0);
    QCOMPARE(Z3aIdx.row(), 0);
    QCOMPARE(Z3aIdx.data().toString(), QStringLiteral("Z3a"));
    QCOMPARE(proxyLayChangeSpy.count(), 1);
    QModelIndex Z1Idx = proxyModel.index(0, 0);
    QCOMPARE(proxyModel.index(0, 0, Z1Idx).data().toString(), QStringLiteral("Z1a"));
    QCOMPARE(proxyModel.index(1, 0, Z1Idx).data().toString(), QStringLiteral("Z1b"));
    QCOMPARE(proxyModel.index(2, 0, Z1Idx).data().toString(), QStringLiteral("Z1c"));
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RootIndexProxyModel::compareModels(const QAbstractItemModel *source, const QAbstractItemModel *proxy, const QModelIndex &sourcePar,
                                            const QModelIndex &proxyPar)
{
    const int maxR = source->rowCount(sourcePar);
    const int maxC = source->columnCount(sourcePar);
    QCOMPARE(proxy->rowCount(proxyPar), maxR);
    QCOMPARE(proxy->columnCount(proxyPar), maxC);
    for (int i = 0; i < maxR; ++i) {
        for (int j = 0; j < maxC; ++j) {
            const QModelIndex sourceIdx = source->index(i, j, sourcePar);
            const QModelIndex proxyIdx = proxy->index(i, j, proxyPar);
            QCOMPARE(proxyIdx.data().toString(), sourceIdx.data().toString());
            const bool hasChildren = source->hasChildren(sourceIdx);
            QCOMPARE(proxy->hasChildren(proxyIdx), hasChildren);
            if (hasChildren)
                compareModels(source, proxy, sourceIdx, proxyIdx);
        }
    }
}
