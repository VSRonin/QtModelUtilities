#include <QtTest/QTest>
#include "tst_binarymodelserialiser.h"
#include <binarymodelserialiser.h>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>

void tst_BinaryModelSerialiser::basicSaveLoadByteArray()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    BinaryModelSerialiser serialiser;
    saveLoadFile(&serialiser, sourceModel, destinationModel, true);
    destinationModel->deleteLater();
}

void tst_BinaryModelSerialiser::basicSaveLoadFile()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    BinaryModelSerialiser serialiser;
    saveLoadByteArray(&serialiser, sourceModel, destinationModel, true);
    destinationModel->deleteLater();
}

void tst_BinaryModelSerialiser::basicSaveLoadStream()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    BinaryModelSerialiser serialiser(sourceModel);
    serialiser.addRoleToSave(Qt::UserRole + 1);
    QByteArray dataArray;
    QDataStream writeStream(&dataArray, QIODevice::WriteOnly);
    QVERIFY(serialiser.saveModel(writeStream));
    QDataStream readStream(dataArray);
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(readStream));
    checkModelEqual(sourceModel, destinationModel);
    destinationModel->deleteLater();
}
