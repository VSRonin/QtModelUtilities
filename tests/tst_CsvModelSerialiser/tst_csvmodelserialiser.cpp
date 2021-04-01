#include <QtTest/QTest>
#include "tst_csvmodelserialiser.h"
#include <../serialisers_common.hpp>
#include <csvmodelserialiser.h>
#include <QByteArray>
#include <QFile>
#include <QTextStream>

void tst_CsvModelSerialiser::basicSaveLoad()
{
    QFETCH(QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    QByteArray dataArray;
    CsvModelSerialiser serialiser(static_cast<const QAbstractItemModel *>(sourceModel));
    serialiser.addRoleToSave(Qt::UserRole + 1);
    QVERIFY(serialiser.saveModel(&dataArray));
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(dataArray));
    SerialiserCommon::modelEqual(sourceModel, destinationModel);

    QVERIFY(destinationModel->removeRows(0, destinationModel->rowCount()));
    serialiser.setModel(static_cast<const QAbstractItemModel *>(sourceModel));
    QFile serialisedCsvStream(QStringLiteral("Serialised Csv Stream ") + QTest::currentDataTag() + QStringLiteral(".csv"));
    QVERIFY(serialisedCsvStream.open(QIODevice::WriteOnly));
    QTextStream writeStream(&serialisedCsvStream);
    QVERIFY(serialiser.saveModel(writeStream));
    serialisedCsvStream.close();
    QVERIFY(serialisedCsvStream.open(QIODevice::ReadOnly));
    QTextStream readStream(&serialisedCsvStream);
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(readStream));
    serialisedCsvStream.close();
    SerialiserCommon::modelEqual(sourceModel, destinationModel);

    QVERIFY(destinationModel->removeRows(0, destinationModel->rowCount()));
    serialiser.setModel(static_cast<const QAbstractItemModel *>(sourceModel));
    QFile serialisedCsvDevice(QStringLiteral("Serialised Csv Device ") + QTest::currentDataTag() + QStringLiteral(".csv"));
    QVERIFY(serialisedCsvDevice.open(QIODevice::WriteOnly));
    QVERIFY(serialiser.saveModel(&serialisedCsvDevice));
    serialisedCsvDevice.close();
    QVERIFY(serialisedCsvDevice.open(QIODevice::ReadOnly));
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(&serialisedCsvDevice));
    serialisedCsvDevice.close();
    SerialiserCommon::modelEqual(sourceModel, destinationModel);

    sourceModel->deleteLater();
    destinationModel->deleteLater();
}

void tst_CsvModelSerialiser::basicSaveLoad_data()
{
    QTest::addColumn<QAbstractItemModel *>("sourceModel");
    QTest::addColumn<QAbstractItemModel *>("destinationModel");
    QTest::newRow("List Single Role") << SerialiserCommon::createStringModel() << static_cast<QAbstractItemModel *>(new QStringListModel());
#ifdef QT_GUI_LIB
    QTest::newRow("Table Single Role") << SerialiserCommon::createComplexModel(false, false)
                                       << static_cast<QAbstractItemModel *>(new QStandardItemModel());
#endif
}
