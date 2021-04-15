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
    void insertChildren();
    void data();
    void headerData_data();
    void headerData();
private:
    void fillTable(QAbstractItemModel *model) const;
};
Q_DECLARE_METATYPE(Qt::Orientation)
#endif // TST_GENERICMODEL_H
