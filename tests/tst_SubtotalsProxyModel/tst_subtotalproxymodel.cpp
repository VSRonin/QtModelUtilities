#include <QtTest/QTest>
#include <subtotalproxymodel.h>
#include <QSignalSpy>
#include <QList>
#include <QVariant>
#include "tst_subtotalproxymodel.h"
#include <modeltestmanager.h>

QAbstractItemModel *createFlatTestModel(QObject *parent)
{
    QAbstractItemModel *result = nullptr;
#ifdef COMPLEX_MODEL_SUPPORT
    result = new ComplexModel(parent);
    result->insertRows(0, 5);
    result->insertColumns(0, 3);
    result->setData(result->index(0, 0), QStringLiteral("Europe"), Qt::EditRole);
    result->setData(result->index(1, 0), QStringLiteral("Europe"), Qt::EditRole);
    result->setData(result->index(2, 0), QStringLiteral("Europe"), Qt::EditRole);
    result->setData(result->index(3, 0), QStringLiteral("Asia"), Qt::EditRole);
    result->setData(result->index(4, 0), QStringLiteral("Asia"), Qt::EditRole);

    result->setData(result->index(0, 1), QStringLiteral("Gemany"), Qt::EditRole);
    result->setData(result->index(1, 1), QStringLiteral("France"), Qt::EditRole);
    result->setData(result->index(2, 1), QStringLiteral("Italy"), Qt::EditRole);
    result->setData(result->index(3, 1), QStringLiteral("India"), Qt::EditRole);
    result->setData(result->index(4, 1), QStringLiteral("China"), Qt::EditRole);

    result->setData(result->index(0, 2), 83491249, Qt::EditRole);
    result->setData(result->index(1, 2), 68736000, Qt::EditRole);
    result->setData(result->index(2, 2), 58925596, Qt::EditRole);
    result->setData(result->index(3, 2), 1417492000, Qt::EditRole);
    result->setData(result->index(4, 2), 1408280000, Qt::EditRole);

    result->setData(result->index(3, 2), 0.173, Qt::UserRole);
    result->setData(result->index(4, 2), 0.171, Qt::UserRole);
    result->setData(result->index(0, 2), 0.01, Qt::UserRole);
    result->setData(result->index(1, 2), 0.008, Qt::UserRole);
    result->setData(result->index(2, 2), 0.007, Qt::UserRole);
#endif
    return result;
}

QAbstractItemModel *createTreeTestModel(QObject *parent)
{
    QAbstractItemModel *result = nullptr;
#ifdef COMPLEX_MODEL_SUPPORT
    result = new ComplexModel(parent);
    result->insertRows(0, 2);
    result->insertColumns(0, 2);
    QModelIndex continentIdx = result->index(0, 0);
    result->setData(continentIdx, QStringLiteral("Europe"), Qt::EditRole);
    result->insertColumns(0, 2, continentIdx);
    result->insertRows(0, 3, continentIdx);
    result->setData(result->index(0, 0, continentIdx), QStringLiteral("Gemany"), Qt::EditRole);
    result->setData(result->index(1, 0, continentIdx), QStringLiteral("France"), Qt::EditRole);
    result->setData(result->index(2, 0, continentIdx), QStringLiteral("Italy"), Qt::EditRole);
    result->setData(result->index(0, 1, continentIdx), 83491249, Qt::EditRole);
    result->setData(result->index(1, 1, continentIdx), 68736000, Qt::EditRole);
    result->setData(result->index(2, 1, continentIdx), 58925596, Qt::EditRole);
    result->setData(result->index(0, 1, continentIdx), 0.01, Qt::UserRole);
    result->setData(result->index(1, 1, continentIdx), 0.008, Qt::UserRole);
    result->setData(result->index(2, 1, continentIdx), 0.007, Qt::UserRole);
    continentIdx = result->index(1, 0);
    result->setData(continentIdx, QStringLiteral("Asia"), Qt::EditRole);
    result->insertColumns(0, 2, continentIdx);
    result->insertRows(0, 2, continentIdx);
    result->setData(result->index(0, 0, continentIdx), QStringLiteral("India"), Qt::EditRole);
    result->setData(result->index(1, 0, continentIdx), QStringLiteral("China"), Qt::EditRole);
    result->setData(result->index(0, 1, continentIdx), 1417492000, Qt::EditRole);
    result->setData(result->index(1, 1, continentIdx), 1408280000, Qt::EditRole);
    result->setData(result->index(0, 1, continentIdx), 0.173, Qt::UserRole);
    result->setData(result->index(1, 1, continentIdx), 0.171, Qt::UserRole);
#endif
    return result;
}

/*
void tst_RoleMaskProxyModel::testItemDataTransParent()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel = new ComplexModel(this);
    baseModel->insertRow(0);
    baseModel->insertColumn(0);
    const QModelIndex baseIdx = baseModel->index(0, 0);
    QVERIFY(baseModel->setData(baseIdx, 1, Qt::UserRole));
    QVERIFY(baseModel->setData(baseIdx, 2, Qt::UserRole + 1));
    auto itemData = baseModel->itemData(baseIdx);
    QCOMPARE(itemData.size(), 2);
    QCOMPARE(itemData.value(Qt::UserRole).toInt(), 1);
    QCOMPARE(itemData.value(Qt::UserRole + 1).toInt(), 2);
    RoleMaskProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    proxyModel.addMaskedRole(Qt::UserRole + 1);
    proxyModel.addMaskedRole(Qt::UserRole + 2);
    const QModelIndex proxyIdx = proxyModel.index(0, 0);
    QVERIFY(proxyModel.setData(proxyIdx, 200, Qt::UserRole + 2));
    proxyModel.setTransparentIfEmpty(true);
    itemData = proxyModel.itemData(proxyIdx);
    QCOMPARE(itemData.size(), 3);
    QCOMPARE(itemData.value(Qt::UserRole).toInt(), 1);
    QCOMPARE(itemData.value(Qt::UserRole + 2).toInt(), 200);
    QCOMPARE(itemData.value(Qt::UserRole + 1).toInt(), 2);
    proxyModel.setTransparentIfEmpty(false);
    itemData = proxyModel.itemData(proxyIdx);
    QCOMPARE(itemData.size(), 2);
    QCOMPARE(itemData.value(Qt::UserRole).toInt(), 1);
    QCOMPARE(itemData.value(Qt::UserRole + 2).toInt(), 200);
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}
*/

