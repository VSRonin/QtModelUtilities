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
    void insertColumn_data();
    void insertColumn();
    void data();
private:
    void fillTable(QAbstractItemModel* model) const;
};
#endif // TST_GENERICMODEL_H
