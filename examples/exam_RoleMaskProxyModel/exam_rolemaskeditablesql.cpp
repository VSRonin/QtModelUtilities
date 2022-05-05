// This example will demonstrate how to make a QSqlQueryModel editable using RoleMaskProxyModel.

#include <RoleMaskProxyModel>
#include <QApplication>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QLabel>
#include <QGridLayout>
#include <QDir>
#include <QHeaderView>
#include <QTemporaryFile>
#include <QFileInfo>
#include <memory>

// We subclass RoleMaskProxyModel and reimplement the flags() method to allow editing
class EditableFlagMask : public RoleMaskProxyModel
{
    Q_DISABLE_COPY(EditableFlagMask)
public:
    explicit EditableFlagMask(QObject *parent = Q_NULLPTR)
        : RoleMaskProxyModel(parent)
    { }
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE { return RoleMaskProxyModel::flags(index) | Qt::ItemIsEditable; }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // we copy the dummy sqlite db from the program resources to a temporary file
    QFile dbResourceFile(QStringLiteral(":/db/exampledb.sqlite3"));
    std::unique_ptr<QTemporaryFile> dbFile(QTemporaryFile::createNativeFile(dbResourceFile));
    // open the sqlite db
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QFileInfo(*dbFile).absoluteFilePath());
    if (!db.open()) {
        qDebug("Unable to open Db");
        return 1;
    }

    // we load a QSqlQueryModel from the dummy db
    QSqlQueryModel baseModel;
    baseModel.setQuery(QStringLiteral("select * from Persons"), db);

    // we instantiate our RoleMaskProxyModel subclass
    EditableFlagMask proxyModel;
    // we make Qt::DisplayRole equivalent to Qt::EditRole, this is the default in all of Qt's models
    proxyModel.setMergeDisplayEdit(true);
    // if the proxy did not overwrite the data we set it to show the data in the source model
    proxyModel.setTransparentIfEmpty(true);
    // we intercept all the changes in the Qt::DisplayRole in the proxy model rather than passing them on to the source model
    proxyModel.addMaskedRole(Qt::DisplayRole);
    // we set the source model
    proxyModel.setSourceModel(&baseModel);

    // we create the ui to show both the source and the proxy model in 2 table views
    QWidget mainWid;
    mainWid.setWindowTitle(QStringLiteral("RoleMaskProxyModel Example - Editable SqlQueryModel"));
    QTableView *sourceView = new QTableView(&mainWid);
    sourceView->setModel(&baseModel);
    sourceView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QTableView *proxyView = new QTableView(&mainWid);
    proxyView->setModel(&proxyModel);
    proxyView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QGridLayout *mainLay = new QGridLayout(&mainWid);
    mainLay->addWidget(new QLabel(QStringLiteral("Original Model")), 0, 0);
    mainLay->addWidget(sourceView, 1, 0);
    mainLay->addWidget(new QLabel(QStringLiteral("Editable Model")), 0, 1);
    mainLay->addWidget(proxyView, 1, 1);
    mainWid.show();
    return app.exec();
}
