#include <RootIndexProxyModel>
#include <QStandardItemModel>
#include <QTreeView>
#include <QGridLayout>
#include <QApplication>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QLineEdit>

void fillModelData(QAbstractItemModel* model);
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget mainWid;
    mainWid.setWindowTitle(QStringLiteral("RootIndexProxyModel Example"));
    QStandardItemModel *baseModel = new QStandardItemModel(&mainWid);
    // fill the model with example data
    fillModelData(baseModel);

    // Create the proxy and assign the source
    RootIndexProxyModel *rootIndexProxy = new RootIndexProxyModel(&mainWid);
    rootIndexProxy->setSourceModel(baseModel);

    // Create the proxy to filter the other proxy
    QSortFilterProxyModel* filtermodel=new QSortFilterProxyModel(&mainWid);
    filtermodel->setSourceModel(rootIndexProxy);
    filtermodel->setSortCaseSensitivity(Qt::CaseInsensitive);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    filtermodel->setRecursiveFilteringEnabled(true);
#endif
    
    // Create a view for the base model and a view for the proxy
    QTreeView *baseView = new QTreeView(&mainWid);
    baseView->header()->setSectionResizeMode(QHeaderView::Stretch);
    baseView->setModel(baseModel);
    baseView->setSelectionMode(QAbstractItemView::SingleSelection);

    QTreeView *proxyView = new QTreeView(&mainWid);
    proxyView->header()->setSectionResizeMode(QHeaderView::Stretch);
    proxyView->setModel(filtermodel);
    proxyView->expandAll();

    // Make so when an item is clicked the children get shown in the proxy view
    QObject::connect(baseView,&QAbstractItemView::clicked,rootIndexProxy,&RootIndexProxyModel::setRootIndex);
    QObject::connect(baseView,&QAbstractItemView::clicked,proxyView,&QTreeView::expandAll);

    // create a linedit to allow filtering the model
    QLineEdit* searchEdit = new QLineEdit(&mainWid);
    QObject::connect(searchEdit,&QLineEdit::textChanged,filtermodel,&QSortFilterProxyModel::setFilterFixedString);
    QObject::connect(searchEdit,&QLineEdit::textChanged,proxyView,&QTreeView::expandAll);

    // Lay out the widget
    QGridLayout *mainLay = new QGridLayout(&mainWid);
    mainLay->addWidget(baseView, 0, 0,2,1);
    mainLay->addWidget(new QLabel(QStringLiteral("Search"),&mainWid), 0, 1);
    mainLay->addWidget(searchEdit, 0, 2);
    mainLay->addWidget(proxyView, 1, 1,1,2);
    mainWid.show();
    return a.exec();
}


