#include <QApplication>
#include <QStandardItemModel>
#include <QTreeView>
#include <QHBoxLayout>
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
    // displays our dummy model that will be saved to file
    QTreeView* savedView = new QTreeView(&mainWidget);
    savedView->setModel(&baseModel);
    // displays the model that gets loaded from file
    QTreeView* loadedView = new QTreeView(&mainWidget);
    loadedView->setModel(&loadedModel);
    // displays the content of the saved file as rich text (Qt HTML)
    QTextBrowser* savedViewer = new QTextBrowser(&mainWidget);
    // displays the content of the saved file as plain text
    QPlainTextEdit* sourceViewer = new QPlainTextEdit(&mainWidget);
    sourceViewer->setReadOnly(true);
    QHBoxLayout* mainLay = new QHBoxLayout(&mainWidget);
    mainLay->addWidget(savedView);
    mainLay->addWidget(loadedView);
    mainLay->addWidget(savedViewer);
    mainLay->addWidget(sourceViewer);
    
    QScopedPointer<AbstractModelSerialiser> serial(new
//////////////////////////////////////////////////////////////////////////
// uncomment one of the following lines to use the corresponding format
    BinaryModelSerialiser
//     XmlModelSerialiser
//     HtmlModelSerialiser
//     CsvModelSerialiser
//////////////////////////////////////////////////////////////////////////
    ); //create the serialiser
    serial->setModel(&baseModel); // set the model to save
    QFile tempFile("TestSave"); // prepare the file to save
    if (tempFile.exists())
        tempFile.remove();
    if (!tempFile.open(QIODevice::ReadWrite)) // open the file
        Q_ASSERT(false);
    if (!serial->saveModel(&tempFile)) // save the model to fille
        Q_ASSERT(false);
    tempFile.seek(0); // go back to the beginning of the file
    serial->setModel(&loadedModel); // set the model to load
    if (!serial->loadModel(&tempFile)) // load the model
        Q_ASSERT(false);
    tempFile.seek(0); // go back to the beginning of the file
    sourceViewer->setPlainText(QTextStream(&tempFile).readAll()); // load the plain text 
    tempFile.close(); // close the file
    savedViewer->setSource(QUrl::fromLocalFile("TestSave")); // load the rich text (Qt HTML)
    
    mainWidget.show();
    return a.exec();
}