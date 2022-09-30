#include "tst_hierarchylevelproxymodel.h"

#include <hierarchylevelproxymodel.h>
#include <QtTest/QTest>
#include <QSignalSpy>
#include "../modeltestmanager.h"
#include <QSortFilterProxyModel>

QAbstractItemModel *createNullModel(QObject *parent)
{
    Q_UNUSED(parent)
    return nullptr;
}

QAbstractItemModel *createListModel(QObject *parent)
{
    return new SimpleModel(
            QStringList() << QStringLiteral("1") << QStringLiteral("2") << QStringLiteral("3") << QStringLiteral("4") << QStringLiteral("5"), parent);
}

QAbstractItemModel *createTreeModel(QObject *parent, int rows=5, int cols=3)
{
    QAbstractItemModel *result = nullptr;
#ifdef COMPLEX_MODEL_SUPPORT
    result = new ComplexModel(parent);
    result->insertRows(0, rows);
    result->insertColumns(0, cols);
    for (int i = 0; i < result->rowCount(); ++i) {
        for (int j = 0; j < result->columnCount(); ++j) {
            result->setData(result->index(i, j), QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            result->setData(result->index(i, j), i, Qt::UserRole);
            result->setData(result->index(i, j), j, Qt::UserRole + 1);
        }
        const QModelIndex parIdx = result->index(i, 0);
        result->insertRows(0, rows, parIdx);
        result->insertColumns(0, cols, parIdx);
        for (int k = 0; k < result->rowCount(parIdx); ++k) {
            for (int h = 0; h < result->columnCount(parIdx); ++h) {
                result->setData(result->index(k, h, parIdx), QStringLiteral("%1,%2,%3").arg(i).arg(k).arg(h), Qt::EditRole);
                result->setData(result->index(k, h, parIdx), h, Qt::UserRole);
                result->setData(result->index(k, h, parIdx), k, Qt::UserRole + 1);

            }
            const QModelIndex childIdx = result->index(i, 0, parIdx);
            result->insertRows(0, rows, childIdx);
            result->insertColumns(0, cols, childIdx);
             for (int j = 0; j < result->rowCount(childIdx); ++j) {
                 for (int h = 0; h < result->columnCount(childIdx); ++h) {
                     result->setData(result->index(j, h, childIdx), QStringLiteral("%1,%2,%3,%4").arg(i).arg(k).arg(j).arg(h), Qt::EditRole);
                     result->setData(result->index(j, h, childIdx), h, Qt::UserRole);
                     result->setData(result->index(j, h, childIdx), k, Qt::UserRole + 1);
                 }
             }
        }
    }
#endif
    return result;
}

void tst_HierarchyLevelProxyModel::autoParent()
{
    QObject *parentObj = new QObject;
    auto testItem = new HierarchyLevelProxyModel(parentObj);
    QSignalSpy testItemDestroyedSpy(testItem, &HierarchyLevelProxyModel::destroyed);
    QVERIFY(testItemDestroyedSpy.isValid());
    delete parentObj;
    QCOMPARE(testItemDestroyedSpy.count(), 1);
}

void tst_HierarchyLevelProxyModel::testSourceInsertCol()
{
#ifdef COMPLEX_MODEL_SUPPORT

#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}
void tst_HierarchyLevelProxyModel::testSourceInsertRow()
{
#ifdef COMPLEX_MODEL_SUPPORT

#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_HierarchyLevelProxyModel::testSourceInsertRow_data()
{
#ifdef COMPLEX_MODEL_SUPPORT
#endif
}

void tst_HierarchyLevelProxyModel::testSourceInsertCol_data()
{

#ifdef COMPLEX_MODEL_SUPPORT
#endif
}



void tst_HierarchyLevelProxyModel::testProperties()
{
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    QCOMPARE(proxyModel.property("hierarchyLevel").toInt(), 0);
    QVERIFY(proxyModel.setProperty("hierarchyLevel", 1));
    QCOMPARE(proxyModel.property("hierarchyLevel").toInt(), 1);
}



void tst_HierarchyLevelProxyModel::testSort()
{
#ifdef COMPLEX_MODEL_SUPPORT

#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_HierarchyLevelProxyModel::testSort_data()
{
#ifdef COMPLEX_MODEL_SUPPORT
#endif
}



void tst_HierarchyLevelProxyModel::testResetModel()
{
#ifdef COMPLEX_MODEL_SUPPORT

#else
    QSKIP("This test requires the Qt GUI or GenericModel modules");
#endif
}

void tst_HierarchyLevelProxyModel::testDisconnectedModel()
{
    SimpleModel baseModel1(QStringList({QStringLiteral("London"), QStringLiteral("Berlin"), QStringLiteral("Paris")}));
    SimpleModel baseModel2(QStringList({QStringLiteral("Rome"), QStringLiteral("Madrid"), QStringLiteral("Prague")}));
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, this);
    proxyModel.setSourceModel(&baseModel1);
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(proxyDataChangeSpy.isValid());
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

void tst_HierarchyLevelProxyModel::testData()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    HierarchyLevelProxyModel proxyModel;
    proxyModel.setSourceModel(baseModel);
    new ModelTest(&proxyModel, baseModel);
    const int sourceRows = baseModel->rowCount();
    const int sourceCols = baseModel->columnCount();
    for (int i = 0; i < sourceRows; ++i) {
        for (int j = 0; j < sourceCols; ++j) {
            QCOMPARE(proxyModel.index(i, j).data(), baseModel->index(i, j).data());
        }
    }

    proxyModel.setHierarchyLevel(1);
    int rootRowIter=0;
    for (int i = 0; i < sourceRows; ++i) {
        for (int j = 0; j < sourceCols; ++j) {
            const QModelIndex sourceParent = baseModel->index(i, j);
            if(baseModel->hasChildren(sourceParent)){
                for(int h=0, hEnd=baseModel->rowCount(sourceParent);h<hEnd;++h){
                    for(int k=0, kEnd=baseModel->columnCount(sourceParent);k<kEnd;++k){
                        const QModelIndex sourceIdx = baseModel->index(h, k, sourceParent);
                        const QModelIndex proxyIdx = proxyModel.index(rootRowIter, k);
                        QCOMPARE(proxyIdx.data(), sourceIdx.data());
                        if(baseModel->hasChildren(sourceIdx)){
                            for(int ii=0, iiEnd=baseModel->rowCount(sourceIdx);ii<iiEnd;++ii){
                                for(int jj=0, jjEnd=baseModel->columnCount(sourceIdx);jj<jjEnd;++jj){
                                    QCOMPARE(proxyModel.index(ii, jj, proxyIdx).data(), baseModel->index(ii, jj, sourceIdx).data());
                                }
                            }
                        }
                    }
                    ++rootRowIter;
                }
            }
        }
    }

    baseModel->deleteLater();
}

void tst_HierarchyLevelProxyModel::testData_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_HierarchyLevelProxyModel::testSetData()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyDataChangeSpy(&proxyModel, &QAbstractItemModel::dataChanged);
    QSignalSpy sourceDataChangeSpy(baseModel, &QAbstractItemModel::dataChanged);
    const QModelIndex proxyIdX = proxyModel.index(0, 0);
    QString idxData = QStringLiteral("Test");
    QVERIFY(proxyModel.setData(proxyIdX, idxData));
    QCOMPARE(proxyModel.data(proxyIdX).toString(), idxData);
    QCOMPARE(baseModel->index(0,0).data().toString(), idxData);
    QCOMPARE(proxyDataChangeSpy.count(), 1);
    QCOMPARE(sourceDataChangeSpy.count(), 1);
    auto spyArgs = proxyDataChangeSpy.takeFirst();
    QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyIdX);
    QCOMPARE(spyArgs.at(1).value<QModelIndex>(), proxyIdX);
    spyArgs = sourceDataChangeSpy.takeFirst();
    QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(0,0));
    QCOMPARE(spyArgs.at(1).value<QModelIndex>(), baseModel->index(0,0));
    baseModel->deleteLater();
    if(baseModel->hasChildren(baseModel->index(0,0))){
        const QModelIndex proxyChildIdX = proxyModel.index(0, 0, proxyIdX);
        QVERIFY(proxyModel.setData(proxyChildIdX, idxData));
        QCOMPARE(proxyModel.data(proxyChildIdX).toString(), idxData);
        QCOMPARE(baseModel->index(0,0,baseModel->index(0,0)).data().toString(), idxData);
        QCOMPARE(proxyDataChangeSpy.count(), 1);
        QCOMPARE(sourceDataChangeSpy.count(), 1);
        auto spyArgs = proxyDataChangeSpy.takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyChildIdX);
        QCOMPARE(spyArgs.at(1).value<QModelIndex>(), proxyChildIdX);
        spyArgs = sourceDataChangeSpy.takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(0,0,baseModel->index(0,0)));
        QCOMPARE(spyArgs.at(1).value<QModelIndex>(), baseModel->index(0,0,baseModel->index(0,0)));
    }
    proxyModel.setHierarchyLevel(1);
    idxData+=QStringLiteral("Test2");
    if(baseModel->hasChildren(baseModel->index(0,0))){
        QVERIFY(proxyModel.setData(proxyModel.index(0, 0), idxData));
        QCOMPARE(proxyModel.data(proxyModel.index(0, 0)).toString(), idxData);
        QCOMPARE(baseModel->index(0,0,baseModel->index(0,0)).data().toString(), idxData);
        QCOMPARE(proxyDataChangeSpy.count(), 1);
        QCOMPARE(sourceDataChangeSpy.count(), 1);
        auto spyArgs = proxyDataChangeSpy.takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), proxyModel.index(0, 0));
        QCOMPARE(spyArgs.at(1).value<QModelIndex>(), proxyModel.index(0, 0));
        spyArgs = sourceDataChangeSpy.takeFirst();
        QCOMPARE(spyArgs.at(0).value<QModelIndex>(), baseModel->index(0,0,baseModel->index(0,0)));
        QCOMPARE(spyArgs.at(1).value<QModelIndex>(), baseModel->index(0,0,baseModel->index(0,0)));
    }
    else{
        QVERIFY(!proxyModel.setData(proxyModel.index(0,0), idxData));
    }
}

