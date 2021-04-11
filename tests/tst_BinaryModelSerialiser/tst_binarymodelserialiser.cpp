#include <QtTest/QTest>
#include "tst_binarymodelserialiser.h"
#include <binarymodelserialiser.h>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QSignalSpy>

void tst_BinaryModelSerialiser::autoParent(){
    QObject* parentObj = new QObject;
    auto testItem = new BinaryModelSerialiser(parentObj);
    QSignalSpy testItemDestroyedSpy(testItem, SIGNAL(destroyed(QObject*)));
    QVERIFY(testItemDestroyedSpy.isValid());
    delete parentObj;
    QCOMPARE(testItemDestroyedSpy.count(),1);
}

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
    BinaryModelSerialiser serialiser(sourceModel,nullptr);
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
