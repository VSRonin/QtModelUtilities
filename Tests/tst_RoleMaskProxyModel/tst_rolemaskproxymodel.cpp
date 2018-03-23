#include <QtTest/QTest>
#include <QStringListModel>
#include <rolemaskproxymodel.h>
#include <QBrush>
#include "tst_rolemaskproxymodel.h"
void tst_RoleMaskProxyModel::testUseRoleStringListModel()
{
    QStringListModel baseModel(QStringList() << QStringLiteral("One") << QStringLiteral("Two") << QStringLiteral("Three"));
    RoleMaskProxyModel proxyModel;
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(&baseModel);
    const int magicNumber= 78;
    QVERIFY(proxyModel.setData(proxyModel.index(0, 0), magicNumber, Qt::UserRole));
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), magicNumber);
    QCOMPARE(baseModel.index(0, 0).data(Qt::UserRole), QVariant());
    for (int i = 0; i < baseModel.rowCount();++i)
        QCOMPARE(proxyModel.index(i, 0).data(), baseModel.index(i, 0).data());
    for (int i = 1; i < proxyModel.rowCount(); ++i)
        QCOMPARE(proxyModel.index(i, 0).data(Qt::UserRole), QVariant());
}