void tst_HierarchyLevelProxyModel::testSetData_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_HierarchyLevelProxyModel::testInsertRowSource_data()
{
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::newRow("List") << createListModel(this);
    QTest::newRow("Tree") << createTreeModel(this);
}

void tst_HierarchyLevelProxyModel::testInsertRowSource()
{
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    HierarchyLevelProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setSourceModel(baseModel);
    QSignalSpy proxyRowAboutToInsertSpy(&proxyModel, &QAbstractItemModel::rowsAboutToBeInserted);
    QSignalSpy proxyRowInsertSpy(&proxyModel, &QAbstractItemModel::rowsInserted);

    int beforeInsertRowCount= baseModel->rowCount();
    QVERIFY(baseModel->insertRow(2));
    QCOMPARE(proxyModel.rowCount(),beforeInsertRowCount+1);
    for(QSignalSpy* spy : {&proxyRowAboutToInsertSpy,&proxyRowInsertSpy}){
        QCOMPARE(spy->count(),1);
        const auto spyArgs = spy->takeFirst();
        QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
        QCOMPARE(spyArgs.at(2).toInt(),2);
        QCOMPARE(spyArgs.at(1).toInt(),2);
    }
    if(baseModel->hasChildren(baseModel->index(0,0))){
        beforeInsertRowCount= 0;
        for(int i=0, iEnd=baseModel->rowCount();i<iEnd;++i)
            beforeInsertRowCount+=baseModel->rowCount(baseModel->index(i,0));
        proxyModel.setHierarchyLevel(1);
        QVERIFY(baseModel->insertRow(2));
        QCOMPARE(proxyModel.rowCount(),beforeInsertRowCount);
        for(QSignalSpy* spy : {&proxyRowAboutToInsertSpy,&proxyRowInsertSpy})
            QCOMPARE(spy->count(),0);
        QVERIFY(baseModel->insertRow(1,baseModel->index(1,0)));
        QCOMPARE(proxyModel.rowCount(),beforeInsertRowCount+1);
        for(QSignalSpy* spy : {&proxyRowAboutToInsertSpy,&proxyRowInsertSpy}){
            QCOMPARE(spy->count(),1);
            const auto spyArgs = spy->takeFirst();
            QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
            QCOMPARE(spyArgs.at(2).toInt(),baseModel->rowCount(baseModel->index(0,0))+1);
            QCOMPARE(spyArgs.at(1).toInt(),baseModel->rowCount(baseModel->index(0,0))+1);
        }
        QCOMPARE(baseModel->rowCount(baseModel->index(2,0)),0);
        QVERIFY(baseModel->insertColumn(0,baseModel->index(2,0)));
        QVERIFY(baseModel->insertRow(0,baseModel->index(2,0)));
        QCOMPARE(proxyModel.rowCount(),beforeInsertRowCount+2);
        for(QSignalSpy* spy : {&proxyRowAboutToInsertSpy,&proxyRowInsertSpy}){
            QCOMPARE(spy->count(),1);
            const auto spyArgs = spy->takeFirst();
            QVERIFY(!spyArgs.at(0).value<QModelIndex>().isValid());
            QCOMPARE(spyArgs.at(2).toInt(),baseModel->rowCount(baseModel->index(0,0))+baseModel->rowCount(baseModel->index(1,0)));
            QCOMPARE(spyArgs.at(1).toInt(),baseModel->rowCount(baseModel->index(0,0))+baseModel->rowCount(baseModel->index(1,0)));
        }
    }
}

