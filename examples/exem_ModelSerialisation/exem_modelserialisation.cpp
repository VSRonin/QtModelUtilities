#include <QApplication>
#include <QStandardItemModel>
#include <QTreeView>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QLabel>
#include <QTextBrowser>
#include <QFileInfo>
#include <QTextStream>
#include <QTemporaryFile>
#include <BinaryModelSerialiser>
#include <XmlModelSerialiser>
#include <HtmlModelSerialiser>
#include <CsvModelSerialiser>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // prepare a dummy tree model
    QPixmap bluePix(20, 20);
    bluePix.fill(Qt::blue);
    QStandardItemModel baseModel;
    baseModel.insertRows(0, 3);
    baseModel.insertColumns(0, 2);
    baseModel.setHeaderData(0, Qt::Horizontal, "Col1");
    baseModel.setHeaderData(1, Qt::Horizontal, "Col2");
    baseModel.setHeaderData(0, Qt::Vertical, "Row1");
    baseModel.setHeaderData(1, Qt::Vertical, "Row2");
    baseModel.setHeaderData(2, Qt::Vertical, "Row3");
    baseModel.setData(baseModel.index(0, 0), "0.0");
    baseModel.setData(baseModel.index(0, 0), bluePix, Qt::DecorationRole);
    baseModel.setData(baseModel.index(0, 1), "0.1");
    baseModel.setData(baseModel.index(1, 0), "1.0");
    baseModel.setData(baseModel.index(1, 1), "1.1");
    baseModel.setData(baseModel.index(2, 0), "2.0");
    baseModel.setData(baseModel.index(2, 1), "2.1 A \"very, \"odd\" one < to \"\"\"\"\" man&age");
    baseModel.insertRows(0, 2, baseModel.index(0, 0));
    baseModel.insertColumns(0, 2, baseModel.index(0, 0));
    baseModel.setData(baseModel.index(0, 0, baseModel.index(0, 0)), "0.0.0");
    baseModel.setData(baseModel.index(0, 1, baseModel.index(0, 0)), "0.0.1");
    baseModel.setData(baseModel.index(1, 0, baseModel.index(0, 0)), "0.1.0");
    baseModel.setData(baseModel.index(1, 1, baseModel.index(0, 0)), "0.1.1");
    QStandardItemModel loadedModel;

    QWidget mainWidget;
    // selector for the format to use to serialise
    QComboBox* formatSelector = new QComboBox(&mainWidget);
    // displays our dummy model that will be saved to file
    QTreeView* savedView = new QTreeView(&mainWidget);
    savedView->setModel(&baseModel);
    // displays the model that gets loaded from file
    QTreeView* loadedView = new QTreeView(&mainWidget);
    loadedView->setModel(&loadedModel);
    loadedView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // displays the content of the saved file as rich text (Qt HTML)
    QTextBrowser* savedViewer = new QTextBrowser(&mainWidget);
    // displays the content of the saved file as plain text
    QPlainTextEdit* sourceViewer = new QPlainTextEdit(&mainWidget);
    sourceViewer->setReadOnly(true);
    QGridLayout* mainLay = new QGridLayout(&mainWidget);
    mainLay->addWidget(new QLabel(QStringLiteral("Select Format"), &mainWidget), 0, 0);
    mainLay->addWidget(formatSelector, 0, 1);
    mainLay->addWidget(new QLabel(QStringLiteral("Saved Model"), &mainWidget), 1, 0);
    mainLay->addWidget(new QLabel(QStringLiteral("Loaded Model"), &mainWidget), 1, 1);
    mainLay->addWidget(new QLabel(QStringLiteral("Saved File, Rich Text"), &mainWidget), 1, 2);
    mainLay->addWidget(new QLabel(QStringLiteral("Saved File, Plain Text"), &mainWidget), 1, 3);
    mainLay->addWidget(savedView, 2, 0);
    mainLay->addWidget(loadedView, 2, 1);
    mainLay->addWidget(savedViewer, 2, 2);
    mainLay->addWidget(sourceViewer, 2, 3);
    // save and load the model when the combo changes
    QObject::connect(formatSelector, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&mainWidget, &baseModel, &loadedModel, sourceViewer, savedViewer](int idx)->void {
        AbstractModelSerialiser* serial=nullptr;
        switch (idx){
        case 0:
            serial = new BinaryModelSerialiser;
            break;
        case 1:
            serial = new XmlModelSerialiser;
            break;
        case 2:
            serial = new HtmlModelSerialiser;
            break;
        case 3:
            serial = new CsvModelSerialiser;
            break;
        default:
            Q_UNREACHABLE();
        }
        serial->setModel(&baseModel); // set the model to save
        QTemporaryFile tempFile("TestSave"); // prepare the file to save
        if (tempFile.exists())
            tempFile.remove();
        if (!tempFile.open()) // open the file
            QMessageBox::critical(&mainWidget, QStringLiteral("Error"), QStringLiteral("Impossible to open the file"));
        if (!serial->saveModel(&tempFile)) // save the model to fille
            QMessageBox::critical(&mainWidget, QStringLiteral("Error"), QStringLiteral("Model Saving Failed"));
        tempFile.seek(0); // go back to the beginning of the file
        serial->setModel(&loadedModel); // set the model to load
        if (!serial->loadModel(&tempFile)) // load the model
            QMessageBox::critical(&mainWidget, QStringLiteral("Error"), QStringLiteral("Model Loading Failed"));
        tempFile.seek(0); // go back to the beginning of the file
        sourceViewer->setPlainText(QTextStream(&tempFile).readAll()); // load the plain text 
        savedViewer->setSource(QUrl::fromLocalFile(QFileInfo(tempFile).absoluteFilePath())); // load the rich text (Qt HTML)
        delete serial;
    });
    formatSelector->addItems(QStringList() << QStringLiteral("Binary") << QStringLiteral("XML") << QStringLiteral("HTML") << QStringLiteral("CSV"));
    mainWidget.show();
    return a.exec();
}