void fillModelData(QAbstractItemModel* model){
    // source Wikipedia April 2021
    model->insertColumns(0, 2);
    model->insertRows(0, 6);
    model->setHeaderData(0,Qt::Horizontal,QStringLiteral("Location"));
    model->setHeaderData(1,Qt::Horizontal,QStringLiteral("Population"));
    model->setData(model->index(0,0),QStringLiteral("Asia"));
    model->setData(model->index(0,1),4581757408);
    model->setData(model->index(1,0),QStringLiteral("Africa"));
    model->setData(model->index(1,1),1216130000);
    model->setData(model->index(2,0),QStringLiteral("Europe"));
    model->setData(model->index(2,1),738849000);
    model->setData(model->index(3,0),QStringLiteral("North America"));
    model->setData(model->index(3,1),579024000);
    model->setData(model->index(4,0),QStringLiteral("South America"));
    model->setData(model->index(4,1),422535000);
    model->setData(model->index(5,0),QStringLiteral("Oceania"));
    model->setData(model->index(5,1),38304000);

    model->insertColumns(0, 2,model->index(0,0));
    model->insertRows(0, 3,model->index(0,0));
    model->setData(model->index(0,0,model->index(0,0)),QStringLiteral("China"));
    model->setData(model->index(0,1,model->index(0,0)),1387160730);
    model->setData(model->index(1,0,model->index(0,0)),QStringLiteral("India"));
    model->setData(model->index(1,1,model->index(0,0)),1324009090);
    model->setData(model->index(2,0,model->index(0,0)),QStringLiteral("Indonesia"));
    model->setData(model->index(2,1,model->index(0,0)),255462000);
    model->insertColumns(0, 2,model->index(1,0));
    model->insertRows(0, 3,model->index(1,0));
    model->setData(model->index(0,0,model->index(1,0)),QStringLiteral("Nigeria"));
    model->setData(model->index(0,1,model->index(1,0)),206139589);
    model->setData(model->index(1,0,model->index(1,0)),QStringLiteral("Ethiopia"));
    model->setData(model->index(1,1,model->index(1,0)),109224414);
    model->setData(model->index(2,0,model->index(1,0)),QStringLiteral("Democratic Republic of the Congo"));
    model->setData(model->index(2,1,model->index(1,0)),102561403);
    model->insertColumns(0, 2,model->index(2,0));
    model->insertRows(0, 3,model->index(2,0));
    model->setData(model->index(0,0,model->index(2,0)),QStringLiteral("Russia"));
    model->setData(model->index(0,1,model->index(2,0)),146121379);
    model->setData(model->index(1,0,model->index(2,0)),QStringLiteral("Turkey"));
    model->setData(model->index(1,1,model->index(2,0)),85427222);
    model->setData(model->index(2,0,model->index(2,0)),QStringLiteral("Germany"));
    model->setData(model->index(2,1,model->index(2,0)),84228453);
    model->insertColumns(0, 2,model->index(3,0));
    model->insertRows(0, 3,model->index(3,0));
    model->setData(model->index(0,0,model->index(3,0)),QStringLiteral("United States"));
    model->setData(model->index(0,1,model->index(3,0)),332574578);
    model->setData(model->index(1,0,model->index(3,0)),QStringLiteral("Mexico"));
    model->setData(model->index(1,1,model->index(3,0)),130061071);
    model->setData(model->index(2,0,model->index(3,0)),QStringLiteral("Canada"));
    model->setData(model->index(2,1,model->index(3,0)),38013451);
    model->insertColumns(0, 2,model->index(4,0));
    model->insertRows(0, 3,model->index(4,0));
    model->setData(model->index(0,0,model->index(4,0)),QStringLiteral("Brazil"));
    model->setData(model->index(0,1,model->index(4,0)),213872592);
    model->setData(model->index(1,0,model->index(4,0)),QStringLiteral("Colombia"));
    model->setData(model->index(1,1,model->index(4,0)),51561617);
    model->setData(model->index(2,0,model->index(4,0)),QStringLiteral("Argentina"));
    model->setData(model->index(2,1,model->index(4,0)),45532143);
    model->insertColumns(0, 2,model->index(5,0));
    model->insertRows(0, 3,model->index(5,0));
    model->setData(model->index(0,0,model->index(5,0)),QStringLiteral("Australia"));
    model->setData(model->index(0,1,model->index(5,0)),25710853);
    model->setData(model->index(1,0,model->index(5,0)),QStringLiteral("Papua New Guinea"));
    model->setData(model->index(1,1,model->index(5,0)), 8558800);
    model->setData(model->index(2,0,model->index(5,0)),QStringLiteral("New Zealand"));
    model->setData(model->index(2,1,model->index(5,0)),4795886);

    model->insertColumns(0, 2,model->index(0,0,model->index(0,0)));
    model->insertRows(0, 3,model->index(0,0,model->index(0,0)));
    model->setData(model->index(0,0,model->index(0,0,model->index(0,0))),QStringLiteral("Shanghai"));
    model->setData(model->index(0,1,model->index(0,0,model->index(0,0))),26917322);
    model->setData(model->index(1,0,model->index(0,0,model->index(0,0))),QStringLiteral("Beijing"));
    model->setData(model->index(1,1,model->index(0,0,model->index(0,0))),20381745 );
    model->setData(model->index(2,0,model->index(0,0,model->index(0,0))),QStringLiteral("Chongqing"));
    model->setData(model->index(2,1,model->index(0,0,model->index(0,0))),15773658);
    model->insertColumns(0, 2,model->index(1,0,model->index(0,0)));
    model->insertRows(0, 3,model->index(1,0,model->index(0,0)));
    model->setData(model->index(0,0,model->index(1,0,model->index(0,0))),QStringLiteral("Mumbai"));
    model->setData(model->index(0,1,model->index(1,0,model->index(0,0))),12442373);
    model->setData(model->index(1,0,model->index(1,0,model->index(0,0))),QStringLiteral("Delhi"));
    model->setData(model->index(1,1,model->index(1,0,model->index(0,0))),11007835 );
    model->setData(model->index(2,0,model->index(1,0,model->index(0,0))),QStringLiteral("Bangalore"));
    model->setData(model->index(2,1,model->index(1,0,model->index(0,0))),8436675);
    model->insertColumns(0, 2,model->index(2,0,model->index(0,0)));
    model->insertRows(0, 3,model->index(2,0,model->index(0,0)));
    model->setData(model->index(0,0,model->index(2,0,model->index(0,0))),QStringLiteral("Surabaya"));
    model->setData(model->index(0,1,model->index(2,0,model->index(0,0))),2847480 );
    model->setData(model->index(1,0,model->index(2,0,model->index(0,0))),QStringLiteral("East Jakarta"));
    model->setData(model->index(1,1,model->index(2,0,model->index(0,0))),2826651  );
    model->setData(model->index(2,0,model->index(2,0,model->index(0,0))),QStringLiteral("Bekasi"));
    model->setData(model->index(2,1,model->index(2,0,model->index(0,0))),2708721 );
    model->insertColumns(0, 2,model->index(0,0,model->index(1,0)));
    model->insertRows(0, 3,model->index(0,0,model->index(1,0)));
    model->setData(model->index(0,0,model->index(0,0,model->index(1,0))),QStringLiteral("Lagos"));
    model->setData(model->index(0,1,model->index(0,0,model->index(1,0))),8048430 );
    model->setData(model->index(1,0,model->index(0,0,model->index(1,0))),QStringLiteral("Kano"));
    model->setData(model->index(1,1,model->index(0,0,model->index(1,0))),2828861  );
    model->setData(model->index(2,0,model->index(0,0,model->index(1,0))),QStringLiteral("Ibadan"));
    model->setData(model->index(2,1,model->index(0,0,model->index(1,0))),2559853 );
    model->insertColumns(0, 2,model->index(1,0,model->index(1,0)));
    model->insertRows(0, 3,model->index(1,0,model->index(1,0)));
    model->setData(model->index(0,0,model->index(1,0,model->index(1,0))),QStringLiteral("Addis Ababa"));
    model->setData(model->index(0,1,model->index(1,0,model->index(1,0))),3352000);
    model->setData(model->index(1,0,model->index(1,0,model->index(1,0))),QStringLiteral("Bahir Dar"));
    model->setData(model->index(1,1,model->index(1,0,model->index(1,0))),797794 );
    model->setData(model->index(2,0,model->index(1,0,model->index(1,0))),QStringLiteral("Dessie"));
    model->setData(model->index(2,1,model->index(1,0,model->index(1,0))),610431);
    model->insertColumns(0, 2,model->index(2,0,model->index(1,0)));
    model->insertRows(0, 3,model->index(2,0,model->index(1,0)));
    model->setData(model->index(0,0,model->index(2,0,model->index(1,0))),QStringLiteral("Kinshasa"));
    model->setData(model->index(0,1,model->index(2,0,model->index(1,0))),8900721 );
    model->setData(model->index(1,0,model->index(2,0,model->index(1,0))),QStringLiteral("Lubumbashi"));
    model->setData(model->index(1,1,model->index(2,0,model->index(1,0))),1630186  );
    model->setData(model->index(2,0,model->index(2,0,model->index(1,0))),QStringLiteral("Mbuji-Mayi"));
    model->setData(model->index(2,1,model->index(2,0,model->index(1,0))),1559073 );
    model->insertColumns(0, 2,model->index(0,0,model->index(2,0)));
    model->insertRows(0, 3,model->index(0,0,model->index(2,0)));
    model->setData(model->index(0,0,model->index(0,0,model->index(2,0))),QStringLiteral("Moscow"));
    model->setData(model->index(0,1,model->index(0,0,model->index(2,0))),12480481 );
    model->setData(model->index(1,0,model->index(0,0,model->index(2,0))),QStringLiteral("Saint Petersburg"));
    model->setData(model->index(1,1,model->index(0,0,model->index(2,0))),5398064  );
    model->setData(model->index(2,0,model->index(0,0,model->index(2,0))),QStringLiteral("Novosibirsk"));
    model->setData(model->index(2,1,model->index(0,0,model->index(2,0))),1625631 );
    model->insertColumns(0, 2,model->index(1,0,model->index(2,0)));
    model->insertRows(0, 3,model->index(1,0,model->index(2,0)));
    model->setData(model->index(0,0,model->index(1,0,model->index(2,0))),QStringLiteral("Istanbul"));
    model->setData(model->index(0,1,model->index(1,0,model->index(2,0))),14025646);
    model->setData(model->index(1,0,model->index(1,0,model->index(2,0))),QStringLiteral("Ankara"));
    model->setData(model->index(1,1,model->index(1,0,model->index(2,0))),4587558 );
    model->setData(model->index(2,0,model->index(1,0,model->index(2,0))),QStringLiteral("Izmir"));
    model->setData(model->index(2,1,model->index(1,0,model->index(2,0))),2847691);
    model->insertColumns(0, 2,model->index(2,0,model->index(2,0)));
    model->insertRows(0, 3,model->index(2,0,model->index(2,0)));
    model->setData(model->index(0,0,model->index(2,0,model->index(2,0))),QStringLiteral("Berlin"));
    model->setData(model->index(0,1,model->index(2,0,model->index(2,0))),3520031 );
    model->setData(model->index(1,0,model->index(2,0,model->index(2,0))),QStringLiteral("Hamburg"));
    model->setData(model->index(1,1,model->index(2,0,model->index(2,0))),1787408  );
    model->setData(model->index(2,0,model->index(2,0,model->index(2,0))),QStringLiteral("Munich"));
    model->setData(model->index(2,1,model->index(2,0,model->index(2,0))),1450381 );
    model->insertColumns(0, 2,model->index(0,0,model->index(3,0)));
    model->insertRows(0, 3,model->index(0,0,model->index(3,0)));
    model->setData(model->index(0,0,model->index(0,0,model->index(3,0))),QStringLiteral("New York City"));
    model->setData(model->index(0,1,model->index(0,0,model->index(3,0))),8336817 );
    model->setData(model->index(1,0,model->index(0,0,model->index(3,0))),QStringLiteral("Los Angeles"));
    model->setData(model->index(1,1,model->index(0,0,model->index(3,0))),3979576  );
    model->setData(model->index(2,0,model->index(0,0,model->index(3,0))),QStringLiteral("Chicago"));
    model->setData(model->index(2,1,model->index(0,0,model->index(3,0))),2693976 );
    model->insertColumns(0, 2,model->index(1,0,model->index(3,0)));
    model->insertRows(0, 3,model->index(1,0,model->index(3,0)));
    model->setData(model->index(0,0,model->index(1,0,model->index(3,0))),QStringLiteral("Mexico City"));
    model->setData(model->index(0,1,model->index(1,0,model->index(3,0))),9209944);
    model->setData(model->index(1,0,model->index(1,0,model->index(3,0))),QStringLiteral("Tijuana"));
    model->setData(model->index(1,1,model->index(1,0,model->index(3,0))),1810645 );
    model->setData(model->index(2,0,model->index(1,0,model->index(3,0))),QStringLiteral("Ecatepec"));
    model->setData(model->index(2,1,model->index(1,0,model->index(3,0))),1643623);
    model->insertColumns(0, 2,model->index(2,0,model->index(3,0)));
    model->insertRows(0, 3,model->index(2,0,model->index(3,0)));
    model->setData(model->index(0,0,model->index(2,0,model->index(3,0))),QStringLiteral("Toronto"));
    model->setData(model->index(0,1,model->index(2,0,model->index(3,0))),2731571 );
    model->setData(model->index(1,0,model->index(2,0,model->index(3,0))),QStringLiteral("Montreal"));
    model->setData(model->index(1,1,model->index(2,0,model->index(3,0))),1704694  );
    model->setData(model->index(2,0,model->index(2,0,model->index(3,0))),QStringLiteral("Calgary"));
    model->setData(model->index(2,1,model->index(2,0,model->index(3,0))),1239220 );
    model->insertColumns(0, 2,model->index(0,0,model->index(4,0)));
    model->insertRows(0, 3,model->index(0,0,model->index(4,0)));
    model->setData(model->index(0,0,model->index(0,0,model->index(4,0))),QStringLiteral("Sao Paulo"));
    model->setData(model->index(0,1,model->index(0,0,model->index(4,0))),12325232 );
    model->setData(model->index(1,0,model->index(0,0,model->index(4,0))),QStringLiteral("Rio de Janeiro"));
    model->setData(model->index(1,1,model->index(0,0,model->index(4,0))),6747815  );
    model->setData(model->index(2,0,model->index(0,0,model->index(4,0))),QStringLiteral("Brasilia"));
    model->setData(model->index(2,1,model->index(0,0,model->index(4,0))),3055149 );
    model->insertColumns(0, 2,model->index(1,0,model->index(4,0)));
    model->insertRows(0, 3,model->index(1,0,model->index(4,0)));
    model->setData(model->index(0,0,model->index(1,0,model->index(4,0))),QStringLiteral("Bogota"));
    model->setData(model->index(0,1,model->index(1,0,model->index(4,0))),8181047);
    model->setData(model->index(1,0,model->index(1,0,model->index(4,0))),QStringLiteral("Medellin"));
    model->setData(model->index(1,1,model->index(1,0,model->index(4,0))),2529403 );
    model->setData(model->index(2,0,model->index(1,0,model->index(4,0))),QStringLiteral("Cali"));
    model->setData(model->index(2,1,model->index(1,0,model->index(4,0))),2445405);
    model->insertColumns(0, 2,model->index(2,0,model->index(4,0)));
    model->insertRows(0, 3,model->index(2,0,model->index(4,0)));
    model->setData(model->index(0,0,model->index(2,0,model->index(4,0))),QStringLiteral("Buenos Aires"));
    model->setData(model->index(0,1,model->index(2,0,model->index(4,0))),2936877  );
    model->setData(model->index(1,0,model->index(2,0,model->index(4,0))),QStringLiteral("Cordoba"));
    model->setData(model->index(1,1,model->index(2,0,model->index(4,0))),1317298   );
    model->setData(model->index(2,0,model->index(2,0,model->index(4,0))),QStringLiteral("Rosario"));
    model->setData(model->index(2,1,model->index(2,0,model->index(4,0))),1193605  );
    model->insertColumns(0, 2,model->index(0,0,model->index(5,0)));
    model->insertRows(0, 3,model->index(0,0,model->index(5,0)));
    model->setData(model->index(0,0,model->index(0,0,model->index(5,0))),QStringLiteral("Melbourne"));
    model->setData(model->index(0,1,model->index(0,0,model->index(5,0))),4969305 );
    model->setData(model->index(1,0,model->index(0,0,model->index(5,0))),QStringLiteral("Sydney"));
    model->setData(model->index(1,1,model->index(0,0,model->index(5,0))),4966806  );
    model->setData(model->index(2,0,model->index(0,0,model->index(5,0))),QStringLiteral("Brisbane"));
    model->setData(model->index(2,1,model->index(0,0,model->index(5,0))),2475680 );
    model->insertColumns(0, 2,model->index(1,0,model->index(5,0)));
    model->insertRows(0, 3,model->index(1,0,model->index(5,0)));
    model->setData(model->index(0,0,model->index(1,0,model->index(5,0))),QStringLiteral("Port Moresby"));
    model->setData(model->index(0,1,model->index(1,0,model->index(5,0))),254158 );
    model->setData(model->index(1,0,model->index(1,0,model->index(5,0))),QStringLiteral("Lae"));
    model->setData(model->index(1,1,model->index(1,0,model->index(5,0))),100677  );
    model->setData(model->index(2,0,model->index(1,0,model->index(5,0))),QStringLiteral("Arawa"));
    model->setData(model->index(2,1,model->index(1,0,model->index(5,0))),36443 );
    model->insertColumns(0, 2,model->index(2,0,model->index(5,0)));
    model->insertRows(0, 3,model->index(2,0,model->index(5,0)));
    model->setData(model->index(0,0,model->index(2,0,model->index(5,0))),QStringLiteral("Auckland"));
    model->setData(model->index(0,1,model->index(2,0,model->index(5,0))),1470100 );
    model->setData(model->index(1,0,model->index(2,0,model->index(5,0))),QStringLiteral("Christchurch"));
    model->setData(model->index(1,1,model->index(2,0,model->index(5,0))),383200  );
    model->setData(model->index(2,0,model->index(2,0,model->index(5,0))),QStringLiteral("Wellington"));
    model->setData(model->index(2,1,model->index(2,0,model->index(5,0))),215100 );
}
