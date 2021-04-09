#ifndef tst_rootindexproxymodel_h__
#define tst_rootindexproxymodel_h__

#include <QObject>
class QAbstractItemModel;
class QModelIndex;
class tst_RootIndexProxyModel : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void showRoot();
    void showRoot_data();
protected:
    void compareModels(const QAbstractItemModel* source,const QAbstractItemModel* proxy, const QModelIndex& sourcePar, const QModelIndex& proxyPar);
};
#endif // tst_rootindexproxymodel_h__
