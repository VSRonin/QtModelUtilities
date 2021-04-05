#include <QtTest/QTest>
#include "tst_csvmodelserialiser.h"
#include <csvmodelserialiser.h>
#include <QByteArray>
#include <QBuffer>
#include <QTextStream>

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
    CsvModelSerialiser serialiser(sourceModel);
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

void tst_CsvModelSerialiser::basicSaveLoadData(QObject* parent)
{
    Q_UNUSED(parent)
    QTest::addColumn<const QAbstractItemModel *>("sourceModel");
    QTest::addColumn<QAbstractItemModel *>("destinationModel");
    QTest::newRow("List Single Role") << static_cast<const QAbstractItemModel *>(createStringModel(this)) << static_cast<QAbstractItemModel *>(new QStringListModel(this));
    QTest::newRow("List Single Role Overwrite") << static_cast<const QAbstractItemModel *>(createStringModel(parent)) << createStringModel(parent);
#ifdef QT_GUI_LIB
    QTest::newRow("Table Single Role") << static_cast<const QAbstractItemModel *>(createComplexModel(false, false, this)) << static_cast<QAbstractItemModel *>(new QStandardItemModel(this));
    QTest::newRow("Table Single Role Overwrite") << static_cast<const QAbstractItemModel *>(createComplexModel(false, false, parent)) << createComplexModel(false, false, parent);
#endif
}
