#include <HierarchyLevelProxyModel>
#include <QStandardItemModel>
#include <QTreeView>
#include <QGridLayout>
#include <QApplication>
#include <QHeaderView>
#include <QLabel>
#include <QSpinBox>

void fillModelData(QAbstractItemModel *model);
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget mainWid;
    mainWid.setWindowTitle(QStringLiteral("HierarchyLevelProxyModel Example"));
    QStandardItemModel *baseModel = new QStandardItemModel(&mainWid);
    // fill the model with example data
    fillModelData(baseModel);

    // Create the proxy and assign the source
    HierarchyLevelProxyModel *hierarchyLevelProxy = new HierarchyLevelProxyModel(&mainWid);
    hierarchyLevelProxy->setHierarchyLevel(1);
    hierarchyLevelProxy->setSourceModel(baseModel);

    // Create a view for the base model and a view for the proxy
    QTreeView *baseView = new QTreeView(&mainWid);
    baseView->header()->setSectionResizeMode(QHeaderView::Stretch);
    baseView->setModel(baseModel);
    baseView->setSelectionMode(QAbstractItemView::SingleSelection);

    QTreeView *proxyView = new QTreeView(&mainWid);
    proxyView->header()->setSectionResizeMode(QHeaderView::Stretch);
    proxyView->setModel(hierarchyLevelProxy);
    proxyView->expandAll();

    // create a spinbox to set the hierarchical level
    QSpinBox *levelSpin = new QSpinBox(&mainWid);
    levelSpin->setValue(1);
    QObject::connect(levelSpin, &QSpinBox::valueChanged, hierarchyLevelProxy, &HierarchyLevelProxyModel::setHierarchyLevel);


    // Lay out the widget
    QGridLayout *mainLay = new QGridLayout(&mainWid);
    mainLay->addWidget(baseView, 0, 0, 2, 1);
    mainLay->addWidget(new QLabel(QStringLiteral("Level"), &mainWid), 0, 1);
    mainLay->addWidget(levelSpin, 0, 2);
    mainLay->addWidget(proxyView, 1, 1, 1, 2);
    mainWid.show();
    return a.exec();
}

