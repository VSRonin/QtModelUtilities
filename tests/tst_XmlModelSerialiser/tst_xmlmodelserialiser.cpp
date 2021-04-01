#include <QtTest/QTest>
#include "tst_xmlmodelserialiser.h"
#include <../serialisers_common.hpp>
#include <xmlmodelserialiser.h>
#include <QByteArray>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

void tst_XmlModelSerialiser::basicSaveLoad()
{
    QFETCH(QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    QByteArray dataArray;
    XmlModelSerialiser serialiser(static_cast<const QAbstractItemModel *>(sourceModel));
    serialiser.addRoleToSave(Qt::UserRole + 1);
    QVERIFY(serialiser.saveModel(&dataArray));
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(dataArray));
    SerialiserCommon::modelEqual(sourceModel, destinationModel);

    QVERIFY(destinationModel->removeRows(0, destinationModel->rowCount()));
    serialiser.setModel(static_cast<const QAbstractItemModel *>(sourceModel));
    QFile serialisedXmlStream(QStringLiteral("SerialisedXmlStream.xml"));
    QVERIFY(serialisedXmlStream.open(QIODevice::WriteOnly));
    QXmlStreamWriter writeStream(&serialisedXmlStream);
    QVERIFY(serialiser.saveModel(writeStream));
    serialisedXmlStream.close();
    QVERIFY(serialisedXmlStream.open(QIODevice::ReadOnly));
    QXmlStreamReader readStream(&serialisedXmlStream);
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(readStream));
    serialisedXmlStream.close();
    SerialiserCommon::modelEqual(sourceModel, destinationModel);

    QVERIFY(destinationModel->removeRows(0, destinationModel->rowCount()));
    serialiser.setModel(static_cast<const QAbstractItemModel *>(sourceModel));
    QFile serialisedXmlDevice(QStringLiteral("SerialisedXmlDevice.xml"));
    QVERIFY(serialisedXmlDevice.open(QIODevice::WriteOnly));
    QVERIFY(serialiser.saveModel(&serialisedXmlDevice));
    serialisedXmlDevice.close();
    QVERIFY(serialisedXmlDevice.open(QIODevice::ReadOnly));
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(&serialisedXmlDevice));
    serialisedXmlDevice.close();
    SerialiserCommon::modelEqual(sourceModel, destinationModel);

    QVERIFY(destinationModel->removeRows(0, destinationModel->rowCount()));
    serialiser.setModel(static_cast<const QAbstractItemModel *>(sourceModel));
    QFile serialisedXmlNested(QStringLiteral("SerialisedXmlNested.xml"));
    QVERIFY(serialisedXmlNested.open(QIODevice::WriteOnly));
    QXmlStreamWriter writeNestedStream(&serialisedXmlNested);
    writeNestedStream.writeStartDocument();
    writeNestedStream.writeStartElement(QStringLiteral("NestForModel"));
    serialiser.setPrintStartDocument(false);
    QVERIFY(serialiser.saveModel(writeNestedStream));
    writeNestedStream.writeEndElement(); // NestForModel
    serialisedXmlNested.close();
    QVERIFY(serialisedXmlNested.open(QIODevice::ReadOnly));
    QXmlStreamReader readNestedStream(&serialisedXmlNested);
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(readNestedStream));
    serialisedXmlNested.close();
    SerialiserCommon::modelEqual(sourceModel, destinationModel);

    sourceModel->deleteLater();
    destinationModel->deleteLater();
}

void tst_XmlModelSerialiser::basicSaveLoad_data()
{
    QTest::addColumn<QAbstractItemModel *>("sourceModel");
    QTest::addColumn<QAbstractItemModel *>("destinationModel");
    QTest::newRow("List Single Role") << SerialiserCommon::createStringModel() << static_cast<QAbstractItemModel *>(new QStringListModel());
#ifdef QT_GUI_LIB
    QTest::newRow("Table Single Role") << SerialiserCommon::createComplexModel(false, false)
                                       << static_cast<QAbstractItemModel *>(new QStandardItemModel());
    QTest::newRow("Table Multi Roles") << SerialiserCommon::createComplexModel(false, true)
                                       << static_cast<QAbstractItemModel *>(new QStandardItemModel());
    QTest::newRow("Tree Single Role") << SerialiserCommon::createComplexModel(true, false)
                                      << static_cast<QAbstractItemModel *>(new QStandardItemModel());
    QTest::newRow("Tree Multi Roles") << SerialiserCommon::createComplexModel(true, true)
                                      << static_cast<QAbstractItemModel *>(new QStandardItemModel());
#endif
}
