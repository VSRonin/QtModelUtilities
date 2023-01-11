#ifndef DEMOHIERARCHYLEVELPROXYMODEL_H
#define DEMOHIERARCHYLEVELPROXYMODEL_H

#include <QWidget>
#include <QStandardItemModel>
#include <HierarchyLevelProxyModel>
namespace Ui {
class DemoHierarchylevelProxyModel;
}
class DemoHierarchylevelProxyModel : public QWidget
{
    Q_OBJECT

public:
    explicit DemoHierarchylevelProxyModel(QWidget *parent = nullptr);
    ~DemoHierarchylevelProxyModel();

private:
    void fillModel(QAbstractItemModel *model);
    void onChangeHierarchylevel(int value);
    Ui::DemoHierarchylevelProxyModel *ui;
    QStandardItemModel *sourceModel;
    HierarchyLevelProxyModel *proxyModel;
};

#endif // DEMOHIERARCHYLEVELPROXYMODEL_H
