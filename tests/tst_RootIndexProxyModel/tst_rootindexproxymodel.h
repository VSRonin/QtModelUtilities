#ifndef TST_ROOTINDEXPROXYMODEL_H
#define TST_ROOTINDEXPROXYMODEL_H

#include <QObject>
class QAbstractItemModel;
class QModelIndex;
class tst_RootIndexProxyModel : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void autoParent();
    void showRoot();
    void showRoot_data();
    void replaceModel();
    void replaceModel_data();
    void switchRoot();
    void rootMoves();
    void rootRemoved();
    void sourceMoveRows();
    void sourceMoveColumns();
    void sourceSortDescendantOfRoot();
    void sourceSortAncestorOfRoot();
    void sourceDataChanged();
    void sourceDataChanged_data();
    void insertRow();
    void insertColumn();
    void removeRow();
    void removeColumn();
    void bug53Rows();
    void bug53MoveRows();
    void bug53Cols();
    void bug53MoveCols();
    void bug60Row();
    void bug60Col();

protected:
    void compareModels(const QAbstractItemModel *source, const QAbstractItemModel *proxy, const QModelIndex &sourcePar, const QModelIndex &proxyPar);
};
#endif // TST_ROOTINDEXPROXYMODEL_H
