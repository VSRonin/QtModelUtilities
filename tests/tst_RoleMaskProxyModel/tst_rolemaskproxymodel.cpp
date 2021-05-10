#include <QtTest/QTest>
#include <QStringListModel>
#include <rolemaskproxymodel.h>
#include <QSortFilterProxyModel>
#include <QSignalSpy>
#include <QList>
#include <QVariant>
#include <random>
#include "tst_rolemaskproxymodel.h"
#include <../modeltestmanager.h>
#ifdef QTMODELUTILITIES_GENERICMODEL
#    include <genericmodel.h>
#endif

QAbstractItemModel *createNullModel(QObject *parent)
{
    Q_UNUSED(parent)
    return nullptr;
}
QAbstractItemModel *createListModel(QObject *parent)
{
    return new QStringListModel(
            QStringList() << QStringLiteral("1") << QStringLiteral("2") << QStringLiteral("3") << QStringLiteral("4") << QStringLiteral("5"), parent);
}

QAbstractItemModel *createTableModel(QObject *parent)
{
    QAbstractItemModel *result = nullptr;
#ifdef COMPLEX_MODEL_SUPPORT
    result = new ComplexModel(parent);
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

QAbstractItemModel *createTreeModel(QObject *parent)
{
    QAbstractItemModel *result = nullptr;
#ifdef COMPLEX_MODEL_SUPPORT
    result = new ComplexModel(parent);
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

void tst_RoleMaskProxyModel::autoParent()
{
    QObject *parentObj = new QObject;
    auto testItem = new RoleMaskProxyModel(parentObj);
    QSignalSpy testItemDestroyedSpy(testItem, SIGNAL(destroyed(QObject *)));
    QVERIFY(testItemDestroyedSpy.isValid());
    delete parentObj;
    QCOMPARE(testItemDestroyedSpy.count(), 1);
}

void tst_RoleMaskProxyModel::initTestCase()
{
    qRegisterMetaType<QVector<int>>();
}

void tst_RoleMaskProxyModel::testUseRoleMask()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    QFETCH(QModelIndexList, magicNumerIndexes);
    QFETCH(bool, userRoleEditable);
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(baseModel);
    const int magicNumber = 785874;
    for (const QModelIndex &singleIdx : magicNumerIndexes) {
        QVERIFY(proxyModel.setData(proxyModel.mapFromSource(singleIdx), magicNumber, Qt::UserRole));
        QVERIFY(userRoleEditable == baseModel->setData(singleIdx, ~magicNumber, Qt::UserRole));
    }
    testUseRoleMaskRecurse(magicNumber, baseModel, &proxyModel, magicNumerIndexes, userRoleEditable);
    baseModel->deleteLater();
}

void tst_RoleMaskProxyModel::testUseRoleMask_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<QModelIndexList>("magicNumerIndexes");
    QTest::addColumn<bool>("userRoleEditable");
    QAbstractItemModel *baseModel = createListModel(this);
    QTest::newRow("List") << baseModel << QModelIndexList({baseModel->index(0, 0)}) << false;
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table") << baseModel << QModelIndexList({baseModel->index(1, 0), baseModel->index(0, 1)}) << true;
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree") << baseModel
                              << QModelIndexList({baseModel->index(1, 0), baseModel->index(0, 1), baseModel->index(0, 1, baseModel->index(0, 0))})
                              << true;
}

