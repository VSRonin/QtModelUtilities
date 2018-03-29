#ifndef tst_insertproxymodel_h__
#define tst_insertproxymodel_h__

#include <QObject>
#include <QList>
class QAbstractItemModel;
class tst_InsertProxyModel : public QObject{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testBenchMark();
private:
    QList<QAbstractItemModel*> m_models;
};
#endif // tst_insertproxymodel_h__