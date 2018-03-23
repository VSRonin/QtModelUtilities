#include <QtTest/QTest>
#include <QStringListModel>
#include <rolemaskproxymodel.h>
#include <QStandardItemModel>
#include "tst_rolemaskproxymodel.h"
void tst_RoleMaskProxyModel::initTestCase()
{
    m_models[0] = new QStringListModel(QStringList() << QStringLiteral("1") << QStringLiteral("2") << QStringLiteral("3") << QStringLiteral("4") << QStringLiteral("5"),this);

    m_models[1] = new QStandardItemModel(this);
    m_models[1]->insertRows(0, 5);
    m_models[1]->insertColumns(0, 3);
    for (int i = 0; i < m_models[1]->rowCount(); ++i) {
        for (int j = 0; j < m_models[1]->columnCount(); ++j) {
            m_models[1]->setData(m_models[1]->index(i, j), QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            m_models[1]->setData(m_models[1]->index(i, j), i, Qt::UserRole);
            m_models[1]->setData(m_models[1]->index(i, j), j, Qt::UserRole+1);
        }
    }

    m_models[2] = new QStandardItemModel(this);
    m_models[2]->insertRows(0, 5);
    m_models[2]->insertColumns(0, 3);
    for (int i = 0; i < m_models[2]->rowCount(); ++i) {
        for (int j = 0; j < m_models[2]->columnCount(); ++j) {
            m_models[2]->setData(m_models[2]->index(i, j), QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            m_models[2]->setData(m_models[2]->index(i, j), i, Qt::UserRole);
            m_models[2]->setData(m_models[2]->index(i, j), j, Qt::UserRole + 1);
        }
        const QModelIndex parIdx = m_models[2]->index(i, 0);
        m_models[2]->insertRows(0, 5, parIdx);
        m_models[2]->insertColumns(0, 3, parIdx);
        for (int k = 0; k < m_models[2]->rowCount(parIdx); ++k) {
            for (int h = 0; h < m_models[2]->columnCount(parIdx); ++h) {
                m_models[2]->setData(m_models[2]->index(k, h, parIdx), QStringLiteral("%1,%2,%3").arg(i).arg(k).arg(h), Qt::EditRole);
                m_models[2]->setData(m_models[2]->index(k, h, parIdx), h, Qt::UserRole);
                m_models[2]->setData(m_models[2]->index(k, h, parIdx), k, Qt::UserRole + 1);
            }
        }
    }
}

void tst_RoleMaskProxyModel::cleanupTestCase()
{
    for (int i = 0; i < m_models.size(); ++i) {
        if (!m_models[i]) continue;
        m_models[i]->deleteLater();
        m_models[i] = nullptr;
    }
}

void tst_RoleMaskProxyModel::testUseRoleMask()
{
    QFETCH(QAbstractItemModel*, baseModel);
    QFETCH(const QModelIndexList, magicNumerIndexes);
    RoleMaskProxyModel proxyModel;
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(baseModel);
    const int magicNumber = 785874;
    Q_FOREACH(const QModelIndex& singleIdx, magicNumerIndexes){
        QVERIFY(proxyModel.setData(proxyModel.mapFromSource(singleIdx), magicNumber, Qt::UserRole));
    }
    testUseRoleMaskRecurse(magicNumber, baseModel, &proxyModel, magicNumerIndexes);
}

void tst_RoleMaskProxyModel::testUseRoleMask_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<QModelIndexList>("magicNumerIndexes");
    QTest::newRow("QStringListModel") << m_models[0] << QModelIndexList({ m_models[0]->index(0, 0) });
    QTest::newRow("QStandadItemModel Table") << m_models[1] << QModelIndexList({ m_models[1]->index(1, 0), m_models[1]->index(0, 1) });
    QTest::newRow("QStandadItemModel Tree") << m_models[2] << QModelIndexList({ m_models[2]->index(1, 0), m_models[2]->index(0, 1), m_models[2]->index(0, 1, m_models[2]->index(0, 0)) });
}

void tst_RoleMaskProxyModel::testInsertRow()
{
    QFETCH(QAbstractItemModel* const, baseModel);
    QFETCH(const int, insertIndex);
    QFETCH(const QModelIndex, parentIndex);
    RoleMaskProxyModel proxyModel;
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(baseModel);
    const int magicNumber = 785874;
    const QModelIndex proxyParent = proxyModel.mapFromSource(parentIndex);
    for (int i = 0; i < baseModel->rowCount(parentIndex); ++i) {
        QVERIFY(proxyModel.setData(proxyModel.index(i, 0, proxyParent), magicNumber + i, Qt::UserRole));
    }
    QVERIFY(baseModel->insertRow(insertIndex, parentIndex));
    for (int i = 0; i < baseModel->rowCount(parentIndex); ++i) {
        if (i == insertIndex)
            QVERIFY(!proxyModel.index(i, 0, proxyParent).data(Qt::UserRole).isValid());
        else if (i>insertIndex)
            QCOMPARE(proxyModel.index(i, 0, proxyParent).data(Qt::UserRole).toInt(), magicNumber + i-1);
        else
            QCOMPARE(proxyModel.index(i, 0, proxyParent).data(Qt::UserRole).toInt(), magicNumber + i);
    }
    QVERIFY(baseModel->removeRow(insertIndex, parentIndex));
}

void tst_RoleMaskProxyModel::testInsertRow_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<int>("insertIndex");
    QTest::addColumn<QModelIndex>("parentIndex");
    QTest::newRow("List Insert Begin") << m_models[0] << 0 << QModelIndex();
    QTest::newRow("Table Insert Begin") << m_models[1] << 0 << QModelIndex();
    QTest::newRow("Tree Insert Begin") << m_models[2] << 0 << QModelIndex();

    QTest::newRow("List Insert End") << m_models[0] << m_models[0]->rowCount() << QModelIndex();
    QTest::newRow("Table Insert End") << m_models[1] << m_models[1]->rowCount() << QModelIndex();
    QTest::newRow("Tree Insert End") << m_models[2] << m_models[2]->rowCount() << QModelIndex();

    QTest::newRow("List Insert Middle") << m_models[0] << 2 << QModelIndex();
    QTest::newRow("Table Insert Middle") << m_models[1] << 2 << QModelIndex();
    QTest::newRow("Tree Insert Middle") << m_models[2] << 2 << QModelIndex();

    QTest::newRow("Tree Insert Begin Child") << m_models[2] << 0 << m_models[2]->index(0, 0);
    QTest::newRow("Tree Insert Middle Child") << m_models[2] << 2 << m_models[2]->index(0, 0);
    QTest::newRow("Tree Insert End Child") << m_models[2] << m_models[2]->rowCount(m_models[2]->index(0, 0)) << m_models[2]->index(0, 0);

}

