#ifndef DEMOROOTINDEXPROXYMODEL_H
#define DEMOROOTINDEXPROXYMODEL_H

#include <QWidget>
class QAbstractItemModel;
class RootIndexProxyModel;
namespace Ui {
class DemoRootIndexProxyModel;
}
class DemoRootIndexProxyModel : public QWidget
{
    Q_OBJECT

public:
    explicit DemoRootIndexProxyModel(QWidget *parent = nullptr);
    ~DemoRootIndexProxyModel();

private:
    void fillModel(QAbstractItemModel *model);
    void onChangeRoot();
    Ui::DemoRootIndexProxyModel *ui;
    QAbstractItemModel *sourceModel;
    RootIndexProxyModel *proxyModel;
};

#endif // DEMOROOTINDEXPROXYMODEL_H
