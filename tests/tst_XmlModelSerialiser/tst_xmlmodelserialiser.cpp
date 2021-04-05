#include <QtTest/QTest>
#include "tst_xmlmodelserialiser.h"
#include <xmlmodelserialiser.h>
#include <QByteArray>
#include <QTemporaryFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

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
    XmlModelSerialiser serialiser(sourceModel);
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
    XmlModelSerialiser serialiser(sourceModel);
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


