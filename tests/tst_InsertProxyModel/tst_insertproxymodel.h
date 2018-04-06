#ifndef tst_insertproxymodel_h__
#define tst_insertproxymodel_h__

#include <QObject>
#include <QList>
class QAbstractItemModel;
class tst_InsertProxyModel : public QObject{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
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
};
#endif // tst_insertproxymodel_h__