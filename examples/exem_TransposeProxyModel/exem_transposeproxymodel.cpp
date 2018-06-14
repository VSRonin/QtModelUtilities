#include <QApplication>
#include <QTreeView>
#include <QTableView>
#include <QLabel>
#include <QStandardItemModel>
#include <TransposeProxyModel>
#include <QGridLayout>
int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    // prepare a dummy table model
    QStandardItemModel tableModel;
    tableModel.insertRows(0, 5);
    tableModel.insertColumns(0, 3);
    for (int i = 0; i < tableModel.rowCount(); ++i) {
        for (int j = 0; j < tableModel.columnCount(); ++j) {
            tableModel.setData(tableModel.index(i, j), QStringLiteral("Row: %1, Col: %2").arg(i).arg(j));
        }
    }

    // proxy to transpose the table model
    TransposeProxyModel tableProxy;
    tableProxy.setSourceModel(&tableModel);
    

    // prepare a dummy tree model
    QStandardItemModel treeModel;
    treeModel.insertRows(0, 5);
    treeModel.insertColumns(0, 3);
    for (int i = 0; i < treeModel.rowCount(); ++i) {
        for (int j = 0; j < treeModel.columnCount(); ++j) {
            const QModelIndex parentIdx = treeModel.index(i, j);
            treeModel.setData(parentIdx, QStringLiteral("Row: %1, Col: %2").arg(i).arg(j));
            treeModel.insertRows(0, 2, parentIdx);
            treeModel.insertColumns(0, 2, parentIdx);
            for (int k = 0; k < treeModel.rowCount(parentIdx); ++k) {
                for (int h = 0; h < treeModel.columnCount(parentIdx); ++h) {
                    treeModel.setData(treeModel.index(k, h, parentIdx), QStringLiteral("Row: %1, Col: %2 [Parent, Row: %3, Col: %4]").arg(k).arg(h).arg(i).arg(j));
                }
            }
        }
    }

    // proxy to transpose the tree model
    TransposeProxyModel treeProxy;
    treeProxy.setSourceModel(&treeModel);

    // create a ui with 4 views to display our 4 models
    QWidget mainW;
    mainW.setWindowTitle(QStringLiteral("TransposeProxyModel Example"));
    QTableView* sourceTableView = new QTableView(&mainW);
    sourceTableView->setModel(&tableModel);
    QTableView* proxyTableView = new QTableView(&mainW);
    proxyTableView->setModel(&tableProxy);
    QTreeView* sourceTreeView = new QTreeView(&mainW);
    sourceTreeView->setModel(&treeModel);
    QTreeView* proxyTreeView = new QTreeView(&mainW);
    proxyTreeView->setModel(&treeProxy);
    QGridLayout* mainLay = new QGridLayout(&mainW);
    mainLay->addWidget(new QLabel(QStringLiteral("Original Table")), 0, 0);
    mainLay->addWidget(sourceTableView,1,0);
    mainLay->addWidget(new QLabel(QStringLiteral("Inverted Table")), 0, 1);
    mainLay->addWidget(proxyTableView,1,1);
    mainLay->addWidget(new QLabel(QStringLiteral("Original Tree")), 2, 0);
    mainLay->addWidget(sourceTreeView, 3, 0);
    mainLay->addWidget(new QLabel(QStringLiteral("Inverted Tree")), 2, 1);
    mainLay->addWidget(proxyTreeView, 3, 1);
    mainW.show();
    return application.exec();
}