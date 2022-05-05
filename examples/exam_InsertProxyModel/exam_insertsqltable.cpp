#include <InsertProxyModel>
#include <QSqlTableModel>
#include <QApplication>
#include <QHeaderView>
#include <QTableView>
#include <QHBoxLayout>
#include <QSqlRecord>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QSqlError>
#include <memory>

// subclass InsertProxyModel and reimplement commitRow to insert
// the record in the source QSqlTableModel
class InsertRecordProxy : public InsertProxyModel
{
    Q_DISABLE_COPY(InsertRecordProxy)
public:
    explicit InsertRecordProxy(QObject *parent = Q_NULLPTR)
        : InsertProxyModel(parent)
    {
        // the proxy will always allow inserting a new row
        setInsertDirection(InsertProxyModel::InsertRow);
    }
public slots:
    bool commitRow() Q_DECL_OVERRIDE
    {
        // if the source model is a QSqlTableModel
        QSqlTableModel *sourceSQLModel = qobject_cast<QSqlTableModel *>(sourceModel());
        if (!sourceSQLModel)
            return InsertProxyModel::commitRow();
        // prepare the record
        QSqlRecord record = sourceSQLModel->record();
        const int sourceCols = sourceModel()->columnCount();
        const int sourceRows = sourceModel()->rowCount();
        // copy the data in the proxy to the record
        for (int i = 0; i < sourceCols; ++i)
            record.setValue(i, index(sourceRows, i).data());
        // if inserting the record in the database is succesfull
        if (sourceSQLModel->insertRecord(-1, record)) {
            // we cleanup the data in the proxy model
            endCommitRow();
            return true;
        }
        // otherwise we print an error
        qDebug() << sourceSQLModel->lastError().text();
        return false;
    }

protected:
    bool validRow() const Q_DECL_OVERRIDE
    {
        // this method will commit the row only if all columns have data
        // see exam_InsertProxyModel_Subclass
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

    // create the main app widget
    QWidget mainWid;
    mainWid.setWindowTitle(QStringLiteral("InsertProxyModel Example for inserting in a SQL table"));

    // create a QSqlTableModel to interact with the db
    QSqlTableModel *baseModel = new QSqlTableModel(&mainWid, db);
    baseModel->setTable(QStringLiteral("Persons"));
    baseModel->select();

    // Create the proxy and assign the source
    InsertRecordProxy *insertProxy = new InsertRecordProxy(&mainWid);
    insertProxy->setSourceModel(baseModel);

    // Create a view for the base model and a view for the proxy
    QTableView *baseView = new QTableView(&mainWid);
    baseView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    baseView->setEditTriggers(QTableView::NoEditTriggers);
    baseView->setModel(baseModel);

    QTableView *proxyView = new QTableView(&mainWid);
    proxyView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    proxyView->setModel(insertProxy);

    // lay out the widget
    QHBoxLayout *mainLay = new QHBoxLayout(&mainWid);
    mainLay->addWidget(baseView);
    mainLay->addWidget(proxyView);
    mainWid.show();
    return app.exec();
}
