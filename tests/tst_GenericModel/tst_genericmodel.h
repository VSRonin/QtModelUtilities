#ifndef TST_GENERICMODEL_H
#define TST_GENERICMODEL_H

#include <QObject>
#include <QModelIndex>
class QAbstractItemModel;
class QModelIndex;
class tst_GenericModel : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void autoParent();
    void insertRow_data();
    void insertRow();
    void insertRows_data();
    void insertRows();
    void insertColumn_data();
    void insertColumn();
    void insertColumns_data();
    void insertColumns();
    void removeRow_data();
    void removeRow();
    void removeRows_data();
    void removeRows();
    void removeColumn_data();
    void removeColumn();
    void removeColumns_data();
    void removeColumns();
    void insertChildren_data();
    void insertChildren();
    void removeChildren_data();
    void removeChildren();
    void data();
    void clearData();
    void headerData_data();
    void headerData();
    void sortList();
    void sortTable();
    void sortByRole();
    void sortTree();
    void sortTreeChildren();
    void sortTreeRecursive();
    void moveRowsList();
    void moveRowsTable();
    void moveRowsTreeSameBranch();
    void moveRowsNewBranch();
    void moveRowsExistingBranch();
    void moveRowsFromRootToBranch();
    void moveRowsFromBranchToRoot();
    void moveColumnsTable();
    void moveColumnsTreeSameBranch();
    void moveColumnsNewBranch();
    void moveColumnsExistingBranch();
    void moveColumnsFromRootToBranch();
    void moveColumnsFromBranchToRoot();
    void roleNames();
    void dragDropInvalid();
    void dragDropList();
    void dragDropTable();
    void dragDropTree();
    // Benchmarks
    void bDataStaticModel_data();
    void bDataStaticModel();
    void bInsertRows_data();
    void bInsertRows();
    void bInsertColumns_data();
    void bInsertColumns();
    void bSort_data();
    void bSort();

private:
    void fillTable(QAbstractItemModel *model, int rows, int cols, const QModelIndex &parent = QModelIndex(), int shift = 0) const;
};
Q_DECLARE_METATYPE(Qt::Orientation)
#endif // TST_GENERICMODEL_H