void tst_HierarchyLevelProxyModel::testSetItemData_data()
{
    /*
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<int>("idxRow");
    QTest::addColumn<int>("idxCol");
    QAbstractItemModel *baseModel = createListModel(this);
    QTest::newRow("List Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createListModel(this);
    QTest::newRow("List Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createListModel(this);
    QTest::newRow("List Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
    QTest::newRow("Table Inside Base Model") << createTableModel(this) << 0 << 0;
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
    QTest::newRow("Tree Inside Base Model") << createTreeModel(this) << 0 << 0;
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
    */
    //#TODO
}

void tst_HierarchyLevelProxyModel::testSetItemDataDataChanged_data()
{
    /*
    QTest::addColumn<QAbstractItemModel *>("baseModel");
    QTest::addColumn<int>("idxRow");
    QTest::addColumn<int>("idxCol");
    QAbstractItemModel *baseModel = createListModel(this);
    QTest::newRow("List Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createListModel(this);
    QTest::newRow("List Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createListModel(this);
    QTest::newRow("List Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createTableModel(this);
    if (baseModel)
        QTest::newRow("Table Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Extra Row") << baseModel << baseModel->rowCount() << 0;
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Extra Col") << baseModel << 0 << baseModel->columnCount();
    baseModel = createTreeModel(this);
    if (baseModel)
        QTest::newRow("Tree Corner") << baseModel << baseModel->rowCount() << baseModel->columnCount();
    */
    //#TODO
}

