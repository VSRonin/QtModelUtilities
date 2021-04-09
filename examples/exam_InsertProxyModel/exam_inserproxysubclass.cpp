#include <InsertProxyModel>
#include <QStandardItemModel>
#include <QComboBox>
#include <QLabel>
#include <QApplication>
#include <QHeaderView>
#include <QGridLayout>
#include <QTableView>
#include <QHBoxLayout>

// subclass InsertProxyModel and reimplement validRow/validColumn so that it
// returns true only when every cell is not empty
class InsertWhenAllDone : public InsertProxyModel
{
    Q_DISABLE_COPY(InsertWhenAllDone)
public:
    explicit InsertWhenAllDone(QObject *parent = Q_NULLPTR)
        : InsertProxyModel(parent)
    { }

protected:
    bool validRow() const Q_DECL_OVERRIDE
    {
        if (!sourceModel())
            return false;
        const int sourceCols = sourceModel()->columnCount();
        const int sourceRows = sourceModel()->rowCount();
        for (int i = 0; i < sourceCols; ++i) {
            if (index(sourceRows, i).data().toString().isEmpty())
                return false;
        }
        return true;
    }
    bool validColumn() const Q_DECL_OVERRIDE
    {
        if (!sourceModel())
            return false;
        const int sourceCols = sourceModel()->columnCount();
        const int sourceRows = sourceModel()->rowCount();
        for (int i = 0; i < sourceRows; ++i) {
            if (index(i, sourceCols).data().toString().isEmpty())
                return false;
        }
        return true;
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget mainWid;
    mainWid.setWindowTitle(QStringLiteral("InsertProxyModel Example Using validRow/validColumn"));
    QStandardItemModel *baseModel = new QStandardItemModel(&mainWid);
    // fill the model with example data
    baseModel->insertColumns(0, 3);
    baseModel->insertRows(0, 5);
    for (int i = 0; i < baseModel->rowCount(); ++i) {
        for (int j = 0; j < baseModel->columnCount(); ++j)
            baseModel->setData(baseModel->index(i, j), QStringLiteral("%1;%2").arg(i + 1).arg(j + 1));
    }
    // Create the proxy and assign the source
    InsertWhenAllDone *insertProxy = new InsertWhenAllDone(&mainWid);
    insertProxy->setSourceModel(baseModel);

    // Create a combobox to select the insert direction
    QComboBox *insertDirectionCombo = new QComboBox(&mainWid);
    insertDirectionCombo->addItem("No Insertion", InsertProxyModel::NoInsert);
    insertDirectionCombo->addItem("Insert Rows", InsertProxyModel::InsertRow);
    insertDirectionCombo->addItem("Insert Columns", InsertProxyModel::InsertColumn);
    insertDirectionCombo->addItem("Insert Both", int(InsertProxyModel::InsertColumn | InsertProxyModel::InsertRow));
    QObject::connect(insertDirectionCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     [insertProxy, insertDirectionCombo](int idx) -> void {
                         const auto newDirection = InsertProxyModel::InsertDirection(insertDirectionCombo->itemData(idx).toInt());
                         insertProxy->setInsertDirection(newDirection);
                     });

    // Create a view for the base model and a view for the proxy
    QTableView *baseView = new QTableView(&mainWid);
    baseView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    baseView->setEditTriggers(QTableView::NoEditTriggers);
    baseView->setModel(baseModel);

    QTableView *proxyView = new QTableView(&mainWid);
    proxyView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    proxyView->setModel(insertProxy);

    // lay out the widget
    QHBoxLayout *topLay = new QHBoxLayout;
    topLay->addWidget(new QLabel(QStringLiteral("Insert Direction"), &mainWid));
    topLay->addWidget(insertDirectionCombo);
    topLay->addStretch();
    QGridLayout *mainLay = new QGridLayout(&mainWid);
    mainLay->addLayout(topLay, 0, 0, 1, 2);
    mainLay->addWidget(baseView, 1, 0);
    mainLay->addWidget(proxyView, 1, 1);
    mainWid.show();
    return a.exec();
}