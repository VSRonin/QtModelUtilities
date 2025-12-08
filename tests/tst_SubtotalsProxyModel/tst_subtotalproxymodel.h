#ifndef TST_SUBTOTALPROXYMODEL_H
#define TST_SUBTOTALPROXYMODEL_H

#include <QObject>
class tst_SubtotalProxyModel : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void autoParent();
    void testDisconnectedModel();
    void testRowCount_data();
    void testRowCount();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    // void testMultiData();
    // void testClearItemData();
#endif
    static void intSum(QVariant &r, const QVariant &a);
    static void doubleSum(QVariant &r, const QVariant &a);
};
#endif // TST_SUBTOTALPROXYMODEL_H