void tst_SubtotalProxyModel::initTestCase() { }

void tst_SubtotalProxyModel::autoParent()
{
    QObject *parentObj = new QObject;
    auto testItem = new SubtotalProxyModel(parentObj);
    QSignalSpy testItemDestroyedSpy(testItem, SIGNAL(destroyed(QObject *)));
    QVERIFY(testItemDestroyedSpy.isValid());
    delete parentObj;
    QCOMPARE(testItemDestroyedSpy.count(), 1);
}

void tst_SubtotalProxyModel::testDisconnectedModel()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QAbstractItemModel *baseModel1 = createFlatTestModel(this);
    QAbstractItemModel *baseModel2 = createFlatTestModel(this);
    SubtotalProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    proxyModel.setSourceModel(baseModel1);
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    baseModel1->setData(baseModel1->index(0, 0), QStringLiteral("America"));
    QCOMPARE(proxyDataChangeSpy.count(), 1);
    proxyDataChangeSpy.clear();
    proxyModel.setSourceModel(baseModel2);
    baseModel1->setData(baseModel1->index(1, 0), QStringLiteral("America"));
    QCOMPARE(proxyDataChangeSpy.count(), 0);
    proxyDataChangeSpy.clear();
    baseModel2->setData(baseModel2->index(0, 0), QStringLiteral("America"));
    QCOMPARE(proxyDataChangeSpy.count(), 1);
    baseModel1->deleteLater();
    baseModel2->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_SubtotalProxyModel::testRowCount_data()
{

    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<SubtotalProxyModel::SubtotalLocations>("locations");
    QTest::addColumn<int>("rootCount");
    QTest::addColumn<int>("childCount");
    QAbstractItemModel *baseModel = createFlatTestModel(this);
    QTest::newRow("Flat top") << baseModel << SubtotalProxyModel::SubtotalLocations(SubtotalProxyModel::slTop) << baseModel->rowCount() + 1 << 0;
    baseModel = createFlatTestModel(this);
    QTest::newRow("Flat bottom") << baseModel << SubtotalProxyModel::SubtotalLocations(SubtotalProxyModel::slBottom) << baseModel->rowCount() + 1
                                 << 0;
    baseModel = createFlatTestModel(this);
    QTest::newRow("Flat both") << baseModel << (SubtotalProxyModel::slTop | SubtotalProxyModel::slBottom) << baseModel->rowCount() + 2 << 0;

    baseModel = createTreeTestModel(this);
    QTest::newRow("Tree top") << baseModel << SubtotalProxyModel::SubtotalLocations(SubtotalProxyModel::slTop) << baseModel->rowCount() + 1
                              << baseModel->rowCount(baseModel->index(0, 0)) + 1;
    baseModel = createTreeTestModel(this);
    QTest::newRow("Tree bottom") << baseModel << SubtotalProxyModel::SubtotalLocations(SubtotalProxyModel::slBottom) << baseModel->rowCount() + 1
                                 << baseModel->rowCount(baseModel->index(1, 0)) + 1;
    baseModel = createTreeTestModel(this);
    QTest::newRow("Tree both") << baseModel << (SubtotalProxyModel::slTop | SubtotalProxyModel::slBottom) << baseModel->rowCount() + 2
                               << baseModel->rowCount(baseModel->index(0, 0)) + 2;
    baseModel = createTreeTestModel(this);
    QTest::newRow("Tree all") << baseModel << (SubtotalProxyModel::slTop | SubtotalProxyModel::slBottom | SubtotalProxyModel::slOnParent)
                              << baseModel->rowCount() + 2 << baseModel->rowCount(baseModel->index(0, 0)) + 2;
}

void tst_SubtotalProxyModel::testRowCount()
{
#ifdef COMPLEX_MODEL_SUPPORT
    QFETCH(QAbstractItemModel *, baseModel);
    QFETCH(SubtotalProxyModel::SubtotalLocations, locations);
    QFETCH(int, rootCount);
    QFETCH(int, childCount);
    SubtotalProxyModel proxyModel;
    const QScopedPointer<ModelTest, QScopedPointerDeleteLater> modelTest(new ModelTest(&proxyModel, this)); // to make clazy happy
    proxyModel.setTotalLocations(locations);
    proxyModel.setSubTotalColumn(1, tst_SubtotalProxyModel::intSum);
    proxyModel.setSourceModel(baseModel);
    QCOMPARE(proxyModel.rowCount(), rootCount);
    QCOMPARE(proxyModel.rowCount(proxyModel.index(1, 0)), childCount);
    baseModel->deleteLater();
#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_SubtotalProxyModel::intSum(QVariant &r, const QVariant &a)
{
    r = r.toInt() + a.toInt();
}

void tst_SubtotalProxyModel::doubleSum(QVariant &r, const QVariant &a)
{
    r = r.toDouble() + a.toDouble();
}
