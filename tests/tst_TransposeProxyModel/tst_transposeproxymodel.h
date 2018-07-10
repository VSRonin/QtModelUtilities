#ifndef tst_transposeproxymodel_h__
#define tst_transposeproxymodel_h__

#include <QObject>
#include <QModelIndex>
class QAbstractItemModel;
class tst_TransposeProxyModel : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void basicTest();
    void basicTest_data();
    void testSort();
    void testInsertRowBase();
    void testInsertRowBase_data();
    void testInsertColumnBase();
    void testInsertColumnBase_data();
    void testInsertColumnProxy();
    void testInsertColumnProxy_data();
    void testInsertRowProxy();
    void testInsertRowProxy_data();
    void testSetData_data();
    void testSetData();
private:
    void testTransposed(const QAbstractItemModel* const baseModel, const QAbstractItemModel* const transposed, const QModelIndex& baseParent = QModelIndex(), const QModelIndex& transposedParent = QModelIndex());
};

#endif // tst_transposeproxymodel_h__
