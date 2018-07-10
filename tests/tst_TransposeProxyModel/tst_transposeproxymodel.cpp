#include "tst_transposeproxymodel.h"
#include <transposeproxymodel.h>
#include <QtTest/QTest>
#include <QStringListModel>
#include <random>
#ifdef QT_GUI_LIB
#include <QStandardItemModel>
#endif
#ifndef MOC_MODEL_TEST
class ModelTest : public QObject
{
    Q_DISABLE_COPY(ModelTest)
public:
    ModelTest(QAbstractItemModel* model, QObject* parent = Q_NULLPTR)
        :QObject(parent)
    {
        Q_UNUSED(model)
    }
};
#else
#include "modeltest.h"
#endif

QAbstractItemModel* createNullModel(QObject* parent)
{
    Q_UNUSED(parent)
        return nullptr;
}
QAbstractItemModel* createListModel(QObject* parent)
{
    QStringList sequence;
    sequence.reserve(10);
    for (int i = 0; i < 10; ++i)
        sequence.append(QString::number(i));
    return new QStringListModel(sequence, parent);
}

QAbstractItemModel* createTableModel(QObject* parent)
{
    QAbstractItemModel* model = nullptr;
#ifdef QT_GUI_LIB
    model = new QStandardItemModel(parent);
    model->insertRows(0, 5);
    model->insertColumns(0, 4);
    for (int i = 0; i < model->rowCount(); ++i) {
        for (int j = 0; j < model->columnCount(); ++j) {
            model->setData(model->index(i, j), QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            model->setData(model->index(i, j), i, Qt::UserRole);
            model->setData(model->index(i, j), j, Qt::UserRole + 1);
        }
    }
#endif
    return model;
}

QAbstractItemModel* createTreeModel(QObject* parent)
{
    QAbstractItemModel* model = nullptr;
#ifdef QT_GUI_LIB
    model = new QStandardItemModel(parent);
    model->insertRows(0, 5);
    model->insertColumns(0, 4);
    for (int i = 0; i < model->rowCount(); ++i) {
        for (int j = 0; j < model->columnCount(); ++j) {
            const QModelIndex parIdx = model->index(i, j);
            model->setData(parIdx, QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            model->setData(parIdx, i, Qt::UserRole);
            model->setData(parIdx, j, Qt::UserRole + 1);
            model->insertRows(0, 3, parIdx);
            model->insertColumns(0, 2, parIdx);
            for (int h = 0; h < model->rowCount(parIdx); ++h) {
                for (int k = 0; k < model->columnCount(parIdx); ++k) {
                    const QModelIndex childIdx = model->index(h, k, parIdx);
                    model->setData(childIdx, QStringLiteral("%1,%2,%3,%4").arg(i).arg(j).arg(h).arg(k), Qt::EditRole);
                    model->setData(childIdx, i, Qt::UserRole);
                    model->setData(childIdx, j, Qt::UserRole + 1);
                    model->setData(childIdx, h, Qt::UserRole + 2);
                    model->setData(childIdx, k, Qt::UserRole + 3);
                }
            }
        }
    }
#endif
    return model;
}

void tst_TransposeProxyModel::testTransposed(const QAbstractItemModel* const baseModel, const QAbstractItemModel* const transposed, const QModelIndex& baseParent, const QModelIndex& transposedParent)
{
    QCOMPARE(transposed->hasChildren(transposedParent), baseModel->hasChildren(baseParent));
    QCOMPARE(transposed->columnCount(transposedParent), baseModel->rowCount(baseParent));
    QCOMPARE(transposed->rowCount(transposedParent), baseModel->columnCount(baseParent));
    for (int i = 0, maxRow = baseModel->rowCount(baseParent); i < maxRow; ++i) {
        for (int j = 0, maxCol = baseModel->columnCount(baseParent); j < maxCol; ++j) {
            const QModelIndex baseIdx = baseModel->index(i, j, baseParent);
            const QModelIndex transIdx = transposed->index(j, i, transposedParent);
            QCOMPARE(transIdx.data(), baseIdx.data());
            QCOMPARE(transIdx.data(Qt::UserRole), baseIdx.data(Qt::UserRole));
            QCOMPARE(transIdx.data(Qt::UserRole + 1), baseIdx.data(Qt::UserRole + 1));
            QCOMPARE(transIdx.data(Qt::UserRole + 2), baseIdx.data(Qt::UserRole + 2));
            QCOMPARE(transIdx.data(Qt::UserRole + 3), baseIdx.data(Qt::UserRole + 3));
            if (baseModel->hasChildren(baseIdx)) {
                testTransposed(baseModel, transposed, baseIdx, transIdx);
            }
        }
    }
}


void tst_TransposeProxyModel::basicTest()
{
    QFETCH(QAbstractItemModel*, model);
    if (!model)
        return;
    TransposeProxyModel proxy;
    new ModelTest(&proxy, &proxy);
    proxy.setSourceModel(model);
    testTransposed(model, &proxy);
    model->deleteLater();
}

void tst_TransposeProxyModel::basicTest_data()
{
    QTest::addColumn<QAbstractItemModel*>("model");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Table") << createTableModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_TransposeProxyModel::testSort()
{
    QStringList sequence;
    sequence.reserve(100);
    for (int i = 0; i < 100; ++i)
        sequence.append(QStringLiteral("%1").arg(i, 3, 10, QLatin1Char('0')));
    std::shuffle(sequence.begin(), sequence.end(), std::mt19937(88));
    QStringListModel baseModel(sequence);
    TransposeProxyModel proxyModel;
    new ModelTest(&proxyModel, &proxyModel);
    proxyModel.setSourceModel(&baseModel);
    baseModel.sort(0, Qt::AscendingOrder);
    testTransposed(&baseModel, &proxyModel);
}
void tst_TransposeProxyModel::testInsertRowBase_data()
{
    QTest::addColumn<QAbstractItemModel*>("model");
    QTest::addColumn<QModelIndex>("parent");
    QTest::newRow("List") << createListModel(this) << QModelIndex();
    QTest::newRow("Table") << createTableModel(this) << QModelIndex();
    QTest::newRow("Tree Root Item") << createTreeModel(this) << QModelIndex();
    QAbstractItemModel* model = createTreeModel(this);
    if (model)
        QTest::newRow("Tree Child Item") << model << model->index(0, 0);
}

void tst_TransposeProxyModel::testInsertColumnBase_data()
{
    QTest::addColumn<QAbstractItemModel*>("model");
    QTest::addColumn<QModelIndex>("parent");
    QTest::newRow("Table") << createTableModel(this) << QModelIndex();
    QTest::newRow("Tree Root Item") << createTreeModel(this) << QModelIndex();
    QAbstractItemModel* model = createTreeModel(this);
    if (model)
        QTest::newRow("Tree Child Item") << model << model->index(0, 0);
}

void tst_TransposeProxyModel::testInsertColumnBase()
{
#if defined(QT_GUI_LIB)
    QFETCH(QAbstractItemModel*, model);
    if (!model)
        return;
    QFETCH(QModelIndex, parent);
    TransposeProxyModel proxy;
    new ModelTest(&proxy, &proxy);
    proxy.setSourceModel(model);
    QVERIFY(model->insertColumn(1, parent));
    testTransposed(model, &proxy);
    model->deleteLater();
#else
    QSKIP("This test requires the Qt GUI module");
#endif
}

void tst_TransposeProxyModel::testInsertRowBase()
{
    QFETCH(QAbstractItemModel*, model);
    if (!model)
        return;
    QFETCH(QModelIndex, parent);
    TransposeProxyModel proxy;
    new ModelTest(&proxy, &proxy);
    proxy.setSourceModel(model);
    QVERIFY(model->insertRow(1, parent));
    testTransposed(model, &proxy);
    model->deleteLater();
}
void tst_TransposeProxyModel::testInsertColumnProxy_data()
{
    QTest::addColumn<QAbstractItemModel*>("model");
    QTest::addColumn<bool>("rootItem");
    QTest::newRow("List") << createListModel(this) << true;
    QTest::newRow("Table") << createTableModel(this) << true;
    QTest::newRow("Tree Root Item") << createTreeModel(this) << true;
    QTest::newRow("Tree Child Item") << createTreeModel(this) << false;
}

void tst_TransposeProxyModel::testInsertColumnProxy()
{
    QFETCH(QAbstractItemModel*, model);
    if (!model)
        return;
    QFETCH(bool, rootItem);
    TransposeProxyModel proxy;
    new ModelTest(&proxy, &proxy);
    proxy.setSourceModel(model);
    if (rootItem)
        QVERIFY(proxy.insertColumn(1));
    else
        QVERIFY(proxy.insertColumn(1, proxy.index(1, 0)));
    testTransposed(model, &proxy);
    model->deleteLater();
}
void tst_TransposeProxyModel::testInsertRowProxy_data()
{
    QTest::addColumn<QAbstractItemModel*>("model");
    QTest::addColumn<bool>("rootItem");
    QTest::newRow("Table") << createTableModel(this) << true;
    QTest::newRow("Tree Root Item") << createTreeModel(this) << true;
    QTest::newRow("Tree Child Item") << createTreeModel(this) << false;
}



void tst_TransposeProxyModel::testInsertRowProxy()
{
#if defined(QT_GUI_LIB)
    QFETCH(QAbstractItemModel*, model);
    if (!model)
        return;
    QFETCH(bool, rootItem);
    TransposeProxyModel proxy;
    new ModelTest(&proxy, &proxy);
    proxy.setSourceModel(model);
    if (rootItem)
        QVERIFY(proxy.insertRow(1));
    else
        QVERIFY(proxy.insertRow(1, proxy.index(1, 0)));
    testTransposed(model, &proxy);
    model->deleteLater();
#else
    QSKIP("This test requires the Qt GUI module");
#endif
}

void tst_TransposeProxyModel::testSetData_data()
{
    QTest::addColumn<QAbstractItemModel*>("model");
    QTest::addColumn<bool>("rootItem");
    QTest::addColumn<bool>("viaProxy");
    QTest::newRow("List via Base") << createListModel(this) << true << false;
    QTest::newRow("Table via Base") << createTableModel(this) << true << false;
    QTest::newRow("Tree via Base Root Item") << createTreeModel(this) << true << false;
    QTest::newRow("Tree via Base Child Item") << createTreeModel(this) << false << false;
    QTest::newRow("List via Proxy") << createListModel(this) << true << true;
    QTest::newRow("Table via Proxy") << createTableModel(this) << true << true;
    QTest::newRow("Tree via Proxy Root Item") << createTreeModel(this) << true << true;
    QTest::newRow("Tree via Proxy Child Item") << createTreeModel(this) << false << true;
}

void tst_TransposeProxyModel::testSetData()
{
    QFETCH(QAbstractItemModel*, model);
    if (!model)
        return;
    QFETCH(bool, rootItem);
    QFETCH(bool, viaProxy);
    TransposeProxyModel proxy;
    new ModelTest(&proxy, &proxy);
    proxy.setSourceModel(model);
    const QString testData = QStringLiteral("TestingSetData");
    if (viaProxy){
        if (rootItem) {
            QVERIFY(proxy.setData(proxy.index(0, 1), testData));
            QCOMPARE(model->index(1, 0).data().toString(), testData);
        }
        else {
            QVERIFY(proxy.setData(proxy.index(0, 1, proxy.index(0, 1)), testData));
            QCOMPARE(model->index(1, 0, model->index(1, 0)).data().toString(), testData);
        }
    }
    else{
        if (rootItem) {
            QVERIFY(model->setData(model->index(1, 0), testData));
            QCOMPARE(proxy.index(0, 1).data().toString(), testData);
        }
        else {
            QVERIFY(model->setData(model->index(1, 0, model->index(1, 0)), testData));
            QCOMPARE(proxy.index(0, 1, proxy.index(0, 1)).data().toString(), testData);
        }
    }
    testTransposed(model, &proxy);
    model->deleteLater();
}
