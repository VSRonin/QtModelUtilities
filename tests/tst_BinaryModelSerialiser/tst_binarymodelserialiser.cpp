#include <QtTest/QTest>
#include "tst_binarymodelserialiser.h"
#include <../serialisers_common.hpp>
#include <binarymodelserialiser.h>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>

void tst_BinaryModelSerialiser::basicSaveLoad()
{
    QFETCH(QAbstractItemModel *, sourceModel);
    QFETCH(QAbstractItemModel *, destinationModel);
    QByteArray dataArray;
    BinaryModelSerialiser serialiser(static_cast<const QAbstractItemModel *>(sourceModel));
    serialiser.addRoleToSave(Qt::UserRole + 1);
    QVERIFY(serialiser.saveModel(&dataArray));
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(dataArray));
    SerialiserCommon::modelEqual(sourceModel, destinationModel);

    QVERIFY(destinationModel->removeRows(0, destinationModel->rowCount()));
    dataArray.clear();
    serialiser.setModel(static_cast<const QAbstractItemModel *>(sourceModel));
    QDataStream writeStream(&dataArray, QIODevice::WriteOnly);
    QVERIFY(serialiser.saveModel(writeStream));
    QDataStream readStream(dataArray);
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(readStream));
    SerialiserCommon::modelEqual(sourceModel, destinationModel);

    QVERIFY(destinationModel->removeRows(0, destinationModel->rowCount()));
    dataArray.clear();
    serialiser.setModel(static_cast<const QAbstractItemModel *>(sourceModel));
    QBuffer writeBuff(&dataArray);
    QVERIFY(writeBuff.open(QIODevice::WriteOnly));
    QVERIFY(serialiser.saveModel(&writeBuff));
    writeBuff.close();
    QBuffer readBuff(&dataArray);
    QVERIFY(readBuff.open(QIODevice::ReadOnly));
    serialiser.setModel(destinationModel);
    QVERIFY(serialiser.loadModel(&readBuff));
    readBuff.close();
    SerialiserCommon::modelEqual(sourceModel, destinationModel);

    sourceModel->deleteLater();
    destinationModel->deleteLater();
}

void tst_BinaryModelSerialiser::basicSaveLoad_data()
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
