#include <QtTest/QTest>
#include "tst_binarymodelserialiser.h"
#include <../serialisers_common.hpp>
#include <binarymodelserialiser.h>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>

void tst_BinaryModelSerialiser::basicSaveLoad()
{
    QFETCH(QAbstractItemModel*, sourceModel);
    QFETCH(QAbstractItemModel*, destinationModel);
    QByteArray dataArray;
    BinaryModelSerialiser serialiser(static_cast<const QAbstractItemModel*>(sourceModel));
    QVERIFY(serialiser.saveModel(&dataArray));
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(dataArray));
    QVERIFY(SerialiserCommon::modelEqual(sourceModel,destinationModel));

    QVERIFY(destinationModel->removeRows(0,destinationModel->rowCount()));
    dataArray.clear();
    serialiser.setModel(static_cast<const QAbstractItemModel*>(sourceModel));
    QDataStream writeStream(&dataArray,QIODevice::WriteOnly);
    QVERIFY(serialiser.saveModel(writeStream));
    QDataStream readStream(dataArray);
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(readStream));
    QVERIFY(SerialiserCommon::modelEqual(sourceModel,destinationModel));

    QVERIFY(destinationModel->removeRows(0,destinationModel->rowCount()));
    dataArray.clear();
    serialiser.setModel(static_cast<const QAbstractItemModel*>(sourceModel));
    QBuffer writeBuff(&dataArray);
    QVERIFY(writeBuff.open(QIODevice::WriteOnly));
    QVERIFY(serialiser.saveModel(&writeBuff));
    writeBuff.close();
    QBuffer readBuff(&dataArray);
    QVERIFY(readBuff.open(QIODevice::ReadOnly));
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(&readBuff));
    readBuff.close();
    QVERIFY(SerialiserCommon::modelEqual(sourceModel,destinationModel));

    sourceModel->deleteLater();
    destinationModel->deleteLater();
}

void tst_BinaryModelSerialiser::basicSaveLoad_data()
{
    QTest::addColumn<QAbstractItemModel*>("sourceModel");
    QTest::addColumn<QAbstractItemModel*>("destinationModel");
    QTest::newRow("QStringListModel") << SerialiserCommon::createStringModel() << static_cast<QAbstractItemModel*>(new QStringListModel());
}