void tst_HierarchyLevelProxyModel::testSetItemDataDataChanged()
{
    /*
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    QFETCH(int, idxRow);
    QFETCH(int, idxCol);
    QMap<int, QVariant> itemDataSet{{// TextAlignmentRole
                                     std::make_pair<int, QVariant>(Qt::UserRole, 5),
                                     std::make_pair<int, QVariant>(Qt::DisplayRole, QStringLiteral("Test")),
                                     std::make_pair<int, QVariant>(Qt::ToolTipRole, QStringLiteral("ToolTip"))}};
    InsertProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.setInsertDirection(InsertProxyModel::InsertColumn | InsertProxyModel::InsertRow);
    proxyModel.setSourceModel(baseModel);
    const QModelIndex proxyIdX = proxyModel.index(idxRow, idxCol);
    QVERIFY(proxyModel.setData(proxyIdX, Qt::AlignRight, Qt::TextAlignmentRole));
    QSignalSpy proxyDataChangeSpy(&proxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
    QCOMPARE(proxyDataChangeSpy.size(), 1);
    auto argList = proxyDataChangeSpy.takeFirst();
    QCOMPARE(argList.at(0).value<QModelIndex>(), proxyIdX);
    QCOMPARE(argList.at(1).value<QModelIndex>(), proxyIdX);
    auto rolesVector = argList.at(2).value<QVector<int>>();
#ifndef SKIP_QTBUG_45114
    QCOMPARE(rolesVector.size(), 4);
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
    rolesVector = argList.at(2).value<QVector<int>>();
    QCOMPARE(rolesVector.size(), 1);
    QVERIFY(rolesVector.contains(Qt::UserRole));
    itemDataSet.clear();
    itemDataSet[Qt::UserRole] = 6;
    QVERIFY(proxyModel.setItemData(proxyIdX, itemDataSet));
#ifndef SKIP_QTBUG_45114
    QCOMPARE(proxyDataChangeSpy.size(), 0);
#endif
    baseModel->deleteLater();
    */
    //#TODO
}

