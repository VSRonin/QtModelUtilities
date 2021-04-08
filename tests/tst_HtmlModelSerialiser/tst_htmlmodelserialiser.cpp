#include <QtTest/QTest>
#include "tst_htmlmodelserialiser.h"
#include <htmlmodelserialiser.h>
#include <QByteArray>
#include <QBuffer>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#ifdef QT_NETWORK_LIB
#    include <QNetworkAccessManager>
#    include <QNetworkReply>
#    include <QNetworkRequest>
#endif

void tst_HtmlModelSerialiser::initTestCase()
{
#ifdef QT_NETWORK_LIB
    qnam = new QNetworkAccessManager(this);
#endif
}

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

void tst_HtmlModelSerialiser::validateHtmlOutput()
{
#ifdef QT_NETWORK_LIB
    QFETCH(const QAbstractItemModel *, sourceModel);
    HtmlModelSerialiser serialiser(sourceModel);
    serialiser.addRoleToSave(Qt::UserRole + 1);
    QByteArray htmlData;
    QVERIFY(serialiser.saveModel(&htmlData));
    QNetworkReply *validateReply = nullptr;
    for (int attempt = 3; attempt > 0; --attempt) {
        QNetworkRequest validateReq(QUrl(QStringLiteral("https://validator.w3.org/nu/?out=json")));
        validateReq.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("text/html; charset=utf-8"));
        validateReply = qnam->post(validateReq, htmlData);
        QEventLoop replyWaitLoop;
        QObject::connect(validateReply, &QNetworkReply::finished, &replyWaitLoop, &QEventLoop::quit);
#    if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
        QObject::connect(validateReply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), &replyWaitLoop,
                         &QEventLoop::quit);
#    else
        QObject::connect(validateReply, &QNetworkReply::errorOccurred, &replyWaitLoop, &QEventLoop::quit);
#    endif
        QObject::connect(validateReply, &QNetworkReply::sslErrors, validateReply,
                         static_cast<void (QNetworkReply::*)()>(&QNetworkReply::ignoreSslErrors));
        replyWaitLoop.exec();
        if (validateReply->error() == QNetworkReply::NoError)
            break;
    }
    if (validateReply->error() != QNetworkReply::NoError)
        QSKIP("Communication with online html validator failed");
    const QByteArray replyData = validateReply->readAll();
    QJsonParseError parseError;
    const QJsonDocument replyDoc = QJsonDocument::fromJson(replyData, &parseError);
    QCOMPARE(parseError.error, QJsonParseError::NoError);
    QVERIFY(replyDoc.isObject());
    const QJsonObject replyObj = replyDoc.object();
    QVERIFY(replyObj.contains(QStringLiteral("messages")));
    QVERIFY(replyObj.value(QStringLiteral("messages")).isArray());
    const QJsonArray msgArr = replyObj.value(QStringLiteral("messages")).toArray();
    for (auto &&i : msgArr) {
        QVERIFY(i.isObject());
        const QJsonObject msgObject = i.toObject();
        QVERIFY(msgObject.contains(QStringLiteral("type")));
        QVERIFY(msgObject.value(QStringLiteral("type")).isString());
        const QString typeString = msgObject.value(QStringLiteral("type")).toString();
        QVERIFY(typeString.compare(QStringLiteral("error"), Qt::CaseInsensitive) != 0);
        QVERIFY(typeString.compare(QStringLiteral("non-document-error"), Qt::CaseInsensitive) != 0);
    }
#else
    QSKIP("This test requires the Qt Network module");
#endif
}

void tst_HtmlModelSerialiser::validateHtmlOutput_data()
{
    QTest::addColumn<const QAbstractItemModel *>("sourceModel");
    QTest::newRow("List Single Role") << static_cast<const QAbstractItemModel *>(createStringModel(this));
#ifdef QT_GUI_LIB
    QTest::newRow("Tree Multi Roles") << static_cast<const QAbstractItemModel *>(createComplexModel(true, true, this));
    QTest::newRow("Table Single Role") << static_cast<const QAbstractItemModel *>(createComplexModel(false, false, this));
    QTest::newRow("Table Multi Roles") << static_cast<const QAbstractItemModel *>(createComplexModel(false, true, this));
    QTest::newRow("Tree Single Role") << static_cast<const QAbstractItemModel *>(createComplexModel(true, false, this));
    QTest::newRow("abvbavole") << static_cast<const QAbstractItemModel *>(createStringModel(this));
#endif
}
