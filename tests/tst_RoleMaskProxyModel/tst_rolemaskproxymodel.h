#ifndef TST_ROLEMASKPROXYMODEL_H
#define TST_ROLEMASKPROXYMODEL_H

#include <QObject>
#include <array>
#include <QModelIndexList>
class QAbstractItemModel;
class RoleMaskProxyModel;
class tst_RoleMaskProxyModel : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void autoParent();
    void initTestCase();
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
    void testSetItemDataDataChanged_data();
    void testSetItemDataDataChanged();
    void testSort_data();
    void testSort();
    void testEmptyProxy();
    void testMoveRowAfter();
    void testMoveRowBefore();
    void testMoveColumnAfter();
    void testMoveColumnBefore();
    void testRemoveRows_data();
    void testRemoveRows();
    void testRemoveColumns_data();
    void testRemoveColumns();
    void testMaskVerticalHeaderData();
    void testMaskHorizontalHeaderData();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    void testMultiData();
    void testClearItemData();
#endif
private:
    void testTransparentIfEmptyRecurse(const QAbstractItemModel *const baseModel, const RoleMaskProxyModel *const proxyModel,
                                       const QModelIndex &maskedIdx, const QVariant &maskedVal, bool nonMaskedIsNull,
                                       const QModelIndex &sourceParent = QModelIndex());
    int countChildren(const QAbstractItemModel *const baseModel, const QModelIndex &parIdx = QModelIndex());
    void testUseRoleMaskRecurse(const int magicNumber, const QAbstractItemModel *const baseModel, const RoleMaskProxyModel *const proxyModel,
                                const QModelIndexList &magicNumerIndexes, const bool userRoleEditable,
                                const QModelIndex &sourceParent = QModelIndex(), const QModelIndex &proxyParent = QModelIndex());
};
#endif // TST_ROLEMASKPROXYMODEL_H