void tst_HierarchyLevelProxyModel::testSetItemData()
{
    /*
    QFETCH(QAbstractItemModel *, baseModel);
    if (!baseModel)
        return;
    QFETCH(int, idxRow);
    QFETCH(int, idxCol);
    const QMap<int, QVariant> itemDataSet{{std::make_pair<int, QVariant>(Qt::UserRole, 5),
                                           std::make_pair<int, QVariant>(Qt::DisplayRole, QStringLiteral("Test")),
                                           std::make_pair<int, QVariant>(Qt::ToolTipRole, QStringLiteral("ToolTip"))}};
    InsertProxyModel proxyModel;
    new ModelTest(&proxyModel, baseModel);
    proxyModel.setMergeDisplayEdit(true);
    proxyModel.setInsertDirection(InsertProxyModel::InsertColumn | InsertProxyModel::InsertRow);
    proxyModel.setSourceModel(baseModel);
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
    */
    //#TODO
}


void tst_HierarchyLevelProxyModel::createPersistentOnLayoutAboutToBeChanged()
{
    SimpleModel model(QStringList{QStringLiteral("1"), QStringLiteral("2"), QStringLiteral("3")});
    HierarchyLevelProxyModel proxy;
    new ModelTest(&proxy, &proxy);
    proxy.setSourceModel(&model);
    /* #TODO
    QList<QPersistentModelIndex> idxList;
    QSignalSpy layoutAboutToBeChangedSpy(&proxy, SIGNAL(layoutAboutToBeChanged()));
    QVERIFY(layoutAboutToBeChangedSpy.isValid());
    QSignalSpy layoutChangedSpy(&proxy, SIGNAL(layoutChanged()));
    QVERIFY(layoutChangedSpy.isValid());
    connect(&proxy, &QAbstractItemModel::layoutAboutToBeChanged, [&idxList, &proxy]() {
        idxList.clear();
        for (int row = 0; row < 4; ++row)
            idxList << QPersistentModelIndex(proxy.index(row, 0));
    });
    connect(&proxy, &QAbstractItemModel::layoutChanged, [&idxList]() {
        QCOMPARE(idxList.size(), 4);
        QCOMPARE(idxList.at(0).row(), 1);
        QCOMPARE(idxList.at(0).column(), 0);
        QCOMPARE(idxList.at(0).data().toString(), QStringLiteral("1"));
        QCOMPARE(idxList.at(1).row(), 0);
        QCOMPARE(idxList.at(1).column(), 0);
        QCOMPARE(idxList.at(1).data().toString(), QStringLiteral("0"));
        QCOMPARE(idxList.at(2).row(), 2);
        QCOMPARE(idxList.at(2).column(), 0);
        QCOMPARE(idxList.at(2).data().toString(), QStringLiteral("3"));
        QCOMPARE(idxList.at(3).row(), 3);
        QCOMPARE(idxList.at(3).column(), 0);
        QCOMPARE(idxList.at(3).data().toString(), QStringLiteral("4"));
    });
    model.setData(model.index(1, 0), QStringLiteral("0"));
    model.sort(0);
    QCOMPARE(layoutAboutToBeChangedSpy.size(), 1);
    QCOMPARE(layoutChangedSpy.size(), 1);
    */
}
