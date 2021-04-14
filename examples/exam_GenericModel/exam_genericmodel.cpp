#include <GenericModel>
#include <QPushButton>
#include <QApplication>
#include <QHeaderView>
#include <QGridLayout>
#include <QTreeView>
#include <QSpinBox>
#include <QLabel>

#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget mainWid;
    mainWid.setWindowTitle(QStringLiteral("GenericModel Example"));
    GenericModel *baseModel = new GenericModel(&mainWid);
    // fill the model with example data
    baseModel->insertColumns(0, 3);
    baseModel->insertRows(0, 5);
    for (int i = 0; i < baseModel->rowCount(); ++i) {
        baseModel->setHeaderData(i,Qt::Vertical,i+1);
        for (int j = 0; j < baseModel->columnCount(); ++j)
            baseModel->setData(baseModel->index(i, j), QStringLiteral("%1;%2").arg(i).arg(j));
    }
    for (int j = 0; j < baseModel->columnCount(); ++j)
        baseModel->setHeaderData(j,Qt::Horizontal,j+1);
    baseModel->setSpan(baseModel->index(2,1),QSize(3,2));



    // Create a view for the model
    QTreeView *baseView = new QTreeView(&mainWid);
    baseView->header()->setSectionResizeMode(QHeaderView::Stretch);
    baseView->setSelectionMode(QAbstractItemView::SingleSelection);
    baseView->setSelectionBehavior(QAbstractItemView::SelectItems);
    baseView->setModel(baseModel);

    QSpinBox* rowNumberSpin = new QSpinBox(&mainWid);
    rowNumberSpin->setMinimum(1);
    QPushButton* insertRowButton = new QPushButton(QStringLiteral("Insert Rows"), &mainWid);
    QObject::connect(insertRowButton,&QPushButton::clicked,[rowNumberSpin,baseModel,baseView](){
        baseModel->insertRows(qMax(0,baseView->currentIndex().row()),rowNumberSpin->value());
    });
    QPushButton* removeRowButton = new QPushButton(QStringLiteral("Remove Rows"), &mainWid);
    QObject::connect(removeRowButton,&QPushButton::clicked,[rowNumberSpin,baseModel,baseView](){
        baseModel->removeRows(qMax(0,baseView->currentIndex().row()),rowNumberSpin->value());
    });

    QSpinBox* columnNumberSpin = new QSpinBox(&mainWid);
    columnNumberSpin->setMinimum(1);
    QPushButton* insertColumnButton = new QPushButton(QStringLiteral("Insert Columns"), &mainWid);
    QObject::connect(insertColumnButton,&QPushButton::clicked,[columnNumberSpin,baseModel,baseView](){
        baseModel->insertColumns(qMax(0,baseView->currentIndex().column()),columnNumberSpin->value());
    });
    QPushButton* removeColumnButton = new QPushButton(QStringLiteral("Remove Columns"), &mainWid);
    QObject::connect(removeColumnButton,&QPushButton::clicked,[columnNumberSpin,baseModel,baseView](){
        baseModel->removeColumns(qMax(0,baseView->currentIndex().column()),columnNumberSpin->value());
    });

    // lay out the widget
    QGridLayout *topLay = new QGridLayout;
    topLay->addWidget(new QLabel(QStringLiteral("Number of Rows")),0,0);
    topLay->addWidget(rowNumberSpin,0,1);
    topLay->addWidget(insertRowButton,0,2);
    topLay->addWidget(removeRowButton,0,3);
    topLay->addWidget(new QLabel(QStringLiteral("Number of Columns")),1,0);
    topLay->addWidget(columnNumberSpin,1,1);
    topLay->addWidget(insertColumnButton,1,2);
    topLay->addWidget(removeColumnButton,1,3);

    QGridLayout *mainLay = new QGridLayout(&mainWid);
    mainLay->addLayout(topLay, 0, 0);
    mainLay->addWidget(baseView, 1, 0);
    mainWid.show();
    return a.exec();
}
