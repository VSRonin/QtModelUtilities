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
    void testProperties();
    void testInsertColumn();
    void testInsertColumn_data();
    void testNullModel();
    void testDataChangeSignals_data();
    void testDataChangeSignals();
    void testTransparentIfEmpty();
    void testTransparentIfEmpty_data();
    void testMergeDisplayEdit();
    void testMergeDisplayEdit_data();
    void testManageMaskedRoles();
    void testDisconnectedModel();
    void testSetItemData_data();
    void testSetItemData();
private:
    void testTransparentIfEmptyRecurse(const QAbstractItemModel* const baseModel, const RoleMaskProxyModel* const proxyModel, const QModelIndex& maskedIdx, const QVariant& maskedVal, bool nonMaskedIsNull, const QModelIndex& sourceParent = QModelIndex());
    int countChildren(const QAbstractItemModel* const baseModel, const QModelIndex& parIdx = QModelIndex());
    void testUseRoleMaskRecurse(const int magicNumber, const QAbstractItemModel* const baseModel, const RoleMaskProxyModel* const proxyModel, const QModelIndexList& magicNumerIndexes, const bool userRoleEditable, const QModelIndex& sourceParent = QModelIndex(), const QModelIndex& proxyParent = QModelIndex());
    QList<QAbstractItemModel*> m_models;
};
#endif // tst_rolemaskproxymodel_h__
