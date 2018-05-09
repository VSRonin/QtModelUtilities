#ifndef tst_insertproxymodel_h__
#define tst_insertproxymodel_h__

#include <QObject>
#include <QList>
class QAbstractItemModel;
class tst_InsertProxyModel : public QObject{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testData();
    void testData_data();
    void testSetData();
    void testSetData_data();
    void testSetItemData_data();
    void testSetItemData();
    void testSetItemDataDataChanged_data();
    void testSetItemDataDataChanged();
    void testCommitSubclass();
    void testCommitSubclass_data();
    void testCommitSlot();
    void testCommitSlot_data();
    void testSourceInsertRow();
    void testSourceInsertRow_data();
    void testSourceInsertCol();
    void testSourceInsertCol_data();
    void testDisconnectedModel();
    void testNullModel();
    void testProperties();
    void testDataForCorner();
    void testSort();
    void testSort_data();
};
#endif // tst_insertproxymodel_h__