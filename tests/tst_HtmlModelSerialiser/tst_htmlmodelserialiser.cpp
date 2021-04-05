#include <QtTest/QTest>
#include "tst_htmlmodelserialiser.h"
#include <htmlmodelserialiser.h>
#include <QByteArray>
#include <QBuffer>
#include <QTextStream>

void tst_HtmlModelSerialiser::basicSaveLoadByteArray()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    HtmlModelSerialiser serialiser;
    saveLoadByteArray(&serialiser, sourceModel, destinationModel, true);
    destinationModel->deleteLater();
}

void tst_HtmlModelSerialiser::basicSaveLoadFile()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    HtmlModelSerialiser serialiser;
    saveLoadFile(&serialiser, sourceModel, destinationModel, true);
    destinationModel->deleteLater();
}

void tst_HtmlModelSerialiser::basicSaveLoadString()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    HtmlModelSerialiser serialiser;
    saveLoadString(&serialiser, sourceModel, destinationModel, true);
    destinationModel->deleteLater();
}

void tst_HtmlModelSerialiser::basicSaveLoadNested()
{
    QFETCH(const QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    HtmlModelSerialiser serialiser(sourceModel);
    serialiser.addRoleToSave(Qt::UserRole + 1);
    QByteArray dataArray;
    QBuffer serialisedHtmlNested(&dataArray);
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
    checkModelEqual(sourceModel, destinationModel);

    destinationModel->deleteLater();
}
