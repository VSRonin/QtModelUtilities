#include <QtTest/QTest>
#include "tst_xmlmodelserialiser.h"
#include <xmlmodelserialiser.h>
#include <QByteArray>
#include <QTemporaryFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QSignalSpy>

void tst_XmlModelSerialiser::autoParent()
{
    QObject *parentObj = new QObject;
    auto testItem = new XmlModelSerialiser(parentObj);
    QSignalSpy testItemDestroyedSpy(testItem, SIGNAL(destroyed(QObject *)));
    QVERIFY(testItemDestroyedSpy.isValid());
    delete parentObj;
    QCOMPARE(testItemDestroyedSpy.count(), 1);
}

void tst_XmlModelSerialiser::basicSaveLoadByteArray()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    XmlModelSerialiser serialiser;
    saveLoadByteArray(&serialiser, sourceModel, destinationModel, true);
    destinationModel->deleteLater();
}

void tst_XmlModelSerialiser::basicSaveLoadFile()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    XmlModelSerialiser serialiser;
    saveLoadFile(&serialiser, sourceModel, destinationModel, true);
    destinationModel->deleteLater();
}

void tst_XmlModelSerialiser::basicSaveLoadString()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    XmlModelSerialiser serialiser;
    saveLoadString(&serialiser, sourceModel, destinationModel, true);
    destinationModel->deleteLater();
}

void tst_XmlModelSerialiser::basicSaveLoadStream()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    XmlModelSerialiser serialiser(sourceModel, nullptr);
    serialiser.addRoleToSave(Qt::UserRole + 1);
    QTemporaryFile serialisedXmlStream;
    QVERIFY(serialisedXmlStream.open());
    QXmlStreamWriter writeStream(&serialisedXmlStream);
    QVERIFY(serialiser.saveModel(writeStream));
    QVERIFY(serialisedXmlStream.seek(0));
    QXmlStreamReader readStream(&serialisedXmlStream);
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(readStream));
    serialisedXmlStream.close();
    checkModelEqual(sourceModel, destinationModel);
    destinationModel->deleteLater();
}

void tst_XmlModelSerialiser::basicSaveLoadNested()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    XmlModelSerialiser serialiser(sourceModel, nullptr);
    serialiser.addRoleToSave(Qt::UserRole + 1);
    QTemporaryFile serialisedXmlNested;
    QVERIFY(serialisedXmlNested.open());
    QXmlStreamWriter writeNestedStream(&serialisedXmlNested);
    writeNestedStream.writeStartDocument();
    writeNestedStream.writeStartElement(QStringLiteral("NestForModel"));
    serialiser.setPrintStartDocument(false);
    QVERIFY(serialiser.saveModel(writeNestedStream));
    writeNestedStream.writeEndElement(); // NestForModel
    QVERIFY(serialisedXmlNested.seek(0));
    QXmlStreamReader readNestedStream(&serialisedXmlNested);
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(readNestedStream));
    serialisedXmlNested.close();
    checkModelEqual(sourceModel, destinationModel);
    destinationModel->deleteLater();
}

void tst_XmlModelSerialiser::validateXmlOutput()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QByteArray modelData;
    XmlModelSerialiser serialiser(sourceModel, nullptr);
    serialiser.saveModel(&modelData);
    QXmlStreamReader xmlReader(modelData);
    while (!xmlReader.atEnd())
        xmlReader.readNext();
    QVERIFY(!xmlReader.hasError());
}

void tst_XmlModelSerialiser::validateXmlOutput_data()
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
