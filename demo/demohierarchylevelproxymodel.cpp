#include "demohierarchylevelproxymodel.h"
#include "ui_demohierarchylevelproxymodel.h"
#include <random>
DemoHierarchylevelProxyModel::DemoHierarchylevelProxyModel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DemoHierarchylevelProxyModel)
{
    ui->setupUi(this);
    sourceModel = new QStandardItemModel(this);
    fillModel(sourceModel);
    proxyModel = new HierarchyLevelProxyModel(this);
    proxyModel->setSourceModel(sourceModel);
    ui->sourceView->setModel(sourceModel);
    ui->proxyView->setModel(proxyModel);
    connect(ui->hierarchySpin, &QSpinBox::valueChanged, this, &DemoHierarchylevelProxyModel::onChangeHierarchylevel);
}

void DemoHierarchylevelProxyModel::fillModel(QAbstractItemModel *model)
{
    std::default_random_engine generator;
    const std::uniform_int_distribution<int> quantityDistrib(0,100);
    const std::uniform_real_distribution<double> weightDistrib(5.0,1000.0);
    model->insertRows(0, 5);
    model->insertColumns(0,3);
    sourceModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    sourceModel->setHeaderData(1, Qt::Horizontal, tr("Quantity"));
    sourceModel->setHeaderData(2, Qt::Horizontal, tr("Weight"));
    for (int i = 0; i < model->rowCount(); ++i){
        int whQuantity=0;
        double whWeight=0.0;
        QModelIndex currParent = model->index(i, 0);
        model->setData(currParent, tr("Warehouse %1").arg(i+1));
        model->insertRows(0, 5, currParent);
        model->insertColumns(0,3, currParent);
        for (int j = 0; j < model->rowCount(currParent); ++j){
            model->setData(model->index(j,0,currParent), tr("Product %1").arg(j+1));
            const int quantity = quantityDistrib(generator);
            model->setData(model->index(j,1,currParent), quantity);
            whQuantity+=quantity;
            const double weight = weightDistrib(generator);
            model->setData(model->index(j,2,currParent), weight);
            whWeight+=weight;
        }
        model->setData(model->index(i,1), whQuantity);
        model->setData(model->index(i,2), whWeight);
    }
}

void DemoHierarchylevelProxyModel::onChangeHierarchylevel(int value)
{
    proxyModel->setHierarchyLevel(value);
};

DemoHierarchylevelProxyModel::~DemoHierarchylevelProxyModel()
{
    delete ui;
}