void tst_RoleMaskProxyModel::testMoveRowAfter()
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
    const QPersistentModelIndex parentIdx = baseModel.index(1, 0);
    baseModel.insertColumns(0, 2, parentIdx);
    baseModel.insertRows(0, 5, parentIdx);
    fillData(&baseModel, parentIdx, 50);
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, &baseModel);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(&baseModel);
    const QPersistentModelIndex proxyParentIdx = proxyModel.mapFromSource(parentIdx);
    for (int i = 0; i < proxyModel.rowCount(); ++i)
        QVERIFY(proxyModel.setData(proxyModel.index(i, 0), 1000 + i, Qt::UserRole));
    for (int i = 0; i < proxyModel.rowCount(proxyParentIdx); ++i)
        QVERIFY(proxyModel.setData(proxyModel.index(i, 0, proxyParentIdx), 2000 + i, Qt::UserRole));
    QVERIFY(baseModel.moveRows(QModelIndex(), 0, 1, QModelIndex(), 5));
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 1001);
    QCOMPARE(proxyModel.index(1, 0).data(Qt::UserRole).toInt(), 1002);
    QCOMPARE(proxyModel.index(2, 0).data(Qt::UserRole).toInt(), 1003);
    QCOMPARE(proxyModel.index(3, 0).data(Qt::UserRole).toInt(), 1004);
    QCOMPARE(proxyModel.index(4, 0).data(Qt::UserRole).toInt(), 1000);
    QCOMPARE(proxyParentIdx.data(Qt::UserRole).toInt(), 1001);
    QVERIFY(baseModel.moveRows(parentIdx, 0, 1, parentIdx, 5));
    QCOMPARE(proxyModel.index(0, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2001);
    QCOMPARE(proxyModel.index(1, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2002);
    QCOMPARE(proxyModel.index(2, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2003);
    QCOMPARE(proxyModel.index(3, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2004);
    QCOMPARE(proxyModel.index(4, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2000);
    QVERIFY(baseModel.moveRows(parentIdx, 0, 1, QModelIndex(), 5));
    QCOMPARE(proxyModel.index(0, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2002);
    QCOMPARE(proxyModel.index(1, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2003);
    QCOMPARE(proxyModel.index(2, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2004);
    QCOMPARE(proxyModel.index(3, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2000);
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 1001);
    QCOMPARE(proxyModel.index(1, 0).data(Qt::UserRole).toInt(), 1002);
    QCOMPARE(proxyModel.index(2, 0).data(Qt::UserRole).toInt(), 1003);
    QCOMPARE(proxyModel.index(3, 0).data(Qt::UserRole).toInt(), 1004);
    QCOMPARE(proxyModel.index(4, 0).data(Qt::UserRole).toInt(), 1000);
    QCOMPARE(proxyModel.index(5, 0).data(Qt::UserRole).toInt(), 2001);
    QVERIFY(baseModel.moveRows(QModelIndex(), 1, 1, parentIdx, 4));
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 1001);
    QCOMPARE(proxyModel.index(1, 0).data(Qt::UserRole).toInt(), 1003);
    QCOMPARE(proxyModel.index(2, 0).data(Qt::UserRole).toInt(), 1004);
    QCOMPARE(proxyModel.index(3, 0).data(Qt::UserRole).toInt(), 1000);
    QCOMPARE(proxyModel.index(4, 0).data(Qt::UserRole).toInt(), 2001);
    QCOMPARE(proxyModel.index(0, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2002);
    QCOMPARE(proxyModel.index(1, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2003);
    QCOMPARE(proxyModel.index(2, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2004);
    QCOMPARE(proxyModel.index(3, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2000);
    QCOMPARE(proxyModel.index(4, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 1002);
#else
    QSKIP("This test requires the GenericModel module");
#endif
}

void tst_RoleMaskProxyModel::testMoveRowBefore()
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
    const QPersistentModelIndex parentIdx = baseModel.index(1, 0);
    baseModel.insertColumns(0, 2, parentIdx);
    baseModel.insertRows(0, 5, parentIdx);
    fillData(&baseModel, parentIdx, 50);
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, &baseModel);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(&baseModel);
    const QPersistentModelIndex proxyParentIdx = proxyModel.mapFromSource(parentIdx);
    for (int i = 0; i < proxyModel.rowCount(); ++i)
        QVERIFY(proxyModel.setData(proxyModel.index(i, 0), 1000 + i, Qt::UserRole));
    for (int i = 0; i < proxyModel.rowCount(proxyParentIdx); ++i)
        QVERIFY(proxyModel.setData(proxyModel.index(i, 0, proxyParentIdx), 2000 + i, Qt::UserRole));
    QVERIFY(baseModel.moveRows(QModelIndex(), 4, 1, QModelIndex(), 1));
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 1000);
    QCOMPARE(proxyModel.index(1, 0).data(Qt::UserRole).toInt(), 1004);
    QCOMPARE(proxyModel.index(2, 0).data(Qt::UserRole).toInt(), 1001);
    QCOMPARE(proxyModel.index(3, 0).data(Qt::UserRole).toInt(), 1002);
    QCOMPARE(proxyModel.index(4, 0).data(Qt::UserRole).toInt(), 1003);
    QCOMPARE(proxyParentIdx.data(Qt::UserRole).toInt(), 1001);
    QVERIFY(baseModel.moveRows(parentIdx, 4, 1, parentIdx, 0));
    QCOMPARE(proxyModel.index(0, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2004);
    QCOMPARE(proxyModel.index(1, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2000);
    QCOMPARE(proxyModel.index(2, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2001);
    QCOMPARE(proxyModel.index(3, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2002);
    QCOMPARE(proxyModel.index(4, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2003);
    QVERIFY(baseModel.moveRows(parentIdx, 3, 1, QModelIndex(), 0));
    QCOMPARE(proxyModel.index(0, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2004);
    QCOMPARE(proxyModel.index(1, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2000);
    QCOMPARE(proxyModel.index(2, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2001);
    QCOMPARE(proxyModel.index(3, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2003);
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 2002);
    QCOMPARE(proxyModel.index(1, 0).data(Qt::UserRole).toInt(), 1000);
    QCOMPARE(proxyModel.index(2, 0).data(Qt::UserRole).toInt(), 1004);
    QCOMPARE(proxyModel.index(3, 0).data(Qt::UserRole).toInt(), 1001);
    QCOMPARE(proxyModel.index(4, 0).data(Qt::UserRole).toInt(), 1002);
    QCOMPARE(proxyModel.index(5, 0).data(Qt::UserRole).toInt(), 1003);
    QVERIFY(baseModel.moveRows(QModelIndex(), 4, 1, parentIdx, 1));
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 2002);
    QCOMPARE(proxyModel.index(1, 0).data(Qt::UserRole).toInt(), 1000);
    QCOMPARE(proxyModel.index(2, 0).data(Qt::UserRole).toInt(), 1004);
    QCOMPARE(proxyModel.index(3, 0).data(Qt::UserRole).toInt(), 1001);
    QCOMPARE(proxyModel.index(4, 0).data(Qt::UserRole).toInt(), 1003);
    QCOMPARE(proxyModel.index(0, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2004);
    QCOMPARE(proxyModel.index(1, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 1002);
    QCOMPARE(proxyModel.index(2, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2000);
    QCOMPARE(proxyModel.index(3, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2001);
    QCOMPARE(proxyModel.index(4, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2003);
#else
    QSKIP("This test requires the GenericModel module");
#endif
}

void tst_RoleMaskProxyModel::testMoveColumnAfter()
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
    const QPersistentModelIndex parentIdx = baseModel.index(1, 0);
    baseModel.insertColumns(0, 5, parentIdx);
    baseModel.insertRows(0, 2, parentIdx);
    fillData(&baseModel, parentIdx, 50);
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, &baseModel);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(&baseModel);
    const QPersistentModelIndex proxyParentIdx = proxyModel.mapFromSource(parentIdx);
    for (int i = 0; i < proxyModel.columnCount(); ++i)
        QVERIFY(proxyModel.setData(proxyModel.index(0, i), 1000 + i, Qt::UserRole));
    for (int i = 0; i < proxyModel.columnCount(proxyParentIdx); ++i)
        QVERIFY(proxyModel.setData(proxyModel.index(0, i, proxyParentIdx), 2000 + i, Qt::UserRole));
    QVERIFY(baseModel.moveColumns(QModelIndex(), 0, 1, QModelIndex(), 5));
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 1001);
    QCOMPARE(proxyModel.index(0, 1).data(Qt::UserRole).toInt(), 1002);
    QCOMPARE(proxyModel.index(0, 2).data(Qt::UserRole).toInt(), 1003);
    QCOMPARE(proxyModel.index(0, 3).data(Qt::UserRole).toInt(), 1004);
    QCOMPARE(proxyModel.index(0, 4).data(Qt::UserRole).toInt(), 1000);
    QVERIFY(baseModel.moveColumns(parentIdx, 0, 1, parentIdx, 5));
    QCOMPARE(proxyModel.index(0, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2001);
    QCOMPARE(proxyModel.index(0, 1, proxyParentIdx).data(Qt::UserRole).toInt(), 2002);
    QCOMPARE(proxyModel.index(0, 2, proxyParentIdx).data(Qt::UserRole).toInt(), 2003);
    QCOMPARE(proxyModel.index(0, 3, proxyParentIdx).data(Qt::UserRole).toInt(), 2004);
    QCOMPARE(proxyModel.index(0, 4, proxyParentIdx).data(Qt::UserRole).toInt(), 2000);
    QVERIFY(baseModel.moveColumns(parentIdx, 0, 1, QModelIndex(), 5));
    QCOMPARE(proxyModel.index(0, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2002);
    QCOMPARE(proxyModel.index(0, 1, proxyParentIdx).data(Qt::UserRole).toInt(), 2003);
    QCOMPARE(proxyModel.index(0, 2, proxyParentIdx).data(Qt::UserRole).toInt(), 2004);
    QCOMPARE(proxyModel.index(0, 3, proxyParentIdx).data(Qt::UserRole).toInt(), 2000);
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 1001);
    QCOMPARE(proxyModel.index(0, 1).data(Qt::UserRole).toInt(), 1002);
    QCOMPARE(proxyModel.index(0, 2).data(Qt::UserRole).toInt(), 1003);
    QCOMPARE(proxyModel.index(0, 3).data(Qt::UserRole).toInt(), 1004);
    QCOMPARE(proxyModel.index(0, 4).data(Qt::UserRole).toInt(), 1000);
    QCOMPARE(proxyModel.index(0, 5).data(Qt::UserRole).toInt(), 2001);
    QVERIFY(baseModel.moveColumns(QModelIndex(), 1, 1, parentIdx, 4));
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 1001);
    QCOMPARE(proxyModel.index(0, 1).data(Qt::UserRole).toInt(), 1003);
    QCOMPARE(proxyModel.index(0, 2).data(Qt::UserRole).toInt(), 1004);
    QCOMPARE(proxyModel.index(0, 3).data(Qt::UserRole).toInt(), 1000);
    QCOMPARE(proxyModel.index(0, 4).data(Qt::UserRole).toInt(), 2001);
    QCOMPARE(proxyModel.index(0, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2002);
    QCOMPARE(proxyModel.index(0, 1, proxyParentIdx).data(Qt::UserRole).toInt(), 2003);
    QCOMPARE(proxyModel.index(0, 2, proxyParentIdx).data(Qt::UserRole).toInt(), 2004);
    QCOMPARE(proxyModel.index(0, 3, proxyParentIdx).data(Qt::UserRole).toInt(), 2000);
    QCOMPARE(proxyModel.index(0, 4, proxyParentIdx).data(Qt::UserRole).toInt(), 1002);
#else
    QSKIP("This test requires the GenericModel module");
#endif
}

void tst_RoleMaskProxyModel::testMoveColumnBefore()
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
    const QPersistentModelIndex parentIdx = baseModel.index(1, 0);
    baseModel.insertColumns(0, 5, parentIdx);
    baseModel.insertRows(0, 2, parentIdx);
    fillData(&baseModel, parentIdx, 50);
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, &baseModel);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(&baseModel);
    const QPersistentModelIndex proxyParentIdx = proxyModel.mapFromSource(parentIdx);
    for (int i = 0; i < proxyModel.columnCount(); ++i)
        QVERIFY(proxyModel.setData(proxyModel.index(0, i), 1000 + i, Qt::UserRole));
    for (int i = 0; i < proxyModel.columnCount(proxyParentIdx); ++i)
        QVERIFY(proxyModel.setData(proxyModel.index(0, i, proxyParentIdx), 2000 + i, Qt::UserRole));
    QVERIFY(baseModel.moveColumns(QModelIndex(), 4, 1, QModelIndex(), 1));
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 1000);
    QCOMPARE(proxyModel.index(0, 1).data(Qt::UserRole).toInt(), 1004);
    QCOMPARE(proxyModel.index(0, 2).data(Qt::UserRole).toInt(), 1001);
    QCOMPARE(proxyModel.index(0, 3).data(Qt::UserRole).toInt(), 1002);
    QCOMPARE(proxyModel.index(0, 4).data(Qt::UserRole).toInt(), 1003);
    QVERIFY(baseModel.moveColumns(parentIdx, 4, 1, parentIdx, 0));
    QCOMPARE(proxyModel.index(0, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2004);
    QCOMPARE(proxyModel.index(0, 1, proxyParentIdx).data(Qt::UserRole).toInt(), 2000);
    QCOMPARE(proxyModel.index(0, 2, proxyParentIdx).data(Qt::UserRole).toInt(), 2001);
    QCOMPARE(proxyModel.index(0, 3, proxyParentIdx).data(Qt::UserRole).toInt(), 2002);
    QCOMPARE(proxyModel.index(0, 4, proxyParentIdx).data(Qt::UserRole).toInt(), 2003);
    QVERIFY(baseModel.moveColumns(parentIdx, 3, 1, QModelIndex(), 0));
    QCOMPARE(proxyModel.index(0, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2004);
    QCOMPARE(proxyModel.index(0, 1, proxyParentIdx).data(Qt::UserRole).toInt(), 2000);
    QCOMPARE(proxyModel.index(0, 2, proxyParentIdx).data(Qt::UserRole).toInt(), 2001);
    QCOMPARE(proxyModel.index(0, 3, proxyParentIdx).data(Qt::UserRole).toInt(), 2003);
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 2002);
    QCOMPARE(proxyModel.index(0, 1).data(Qt::UserRole).toInt(), 1000);
    QCOMPARE(proxyModel.index(0, 2).data(Qt::UserRole).toInt(), 1004);
    QCOMPARE(proxyModel.index(0, 3).data(Qt::UserRole).toInt(), 1001);
    QCOMPARE(proxyModel.index(0, 4).data(Qt::UserRole).toInt(), 1002);
    QCOMPARE(proxyModel.index(0, 5).data(Qt::UserRole).toInt(), 1003);
    QVERIFY(baseModel.moveColumns(QModelIndex(), 4, 1, parentIdx, 1));
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 2002);
    QCOMPARE(proxyModel.index(0, 1).data(Qt::UserRole).toInt(), 1000);
    QCOMPARE(proxyModel.index(0, 2).data(Qt::UserRole).toInt(), 1004);
    QCOMPARE(proxyModel.index(0, 3).data(Qt::UserRole).toInt(), 1001);
    QCOMPARE(proxyModel.index(0, 4).data(Qt::UserRole).toInt(), 1003);
    QCOMPARE(proxyModel.index(0, 0, proxyParentIdx).data(Qt::UserRole).toInt(), 2004);
    QCOMPARE(proxyModel.index(0, 1, proxyParentIdx).data(Qt::UserRole).toInt(), 1002);
    QCOMPARE(proxyModel.index(0, 2, proxyParentIdx).data(Qt::UserRole).toInt(), 2000);
    QCOMPARE(proxyModel.index(0, 3, proxyParentIdx).data(Qt::UserRole).toInt(), 2001);
    QCOMPARE(proxyModel.index(0, 4, proxyParentIdx).data(Qt::UserRole).toInt(), 2003);
#else
    QSKIP("This test requires the GenericModel module");
#endif
}

void tst_RoleMaskProxyModel::testRemoveRows_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<int>("removeIndex");
    QTest::addColumn<QModelIndex>("parentIndex");
    QTest::newRow("List Remove Begin") << createListModel(this) << 0 << QModelIndex();
    QTest::newRow("Table Remove Begin") << createTableModel(this) << 0 << QModelIndex();
    QTest::newRow("Tree Remove Begin") << createTreeModel(this) << 0 << QModelIndex();
    QAbstractItemModel *baseModel = createListModel(this);
    QTest::newRow("List Remove End") << baseModel << baseModel->rowCount() - 1 << QModelIndex();
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Remove End") << baseModel << baseModel->rowCount() - 1 << QModelIndex();
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Remove End") << baseModel << baseModel->rowCount() - 1 << QModelIndex();
    QTest::newRow("List Remove Middle") << createListModel(this) << 2 << QModelIndex();
    QTest::newRow("Table Remove Middle") << createTableModel(this) << 2 << QModelIndex();
    QTest::newRow("Tree Remove Middle") << createTreeModel(this) << 2 << QModelIndex();

    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Remove Begin Child") << baseModel << 0 << baseModel->index(0, 0);
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Remove Middle Child") << baseModel << 2 << baseModel->index(0, 0);
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Remove End Child") << baseModel << baseModel->rowCount(baseModel->index(0, 0)) - 1 << baseModel->index(0, 0);
}

void tst_RoleMaskProxyModel::testRemoveRows()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    QFETCH(int, removeIndex);
    QFETCH(QModelIndex, parentIndex);
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(baseModel);
    const int magicNumber = 785874;
    const QModelIndex proxyParent = proxyModel.mapFromSource(parentIndex);
    for (int i = 0; i < proxyModel.rowCount(proxyParent); ++i) {
        QVERIFY(proxyModel.setData(proxyModel.index(i, 0, proxyParent), magicNumber + i, Qt::UserRole));
    }
    QVERIFY(baseModel->removeRow(removeIndex, parentIndex));
    for (int i = 0; i < proxyModel.rowCount(proxyParent); ++i) {
        if (i >= removeIndex)
            QCOMPARE(proxyModel.index(i, 0, proxyParent).data(Qt::UserRole).toInt(), magicNumber + i + 1);
        else
            QCOMPARE(proxyModel.index(i, 0, proxyParent).data(Qt::UserRole).toInt(), magicNumber + i);
    }
    baseModel->deleteLater();
}

void tst_RoleMaskProxyModel::testRemoveColumns_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<int>("removeIndex");
    QTest::addColumn<QModelIndex>("parentIndex");

    QTest::newRow("Table Remove Begin") << createTableModel(this) << 0 << QModelIndex();
    QTest::newRow("Tree Remove Begin") << createTreeModel(this) << 0 << QModelIndex();

    QAbstractItemModel *baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Remove End") << baseModel << baseModel->columnCount() - 1 << QModelIndex();
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Remove End") << baseModel << baseModel->columnCount() - 1 << QModelIndex();

    QTest::newRow("Table Remove Middle") << createTableModel(this) << 2 << QModelIndex();
    QTest::newRow("Tree Remove Middle") << createTreeModel(this) << 2 << QModelIndex();

    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Remove Begin Child") << baseModel << 0 << baseModel->index(0, 0);
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Remove Middle Child") << baseModel << 2 << baseModel->index(0, 0);
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Remove End Child") << baseModel << baseModel->columnCount(baseModel->index(0, 0)) - 1 << baseModel->index(0, 0);
}

void tst_RoleMaskProxyModel::testRemoveColumns()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    QFETCH(int, removeIndex);
    QFETCH(QModelIndex, parentIndex);
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(baseModel);
    const int magicNumber = 785874;
    const QModelIndex proxyParent = proxyModel.mapFromSource(parentIndex);
    for (int i = 0; i < baseModel->columnCount(parentIndex); ++i) {
        QVERIFY(proxyModel.setData(proxyModel.index(0, i, proxyParent), magicNumber + i, Qt::UserRole));
    }
    QVERIFY(baseModel->removeColumn(removeIndex, parentIndex));
    for (int i = 0; i < baseModel->columnCount(parentIndex); ++i) {
        if (i >= removeIndex)
            QCOMPARE(proxyModel.index(0, i, proxyParent).data(Qt::UserRole).toInt(), magicNumber + i + 1);
        else
            QCOMPARE(proxyModel.index(0, i, proxyParent).data(Qt::UserRole).toInt(), magicNumber + i);
    }
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RoleMaskProxyModel::testInsertRow()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    QFETCH(int, insertIndex);
    QFETCH(QModelIndex, parentIndex);
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(baseModel);
    const int magicNumber = 785874;
    const QModelIndex proxyParent = proxyModel.mapFromSource(parentIndex);
    for (int i = 0; i < proxyModel.rowCount(proxyParent); ++i) {
        QVERIFY(proxyModel.setData(proxyModel.index(i, 0, proxyParent), magicNumber + i, Qt::UserRole));
    }
    QVERIFY(baseModel->insertRow(insertIndex, parentIndex));
    for (int i = 0; i < proxyModel.rowCount(proxyParent); ++i) {
        if (i == insertIndex)
            QVERIFY(!proxyModel.index(i, 0, proxyParent).data(Qt::UserRole).isValid());
        else if (i > insertIndex)
            QCOMPARE(proxyModel.index(i, 0, proxyParent).data(Qt::UserRole).toInt(), magicNumber + i - 1);
        else
            QCOMPARE(proxyModel.index(i, 0, proxyParent).data(Qt::UserRole).toInt(), magicNumber + i);
    }
    QVERIFY(baseModel->removeRow(insertIndex, parentIndex));
    baseModel->deleteLater();
}

void tst_RoleMaskProxyModel::testInsertRow_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<int>("insertIndex");
    QTest::addColumn<QModelIndex>("parentIndex");
    QTest::newRow("List Insert Begin") << createListModel(this) << 0 << QModelIndex();
    QTest::newRow("Table Insert Begin") << createTableModel(this) << 0 << QModelIndex();
    QTest::newRow("Tree Insert Begin") << createTreeModel(this) << 0 << QModelIndex();
    QAbstractItemModel *baseModel = createListModel(this);
    QTest::newRow("List Insert End") << baseModel << baseModel->rowCount() << QModelIndex();
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Insert End") << baseModel << baseModel->rowCount() << QModelIndex();
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Insert End") << baseModel << baseModel->rowCount() << QModelIndex();
    QTest::newRow("List Insert Middle") << createListModel(this) << 2 << QModelIndex();
    QTest::newRow("Table Insert Middle") << createTableModel(this) << 2 << QModelIndex();
    QTest::newRow("Tree Insert Middle") << createTreeModel(this) << 2 << QModelIndex();

    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Insert Begin Child") << baseModel << 0 << baseModel->index(0, 0);
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Insert Middle Child") << baseModel << 2 << baseModel->index(0, 0);
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Insert End Child") << baseModel << baseModel->rowCount(baseModel->index(0, 0)) << baseModel->index(0, 0);
}
void tst_RoleMaskProxyModel::testProperties()
{
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    QVERIFY(proxyModel.setProperty("maskedRoles", QVariant::fromValue(QList<int>({Qt::UserRole, Qt::DisplayRole}))));
    const auto roleList = proxyModel.property("maskedRoles").value<QList<int>>();
    QVERIFY(roleList.contains(Qt::UserRole));
    QVERIFY(roleList.contains(Qt::DisplayRole));
    QVERIFY(proxyModel.setProperty("transparentIfEmpty", false));
    QCOMPARE(proxyModel.property("transparentIfEmpty").toBool(), false);
    QVERIFY(proxyModel.setProperty("mergeDisplayEdit", false));
    QCOMPARE(proxyModel.property("mergeDisplayEdit").toBool(), false);
}
void tst_RoleMaskProxyModel::testInsertColumn()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    QFETCH(int, insertIndex);
    QFETCH(QModelIndex, parentIndex);
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(baseModel);
    const int magicNumber = 785874;
    const QModelIndex proxyParent = proxyModel.mapFromSource(parentIndex);
    for (int i = 0; i < baseModel->columnCount(parentIndex); ++i) {
        QVERIFY(proxyModel.setData(proxyModel.index(0, i, proxyParent), magicNumber + i, Qt::UserRole));
    }
    QVERIFY(baseModel->insertColumn(insertIndex, parentIndex));
    for (int i = 0; i < baseModel->columnCount(parentIndex); ++i) {
        if (i == insertIndex)
            QVERIFY(!proxyModel.index(0, i, proxyParent).data(Qt::UserRole).isValid());
        else if (i > insertIndex)
            QCOMPARE(proxyModel.index(0, i, proxyParent).data(Qt::UserRole).toInt(), magicNumber + i - 1);
        else
            QCOMPARE(proxyModel.index(0, i, proxyParent).data(Qt::UserRole).toInt(), magicNumber + i);
    }
    QVERIFY(baseModel->removeColumn(insertIndex, parentIndex));
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_RoleMaskProxyModel::testInsertColumn_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<int>("insertIndex");
    QTest::addColumn<QModelIndex>("parentIndex");

    QTest::newRow("Table Insert Begin") << createTableModel(this) << 0 << QModelIndex();
    QTest::newRow("Tree Insert Begin") << createTreeModel(this) << 0 << QModelIndex();

    QAbstractItemModel *baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Insert End") << baseModel << baseModel->columnCount() << QModelIndex();
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Insert End") << baseModel << baseModel->columnCount() << QModelIndex();

    QTest::newRow("Table Insert Middle") << createTableModel(this) << 2 << QModelIndex();
    QTest::newRow("Tree Insert Middle") << createTreeModel(this) << 2 << QModelIndex();

    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Insert Begin Child") << baseModel << 0 << baseModel->index(0, 0);
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Insert Middle Child") << baseModel << 2 << baseModel->index(0, 0);
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Insert End Child") << baseModel << baseModel->columnCount(baseModel->index(0, 0)) << baseModel->index(0, 0);
}

void tst_RoleMaskProxyModel::testNullModel()
{
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(nullptr);
    QVERIFY(!proxyModel.setData(proxyModel.index(0, 0), 1, Qt::UserRole));
    QVERIFY(!proxyModel.index(0, 0).data(Qt::UserRole).isValid());
    QStringListModel validModel(QStringList() << QStringLiteral("1"));
    proxyModel.setSourceModel(&validModel);
    QVERIFY(proxyModel.setData(proxyModel.index(0, 0), 1, Qt::UserRole));
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 1);
    proxyModel.setSourceModel(nullptr);
    QVERIFY(!proxyModel.index(0, 0).data(Qt::UserRole).isValid());
    QVERIFY(!proxyModel.setData(proxyModel.index(0, 0), 1, Qt::UserRole));
    QVERIFY(!proxyModel.index(0, 0).data(Qt::UserRole).isValid());
}

void tst_RoleMaskProxyModel::testDataChangeSignals_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<bool>("implementsRoles");

    QTest::newRow("List") << createListModel(this) << bool(QT_VERSION >= QT_VERSION_CHECK(5, 0, 2));
    QTest::newRow("Table") << createTableModel(this) << bool(QT_VERSION >= QT_VERSION_CHECK(5, 11, 0));
    QTest::newRow("Tree") << createTreeModel(this) << bool(QT_VERSION >= QT_VERSION_CHECK(5, 11, 0));
}

void tst_RoleMaskProxyModel::testDataChangeSignals()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    QFETCH(bool, implementsRoles);
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setTransparentIfEmpty(true);
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy baseDataChangeSpy(baseModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(baseDataChangeSpy.isValid());
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(baseDataChangeSpy.isValid());
    int timesFired = 0;
    connect(&proxyModel, &QAbstractItemModel::dataChanged,
            [&timesFired](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) -> void {
                Q_UNUSED(topLeft)
                Q_UNUSED(bottomRight)
                Q_UNUSED(roles)
                ++timesFired;
            });
    const QModelIndex proxyDataIdx = proxyModel.index(0, 0);
    const QModelIndex baseDataIdx = baseModel->index(0, 0);
    QVERIFY(proxyModel.setData(proxyDataIdx, 1, Qt::UserRole));
    QCOMPARE(baseDataChangeSpy.count(), 0);
    QCOMPARE(proxyDataChangeSpy.count(), 1);
    QList<QVariant> arguments = proxyDataChangeSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), proxyDataIdx);
    QCOMPARE(arguments.at(1).value<QModelIndex>(), proxyDataIdx);
    QVector<int> rolesVector = arguments.at(2).value<QVector<int>>();
    QCOMPARE(rolesVector.size(), 1);
    QCOMPARE(rolesVector.first(), int(Qt::UserRole));
    if (baseModel->setData(baseDataIdx, 1, Qt::UserRole)) {
        QCOMPARE(baseDataChangeSpy.count(), 1);
        baseDataChangeSpy.clear();
        if (implementsRoles)
            QCOMPARE(proxyDataChangeSpy.count(), 0);
        else
            QCOMPARE(proxyDataChangeSpy.count(), 1);
        proxyDataChangeSpy.clear();
    }
    proxyModel.addMaskedRole(Qt::EditRole);
    proxyDataChangeSpy.clear();
    baseDataChangeSpy.clear();
    QVERIFY(proxyModel.setData(proxyDataIdx, 5, Qt::EditRole));
    QCOMPARE(baseDataChangeSpy.count(), 0);
    QCOMPARE(proxyDataChangeSpy.count(), 1);
    arguments = proxyDataChangeSpy.value(0);
    proxyDataChangeSpy.clear();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), proxyDataIdx);
    QCOMPARE(arguments.at(1).value<QModelIndex>(), proxyDataIdx);
    rolesVector = arguments.at(2).value<QVector<int>>();
    QCOMPARE(rolesVector.size(), 2);
    QVERIFY(rolesVector.contains(Qt::DisplayRole));
    QVERIFY(rolesVector.contains(Qt::EditRole));
    if (baseModel->setData(baseDataIdx, 6, Qt::EditRole)) {
        QCOMPARE(baseDataChangeSpy.count(), 1);
        baseDataChangeSpy.clear();
        QCOMPARE(proxyDataChangeSpy.count(), implementsRoles ? 0 : 1);
        proxyDataChangeSpy.clear();
    }
    proxyModel.setTransparentIfEmpty(false);
    QCOMPARE(baseDataChangeSpy.count(), 0);
    QCOMPARE(proxyDataChangeSpy.count(), 1 + countChildren(baseModel));
    proxyDataChangeSpy.clear();
    QVERIFY(proxyModel.setData(proxyDataIdx, QVariant(), Qt::EditRole));
    QCOMPARE(baseDataChangeSpy.count(), 0);
    QCOMPARE(proxyDataChangeSpy.count(), 1);
    arguments = proxyDataChangeSpy.value(0);
    proxyDataChangeSpy.clear();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), proxyDataIdx);
    QCOMPARE(arguments.at(1).value<QModelIndex>(), proxyDataIdx);
    rolesVector = arguments.at(2).value<QVector<int>>();
    QCOMPARE(rolesVector.size(), 2);
    QVERIFY(rolesVector.contains(Qt::DisplayRole));
    QVERIFY(rolesVector.contains(Qt::EditRole));
    if (baseModel->setData(baseDataIdx, 86, Qt::EditRole)) {
        QCOMPARE(baseDataChangeSpy.count(), 1);
        baseDataChangeSpy.clear();
        QCOMPARE(proxyDataChangeSpy.count(), implementsRoles ? 0 : 1);
        proxyDataChangeSpy.clear();
    }
    baseDataChangeSpy.clear();
    proxyDataChangeSpy.clear();
    proxyModel.setMergeDisplayEdit(false);
    QCOMPARE(baseDataChangeSpy.count(), 0);
    QCOMPARE(proxyDataChangeSpy.count(), 1 + countChildren(baseModel));
    proxyDataChangeSpy.clear();
    proxyModel.removeMaskedRole(Qt::DisplayRole);
    QCOMPARE(baseDataChangeSpy.count(), 0);
    QCOMPARE(proxyDataChangeSpy.count(), 1 + countChildren(baseModel));
    proxyDataChangeSpy.clear();
    QVERIFY(proxyModel.setData(proxyDataIdx, 37, Qt::EditRole));
    QCOMPARE(baseDataChangeSpy.count(), 0);
    QCOMPARE(proxyDataChangeSpy.count(), 1);
    arguments = proxyDataChangeSpy.value(0);
    proxyDataChangeSpy.clear();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), proxyDataIdx);
    QCOMPARE(arguments.at(1).value<QModelIndex>(), proxyDataIdx);
    rolesVector = arguments.at(2).value<QVector<int>>();
    QCOMPARE(rolesVector.size(), 1);
    QVERIFY(rolesVector.contains(Qt::EditRole));
    proxyDataChangeSpy.clear();
    if (baseModel->setData(baseDataIdx, 123, Qt::DisplayRole)) {
        QCOMPARE(baseDataChangeSpy.count(), 1);
        baseDataChangeSpy.clear();
        QCOMPARE(proxyDataChangeSpy.count(), 1);
        proxyDataChangeSpy.clear();
    }
    if (baseModel->setData(baseDataIdx, 147, Qt::EditRole)) {
        QCOMPARE(baseDataChangeSpy.count(), 1);
        baseDataChangeSpy.clear();
        if (proxyDataChangeSpy.count() > 0) {
            arguments = proxyDataChangeSpy.value(0);
            QCOMPARE(arguments.at(0).value<QModelIndex>(), proxyModel.mapFromSource(baseDataIdx));
            QCOMPARE(arguments.at(1).value<QModelIndex>(), proxyModel.mapFromSource(baseDataIdx));
            rolesVector = arguments.at(2).value<QVector<int>>();
            QVERIFY(rolesVector.size() < 2);
            if (rolesVector.size())
                QCOMPARE(rolesVector.first(), int(Qt::DisplayRole));
        }
        proxyDataChangeSpy.clear();
    }
    baseModel->deleteLater();
}

