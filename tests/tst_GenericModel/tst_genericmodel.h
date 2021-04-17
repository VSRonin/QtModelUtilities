#ifndef TST_GENERICMODEL_H
#define TST_GENERICMODEL_H

#include <QObject>
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
    void headerData_data();
    void headerData();
    void sortList();
    void sortTable();
    void sortByRole();
    void sortTree();
    void sortTreeChildren();
    void sortTreeRecursive();
private:
    void fillTable(QAbstractItemModel *model) const;
};
Q_DECLARE_METATYPE(Qt::Orientation)
#endif // TST_GENERICMODEL_H
