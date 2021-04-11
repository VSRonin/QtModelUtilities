#include <QtTest/QTest>
#include "tst_csvmodelserialiser.h"
#include <csvmodelserialiser.h>
#include <QByteArray>
#include <QBuffer>
#include <QTextStream>
#include <QSignalSpy>

void tst_CsvModelSerialiser::autoParent(){
    QObject* parentObj = new QObject;
    auto testItem = new CsvModelSerialiser(parentObj);
    QSignalSpy testItemDestroyedSpy(testItem, SIGNAL(destroyed(QObject*)));
    QVERIFY(testItemDestroyedSpy.isValid());
    delete parentObj;
    QCOMPARE(testItemDestroyedSpy.count(),1);
}

void tst_CsvModelSerialiser::basicSaveLoadByteArray()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    CsvModelSerialiser serialiser;
    saveLoadByteArray(&serialiser, sourceModel, destinationModel, false);
    destinationModel->deleteLater();
}

void tst_CsvModelSerialiser::basicSaveLoadFile()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    CsvModelSerialiser serialiser;
    saveLoadFile(&serialiser, sourceModel, destinationModel, false);
    destinationModel->deleteLater();
}

void tst_CsvModelSerialiser::basicSaveLoadString()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    CsvModelSerialiser serialiser;
    saveLoadString(&serialiser, sourceModel, destinationModel, false);
    destinationModel->deleteLater();
}

void tst_CsvModelSerialiser::basicSaveLoadStream()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    CsvModelSerialiser serialiser(sourceModel,nullptr);
    QByteArray dataArray;
    QBuffer serialisedCsvStream(&dataArray);
    QVERIFY(serialisedCsvStream.open(QIODevice::WriteOnly));
    QTextStream writeStream(&serialisedCsvStream);
    QVERIFY(serialiser.saveModel(writeStream));
    serialisedCsvStream.close();
    QVERIFY(serialisedCsvStream.open(QIODevice::ReadOnly));
    QTextStream readStream(&serialisedCsvStream);
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(readStream));
    checkModelEqual(sourceModel, destinationModel);
    destinationModel->deleteLater();
}

void tst_CsvModelSerialiser::basicSaveLoadByteArrayNoHeader()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    CsvModelSerialiser serialiser;
    serialiser.setFirstColumnIsHeader(false);
    serialiser.setFirstRowIsHeader(false);
    saveLoadByteArray(&serialiser, sourceModel, destinationModel, false, false);
    destinationModel->deleteLater();
}

void tst_CsvModelSerialiser::basicSaveLoadFileNoHeader()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    CsvModelSerialiser serialiser;
    serialiser.setFirstColumnIsHeader(false);
    serialiser.setFirstRowIsHeader(false);
    saveLoadFile(&serialiser, sourceModel, destinationModel, false, false);
    destinationModel->deleteLater();
}

void tst_CsvModelSerialiser::basicSaveLoadStringNoHeader()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    CsvModelSerialiser serialiser;
    serialiser.setFirstColumnIsHeader(false);
    serialiser.setFirstRowIsHeader(false);
    saveLoadString(&serialiser, sourceModel, destinationModel, false, false);
    destinationModel->deleteLater();
}

void tst_CsvModelSerialiser::basicSaveLoadStreamNoHeader()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    CsvModelSerialiser serialiser(sourceModel,nullptr);
    serialiser.setFirstColumnIsHeader(false);
    serialiser.setFirstRowIsHeader(false);
    QByteArray dataArray;
    QBuffer serialisedCsvStream(&dataArray);
    QVERIFY(serialisedCsvStream.open(QIODevice::WriteOnly));
    QTextStream writeStream(&serialisedCsvStream);
    QVERIFY(serialiser.saveModel(writeStream));
    serialisedCsvStream.close();
    QVERIFY(serialisedCsvStream.open(QIODevice::ReadOnly));
    QTextStream readStream(&serialisedCsvStream);
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(readStream));
    checkModelEqual(sourceModel, destinationModel, QModelIndex(), QModelIndex(), false);
    destinationModel->deleteLater();
}

void tst_CsvModelSerialiser::basicSaveLoadByteArrayCustomSeparator()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    CsvModelSerialiser serialiser;
    serialiser.setCsvSeparator(QStringLiteral("\t"));
    saveLoadByteArray(&serialiser, sourceModel, destinationModel, false);
    destinationModel->deleteLater();
}

void tst_CsvModelSerialiser::basicSaveLoadFileCustomSeparator()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    CsvModelSerialiser serialiser;
    serialiser.setCsvSeparator(QStringLiteral("\t"));
    saveLoadFile(&serialiser, sourceModel, destinationModel, false);
    destinationModel->deleteLater();
}

void tst_CsvModelSerialiser::basicSaveLoadStringCustomSeparator()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    CsvModelSerialiser serialiser;
    serialiser.setCsvSeparator(QStringLiteral("\t"));
    saveLoadString(&serialiser, sourceModel, destinationModel, false);
    destinationModel->deleteLater();
}

void tst_CsvModelSerialiser::basicSaveLoadStreamCustomSeparator()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    CsvModelSerialiser serialiser(sourceModel,nullptr);
    serialiser.setCsvSeparator(QStringLiteral("\t"));
    QByteArray dataArray;
    QBuffer serialisedCsvStream(&dataArray);
    QVERIFY(serialisedCsvStream.open(QIODevice::WriteOnly));
    QTextStream writeStream(&serialisedCsvStream);
    QVERIFY(serialiser.saveModel(writeStream));
    serialisedCsvStream.close();
    QVERIFY(serialisedCsvStream.open(QIODevice::ReadOnly));
    QTextStream readStream(&serialisedCsvStream);
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(readStream));
    checkModelEqual(sourceModel, destinationModel);
    destinationModel->deleteLater();
}

void tst_CsvModelSerialiser::basicSaveLoadData(QObject *parent)
{
    Q_UNUSED(parent)
    QTest::addColumn<const QAbstractItemModel *>("sourceModel");
    QTest::addColumn<QAbstractItemModel *>("destinationModel");
    QTest::newRow("List Single Role") << static_cast<const QAbstractItemModel *>(createStringModel(this))
                                      << static_cast<QAbstractItemModel *>(new QStringListModel(this));
    QTest::newRow("List Single Role Overwrite") << static_cast<const QAbstractItemModel *>(createStringModel(this)) << createStringModel(this);
#ifdef QT_GUI_LIB
    QTest::newRow("Table Single Role") << static_cast<const QAbstractItemModel *>(createComplexModel(false, false, this))
                                       << static_cast<QAbstractItemModel *>(new QStandardItemModel(this));
    QTest::newRow("Table Single Role Overwrite") << static_cast<const QAbstractItemModel *>(createComplexModel(false, false, this))
                                                 << createComplexModel(false, false, this);
#endif
}