void tst_RoleMaskProxyModel::testTransparentIfEmpty()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.setTransparentIfEmpty(true);
    proxyModel.addMaskedRole(Qt::DisplayRole);
    proxyModel.setSourceModel(baseModel);
    const QModelIndex proxyActionIndex = proxyModel.index(1, 0);
    proxyModel.setData(proxyActionIndex, 777888999, Qt::DisplayRole);
    testTransparentIfEmptyRecurse(baseModel, &proxyModel, proxyActionIndex, 777888999, false);
    QSignalSpy proxyTransparentChangeSpy(&proxyModel, SIGNAL(transparentIfEmptyChanged(bool)));
    QVERIFY(proxyTransparentChangeSpy.isValid());
    proxyModel.setTransparentIfEmpty(false);
    QCOMPARE(proxyTransparentChangeSpy.count(), 1);
    testTransparentIfEmptyRecurse(baseModel, &proxyModel, proxyActionIndex, 777888999, true);
    proxyModel.setTransparentIfEmpty(false);
    QCOMPARE(proxyTransparentChangeSpy.count(), 1);
    baseModel->deleteLater();
}

void tst_RoleMaskProxyModel::testTransparentIfEmptyRecurse(const QAbstractItemModel *const baseModel, const RoleMaskProxyModel *const proxyModel,
                                                           const QModelIndex &maskedIdx, const QVariant &maskedVal, bool nonMaskedIsNull,
                                                           const QModelIndex &sourceParent)
{
    for (int i = 0; i < baseModel->rowCount(sourceParent); ++i) {
        for (int j = 0; j < baseModel->columnCount(sourceParent); ++j) {
            const QModelIndex currBsaIdx = baseModel->index(i, j, sourceParent);
            const QModelIndex currProxIdx = proxyModel->mapFromSource(currBsaIdx);
            if (currProxIdx == maskedIdx) {
                QCOMPARE(currProxIdx.data(), maskedVal);
                QVERIFY(currBsaIdx.data() != maskedVal);
            } else if (nonMaskedIsNull) {
                QVERIFY(!currProxIdx.data().isValid());
                QVERIFY(currBsaIdx.data().isValid());
            } else {
                QCOMPARE(currProxIdx.data(), currBsaIdx.data());
            }
            if (baseModel->hasChildren(currBsaIdx)) {
                testTransparentIfEmptyRecurse(baseModel, proxyModel, maskedIdx, maskedVal, nonMaskedIsNull, currBsaIdx);
            }
        }
    }
}

