#include <QtTest/QTest>
#include <QStringListModel>
#include <rolemaskproxymodel.h>
#include <QBrush>
#include "tst_rolemaskproxymodel.h"
void tst_RoleMaskProxyModel::testBackGroundStringListModel()
{
    QStringListModel baseModel(QStringList() << QStringLiteral("One") << QStringLiteral("Two") << QStringLiteral("Three"));
    RoleMaskProxyModel proxyModel;
    proxyModel.addMaskedRole(Qt::BackgroundRole);
    proxyModel.setSourceModel(&baseModel);
    const QBrush redBrush(Qt::red);
    QVERIFY(proxyModel.setData(proxyModel.index(0, 0), redBrush,Qt::BackgroundRole));
    QCOMPARE(proxyModel.index(0, 0).data(Qt::BackgroundRole).value<QBrush>(), redBrush);
    QCOMPARE(baseModel.index(0, 0).data(Qt::BackgroundRole), QVariant());
    for (int i = 0; i < baseModel.rowCount();++i)
        QCOMPARE(proxyModel.index(i, 0).data(), baseModel.index(i, 0).data());
    for (int i = 1; i < proxyModel.rowCount(); ++i)
        QCOMPARE(proxyModel.index(i, 0).data(Qt::BackgroundRole), QVariant());
}


