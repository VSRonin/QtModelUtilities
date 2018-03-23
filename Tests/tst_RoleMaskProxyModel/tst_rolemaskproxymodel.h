#ifndef tst_rolemaskproxymodel_h__
#define tst_rolemaskproxymodel_h__

#include <QObject>
#include <array>
#include <QModelIndexList>
class QAbstractItemModel;
class RoleMaskProxyModel;
class tst_RoleMaskProxyModel :public QObject{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testUseRoleMask();
    void testUseRoleMask_data();
    void testInsertRow();
    void testInsertRow_data();
    void testInsertColumn();
    void testInsertColumn_data();
    void testNullModel();
private:
    void testUseRoleMaskRecurse(const int magicNumber, const QAbstractItemModel* const baseModel, const RoleMaskProxyModel* const proxyModel, const QModelIndexList& magicNumerIndexes, const QModelIndex& sourceParent = QModelIndex(), const QModelIndex& proxyParent = QModelIndex());
    std::array<QAbstractItemModel*, 3> m_models;
};
#endif // tst_rolemaskproxymodel_h__