void tst_RoleMaskProxyModel::testTransparentIfEmpty_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Table") << createTableModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_RoleMaskProxyModel::testMergeDisplayEdit()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.setTransparentIfEmpty(false);
    proxyModel.addMaskedRole(Qt::DisplayRole);
    proxyModel.setSourceModel(baseModel);
    const QModelIndex proxyActionIndex = proxyModel.index(1, 0);
    QVERIFY(proxyModel.setData(proxyActionIndex, 665522, Qt::EditRole));
    QCOMPARE(proxyActionIndex.data(Qt::DisplayRole).toInt(), 665522);
    QCOMPARE(proxyActionIndex.data(Qt::EditRole).toInt(), 665522);
    QSignalSpy proxyMergeDisplayEditChangeSpy(&proxyModel, SIGNAL(mergeDisplayEditChanged(bool)));
    QVERIFY(proxyMergeDisplayEditChangeSpy.isValid());
    proxyModel.setMergeDisplayEdit(false);
    QCOMPARE(proxyMergeDisplayEditChangeSpy.count(), 1);
    QCOMPARE(proxyActionIndex.data(Qt::DisplayRole).toInt(), 665522);
    QCOMPARE(proxyActionIndex.data(Qt::EditRole).toInt(), 665522);
    QVERIFY(proxyModel.setData(proxyActionIndex, QVariant(), Qt::EditRole));
    QCOMPARE(proxyActionIndex.data(Qt::DisplayRole).toInt(), 665522);
    QCOMPARE(proxyActionIndex.data(Qt::EditRole), QVariant());
    proxyModel.setMergeDisplayEdit(false);
    QCOMPARE(proxyMergeDisplayEditChangeSpy.count(), 1);
    baseModel->deleteLater();
}

