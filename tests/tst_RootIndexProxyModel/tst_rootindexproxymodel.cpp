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

void tst_RootIndexProxyModel::showRoot()
{
#ifdef QT_GUI_LIB
    QFETCH(QAbstractItemModel *, baseModel);
    QFETCH(QModelIndex, root);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(root);
    QCOMPARE(proxyModel.rootIndex(),root);
    compareModels(baseModel,&proxyModel,root,QModelIndex());
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
    QTest::newRow("Child") << baseModel << baseModel->index(1,0);
    baseModel = createTreeModel(this);
    QTest::newRow("Grandchild") << baseModel << baseModel->index(2,0,baseModel->index(1,0));
}
void tst_RootIndexProxyModel::replaceModel(){
    QFETCH(QAbstractItemModel *, baseModel);
    QFETCH(QModelIndex, baseRoot);
    QFETCH(QAbstractItemModel *, replaceModel);
    QFETCH(QModelIndex, replaceRoot);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseRoot);
    QCOMPARE(proxyModel.rootIndex(),baseRoot);
    proxyModel.setSourceModel(replaceModel);
    QCOMPARE(proxyModel.rootIndex(),QModelIndex());
    compareModels(replaceModel,&proxyModel,QModelIndex(),QModelIndex());
    proxyModel.setRootIndex(replaceRoot);
    QCOMPARE(proxyModel.rootIndex(),replaceRoot);
    baseModel->deleteLater();
    replaceModel->deleteLater();
}
void tst_RootIndexProxyModel::replaceModel_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<QModelIndex>("baseRoot");
    QTest::addColumn<QAbstractItemModel *>("replaceModel");
    QTest::addColumn<QModelIndex>("replaceRoot");

    QAbstractItemModel *baseModel = createTreeModel(this);
    QAbstractItemModel *replaceModel = createTreeModel(this);
    QTest::newRow("Child") << baseModel << baseModel->index(1,0) << replaceModel << replaceModel->index(0,0);
    baseModel = createTreeModel(this);
    replaceModel = createTreeModel(this);
    QTest::newRow("Grandchild") << baseModel << baseModel->index(1,0) << replaceModel << replaceModel->index(2,0,replaceModel->index(1,0));
}

void tst_RootIndexProxyModel::switchRoot()
{
    QAbstractItemModel *baseModel = createTreeModel(this);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyRootChangeSpy(&proxyModel, SIGNAL(rootIndexChanged()));
    QVERIFY(proxyRootChangeSpy.isValid());
    QSignalSpy proxyResetSpy(&proxyModel, SIGNAL(modelReset()));
    QVERIFY(proxyResetSpy.isValid());
    proxyModel.setRootIndex(baseModel->index(1,0));
    QCOMPARE(proxyRootChangeSpy.count(), 1);
    QCOMPARE(proxyResetSpy.count(), 1);
    proxyModel.setRootIndex(baseModel->index(1,0));
    QCOMPARE(proxyRootChangeSpy.count(), 1);
    QCOMPARE(proxyResetSpy.count(), 1);
    proxyModel.setRootIndex(baseModel->index(2,0,baseModel->index(1,0)));
    QCOMPARE(proxyModel.rootIndex(),baseModel->index(2,0,baseModel->index(1,0)));
    QCOMPARE(proxyRootChangeSpy.count(), 2);
    QCOMPARE(proxyResetSpy.count(), 2);
    compareModels(baseModel,&proxyModel,baseModel->index(2,0,baseModel->index(1,0)),QModelIndex());
    baseModel->deleteLater();
}

void tst_RootIndexProxyModel::rootMoves()
{
    QAbstractItemModel *baseModel = createTreeModel(this);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseModel->index(1,0));
    QCOMPARE(proxyModel.rootIndex(),baseModel->index(1,0));
    baseModel->insertRow(0);
    QCOMPARE(proxyModel.rootIndex(),baseModel->index(2,0));
    compareModels(baseModel,&proxyModel,baseModel->index(2,0),QModelIndex());
    baseModel->insertRow(3);
    QCOMPARE(proxyModel.rootIndex(),baseModel->index(2,0));
    compareModels(baseModel,&proxyModel,baseModel->index(2,0),QModelIndex());
    baseModel->insertColumn(0);
    QCOMPARE(proxyModel.rootIndex(),baseModel->index(2,1));
    compareModels(baseModel,&proxyModel,baseModel->index(2,1),QModelIndex());
    baseModel->insertColumn(2);
    QCOMPARE(proxyModel.rootIndex(),baseModel->index(2,1));
    compareModels(baseModel,&proxyModel,baseModel->index(2,1),QModelIndex());
}

void tst_RootIndexProxyModel::rootRemoved()
{
    QAbstractItemModel *baseModel = createTreeModel(this);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(baseModel->index(1,0));
    QSignalSpy proxyRootChangeSpy(&proxyModel, SIGNAL(rootIndexChanged()));
    QVERIFY(proxyRootChangeSpy.isValid());
    QSignalSpy proxyResetSpy(&proxyModel, SIGNAL(modelReset()));
    QVERIFY(proxyResetSpy.isValid());
    baseModel->removeRow(1);
    QCOMPARE(proxyModel.rootIndex(),QModelIndex());
    QCOMPARE(proxyRootChangeSpy.count(), 1);
    QCOMPARE(proxyResetSpy.count(), 1);
    compareModels(baseModel,&proxyModel,QModelIndex(),QModelIndex());
    proxyModel.setRootIndex(baseModel->index(1,0));
    QCOMPARE(proxyRootChangeSpy.count(), 2);
    QCOMPARE(proxyResetSpy.count(), 2);
    compareModels(baseModel,&proxyModel,baseModel->index(1,0),QModelIndex());
    baseModel->removeColumn(0);
    QCOMPARE(proxyModel.rootIndex(),QModelIndex());
    QCOMPARE(proxyRootChangeSpy.count(), 3);
    QCOMPARE(proxyResetSpy.count(), 3);
    compareModels(baseModel,&proxyModel,QModelIndex(),QModelIndex());
}

void tst_RootIndexProxyModel::compareModels(const QAbstractItemModel *source, const QAbstractItemModel *proxy, const QModelIndex &sourcePar, const QModelIndex &proxyPar)
{
    const int maxR = source->rowCount(sourcePar);
    const int maxC = source->columnCount(sourcePar);
    QCOMPARE(proxy->rowCount(proxyPar),maxR);
    QCOMPARE(proxy->columnCount(proxyPar),maxC);
    for(int i=0;i<maxR;++i){
        for(int j=0;j<maxC;++j){
            const QModelIndex sourceIdx = source->index(i,j,sourcePar);
            const QModelIndex proxyIdx = proxy->index(i,j,proxyPar);
            QCOMPARE(proxyIdx.data().toString(),sourceIdx.data().toString());
            const bool hasChildren = source->hasChildren(sourceIdx);
            QCOMPARE(proxy->hasChildren(proxyIdx),hasChildren);
            if(hasChildren)
                compareModels(source,proxy,sourceIdx,proxyIdx);
        }
    }
}