void tst_RoleMaskProxyModel::testInsertColumn()
{
    QFETCH(QAbstractItemModel* const, baseModel);
    QFETCH(const int, insertIndex);
    QFETCH(const QModelIndex, parentIndex);
    RoleMaskProxyModel proxyModel;
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

}

void tst_RoleMaskProxyModel::testInsertColumn_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<int>("insertIndex");
    QTest::addColumn<QModelIndex>("parentIndex");
    QTest::newRow("Table Insert Begin") << m_models[1] << 0 << QModelIndex();
    QTest::newRow("Tree Insert Begin") << m_models[2] << 0 << QModelIndex();

    QTest::newRow("Table Insert End") << m_models[1] << m_models[1]->columnCount() << QModelIndex();
    QTest::newRow("Tree Insert End") << m_models[2] << m_models[2]->columnCount() << QModelIndex();

    QTest::newRow("Table Insert Middle") << m_models[1] << 2 << QModelIndex();
    QTest::newRow("Tree Insert Middle") << m_models[2] << 2 << QModelIndex();

    QTest::newRow("Tree Insert Begin Child") << m_models[2] << 0 << m_models[2]->index(0, 0);
    QTest::newRow("Tree Insert Middle Child") << m_models[2] << 2 << m_models[2]->index(0, 0);
    QTest::newRow("Tree Insert End Child") << m_models[2] << m_models[2]->columnCount(m_models[2]->index(0, 0)) << m_models[2]->index(0, 0);
}

void tst_RoleMaskProxyModel::testNullModel()
{
    RoleMaskProxyModel proxyModel;
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

void tst_RoleMaskProxyModel::testUseRoleMaskRecurse(const int magicNumber, const QAbstractItemModel* const baseModel, const RoleMaskProxyModel* const proxyModel, const QModelIndexList& magicNumerIndexes, const QModelIndex& srcParent, const QModelIndex& prxParent)
{
    const int rowCnt = baseModel->rowCount(srcParent);
    const int colCnt = baseModel->columnCount(srcParent);
    for (int i = 0; i < rowCnt; ++i) {
        for (int j = 0; j < colCnt; ++j) {
            const QModelIndex baseParent = baseModel->index(i, j, srcParent);
            const QModelIndex proxyParent = proxyModel->index(i, j, prxParent);
            if (magicNumerIndexes.contains(baseParent)) {
                QCOMPARE(proxyParent.data(Qt::UserRole).toInt(), magicNumber);
                QVERIFY(baseParent.data(Qt::UserRole).toInt() != magicNumber);
            }
            else {
                QCOMPARE(proxyParent.data(Qt::UserRole), baseParent.data(Qt::UserRole));
            }
            QCOMPARE(proxyParent.data(), baseParent.data());
            QCOMPARE(proxyModel->hasChildren(proxyParent), baseModel->hasChildren(baseParent));
            if (baseModel->hasChildren(baseParent))
                testUseRoleMaskRecurse(magicNumber, baseModel, proxyModel, magicNumerIndexes, baseParent, proxyParent);
        }
    }
}

