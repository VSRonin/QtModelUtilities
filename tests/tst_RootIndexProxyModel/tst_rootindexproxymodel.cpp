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

void tst_RootIndexProxyModel::showRoot_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<QModelIndex>("root");
    QAbstractItemModel *baseModel = createTreeModel(this);
    QTest::newRow("Child") << baseModel << baseModel->index(1,0);
    QTest::newRow("Grandchild") << baseModel << baseModel->index(2,0,baseModel->index(1,0));
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

void tst_RootIndexProxyModel::showRoot()
{
    QFETCH(QAbstractItemModel *, baseModel);
    QFETCH(QModelIndex, root);
    RootIndexProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setRootIndex(root);
    compareModels(baseModel,&proxyModel,root,QModelIndex());
    baseModel->deleteLater();
}