void tst_RoleMaskProxyModel::testMergeDisplayEdit_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Table") << createTableModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_RoleMaskProxyModel::testManageMaskedRoles()
{
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    proxyModel.setMergeDisplayEdit(true);
    QVERIFY(proxyModel.maskedRoles().isEmpty());
    proxyModel.addMaskedRole(Qt::EditRole);
    QCOMPARE(proxyModel.maskedRoles().size(), 1);
    QCOMPARE(proxyModel.maskedRoles().value(0), int(Qt::DisplayRole));
    proxyModel.addMaskedRole(Qt::DisplayRole);
    QCOMPARE(proxyModel.maskedRoles().size(), 1);
    QCOMPARE(proxyModel.maskedRoles().value(0), int(Qt::DisplayRole));
    proxyModel.addMaskedRole(Qt::UserRole);
    QCOMPARE(proxyModel.maskedRoles().size(), 2);
    QVERIFY(proxyModel.maskedRoles().contains(Qt::DisplayRole));
    QVERIFY(proxyModel.maskedRoles().contains(Qt::UserRole));
    proxyModel.addMaskedRole(Qt::UserRole);
    QCOMPARE(proxyModel.maskedRoles().size(), 2);
    QVERIFY(proxyModel.maskedRoles().contains(Qt::DisplayRole));
    QVERIFY(proxyModel.maskedRoles().contains(Qt::UserRole));
    proxyModel.clearMaskedRoles();
    QCOMPARE(proxyModel.maskedRoles().size(), 0);
    proxyModel.setMaskedRoles({Qt::UserRole, Qt::EditRole});
    QCOMPARE(proxyModel.maskedRoles().size(), 2);
    QVERIFY(proxyModel.maskedRoles().contains(Qt::DisplayRole));
    QVERIFY(proxyModel.maskedRoles().contains(Qt::UserRole));
    proxyModel.removeMaskedRole(Qt::UserRole);
    QCOMPARE(proxyModel.maskedRoles().size(), 1);
    QCOMPARE(proxyModel.maskedRoles().value(0), int(Qt::DisplayRole));
    proxyModel.removeMaskedRole(Qt::EditRole);
    QVERIFY(proxyModel.maskedRoles().isEmpty());
    proxyModel.setMaskedRoles({Qt::DisplayRole, Qt::EditRole});
    QCOMPARE(proxyModel.maskedRoles().size(), 1);
    QCOMPARE(proxyModel.maskedRoles().value(0), int(Qt::DisplayRole));
    proxyModel.removeMaskedRole(Qt::DisplayRole);
    QVERIFY(proxyModel.maskedRoles().isEmpty());

    proxyModel.setMergeDisplayEdit(false);
    proxyModel.addMaskedRole(Qt::EditRole);
    QCOMPARE(proxyModel.maskedRoles().size(), 1);
    QCOMPARE(proxyModel.maskedRoles().value(0), int(Qt::EditRole));
    proxyModel.addMaskedRole(Qt::DisplayRole);
    QCOMPARE(proxyModel.maskedRoles().size(), 2);
    QVERIFY(proxyModel.maskedRoles().contains(Qt::DisplayRole));
    QVERIFY(proxyModel.maskedRoles().contains(Qt::EditRole));
    proxyModel.addMaskedRole(Qt::UserRole);
    QCOMPARE(proxyModel.maskedRoles().size(), 3);
    QVERIFY(proxyModel.maskedRoles().contains(Qt::DisplayRole));
    QVERIFY(proxyModel.maskedRoles().contains(Qt::EditRole));
    QVERIFY(proxyModel.maskedRoles().contains(Qt::UserRole));
    proxyModel.addMaskedRole(Qt::UserRole);
    QCOMPARE(proxyModel.maskedRoles().size(), 3);
    QVERIFY(proxyModel.maskedRoles().contains(Qt::DisplayRole));
    QVERIFY(proxyModel.maskedRoles().contains(Qt::EditRole));
    QVERIFY(proxyModel.maskedRoles().contains(Qt::UserRole));
    proxyModel.clearMaskedRoles();
    QCOMPARE(proxyModel.maskedRoles().size(), 0);
    proxyModel.setMaskedRoles({Qt::UserRole, Qt::EditRole, Qt::DisplayRole});
    QCOMPARE(proxyModel.maskedRoles().size(), 3);
    QVERIFY(proxyModel.maskedRoles().contains(Qt::DisplayRole));
    QVERIFY(proxyModel.maskedRoles().contains(Qt::UserRole));
    QVERIFY(proxyModel.maskedRoles().contains(Qt::EditRole));
    proxyModel.removeMaskedRole(Qt::UserRole);
    QCOMPARE(proxyModel.maskedRoles().size(), 2);
    QVERIFY(proxyModel.maskedRoles().contains(Qt::DisplayRole));
    QVERIFY(proxyModel.maskedRoles().contains(Qt::EditRole));
    proxyModel.removeMaskedRole(Qt::EditRole);
    QCOMPARE(proxyModel.maskedRoles().size(), 1);
    QCOMPARE(proxyModel.maskedRoles().value(0), int(Qt::DisplayRole));
    proxyModel.removeMaskedRole(Qt::DisplayRole);
    QVERIFY(proxyModel.maskedRoles().isEmpty());
}

