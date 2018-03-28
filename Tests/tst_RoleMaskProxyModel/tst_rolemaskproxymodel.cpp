#include <QtTest/QTest>
#include <QStringListModel>
#include <rolemaskproxymodel.h>
#include <QStandardItemModel>
#include <QSignalSpy>
#include <QList>
#include <QVariant>
#include "tst_rolemaskproxymodel.h"
void tst_RoleMaskProxyModel::initTestCase()
{
    m_models.append(new QStringListModel(QStringList() << QStringLiteral("1") << QStringLiteral("2") << QStringLiteral("3") << QStringLiteral("4") << QStringLiteral("5"),this));

    m_models.append(new QStandardItemModel(this));
    m_models.last()->insertRows(0, 5);
    m_models.last()->insertColumns(0, 3);
    for (int i = 0; i < m_models.last()->rowCount(); ++i) {
        for (int j = 0; j < m_models.last()->columnCount(); ++j) {
            m_models.last()->setData(m_models.last()->index(i, j), QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            m_models.last()->setData(m_models.last()->index(i, j), i, Qt::UserRole);
            m_models.last()->setData(m_models.last()->index(i, j), j, Qt::UserRole+1);
        }
    }

    m_models.append(new QStandardItemModel(this));
    m_models.last()->insertRows(0, 5);
    m_models.last()->insertColumns(0, 3);
    for (int i = 0; i < m_models.last()->rowCount(); ++i) {
        for (int j = 0; j < m_models.last()->columnCount(); ++j) {
            m_models.last()->setData(m_models.last()->index(i, j), QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            m_models.last()->setData(m_models.last()->index(i, j), i, Qt::UserRole);
            m_models.last()->setData(m_models.last()->index(i, j), j, Qt::UserRole + 1);
        }
        const QModelIndex parIdx = m_models.last()->index(i, 0);
        m_models.last()->insertRows(0, 5, parIdx);
        m_models.last()->insertColumns(0, 3, parIdx);
        for (int k = 0; k < m_models.last()->rowCount(parIdx); ++k) {
            for (int h = 0; h < m_models.last()->columnCount(parIdx); ++h) {
                m_models.last()->setData(m_models.last()->index(k, h, parIdx), QStringLiteral("%1,%2,%3").arg(i).arg(k).arg(h), Qt::EditRole);
                m_models.last()->setData(m_models.last()->index(k, h, parIdx), h, Qt::UserRole);
                m_models.last()->setData(m_models.last()->index(k, h, parIdx), k, Qt::UserRole + 1);
            }
        }
    }
}

void tst_RoleMaskProxyModel::cleanupTestCase()
{
    while (!m_models.isEmpty())
        m_models.takeLast()->deleteLater();
}

void tst_RoleMaskProxyModel::testUseRoleMask()
{
    QFETCH(QAbstractItemModel*, baseModel);
    QFETCH(const QModelIndexList, magicNumerIndexes);
    QFETCH(const bool, userRoleEditable);
    RoleMaskProxyModel proxyModel;
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(baseModel);
    const int magicNumber = 785874;
    Q_FOREACH(const QModelIndex& singleIdx, magicNumerIndexes){
        QVERIFY(proxyModel.setData(proxyModel.mapFromSource(singleIdx), magicNumber, Qt::UserRole));
        QVERIFY(userRoleEditable == baseModel->setData(singleIdx, ~magicNumber, Qt::UserRole));
    }
    testUseRoleMaskRecurse(magicNumber, baseModel, &proxyModel, magicNumerIndexes, userRoleEditable);
}

void tst_RoleMaskProxyModel::testUseRoleMask_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<QModelIndexList>("magicNumerIndexes");
    QTest::addColumn<bool>("userRoleEditable");
    QTest::newRow("QStringListModel") << m_models.at(0) << QModelIndexList({ m_models.at(0)->index(0, 0) }) << false;
    QTest::newRow("QStandadItemModel Table") << m_models.at(1) << QModelIndexList({ m_models.at(1)->index(1, 0), m_models.at(1)->index(0, 1) }) << true;
    QTest::newRow("QStandadItemModel Tree") << m_models.at(2) << QModelIndexList({ m_models.at(2)->index(1, 0), m_models.at(2)->index(0, 1), m_models.at(2)->index(0, 1, m_models.at(2)->index(0, 0)) }) << true;
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
    QTest::newRow("List Insert Begin") << m_models.at(0) << 0 << QModelIndex();
    QTest::newRow("Table Insert Begin") << m_models.at(1) << 0 << QModelIndex();
    QTest::newRow("Tree Insert Begin") << m_models.at(2) << 0 << QModelIndex();

    QTest::newRow("List Insert End") << m_models.at(0) << m_models.at(0)->rowCount() << QModelIndex();
    QTest::newRow("Table Insert End") << m_models.at(1) << m_models.at(1)->rowCount() << QModelIndex();
    QTest::newRow("Tree Insert End") << m_models.at(2) << m_models.at(2)->rowCount() << QModelIndex();

    QTest::newRow("List Insert Middle") << m_models.at(0) << 2 << QModelIndex();
    QTest::newRow("Table Insert Middle") << m_models.at(1) << 2 << QModelIndex();
    QTest::newRow("Tree Insert Middle") << m_models.at(2) << 2 << QModelIndex();

    QTest::newRow("Tree Insert Begin Child") << m_models.at(2) << 0 << m_models.at(2)->index(0, 0);
    QTest::newRow("Tree Insert Middle Child") << m_models.at(2) << 2 << m_models.at(2)->index(0, 0);
    QTest::newRow("Tree Insert End Child") << m_models.at(2) << m_models.at(2)->rowCount(m_models.at(2)->index(0, 0)) << m_models.at(2)->index(0, 0);

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
    QTest::newRow("Table Insert Begin") << m_models.at(1) << 0 << QModelIndex();
    QTest::newRow("Tree Insert Begin") << m_models.at(2) << 0 << QModelIndex();

    QTest::newRow("Table Insert End") << m_models.at(1) << m_models.at(1)->columnCount() << QModelIndex();
    QTest::newRow("Tree Insert End") << m_models.at(2) << m_models.at(2)->columnCount() << QModelIndex();

    QTest::newRow("Table Insert Middle") << m_models.at(1) << 2 << QModelIndex();
    QTest::newRow("Tree Insert Middle") << m_models.at(2) << 2 << QModelIndex();

    QTest::newRow("Tree Insert Begin Child") << m_models.at(2) << 0 << m_models.at(2)->index(0, 0);
    QTest::newRow("Tree Insert Middle Child") << m_models.at(2) << 2 << m_models.at(2)->index(0, 0);
    QTest::newRow("Tree Insert End Child") << m_models.at(2) << m_models.at(2)->columnCount(m_models.at(2)->index(0, 0)) << m_models.at(2)->index(0, 0);
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

void tst_RoleMaskProxyModel::testDataChangeSignals_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<bool>("implementsRoles");

    QTest::newRow("List") << m_models.at(0) << bool(QT_VERSION >= QT_VERSION_CHECK(5, 6, 2));
    QTest::newRow("Table") << m_models.at(1) << bool(QT_VERSION >= QT_VERSION_CHECK(5, 11, 0));
    QTest::newRow("Tree") << m_models.at(2) << bool(QT_VERSION >= QT_VERSION_CHECK(5, 11, 0));
}

void tst_RoleMaskProxyModel::testDataChangeSignals()
{
    QFETCH(QAbstractItemModel* const, baseModel);
    QFETCH(const bool, implementsRoles);
    RoleMaskProxyModel proxyModel;
    proxyModel.setTransparentIfEmpty(true);
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy baseDataChangeSpy(baseModel, &QAbstractItemModel::dataChanged);
    QSignalSpy proxyDataChangeSpy(&proxyModel, &QAbstractItemModel::dataChanged);
    int timesFired = 0;
    connect(&proxyModel, &QAbstractItemModel::dataChanged, [&timesFired](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)->void {
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
    QVector<int> rolesVector = arguments.at(2).value<QVector<int> >();
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
    arguments = proxyDataChangeSpy.takeFirst();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), proxyDataIdx);
    QCOMPARE(arguments.at(1).value<QModelIndex>(), proxyDataIdx);
    rolesVector = arguments.at(2).value<QVector<int> >();
    QCOMPARE(rolesVector.size(), 2);
    QVERIFY(rolesVector.contains(Qt::DisplayRole));
    QVERIFY(rolesVector.contains(Qt::EditRole));
    if (baseModel->setData(baseDataIdx, 6, Qt::EditRole)){
        QCOMPARE(baseDataChangeSpy.count(), 1);
        baseDataChangeSpy.clear();
        QCOMPARE(proxyDataChangeSpy.count(), implementsRoles ? 0:1);
        proxyDataChangeSpy.clear();
    }
    proxyModel.setTransparentIfEmpty(false);
    QCOMPARE(baseDataChangeSpy.count(), 0);
    QCOMPARE(proxyDataChangeSpy.count(), 1 + countChildren(baseModel));
    proxyDataChangeSpy.clear();

}

int tst_RoleMaskProxyModel::countChildren(const QAbstractItemModel* const baseModel, const QModelIndex& parIdx)
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

void tst_RoleMaskProxyModel::testUseRoleMaskRecurse(const int magicNumber, const QAbstractItemModel* const baseModel, const RoleMaskProxyModel* const proxyModel, const QModelIndexList& magicNumerIndexes, const bool userRoleEditable, const QModelIndex& srcParent, const QModelIndex& prxParent)
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
            }
            else {
                QCOMPARE(proxyParent.data(Qt::UserRole), baseParent.data(Qt::UserRole));
            }
            QCOMPARE(proxyParent.data(), baseParent.data());
            QCOMPARE(proxyModel->hasChildren(proxyParent), baseModel->hasChildren(baseParent));
            if (baseModel->hasChildren(baseParent))
                testUseRoleMaskRecurse(magicNumber, baseModel, proxyModel, magicNumerIndexes, userRoleEditable, baseParent, proxyParent);
        }
    }
}

