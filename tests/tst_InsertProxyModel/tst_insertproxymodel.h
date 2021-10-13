#ifndef TST_INSERTPROXYMODEL_H
#define TST_INSERTPROXYMODEL_H

#include <QObject>
#include <QList>
class QAbstractItemModel;
class tst_InsertProxyModel : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void autoParent();
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
    void testInsertOnEmptyModel();
    void testResetModel();
    void createPersistentOnLayoutAboutToBeChanged();
};
#endif // TST_INSERTPROXYMODEL_H