void tst_RoleMaskProxyModel::testDisconnectedModel()
{
    QStringListModel baseModel1(QStringList({QStringLiteral("London"), QStringLiteral("Berlin"), QStringLiteral("Paris")}));
    QStringListModel baseModel2(QStringList({QStringLiteral("Rome"), QStringLiteral("Madrid"), QStringLiteral("Prague")}));
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.setTransparentIfEmpty(true);
    proxyModel.setSourceModel(&baseModel1);
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    baseModel1.setData(baseModel1.index(0, 0), QStringLiteral("New York"));
    QCOMPARE(proxyDataChangeSpy.count(), 1);
    proxyDataChangeSpy.clear();
    proxyModel.setSourceModel(&baseModel2);
    baseModel1.setData(baseModel1.index(1, 0), QStringLiteral("Tokyo"));
    QCOMPARE(proxyDataChangeSpy.count(), 0);
    proxyDataChangeSpy.clear();
    baseModel2.setData(baseModel2.index(0, 0), QStringLiteral("Lima"));
    QCOMPARE(proxyDataChangeSpy.count(), 1);
}

int tst_RoleMaskProxyModel::countChildren(const QAbstractItemModel *const baseModel, const QModelIndex &parIdx)
{
    const int rowCnt = baseModel->rowCount(parIdx);
    const int colCnt = baseModel->columnCount(parIdx);
    int result = 0;
    for (int i = 0; i < rowCnt; ++i) {
        for (int j = 0; j < colCnt; ++j) {
            const QModelIndex tempIdx = baseModel->index(i, j, parIdx);
            if (baseModel->hasChildren(tempIdx))
                result += 1 + countChildren(baseModel, tempIdx);
        }
    }
    return result;
}

