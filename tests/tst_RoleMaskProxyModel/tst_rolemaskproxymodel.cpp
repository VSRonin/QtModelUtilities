#include <QtTest/QTest>
#include <QStringListModel>
#include <rolemaskproxymodel.h>
#ifdef QT_GUI_LIB
#include <QStandardItemModel>
#endif
#include <QtTest/QSignalSpy>
#include <QList>
#include <QVariant>
#include "tst_rolemaskproxymodel.h"

QAbstractItemModel* createNullModel(QObject* parent)
{
    Q_UNUSED(parent)
        return nullptr;
}
QAbstractItemModel* createListModel(QObject* parent)
{
    return new QStringListModel(QStringList() << QStringLiteral("1") << QStringLiteral("2") << QStringLiteral("3") << QStringLiteral("4") << QStringLiteral("5"), parent);
}

QAbstractItemModel* createTableModel(QObject* parent)
{
    QAbstractItemModel* result = nullptr;
#ifdef QT_GUI_LIB
    result = new QStandardItemModel(parent);
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

QAbstractItemModel* createTreeModel(QObject* parent)
{
    QAbstractItemModel* result = nullptr;
#ifdef QT_GUI_LIB
    result = new QStandardItemModel(parent);
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

void tst_RoleMaskProxyModel::testUseRoleMask()
{
    QFETCH(QAbstractItemModel*, baseModel);
    QFETCH(QModelIndexList, magicNumerIndexes);
    QFETCH(bool, userRoleEditable);
    RoleMaskProxyModel proxyModel;
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(baseModel);
    const int magicNumber = 785874;
    for(const QModelIndex& singleIdx : magicNumerIndexes){
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
    QTest::newRow("QStringListModel") << createListModel(this) << QModelIndexList({ createListModel(this)->index(0, 0) }) << false;
#ifdef QT_GUI_LIB
    QTest::newRow("QStandadItemModel Table") << createTableModel(this) << QModelIndexList({ createTableModel(this)->index(1, 0), createTableModel(this)->index(0, 1) }) << true;
    QTest::newRow("QStandadItemModel Tree") << createTreeModel(this) << QModelIndexList({ createTreeModel(this)->index(1, 0), createTreeModel(this)->index(0, 1), createTreeModel(this)->index(0, 1, createTreeModel(this)->index(0, 0)) }) << true;
#endif
}

void tst_RoleMaskProxyModel::testInsertRow()
{
    QFETCH(QAbstractItemModel*, baseModel);
    QFETCH(int, insertIndex);
    QFETCH(QModelIndex, parentIndex);
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
    QTest::newRow("List Insert Begin") << createListModel(this) << 0 << QModelIndex();
#ifdef QT_GUI_LIB
    QTest::newRow("Table Insert Begin") << createTableModel(this) << 0 << QModelIndex();
    QTest::newRow("Tree Insert Begin") << createTreeModel(this) << 0 << QModelIndex();
#endif
    QTest::newRow("List Insert End") << createListModel(this) << createListModel(this)->rowCount() << QModelIndex();
#ifdef QT_GUI_LIB
    QTest::newRow("Table Insert End") << createTableModel(this) << createTableModel(this)->rowCount() << QModelIndex();
    QTest::newRow("Tree Insert End") << createTreeModel(this) << createTreeModel(this)->rowCount() << QModelIndex();
#endif
    QTest::newRow("List Insert Middle") << createListModel(this) << 2 << QModelIndex();
#ifdef QT_GUI_LIB
    QTest::newRow("Table Insert Middle") << createTableModel(this) << 2 << QModelIndex();
    QTest::newRow("Tree Insert Middle") << createTreeModel(this) << 2 << QModelIndex();

    QTest::newRow("Tree Insert Begin Child") << createTreeModel(this) << 0 << createTreeModel(this)->index(0, 0);
    QTest::newRow("Tree Insert Middle Child") << createTreeModel(this) << 2 << createTreeModel(this)->index(0, 0);
    QTest::newRow("Tree Insert End Child") << createTreeModel(this) << createTreeModel(this)->rowCount(createTreeModel(this)->index(0, 0)) << createTreeModel(this)->index(0, 0);
#endif
}
void tst_RoleMaskProxyModel::testProperties()
{
    RoleMaskProxyModel proxyModel;
    QVERIFY(proxyModel.setProperty("maskedRoles", QVariant::fromValue(QList<int>({Qt::UserRole, Qt::DisplayRole}))));
    const auto roleList = proxyModel.property("maskedRoles").value<QList<int> >();
    QVERIFY(roleList.contains(Qt::UserRole));
    QVERIFY(roleList.contains(Qt::DisplayRole));
    QVERIFY(proxyModel.setProperty("transparentIfEmpty", false));
    QCOMPARE(proxyModel.property("transparentIfEmpty").toBool(), false);
    QVERIFY(proxyModel.setProperty("mergeDisplayEdit", false));
    QCOMPARE(proxyModel.property("mergeDisplayEdit").toBool(), false);
}
void tst_RoleMaskProxyModel::testInsertColumn()
{
#ifndef QT_GUI_LIB
    QSKIP("This test requires the Qt GUI module");
#else
    QFETCH(QAbstractItemModel*, baseModel);
    QFETCH(int, insertIndex);
    QFETCH(QModelIndex, parentIndex);
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
#endif
}

void tst_RoleMaskProxyModel::testInsertColumn_data()
{
#ifdef QT_GUI_LIB
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<int>("insertIndex");
    QTest::addColumn<QModelIndex>("parentIndex");
    QTest::newRow("Table Insert Begin") << createTableModel(this) << 0 << QModelIndex();
    QTest::newRow("Tree Insert Begin") << createTreeModel(this) << 0 << QModelIndex();

    QTest::newRow("Table Insert End") << createTableModel(this) << createTableModel(this)->columnCount() << QModelIndex();
    QTest::newRow("Tree Insert End") << createTreeModel(this) << createTreeModel(this)->columnCount() << QModelIndex();

    QTest::newRow("Table Insert Middle") << createTableModel(this) << 2 << QModelIndex();
    QTest::newRow("Tree Insert Middle") << createTreeModel(this) << 2 << QModelIndex();

    QTest::newRow("Tree Insert Begin Child") << createTreeModel(this) << 0 << createTreeModel(this)->index(0, 0);
    QTest::newRow("Tree Insert Middle Child") << createTreeModel(this) << 2 << createTreeModel(this)->index(0, 0);
    QTest::newRow("Tree Insert End Child") << createTreeModel(this) << createTreeModel(this)->columnCount(createTreeModel(this)->index(0, 0)) << createTreeModel(this)->index(0, 0);
#endif
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

    QTest::newRow("List") << createListModel(this) << bool(QT_VERSION >= QT_VERSION_CHECK(5, 0, 2));
#ifdef QT_GUI_LIB
    QTest::newRow("Table") << createTableModel(this) << bool(QT_VERSION >= QT_VERSION_CHECK(5, 11, 0));
    QTest::newRow("Tree") << createTreeModel(this) << bool(QT_VERSION >= QT_VERSION_CHECK(5, 11, 0));
#endif
}

void tst_RoleMaskProxyModel::testDataChangeSignals()
{
    QFETCH(QAbstractItemModel*, baseModel);
    QFETCH(bool, implementsRoles);
    RoleMaskProxyModel proxyModel;
    proxyModel.setTransparentIfEmpty(true);
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.addMaskedRole(Qt::UserRole);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy baseDataChangeSpy(baseModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    QVERIFY(baseDataChangeSpy.isValid());
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    QVERIFY(baseDataChangeSpy.isValid());
    int timesFired = 0;
    connect(&proxyModel, &QAbstractItemModel::dataChanged, [&timesFired](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)->void {
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
    QList<QVariant> arguments = proxyDataChangeSpy.value(0);
    proxyDataChangeSpy.clear();
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
    arguments = proxyDataChangeSpy.value(0);
    proxyDataChangeSpy.clear();
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
    QVERIFY(proxyModel.setData(proxyDataIdx, QVariant(), Qt::EditRole));
    QCOMPARE(baseDataChangeSpy.count(), 0);
    QCOMPARE(proxyDataChangeSpy.count(), 1);
    arguments = proxyDataChangeSpy.value(0);
    proxyDataChangeSpy.clear();
    QCOMPARE(arguments.at(0).value<QModelIndex>(), proxyDataIdx);
    QCOMPARE(arguments.at(1).value<QModelIndex>(), proxyDataIdx);
    rolesVector = arguments.at(2).value<QVector<int> >();
    QCOMPARE(rolesVector.size(), 2);
    QVERIFY(rolesVector.contains(Qt::DisplayRole));
    QVERIFY(rolesVector.contains(Qt::EditRole));
    if (baseModel->setData(baseDataIdx, 86, Qt::EditRole)){
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
    rolesVector = arguments.at(2).value<QVector<int> >();
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
        if (proxyDataChangeSpy.count()>0){
            arguments = proxyDataChangeSpy.value(0);
            QCOMPARE(arguments.at(0).value<QModelIndex>(), proxyModel.mapFromSource(baseDataIdx));
            QCOMPARE(arguments.at(1).value<QModelIndex>(), proxyModel.mapFromSource(baseDataIdx));
            rolesVector = arguments.at(2).value<QVector<int> >();
            QVERIFY(rolesVector.size() < 2);
            if (rolesVector.size())
                QCOMPARE(rolesVector.first(), int(Qt::DisplayRole));
        }
        proxyDataChangeSpy.clear();
    }
}

void tst_RoleMaskProxyModel::testTransparentIfEmpty()
{
    QFETCH(QAbstractItemModel*, baseModel);
    RoleMaskProxyModel proxyModel;
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
}

void tst_RoleMaskProxyModel::testTransparentIfEmptyRecurse(const QAbstractItemModel* const baseModel, const RoleMaskProxyModel* const proxyModel, const QModelIndex& maskedIdx, const QVariant& maskedVal, bool nonMaskedIsNull, const QModelIndex& sourceParent)
{
    for (int i = 0; i < baseModel->rowCount(sourceParent); ++i) {
        for (int j = 0; j < baseModel->columnCount(sourceParent); ++j) {
            const QModelIndex currBsaIdx = baseModel->index(i, j, sourceParent);
            const QModelIndex currProxIdx = proxyModel->mapFromSource(currBsaIdx);
            if (currProxIdx == maskedIdx) {
                QCOMPARE(currProxIdx.data(), maskedVal);
                QVERIFY(currBsaIdx.data()!= maskedVal);
            }
            else if(nonMaskedIsNull){
                QVERIFY(!currProxIdx.data().isValid());
                QVERIFY(currBsaIdx.data().isValid());
            }
            else{
                QCOMPARE(currProxIdx.data(), currBsaIdx.data());
            }
            if (baseModel->hasChildren(currBsaIdx)){
                testTransparentIfEmptyRecurse(baseModel, proxyModel, maskedIdx, maskedVal, nonMaskedIsNull, currBsaIdx);
            }
        }
    }
}

void tst_RoleMaskProxyModel::testTransparentIfEmpty_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::newRow("List") << createListModel(this);
#ifdef QT_GUI_LIB
    QTest::newRow("Table") << createTableModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
#endif
}

void tst_RoleMaskProxyModel::testMergeDisplayEdit()
{
    QFETCH(QAbstractItemModel*, baseModel);
    RoleMaskProxyModel proxyModel;
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
}

void tst_RoleMaskProxyModel::testMergeDisplayEdit_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::newRow("List") << createListModel(this);
#ifdef QT_GUI_LIB
    QTest::newRow("Table") << createTableModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
#endif
}

void tst_RoleMaskProxyModel::testManageMaskedRoles()
{
    RoleMaskProxyModel proxyModel;
    proxyModel.setMergeDisplayEdit(true);
    QVERIFY(proxyModel.maskedRoles().isEmpty());
    proxyModel.addMaskedRole(Qt::EditRole);
    QCOMPARE(proxyModel.maskedRoles().size(),1);
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
    proxyModel.setMaskedRoles({ Qt::UserRole, Qt::EditRole });
    QCOMPARE(proxyModel.maskedRoles().size(), 2);
    QVERIFY(proxyModel.maskedRoles().contains(Qt::DisplayRole));
    QVERIFY(proxyModel.maskedRoles().contains(Qt::UserRole));
    proxyModel.removeMaskedRole(Qt::UserRole);
    QCOMPARE(proxyModel.maskedRoles().size(), 1);
    QCOMPARE(proxyModel.maskedRoles().value(0), int(Qt::DisplayRole));
    proxyModel.removeMaskedRole(Qt::EditRole);
    QVERIFY(proxyModel.maskedRoles().isEmpty());
    proxyModel.setMaskedRoles({ Qt::DisplayRole, Qt::EditRole });
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
    proxyModel.setMaskedRoles({ Qt::UserRole, Qt::EditRole, Qt::DisplayRole });
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
    QStringListModel baseModel1(QStringList({ QStringLiteral("London"), QStringLiteral("Berlin"), QStringLiteral("Paris") }));
    QStringListModel baseModel2(QStringList({ QStringLiteral("Rome"), QStringLiteral("Madrid"), QStringLiteral("Prague") }));
    RoleMaskProxyModel proxyModel;
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.setTransparentIfEmpty(true);
    proxyModel.setSourceModel(&baseModel1);
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
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

void tst_RoleMaskProxyModel::testSetItemData_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<int>("idxRow");
    QTest::addColumn<int>("idxCol");
    QTest::newRow("List") << createListModel(this) << 0 << 0;
#ifdef QT_GUI_LIB
    QTest::newRow("Table") << createTableModel(this) << 1 << 1;
    QTest::newRow("Tree Root") << createTreeModel(this) << 1 << 0;
#endif
}

void tst_RoleMaskProxyModel::testSetItemDataDataChanged_data()
{
    QTest::addColumn<QAbstractItemModel*>("baseModel");
    QTest::addColumn<int>("idxRow");
    QTest::addColumn<int>("idxCol");
    QTest::newRow("List") << createListModel(this) << 0 << 0;
#ifdef QT_GUI_LIB
    QTest::newRow("Table") << createTableModel(this) << 1 << 1;
    QTest::newRow("Tree Root") << createTreeModel(this) << 1 << 0;
#endif
}

void tst_RoleMaskProxyModel::testSetItemDataDataChanged()
{
    QFETCH(QAbstractItemModel*, baseModel);
    QFETCH(int, idxRow);
    QFETCH(int, idxCol);
    QMap<int, QVariant> itemDataSet{ { //TextAlignmentRole
            std::make_pair<int, QVariant>(Qt::UserRole, 5)
            , std::make_pair<int, QVariant>(Qt::DisplayRole, QStringLiteral("Test"))
            , std::make_pair<int, QVariant>(Qt::ToolTipRole, QStringLiteral("ToolTip"))
        } };
    RoleMaskProxyModel proxyModel;
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.setTransparentIfEmpty(true);
    proxyModel.setMaskedRoles({ Qt::UserRole, Qt::ToolTipRole, Qt::TextAlignmentRole });
    proxyModel.setSourceModel(baseModel);
    const QModelIndex proxyIdX = proxyModel.index(idxRow, idxCol);
    QVERIFY(proxyModel.setData(proxyIdX, Qt::AlignRight, Qt::TextAlignmentRole));
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QCOMPARE(proxyDataChangeSpy.size(), 1);
    auto argList = proxyDataChangeSpy.takeFirst();
    QCOMPARE(argList.at(0).value<QModelIndex>(), proxyIdX);
    QCOMPARE(argList.at(1).value<QModelIndex>(), proxyIdX);
    auto rolesVector = argList.at(2).value<QVector<int> >();
    QCOMPARE(rolesVector.size(), 5);
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    // bug fixed by Qt commit 1382374deaa4a854aeb542e6c8f7e1841f2abb10
    QVERIFY(!rolesVector.contains(Qt::TextAlignmentRole));
#endif
    QVERIFY(rolesVector.contains(Qt::ToolTipRole));
    QVERIFY(rolesVector.contains(Qt::EditRole));
    QVERIFY(rolesVector.contains(Qt::DisplayRole));
    QVERIFY(rolesVector.contains(Qt::UserRole));
    itemDataSet[Qt::UserRole] = 6;
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QCOMPARE(proxyDataChangeSpy.size(), 1);
    argList = proxyDataChangeSpy.takeFirst();
    QCOMPARE(argList.at(0).value<QModelIndex>(), proxyIdX);
    QCOMPARE(argList.at(1).value<QModelIndex>(), proxyIdX);
    rolesVector = argList.at(2).value<QVector<int> >();
    QCOMPARE(rolesVector.size(), 1);
    QVERIFY(rolesVector.contains(Qt::UserRole));
    itemDataSet.clear();
    itemDataSet[Qt::UserRole] = 6;
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QCOMPARE(proxyDataChangeSpy.size(), 1);
    argList = proxyDataChangeSpy.takeFirst();
    QCOMPARE(argList.at(0).value<QModelIndex>(), proxyIdX);
    QCOMPARE(argList.at(1).value<QModelIndex>(), proxyIdX);
    rolesVector = argList.at(2).value<QVector<int> >();
    QCOMPARE(rolesVector.size(), 3);
    QVERIFY(rolesVector.contains(Qt::ToolTipRole));
    QVERIFY(rolesVector.contains(Qt::EditRole));
    QVERIFY(rolesVector.contains(Qt::DisplayRole));
    baseModel->deleteLater();
}

void tst_RoleMaskProxyModel::testSetItemData()
{
    QFETCH(QAbstractItemModel*, baseModel);
    QFETCH(int, idxRow);
    QFETCH(int, idxCol);
    const QMap<int, QVariant> itemDataSet{ {
            std::make_pair<int, QVariant>(Qt::UserRole, 5)
            , std::make_pair<int, QVariant>(Qt::DisplayRole, QStringLiteral("Test"))
            , std::make_pair<int, QVariant>(Qt::ToolTipRole, QStringLiteral("ToolTip"))
        } };
    RoleMaskProxyModel proxyModel;
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.setTransparentIfEmpty(true);
    proxyModel.setSourceModel(baseModel);
    proxyModel.setMaskedRoles({ Qt::UserRole, Qt::ToolTipRole, Qt::TextAlignmentRole });
    const QModelIndex proxyIdX = proxyModel.index(idxRow, idxCol);
    QVERIFY(proxyModel.setData(proxyIdX, Qt::AlignRight, Qt::TextAlignmentRole));
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::DisplayRole).toString(), QStringLiteral("Test"));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::EditRole).toString(), QStringLiteral("Test"));
    QCOMPARE(proxyModel.data(proxyIdX, Qt::UserRole).toInt(), 5);
    QCOMPARE(proxyModel.data(proxyIdX, Qt::ToolTipRole).toString(), QStringLiteral("ToolTip"));
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    // bug fixed by Qt commit 1382374deaa4a854aeb542e6c8f7e1841f2abb10
    QCOMPARE(proxyModel.data(proxyIdX, Qt::TextAlignmentRole).toInt(), Qt::AlignRight);
#endif
    baseModel->deleteLater();
}