void fillModelData(QAbstractItemModel *model)
{
    // source Wikipedia April 2021
    model->insertColumns(0, 2);
    model->insertRows(0, 6);
    model->setHeaderData(0, Qt::Horizontal, QStringLiteral("Location"));
    model->setHeaderData(1, Qt::Horizontal, QStringLiteral("Population"));
    model->setData(model->index(0, 0), QStringLiteral("Asia"));
    model->setData(model->index(0, 1), QVariant::fromValue<double>(4581757408.0));
    model->setData(model->index(1, 0), QStringLiteral("Africa"));
    model->setData(model->index(1, 1), QVariant::fromValue<double>(1216130000.0));
    model->setData(model->index(2, 0), QStringLiteral("Europe"));
    model->setData(model->index(2, 1), QVariant::fromValue<double>(738849000.0));
    model->setData(model->index(3, 0), QStringLiteral("North America"));
    model->setData(model->index(3, 1), QVariant::fromValue<double>(579024000.0));
    model->setData(model->index(4, 0), QStringLiteral("South America"));
    model->setData(model->index(4, 1), QVariant::fromValue<double>(422535000.0));
    model->setData(model->index(5, 0), QStringLiteral("Oceania"));
    model->setData(model->index(5, 1), QVariant::fromValue<double>(38304000.0));

    model->insertColumns(0, 2, model->index(0, 0));
    model->insertRows(0, 3, model->index(0, 0));
    model->setData(model->index(0, 0, model->index(0, 0)), QStringLiteral("China"));
    model->setData(model->index(0, 1, model->index(0, 0)), QVariant::fromValue<double>(1387160730.0));
    model->setData(model->index(1, 0, model->index(0, 0)), QStringLiteral("India"));
    model->setData(model->index(1, 1, model->index(0, 0)), QVariant::fromValue<double>(1324009090.0));
    model->setData(model->index(2, 0, model->index(0, 0)), QStringLiteral("Indonesia"));
    model->setData(model->index(2, 1, model->index(0, 0)), QVariant::fromValue<double>(255462000.0));
    model->insertColumns(0, 2, model->index(1, 0));
    model->insertRows(0, 3, model->index(1, 0));
    model->setData(model->index(0, 0, model->index(1, 0)), QStringLiteral("Nigeria"));
    model->setData(model->index(0, 1, model->index(1, 0)), QVariant::fromValue<double>(206139589.0));
    model->setData(model->index(1, 0, model->index(1, 0)), QStringLiteral("Ethiopia"));
    model->setData(model->index(1, 1, model->index(1, 0)), QVariant::fromValue<double>(109224414.0));
    model->setData(model->index(2, 0, model->index(1, 0)), QStringLiteral("Democratic Republic of the Congo"));
    model->setData(model->index(2, 1, model->index(1, 0)), QVariant::fromValue<double>(102561403.0));
    model->insertColumns(0, 2, model->index(2, 0));
    model->insertRows(0, 3, model->index(2, 0));
    model->setData(model->index(0, 0, model->index(2, 0)), QStringLiteral("Russia"));
    model->setData(model->index(0, 1, model->index(2, 0)), QVariant::fromValue<double>(146121379.0));
    model->setData(model->index(1, 0, model->index(2, 0)), QStringLiteral("Turkey"));
    model->setData(model->index(1, 1, model->index(2, 0)), QVariant::fromValue<double>(85427222.0));
    model->setData(model->index(2, 0, model->index(2, 0)), QStringLiteral("Germany"));
    model->setData(model->index(2, 1, model->index(2, 0)), QVariant::fromValue<double>(84228453.0));
    model->insertColumns(0, 2, model->index(3, 0));
    model->insertRows(0, 3, model->index(3, 0));
    model->setData(model->index(0, 0, model->index(3, 0)), QStringLiteral("United States"));
    model->setData(model->index(0, 1, model->index(3, 0)), QVariant::fromValue<double>(332574578.0));
    model->setData(model->index(1, 0, model->index(3, 0)), QStringLiteral("Mexico"));
    model->setData(model->index(1, 1, model->index(3, 0)), QVariant::fromValue<double>(130061071.0));
    model->setData(model->index(2, 0, model->index(3, 0)), QStringLiteral("Canada"));
    model->setData(model->index(2, 1, model->index(3, 0)), QVariant::fromValue<double>(38013451.0));
    model->insertColumns(0, 2, model->index(4, 0));
    model->insertRows(0, 3, model->index(4, 0));
    model->setData(model->index(0, 0, model->index(4, 0)), QStringLiteral("Brazil"));
    model->setData(model->index(0, 1, model->index(4, 0)), QVariant::fromValue<double>(213872592.0));
    model->setData(model->index(1, 0, model->index(4, 0)), QStringLiteral("Colombia"));
    model->setData(model->index(1, 1, model->index(4, 0)), QVariant::fromValue<double>(51561617.0));
    model->setData(model->index(2, 0, model->index(4, 0)), QStringLiteral("Argentina"));
    model->setData(model->index(2, 1, model->index(4, 0)), QVariant::fromValue<double>(45532143.0));
    model->insertColumns(0, 2, model->index(5, 0));
    model->insertRows(0, 3, model->index(5, 0));
    model->setData(model->index(0, 0, model->index(5, 0)), QStringLiteral("Australia"));
    model->setData(model->index(0, 1, model->index(5, 0)), QVariant::fromValue<double>(25710853.0));
    model->setData(model->index(1, 0, model->index(5, 0)), QStringLiteral("Papua New Guinea"));
    model->setData(model->index(1, 1, model->index(5, 0)), QVariant::fromValue<double>(8558800.0));
    model->setData(model->index(2, 0, model->index(5, 0)), QStringLiteral("New Zealand"));
    model->setData(model->index(2, 1, model->index(5, 0)), QVariant::fromValue<double>(4795886.0));

    model->insertColumns(0, 2, model->index(0, 0, model->index(0, 0)));
    model->insertRows(0, 3, model->index(0, 0, model->index(0, 0)));
    model->setData(model->index(0, 0, model->index(0, 0, model->index(0, 0))), QStringLiteral("Shanghai"));
    model->setData(model->index(0, 1, model->index(0, 0, model->index(0, 0))), QVariant::fromValue<double>(26917322.0));
    model->setData(model->index(1, 0, model->index(0, 0, model->index(0, 0))), QStringLiteral("Beijing"));
    model->setData(model->index(1, 1, model->index(0, 0, model->index(0, 0))), QVariant::fromValue<double>(20381745.0));
    model->setData(model->index(2, 0, model->index(0, 0, model->index(0, 0))), QStringLiteral("Chongqing"));
    model->setData(model->index(2, 1, model->index(0, 0, model->index(0, 0))), QVariant::fromValue<double>(15773658.0));
    model->insertColumns(0, 2, model->index(1, 0, model->index(0, 0)));
    model->insertRows(0, 3, model->index(1, 0, model->index(0, 0)));
    model->setData(model->index(0, 0, model->index(1, 0, model->index(0, 0))), QStringLiteral("Mumbai"));
    model->setData(model->index(0, 1, model->index(1, 0, model->index(0, 0))), QVariant::fromValue<double>(12442373.0));
    model->setData(model->index(1, 0, model->index(1, 0, model->index(0, 0))), QStringLiteral("Delhi"));
    model->setData(model->index(1, 1, model->index(1, 0, model->index(0, 0))), QVariant::fromValue<double>(11007835.0));
    model->setData(model->index(2, 0, model->index(1, 0, model->index(0, 0))), QStringLiteral("Bangalore"));
    model->setData(model->index(2, 1, model->index(1, 0, model->index(0, 0))), QVariant::fromValue<double>(8436675.0));
    model->insertColumns(0, 2, model->index(2, 0, model->index(0, 0)));
    model->insertRows(0, 3, model->index(2, 0, model->index(0, 0)));
    model->setData(model->index(0, 0, model->index(2, 0, model->index(0, 0))), QStringLiteral("Surabaya"));
    model->setData(model->index(0, 1, model->index(2, 0, model->index(0, 0))), QVariant::fromValue<double>(2847480.0));
    model->setData(model->index(1, 0, model->index(2, 0, model->index(0, 0))), QStringLiteral("East Jakarta"));
    model->setData(model->index(1, 1, model->index(2, 0, model->index(0, 0))), QVariant::fromValue<double>(2826651.0));
    model->setData(model->index(2, 0, model->index(2, 0, model->index(0, 0))), QStringLiteral("Bekasi"));
    model->setData(model->index(2, 1, model->index(2, 0, model->index(0, 0))), QVariant::fromValue<double>(2708721.0));
    model->insertColumns(0, 2, model->index(0, 0, model->index(1, 0)));
    model->insertRows(0, 3, model->index(0, 0, model->index(1, 0)));
    model->setData(model->index(0, 0, model->index(0, 0, model->index(1, 0))), QStringLiteral("Lagos"));
    model->setData(model->index(0, 1, model->index(0, 0, model->index(1, 0))), QVariant::fromValue<double>(8048430.0));
    model->setData(model->index(1, 0, model->index(0, 0, model->index(1, 0))), QStringLiteral("Kano"));
    model->setData(model->index(1, 1, model->index(0, 0, model->index(1, 0))), QVariant::fromValue<double>(2828861.0));
    model->setData(model->index(2, 0, model->index(0, 0, model->index(1, 0))), QStringLiteral("Ibadan"));
    model->setData(model->index(2, 1, model->index(0, 0, model->index(1, 0))), QVariant::fromValue<double>(2559853.0));
    model->insertColumns(0, 2, model->index(1, 0, model->index(1, 0)));
    model->insertRows(0, 3, model->index(1, 0, model->index(1, 0)));
    model->setData(model->index(0, 0, model->index(1, 0, model->index(1, 0))), QStringLiteral("Addis Ababa"));
    model->setData(model->index(0, 1, model->index(1, 0, model->index(1, 0))), QVariant::fromValue<double>(3352000.0));
    model->setData(model->index(1, 0, model->index(1, 0, model->index(1, 0))), QStringLiteral("Bahir Dar"));
    model->setData(model->index(1, 1, model->index(1, 0, model->index(1, 0))), QVariant::fromValue<double>(797794.0));
    model->setData(model->index(2, 0, model->index(1, 0, model->index(1, 0))), QStringLiteral("Dessie"));
    model->setData(model->index(2, 1, model->index(1, 0, model->index(1, 0))), QVariant::fromValue<double>(610431.0));
    model->insertColumns(0, 2, model->index(2, 0, model->index(1, 0)));
    model->insertRows(0, 3, model->index(2, 0, model->index(1, 0)));
    model->setData(model->index(0, 0, model->index(2, 0, model->index(1, 0))), QStringLiteral("Kinshasa"));
    model->setData(model->index(0, 1, model->index(2, 0, model->index(1, 0))), QVariant::fromValue<double>(8900721.0));
    model->setData(model->index(1, 0, model->index(2, 0, model->index(1, 0))), QStringLiteral("Lubumbashi"));
    model->setData(model->index(1, 1, model->index(2, 0, model->index(1, 0))), QVariant::fromValue<double>(1630186.0));
    model->setData(model->index(2, 0, model->index(2, 0, model->index(1, 0))), QStringLiteral("Mbuji-Mayi"));
    model->setData(model->index(2, 1, model->index(2, 0, model->index(1, 0))), QVariant::fromValue<double>(1559073.0));
    model->insertColumns(0, 2, model->index(0, 0, model->index(2, 0)));
    model->insertRows(0, 3, model->index(0, 0, model->index(2, 0)));
    model->setData(model->index(0, 0, model->index(0, 0, model->index(2, 0))), QStringLiteral("Moscow"));
    model->setData(model->index(0, 1, model->index(0, 0, model->index(2, 0))), QVariant::fromValue<double>(12480481.0));
    model->setData(model->index(1, 0, model->index(0, 0, model->index(2, 0))), QStringLiteral("Saint Petersburg"));
    model->setData(model->index(1, 1, model->index(0, 0, model->index(2, 0))), QVariant::fromValue<double>(5398064.0));
    model->setData(model->index(2, 0, model->index(0, 0, model->index(2, 0))), QStringLiteral("Novosibirsk"));
    model->setData(model->index(2, 1, model->index(0, 0, model->index(2, 0))), QVariant::fromValue<double>(1625631.0));
    model->insertColumns(0, 2, model->index(1, 0, model->index(2, 0)));
    model->insertRows(0, 3, model->index(1, 0, model->index(2, 0)));
    model->setData(model->index(0, 0, model->index(1, 0, model->index(2, 0))), QStringLiteral("Istanbul"));
    model->setData(model->index(0, 1, model->index(1, 0, model->index(2, 0))), QVariant::fromValue<double>(14025646.0));
    model->setData(model->index(1, 0, model->index(1, 0, model->index(2, 0))), QStringLiteral("Ankara"));
    model->setData(model->index(1, 1, model->index(1, 0, model->index(2, 0))), QVariant::fromValue<double>(4587558.0));
    model->setData(model->index(2, 0, model->index(1, 0, model->index(2, 0))), QStringLiteral("Izmir"));
    model->setData(model->index(2, 1, model->index(1, 0, model->index(2, 0))), QVariant::fromValue<double>(2847691.0));
    model->insertColumns(0, 2, model->index(2, 0, model->index(2, 0)));
    model->insertRows(0, 3, model->index(2, 0, model->index(2, 0)));
    model->setData(model->index(0, 0, model->index(2, 0, model->index(2, 0))), QStringLiteral("Berlin"));
    model->setData(model->index(0, 1, model->index(2, 0, model->index(2, 0))), QVariant::fromValue<double>(3520031.0));
    model->setData(model->index(1, 0, model->index(2, 0, model->index(2, 0))), QStringLiteral("Hamburg"));
    model->setData(model->index(1, 1, model->index(2, 0, model->index(2, 0))), QVariant::fromValue<double>(1787408.0));
    model->setData(model->index(2, 0, model->index(2, 0, model->index(2, 0))), QStringLiteral("Munich"));
    model->setData(model->index(2, 1, model->index(2, 0, model->index(2, 0))), QVariant::fromValue<double>(1450381.0));
    model->insertColumns(0, 2, model->index(0, 0, model->index(3, 0)));
    model->insertRows(0, 3, model->index(0, 0, model->index(3, 0)));
    model->setData(model->index(0, 0, model->index(0, 0, model->index(3, 0))), QStringLiteral("New York City"));
    model->setData(model->index(0, 1, model->index(0, 0, model->index(3, 0))), QVariant::fromValue<double>(8336817.0));
    model->setData(model->index(1, 0, model->index(0, 0, model->index(3, 0))), QStringLiteral("Los Angeles"));
    model->setData(model->index(1, 1, model->index(0, 0, model->index(3, 0))), QVariant::fromValue<double>(3979576.0));
    model->setData(model->index(2, 0, model->index(0, 0, model->index(3, 0))), QStringLiteral("Chicago"));
    model->setData(model->index(2, 1, model->index(0, 0, model->index(3, 0))), QVariant::fromValue<double>(2693976.0));
    model->insertColumns(0, 2, model->index(1, 0, model->index(3, 0)));
    model->insertRows(0, 3, model->index(1, 0, model->index(3, 0)));
    model->setData(model->index(0, 0, model->index(1, 0, model->index(3, 0))), QStringLiteral("Mexico City"));
    model->setData(model->index(0, 1, model->index(1, 0, model->index(3, 0))), QVariant::fromValue<double>(9209944.0));
    model->setData(model->index(1, 0, model->index(1, 0, model->index(3, 0))), QStringLiteral("Tijuana"));
    model->setData(model->index(1, 1, model->index(1, 0, model->index(3, 0))), QVariant::fromValue<double>(1810645.0));
    model->setData(model->index(2, 0, model->index(1, 0, model->index(3, 0))), QStringLiteral("Ecatepec"));
    model->setData(model->index(2, 1, model->index(1, 0, model->index(3, 0))), QVariant::fromValue<double>(1643623.0));
    model->insertColumns(0, 2, model->index(2, 0, model->index(3, 0)));
    model->insertRows(0, 3, model->index(2, 0, model->index(3, 0)));
    model->setData(model->index(0, 0, model->index(2, 0, model->index(3, 0))), QStringLiteral("Toronto"));
    model->setData(model->index(0, 1, model->index(2, 0, model->index(3, 0))), QVariant::fromValue<double>(2731571.0));
    model->setData(model->index(1, 0, model->index(2, 0, model->index(3, 0))), QStringLiteral("Montreal"));
    model->setData(model->index(1, 1, model->index(2, 0, model->index(3, 0))), QVariant::fromValue<double>(1704694.0));
    model->setData(model->index(2, 0, model->index(2, 0, model->index(3, 0))), QStringLiteral("Calgary"));
    model->setData(model->index(2, 1, model->index(2, 0, model->index(3, 0))), QVariant::fromValue<double>(1239220.0));
    model->insertColumns(0, 2, model->index(0, 0, model->index(4, 0)));
    model->insertRows(0, 3, model->index(0, 0, model->index(4, 0)));
    model->setData(model->index(0, 0, model->index(0, 0, model->index(4, 0))), QStringLiteral("Sao Paulo"));
    model->setData(model->index(0, 1, model->index(0, 0, model->index(4, 0))), QVariant::fromValue<double>(12325232.0));
    model->setData(model->index(1, 0, model->index(0, 0, model->index(4, 0))), QStringLiteral("Rio de Janeiro"));
    model->setData(model->index(1, 1, model->index(0, 0, model->index(4, 0))), QVariant::fromValue<double>(6747815.0));
    model->setData(model->index(2, 0, model->index(0, 0, model->index(4, 0))), QStringLiteral("Brasilia"));
    model->setData(model->index(2, 1, model->index(0, 0, model->index(4, 0))), QVariant::fromValue<double>(3055149.0));
    model->insertColumns(0, 2, model->index(1, 0, model->index(4, 0)));
    model->insertRows(0, 3, model->index(1, 0, model->index(4, 0)));
    model->setData(model->index(0, 0, model->index(1, 0, model->index(4, 0))), QStringLiteral("Bogota"));
    model->setData(model->index(0, 1, model->index(1, 0, model->index(4, 0))), QVariant::fromValue<double>(8181047.0));
    model->setData(model->index(1, 0, model->index(1, 0, model->index(4, 0))), QStringLiteral("Medellin"));
    model->setData(model->index(1, 1, model->index(1, 0, model->index(4, 0))), QVariant::fromValue<double>(2529403.0));
    model->setData(model->index(2, 0, model->index(1, 0, model->index(4, 0))), QStringLiteral("Cali"));
    model->setData(model->index(2, 1, model->index(1, 0, model->index(4, 0))), QVariant::fromValue<double>(2445405.0));
    model->insertColumns(0, 2, model->index(2, 0, model->index(4, 0)));
    model->insertRows(0, 3, model->index(2, 0, model->index(4, 0)));
    model->setData(model->index(0, 0, model->index(2, 0, model->index(4, 0))), QStringLiteral("Buenos Aires"));
    model->setData(model->index(0, 1, model->index(2, 0, model->index(4, 0))), QVariant::fromValue<double>(2936877.0));
    model->setData(model->index(1, 0, model->index(2, 0, model->index(4, 0))), QStringLiteral("Cordoba"));
    model->setData(model->index(1, 1, model->index(2, 0, model->index(4, 0))), QVariant::fromValue<double>(1317298.0));
    model->setData(model->index(2, 0, model->index(2, 0, model->index(4, 0))), QStringLiteral("Rosario"));
    model->setData(model->index(2, 1, model->index(2, 0, model->index(4, 0))), QVariant::fromValue<double>(1193605.0));
    model->insertColumns(0, 2, model->index(0, 0, model->index(5, 0)));
    model->insertRows(0, 3, model->index(0, 0, model->index(5, 0)));
    model->setData(model->index(0, 0, model->index(0, 0, model->index(5, 0))), QStringLiteral("Melbourne"));
    model->setData(model->index(0, 1, model->index(0, 0, model->index(5, 0))), QVariant::fromValue<double>(4969305.0));
    model->setData(model->index(1, 0, model->index(0, 0, model->index(5, 0))), QStringLiteral("Sydney"));
    model->setData(model->index(1, 1, model->index(0, 0, model->index(5, 0))), QVariant::fromValue<double>(4966806.0));
    model->setData(model->index(2, 0, model->index(0, 0, model->index(5, 0))), QStringLiteral("Brisbane"));
    model->setData(model->index(2, 1, model->index(0, 0, model->index(5, 0))), QVariant::fromValue<double>(2475680.0));
    model->insertColumns(0, 2, model->index(1, 0, model->index(5, 0)));
    model->insertRows(0, 3, model->index(1, 0, model->index(5, 0)));
    model->setData(model->index(0, 0, model->index(1, 0, model->index(5, 0))), QStringLiteral("Port Moresby"));
    model->setData(model->index(0, 1, model->index(1, 0, model->index(5, 0))), QVariant::fromValue<double>(254158.0));
    model->setData(model->index(1, 0, model->index(1, 0, model->index(5, 0))), QStringLiteral("Lae"));
    model->setData(model->index(1, 1, model->index(1, 0, model->index(5, 0))), QVariant::fromValue<double>(100677.0));
    model->setData(model->index(2, 0, model->index(1, 0, model->index(5, 0))), QStringLiteral("Arawa"));
    model->setData(model->index(2, 1, model->index(1, 0, model->index(5, 0))), QVariant::fromValue<double>(36443.0));
    model->insertColumns(0, 2, model->index(2, 0, model->index(5, 0)));
    model->insertRows(0, 3, model->index(2, 0, model->index(5, 0)));
    model->setData(model->index(0, 0, model->index(2, 0, model->index(5, 0))), QStringLiteral("Auckland"));
    model->setData(model->index(0, 1, model->index(2, 0, model->index(5, 0))), QVariant::fromValue<double>(1470100.0));
    model->setData(model->index(1, 0, model->index(2, 0, model->index(5, 0))), QStringLiteral("Christchurch"));
    model->setData(model->index(1, 1, model->index(2, 0, model->index(5, 0))), QVariant::fromValue<double>(383200.0));
    model->setData(model->index(2, 0, model->index(2, 0, model->index(5, 0))), QStringLiteral("Wellington"));
    model->setData(model->index(2, 1, model->index(2, 0, model->index(5, 0))), QVariant::fromValue<double>(215100.0));
}
