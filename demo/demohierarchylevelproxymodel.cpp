#include "demohierarchylevelproxymodel.h"
#include "ui_demohierarchylevelproxymodel.h"

DemoHierarchylevelProxyModel::DemoHierarchylevelProxyModel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DemoHierarchylevelProxyModel)
{
    ui->setupUi(this);
    sourceModel=new QStandardItemModel(this);
    buildBranch(sourceModel);
    sourceModel->setHeaderData(0,Qt::Horizontal,tr("Values"));
    proxyModel = new HierarchyLevelProxyModel(this);
    proxyModel->setSourceModel(sourceModel);
    ui->sourceView->setModel(sourceModel);
    ui->proxyView->setModel(proxyModel);
    connect(ui->hierarchySpin,&QSpinBox::valueChanged,this,&DemoHierarchylevelProxyModel::onChangeHierarchylevel);
}

void DemoHierarchylevelProxyModel::buildBranch(QAbstractItemModel* model, const QModelIndex& parent, const QString& prefix, int depth){
        model->insertRows(0,5,parent);
        model->insertColumn(0,parent);
        for(int i=0;i<model->rowCount(parent);++i){
            QString dataVal = prefix;
            if(depth==0)
                dataVal += QLatin1Char('A'+i);
            else if(depth==1 || depth==3)
                dataVal += QLatin1Char('1'+i);
            else if(depth==2)
                dataVal += QLatin1Char('a'+i);
            else
                return;
            const QModelIndex currIdx = model->index(i,0,parent);
            model->setData(currIdx,dataVal);
            if(depth<3)
                buildBranch(model,currIdx,dataVal,depth+1);
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
