#ifndef TST_INSERTPROXYMODEL_H
#define TST_INSERTPROXYMODEL_H

#include <QObject>
class QAbstractItemModel;
class tst_HierarchyLevelProxyModel : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void autoParent();

    void testData();
    void testData_data();
    void testSetData();
    void testSetData_data();
    void testInsertRowSource_data();
    void testInsertRowSource();
    void testInsertRowsSource_data();
    void testInsertRowsSource();
    void testInsertRowProxy_data();
    void testInsertRowProxy();
    void testInsertRowsProxy_data();
    void testInsertRowsProxy();

    void testInsertBehaviour();

    void testSetItemData_data();
    void testSetItemData();
    void testSetItemDataDataChanged_data();
    void testSetItemDataDataChanged();
    void testSourceInsertRow();
    void testSourceInsertRow_data();
    void testSourceInsertCol();
    void testSourceInsertCol_data();
    void testDisconnectedModel();
    void testProperties();
    void testSort();
    void testSort_data();
    void testResetModel();
    void createPersistentOnLayoutAboutToBeChanged();
};
#endif // TST_INSERTPROXYMODEL_H
