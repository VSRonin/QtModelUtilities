#include <QtTest/QTest>
#include "tst_htmlmodelserialiser.h"
#include <../serialisers_common.hpp>
#include <htmlmodelserialiser.h>
#include <QByteArray>
#include <QFile>
#include <QTextStream>

void tst_HtmlModelSerialiser::basicSaveLoad()
{
    QFETCH(QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    QByteArray dataArray;
    HtmlModelSerialiser serialiser(static_cast<const QAbstractItemModel *>(sourceModel));
    serialiser.addRoleToSave(Qt::UserRole + 1);
    QVERIFY(serialiser.saveModel(&dataArray));
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(dataArray));
    SerialiserCommon::modelEqual(sourceModel, destinationModel);

    QVERIFY(destinationModel->removeRows(0, destinationModel->rowCount()));
    serialiser.setModel(static_cast<const QAbstractItemModel *>(sourceModel));
    QFile serialisedHtmlDevice(QStringLiteral("Serialised Html ") + QTest::currentDataTag() + QStringLiteral(".html"));
    QVERIFY(serialisedHtmlDevice.open(QIODevice::WriteOnly));
    QVERIFY(serialiser.saveModel(&serialisedHtmlDevice));
    serialisedHtmlDevice.close();
    QVERIFY(serialisedHtmlDevice.open(QIODevice::ReadOnly));
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(&serialisedHtmlDevice));
    serialisedHtmlDevice.close();
    SerialiserCommon::modelEqual(sourceModel, destinationModel);

    QVERIFY(destinationModel->removeRows(0, destinationModel->rowCount()));
    serialiser.setModel(static_cast<const QAbstractItemModel *>(sourceModel));
    QFile serialisedHtmlNested(QStringLiteral("Serialised Html Nested ") + QTest::currentDataTag() + QStringLiteral(".html"));
    QVERIFY(serialisedHtmlNested.open(QIODevice::WriteOnly));
    serialiser.setPrintStartDocument(false);
    QTextStream nestStream(&serialisedHtmlNested);
    nestStream << QStringLiteral("<!DOCTYPE html><html><head><title>Nested Model</title><style>table { border-collapse: collapse; width: 100%; } th, "
                                 "td { text-align: left; padding: 8px; } tr:nth-child(even){background-color: #f2f2f2} th { background-color: "
                                 "#4CAF50; color: white; }</style></head><body><h2>Actual Model</h2>");
    QString modelString;
    QVERIFY(serialiser.saveModel(&modelString));
    nestStream << modelString << QStringLiteral("</body></html>");
    serialisedHtmlNested.close();
    QVERIFY(serialisedHtmlNested.open(QIODevice::ReadOnly));
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(&serialisedHtmlNested));
    serialisedHtmlNested.close();
    SerialiserCommon::modelEqual(sourceModel, destinationModel);

    sourceModel->deleteLater();
    destinationModel->deleteLater();
}

void tst_HtmlModelSerialiser::basicSaveLoad_data()
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
