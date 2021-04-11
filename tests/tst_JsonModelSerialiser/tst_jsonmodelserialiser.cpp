#include <QtTest/QTest>
#include "tst_jsonmodelserialiser.h"
#include <jsonmodelserialiser.h>
#include <QByteArray>
#include <QFile>
#include <QJsonObject>
#include <QSignalSpy>

void tst_JsonModelSerialiser::autoParent(){
    QObject* parentObj = new QObject;
    auto testItem = new JsonModelSerialiser(parentObj);
    QSignalSpy testItemDestroyedSpy(testItem, SIGNAL(destroyed(QObject*)));
    QVERIFY(testItemDestroyedSpy.isValid());
    delete parentObj;
    QCOMPARE(testItemDestroyedSpy.count(),1);
}

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

void tst_JsonModelSerialiser::validateJsonOutput()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QByteArray modelData;
    JsonModelSerialiser serialiser(sourceModel,nullptr);
    serialiser.saveModel(&modelData);
    QJsonParseError parseErr;
    QJsonDocument::fromJson(modelData, &parseErr);
    QCOMPARE(parseErr.error, QJsonParseError::NoError);
}

void tst_JsonModelSerialiser::validateJsonOutput_data()
{
    QTest::addColumn<const QAbstractItemModel *>("sourceModel");
    QTest::newRow("List Single Role") << static_cast<const QAbstractItemModel *>(createStringModel(this));
#ifdef QT_GUI_LIB
    QTest::newRow("Table Single Role") << static_cast<const QAbstractItemModel *>(createComplexModel(false, false, this));
    QTest::newRow("Table Multi Roles") << static_cast<const QAbstractItemModel *>(createComplexModel(false, true, this));
    QTest::newRow("Tree Single Role") << static_cast<const QAbstractItemModel *>(createComplexModel(true, false, this));
    QTest::newRow("Tree Multi Roles") << static_cast<const QAbstractItemModel *>(createComplexModel(true, true, this));
#endif
}

void tst_JsonModelSerialiser::basicSaveLoadObject()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    JsonModelSerialiser serialiser(sourceModel,nullptr);
    serialiser.addRoleToSave(Qt::UserRole + 1);
    QJsonObject writeObject = serialiser.toJsonObject();
    QVERIFY(!writeObject.isEmpty());
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.fromJsonObject(writeObject));
    checkModelEqual(sourceModel, destinationModel);
    destinationModel->deleteLater();
}
