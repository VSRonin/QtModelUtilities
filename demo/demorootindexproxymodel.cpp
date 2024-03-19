#include "demorootindexproxymodel.h"
#include "ui_demorootindexproxymodel.h"
#include <random>
#include <QStandardItemModel>
#include <RootIndexProxyModel>
#include <QAction>
#if defined(QT_TESTLIB_LIB)
#include <QAbstractItemModelTester>
#endif
DemoRootIndexProxyModel::DemoRootIndexProxyModel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DemoRootIndexProxyModel)
{
    ui->setupUi(this);
    sourceModel = new QStandardItemModel(this);
    fillModel(sourceModel);
    proxyModel = new RootIndexProxyModel(this);
#if defined(QT_TESTLIB_LIB)
    new QAbstractItemModelTester(proxyModel,this);
#endif
    proxyModel->setSourceModel(sourceModel);
    ui->sourceView->setModel(sourceModel);
    ui->proxyView->setModel(proxyModel);
    QAction *setRootAction=new QAction(tr("Set Proxy Root Index"),this);
    connect(setRootAction,&QAction::triggered,this,&DemoRootIndexProxyModel::onChangeRoot);
    ui->sourceView->prependContextMenuAction(setRootAction);
}

void DemoRootIndexProxyModel::fillModel(QAbstractItemModel *model)
{
    std::default_random_engine generator;
    const std::uniform_int_distribution<int> quantityDistrib(0,100);
    const std::uniform_real_distribution<double> weightDistrib(5.0,1000.0);
    model->insertRows(0, 5);
    model->insertColumns(0,3);
    model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    model->setHeaderData(1, Qt::Horizontal, tr("Quantity"));
    model->setHeaderData(2, Qt::Horizontal, tr("Weight"));
    for (int i = 0; i < model->rowCount(); ++i){
        int whQuantity=0;
        double whWeight=0.0;
        const QModelIndex currParent = model->index(i, 0);
        model->setData(currParent, tr("Warehouse %1").arg(i+1));
        model->insertRows(0, 5, currParent);
        model->insertColumns(0,3, currParent);
        for (int j = 0; j < model->rowCount(currParent); ++j){
            int prQuantity=0;
            double prWeight=0.0;
            const QModelIndex currChild = model->index(j,0,currParent);
            model->setData(currChild, tr("Product %1").arg(j+1));
            model->insertRows(0, 5, currChild);
            model->insertColumns(0,3, currChild);
            for (int h=0;h< model->rowCount(currChild); ++h){
                const int quantity = quantityDistrib(generator);
                const double weight = weightDistrib(generator);
                model->setData(model->index(h,0,currChild), tr("Component %1").arg(h+1));
                model->setData(model->index(h,1,currChild), quantity);
                model->setData(model->index(h,2,currChild), weight);
                prQuantity+=quantity;
                prWeight+=weight;
            }
            model->setData(model->index(j,1,currParent), prQuantity);
            model->setData(model->index(j,2,currParent), prWeight);
            whQuantity+=prQuantity;
            whWeight+=prWeight;
        }
        model->setData(model->index(i,1), whQuantity);
        model->setData(model->index(i,2), whWeight);
    }
}

void DemoRootIndexProxyModel::onChangeRoot()
{
    proxyModel->setRootIndex(ui->sourceView->currentIndex());
}

DemoRootIndexProxyModel::~DemoRootIndexProxyModel()
{
    delete ui;
}
