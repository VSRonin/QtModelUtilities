#ifndef TST_ROOTINDEXPROXYMODEL_H
#define TST_ROOTINDEXPROXYMODEL_H

#include <QObject>
class QAbstractItemModel;
class QModelIndex;
class tst_RootIndexProxyModel : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void showRoot();
    void showRoot_data();
    void replaceModel();
    void replaceModel_data();
    void switchRoot();
    void rootMoves();
    void rootRemoved();
protected:
    void compareModels(const QAbstractItemModel* source,const QAbstractItemModel* proxy, const QModelIndex& sourcePar, const QModelIndex& proxyPar);
};
#endif // TST_ROOTINDEXPROXYMODEL_H