void tst_RoleMaskProxyModel::testUseRoleMaskRecurse(const int magicNumber, const QAbstractItemModel *const baseModel,
                                                    const RoleMaskProxyModel *const proxyModel, const QModelIndexList &magicNumerIndexes,
                                                    const bool userRoleEditable, const QModelIndex &srcParent, const QModelIndex &prxParent)
{
    const int rowCnt = baseModel->rowCount(srcParent);
    const int colCnt = baseModel->columnCount(srcParent);
    for (int i = 0; i < rowCnt; ++i) {
        for (int j = 0; j < colCnt; ++j) {
            const QModelIndex baseParent = baseModel->index(i, j, srcParent);
            const QModelIndex proxyParent = proxyModel->index(i, j, prxParent);
            if (magicNumerIndexes.contains(baseParent)) {
                QCOMPARE(proxyParent.data(Qt::UserRole).toInt(), magicNumber);
                if (userRoleEditable)
                    QCOMPARE(baseParent.data(Qt::UserRole).toInt(), ~magicNumber);
                else
                    QVERIFY(baseParent.data(Qt::UserRole).toInt() != magicNumber);
            } else {
                QCOMPARE(proxyParent.data(Qt::UserRole), baseParent.data(Qt::UserRole));
            }
            QCOMPARE(proxyParent.data(), baseParent.data());
            QCOMPARE(proxyModel->hasChildren(proxyParent), baseModel->hasChildren(baseParent));
            if (baseModel->hasChildren(baseParent))
                testUseRoleMaskRecurse(magicNumber, baseModel, proxyModel, magicNumerIndexes, userRoleEditable, baseParent, proxyParent);
        }
    }
}

void tst_RoleMaskProxyModel::testSetItemData_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<int>("idxRow");
    QTest::addColumn<int>("idxCol");
    QTest::newRow("List") << createListModel(this) << 0 << 0;
    QTest::newRow("Table") << createTableModel(this) << 1 << 1;
    QTest::newRow("Tree Root") << createTreeModel(this) << 1 << 0;
}

void tst_RoleMaskProxyModel::testSetItemDataDataChanged_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<int>("idxRow");
    QTest::addColumn<int>("idxCol");
    QTest::newRow("List") << createListModel(this) << 0 << 0;
    QTest::newRow("Table") << createTableModel(this) << 1 << 1;
    QTest::newRow("Tree Root") << createTreeModel(this) << 1 << 0;
}

void tst_RoleMaskProxyModel::testSetItemDataDataChanged()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    QFETCH(int, idxRow);
    QFETCH(int, idxCol);
    QMap<int, QVariant> itemDataSet{{// TextAlignmentRole
                                     std::make_pair<int, QVariant>(Qt::UserRole, 5),
                                     std::make_pair<int, QVariant>(Qt::DisplayRole, QStringLiteral("Test")),
                                     std::make_pair<int, QVariant>(Qt::ToolTipRole, QStringLiteral("ToolTip"))}};
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.setTransparentIfEmpty(true);
    proxyModel.setMaskedRoles({Qt::UserRole, Qt::ToolTipRole, Qt::TextAlignmentRole});
    proxyModel.setSourceModel(baseModel);
    const QModelIndex proxyIdX = proxyModel.index(idxRow, idxCol);
    QVERIFY(proxyModel.setData(proxyIdX, Qt::AlignRight, Qt::TextAlignmentRole));
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QCOMPARE(proxyDataChangeSpy.size(), 2);
    auto argList = proxyDataChangeSpy.takeFirst();
    QCOMPARE(argList.at(0).value<QModelIndex>(), proxyIdX);
    QCOMPARE(argList.at(1).value<QModelIndex>(), proxyIdX);
    auto rolesVector = argList.at(2).value<QVector<int>>();
#ifndef SKIP_QTBUG_45114
    QCOMPARE(rolesVector.size(), 2);
    QVERIFY(!rolesVector.contains(Qt::TextAlignmentRole));
    QVERIFY(rolesVector.contains(Qt::ToolTipRole));
    QVERIFY(rolesVector.contains(Qt::UserRole));
#endif
    argList = proxyDataChangeSpy.takeFirst();
    QCOMPARE(argList.at(0).value<QModelIndex>(), proxyIdX);
    QCOMPARE(argList.at(1).value<QModelIndex>(), proxyIdX);
#ifndef SKIP_QTBUG_45114
    rolesVector = argList.at(2).value<QVector<int>>();
    QCOMPARE(rolesVector.size(), 2);
    QVERIFY(rolesVector.contains(Qt::DisplayRole));
    QVERIFY(rolesVector.contains(Qt::EditRole));
