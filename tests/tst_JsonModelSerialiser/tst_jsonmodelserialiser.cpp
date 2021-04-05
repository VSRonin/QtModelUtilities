#include <QtTest/QTest>
#include "tst_jsonmodelserialiser.h"
#include <jsonmodelserialiser.h>
#include <QByteArray>
#include <QFile>
#include <QJsonObject>

void tst_JsonModelSerialiser::basicSaveLoadByteArray()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    JsonModelSerialiser serialiser;
    saveLoadByteArray(&serialiser, sourceModel, destinationModel, true);
    destinationModel->deleteLater();
}

void tst_JsonModelSerialiser::basicSaveLoadFile()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    JsonModelSerialiser serialiser;
    saveLoadFile(&serialiser, sourceModel, destinationModel, true);
    destinationModel->deleteLater();
}

void tst_JsonModelSerialiser::basicSaveLoadString()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    JsonModelSerialiser serialiser;
    saveLoadString(&serialiser, sourceModel, destinationModel, true);
    destinationModel->deleteLater();
}

void tst_JsonModelSerialiser::basicSaveLoadObject()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    JsonModelSerialiser serialiser(sourceModel);
    serialiser.addRoleToSave(Qt::UserRole + 1);
    QJsonObject writeObject = serialiser.toJsonObject();
    QVERIFY(!writeObject.isEmpty());
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.fromJsonObject(writeObject));
    checkModelEqual(sourceModel, destinationModel);
    destinationModel->deleteLater();
}
