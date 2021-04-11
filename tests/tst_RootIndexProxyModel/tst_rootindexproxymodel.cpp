#include "tst_rootindexproxymodel.h"
#include <QStringListModel>
#include <rootindexproxymodel.h>
#ifdef QT_GUI_LIB
#    include <QStandardItemModel>
#endif
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include "../modeltestmanager.h"

QAbstractItemModel *createTreeModel(QObject *parent)
{
    QAbstractItemModel *result = nullptr;
#ifdef QT_GUI_LIB
    result = new QStandardItemModel(parent);
    result->insertRows(0, 3);
    result->insertColumns(0, 3);
    for (int i = 0; i < result->rowCount(); ++i) {
        for (int j = 0; j < result->columnCount(); ++j)
            result->setData(result->index(i, j), QStringLiteral("%1,%2").arg(i).arg(j));
        const QModelIndex parIdx = result->index(i, 0);
        result->insertRows(0, 4, parIdx);
        result->insertColumns(0, 2, parIdx);
        for (int k = 0; k < result->rowCount(parIdx); ++k) {
            for (int h = 0; h < result->columnCount(parIdx); ++h)
                result->setData(result->index(k, h, parIdx), QStringLiteral("%1,%2,%3").arg(i).arg(k).arg(h));
            const QModelIndex grandParIdx = result->index(i, 0, parIdx);
            result->insertRows(0, 6, grandParIdx);
            result->insertColumns(0, 2, grandParIdx);
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
#ifdef QT_GUI_LIB
    QFETCH(QAbstractItemModel *, baseModel);
    QFETCH(QModelIndex, root);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(root);
    QCOMPARE(proxyModel.rootIndex(), root);
    compareModels(baseModel, &proxyModel, root, QModelIndex());
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI module");
#endif
}

void tst_RootIndexProxyModel::showRoot_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<QModelIndex>("root");
    QTest::newRow("Parent") << createTreeModel(this) << QModelIndex();
    QAbstractItemModel *baseModel = createTreeModel(this);
    QTest::newRow("Child") << baseModel << baseModel->index(1, 0);
    baseModel = createTreeModel(this);
    QTest::newRow("Grandchild") << baseModel << baseModel->index(2, 0, baseModel->index(1, 0));
}
void tst_RootIndexProxyModel::replaceModel()
{
#ifdef QT_GUI_LIB
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
#else
    QSKIP("This test requires the Qt GUI module");
#endif
}
void tst_RootIndexProxyModel::replaceModel_data()
{
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
}

void tst_RootIndexProxyModel::sourceDataChanged()
{
#ifdef QT_GUI_LIB
    QFETCH(QAbstractItemModel *, baseModel);
    QFETCH(QModelIndex, changeIndex);
    QFETCH(const int, expectedSignals);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseModel->index(1, 0, baseModel->index(1, 0)));
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(proxyDataChangeSpy.isValid());
    baseModel->setData(changeIndex, QStringLiteral("Hello"));
    QCOMPARE(proxyDataChangeSpy.count(), expectedSignals);
#else
    QSKIP("This test requires the Qt GUI module");
#endif
}

void tst_RootIndexProxyModel::sourceDataChanged_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<QModelIndex>("changeIndex");
    QTest::addColumn<const int>("expectedSignals");
    QAbstractItemModel *baseModel = createTreeModel(this);
    QTest::newRow("Sibling of Root") << baseModel << baseModel->index(0, 0, baseModel->index(1, 0)) << 0;
    baseModel = createTreeModel(this);
    QTest::newRow("Ancestor of Root") << baseModel << baseModel->index(0, 0) << 0;
    baseModel = createTreeModel(this);
    QTest::newRow("Root") << baseModel << baseModel->index(1, 0, baseModel->index(1, 0)) << 0;
    baseModel = createTreeModel(this);
    QTest::newRow("Descendant of Root") << baseModel << baseModel->index(0, 0, baseModel->index(1, 0, baseModel->index(1, 0))) << 1;
}

void tst_RootIndexProxyModel::switchRoot()
{
#ifdef QT_GUI_LIB
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
    QSKIP("This test requires the Qt GUI module");
#endif
}

void tst_RootIndexProxyModel::rootMoves()
{
#ifdef QT_GUI_LIB
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
    QSKIP("This test requires the Qt GUI module");
#endif
}

void tst_RootIndexProxyModel::rootRemoved()
{
#ifdef QT_GUI_LIB
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
    QSKIP("This test requires the Qt GUI module");
#endif
}

void tst_RootIndexProxyModel::sourceMoveRows()
{
    QSKIP("No tree model implements row movement");
}

void tst_RootIndexProxyModel::sourceMoveColumns()
{
    QSKIP("No tree model implements column movement");
}

void tst_RootIndexProxyModel::sourceSortDescendantOfRoot()
{
#ifdef QT_GUI_LIB
    QStandardItemModel baseModel;
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
    QSignalSpy proxyLayChangeSpy(&proxyModel, SIGNAL(layoutChanged(QList<QPersistentModelIndex>, QAbstractItemModel::LayoutChangeHint)));
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
    QSKIP("This test requires the Qt GUI module");
#endif
}

void tst_RootIndexProxyModel::sourceSortAncestorOfRoot()
{
#ifdef QT_GUI_LIB
    QStandardItemModel baseModel;
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
    QSignalSpy proxyLayChangeSpy(&proxyModel, SIGNAL(layoutChanged(QList<QPersistentModelIndex>, QAbstractItemModel::LayoutChangeHint)));
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
    QSKIP("This test requires the Qt GUI module");
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