#endif
    itemDataSet[Qt::UserRole] = 6;
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QVERIFY(proxyDataChangeSpy.size() >= 1); // the source model signal depends on the version of Qt
    argList = proxyDataChangeSpy.takeFirst();
    QCOMPARE(argList.at(0).value<QModelIndex>(), proxyIdX);
    QCOMPARE(argList.at(1).value<QModelIndex>(), proxyIdX);
    rolesVector = argList.at(2).value<QVector<int>>();
    QCOMPARE(rolesVector.size(), 1);
    QVERIFY(rolesVector.contains(Qt::UserRole));
    itemDataSet.clear();
    itemDataSet[Qt::UserRole] = 6;
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
#ifndef SKIP_QTBUG_67511
    QCOMPARE(proxyDataChangeSpy.size(), 0);
#endif
    baseModel->deleteLater();
}

void tst_RoleMaskProxyModel::testSort_data()
{
    QTest::addColumn<bool>("sortViaProxy");
    QTest::newRow("Sort via Base") << false;
    QTest::newRow("Sort via Proxy") << true;
}

void tst_RoleMaskProxyModel::testSort()
{
    QFETCH(bool, sortViaProxy);
    QStringList sequence;
    sequence.reserve(100);
    for (int i = 0; i < 100; ++i)
        sequence.append(QStringLiteral("%1").arg(i, 3, 10, QLatin1Char('0')));
    std::shuffle(sequence.begin(), sequence.end(), std::mt19937(88));
    QStringListModel baseModel(sequence);
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    proxyModel.setSourceModel(&baseModel);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setTransparentIfEmpty(true);
    for (int i = 0; i < 100; ++i) {
        QVERIFY(proxyModel.setData(proxyModel.index(i, 0), proxyModel.index(i, 0).data().toString().toInt(), Qt::UserRole));
    }
    if (sortViaProxy)
        proxyModel.sort(0, Qt::AscendingOrder);
    else
        baseModel.sort(0, Qt::AscendingOrder);
    for (int i = 1; i < 100; ++i) {
        QCOMPARE(proxyModel.index(i, 0).data().toString().toInt(), proxyModel.index(i - 1, 0).data().toString().toInt() + 1);
        QCOMPARE(proxyModel.index(i, 0).data(Qt::UserRole).toInt(), proxyModel.index(i - 1, 0).data(Qt::UserRole).toInt() + 1);
    }
}

void tst_RoleMaskProxyModel::testEmptyProxy()
{
    QSortFilterProxyModel emptyProxy;
    RoleMaskProxyModel maskProxyModel;
    new ModelTest(&maskProxyModel, this);
    maskProxyModel.setMaskedRoles({Qt::DisplayRole, Qt::ToolTipRole, Qt::BackgroundRole});
    maskProxyModel.setMergeDisplayEdit(true);
    maskProxyModel.setTransparentIfEmpty(true);
    maskProxyModel.setSourceModel(&emptyProxy);
    QCOMPARE(maskProxyModel.rowCount(), 0);
    QCOMPARE(maskProxyModel.columnCount(), 0);
    QCOMPARE(maskProxyModel.sourceModel(), &emptyProxy);
}

void tst_RoleMaskProxyModel::testSetItemData()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    QFETCH(int, idxRow);
    QFETCH(int, idxCol);
    const QMap<int, QVariant> itemDataSet{{std::make_pair<int, QVariant>(Qt::UserRole, 5),
                                           std::make_pair<int, QVariant>(Qt::DisplayRole, QStringLiteral("Test")),
                                           std::make_pair<int, QVariant>(Qt::ToolTipRole, QStringLiteral("ToolTip"))}};
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.setTransparentIfEmpty(true);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setMaskedRoles({Qt::UserRole, Qt::ToolTipRole, Qt::TextAlignmentRole});
    const QModelIndex proxyIdX = proxyModel.index(idxRow, idxCol);
    QVERIFY(proxyModel.setData(proxyIdX, Qt::AlignRight, Qt::TextAlignmentRole));
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::DisplayRole).toString(), QStringLiteral("Test"));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::EditRole).toString(), QStringLiteral("Test"));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::UserRole).toInt(), 5);
    QCOMPARE(proxyModel.data(proxyIdX, Qt::ToolTipRole).toString(), QStringLiteral("ToolTip"));
#ifndef SKIP_QTBUG_45114
    QCOMPARE(proxyModel.data(proxyIdX, Qt::TextAlignmentRole).toInt(), Qt::AlignRight);
#endif
    baseModel->deleteLater();
}
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
void tst_RoleMaskProxyModel::testClearItemData()
{
    QStringListModel baseModel(QStringList() << QStringLiteral("Test") << QStringLiteral("Test1"));
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, &baseModel);
    proxyModel.setSourceModel(&baseModel);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setData(proxyModel.index(0, 0), 11, Qt::UserRole);
    proxyModel.setData(proxyModel.index(1, 0), 21, Qt::UserRole);
    QVERIFY(proxyModel.clearItemData(proxyModel.index(1, 0)));
    QCOMPARE(proxyModel.index(0, 0).data(Qt::UserRole).toInt(), 11);
    QVERIFY(!proxyModel.index(1, 0).data(Qt::UserRole).isValid());
    QCOMPARE(proxyModel.index(0, 0).data().toString(), QStringLiteral("Test"));
    QVERIFY(proxyModel.index(1, 0).data().toString().isEmpty());
}
void tst_RoleMaskProxyModel::testMultiData()
{
    QStringListModel baseModel(QStringList() << QStringLiteral("Test") << QStringLiteral("Test1"));
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, &baseModel);
    proxyModel.setSourceModel(&baseModel);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setData(proxyModel.index(0, 0), 11, Qt::UserRole);

    QModelRoleData rolesData[] = {QModelRoleData(Qt::EditRole), QModelRoleData(Qt::DisplayRole), QModelRoleData(Qt::UserRole),
                                  QModelRoleData(Qt::UserRole + 1)};
    QModelRoleDataSpan span(rolesData);
    proxyModel.multiData(proxyModel.index(0, 0), span);
    QCOMPARE(span.dataForRole(Qt::EditRole)->toString(), QStringLiteral("Test"));
    QCOMPARE(span.dataForRole(Qt::DisplayRole)->toString(), QStringLiteral("Test"));
    QCOMPARE(span.dataForRole(Qt::UserRole)->toInt(), 11);
    QVERIFY(!span.dataForRole(Qt::UserRole + 1)->isValid());

    proxyModel.setTransparentIfEmpty(false);
    proxyModel.addMaskedRole(Qt::DisplayRole);
    proxyModel.multiData(proxyModel.index(0, 0), span);
    QVERIFY(!span.dataForRole(Qt::EditRole)->isValid());
    QVERIFY(!span.dataForRole(Qt::DisplayRole)->isValid());
    QCOMPARE(span.dataForRole(Qt::UserRole)->toInt(), 11);
    QVERIFY(!span.dataForRole(Qt::UserRole + 1)->isValid());

    proxyModel.setMergeDisplayEdit(false);
    proxyModel.setMaskedRoles({Qt::DisplayRole});
    proxyModel.setData(proxyModel.index(0, 0), 21, Qt::DisplayRole);
    proxyModel.multiData(proxyModel.index(0, 0), span);
    QCOMPARE(span.dataForRole(Qt::EditRole)->toString(), QStringLiteral("Test"));
    QVERIFY(!span.dataForRole(Qt::UserRole)->isValid());
    QVERIFY(!span.dataForRole(Qt::UserRole + 1)->isValid());
    QCOMPARE(span.dataForRole(Qt::DisplayRole)->toInt(), 21);
}
#endif
