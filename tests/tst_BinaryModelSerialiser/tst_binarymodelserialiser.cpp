#include <QtTest/QTest>
#include "tst_binarymodelserialiser.h"
#include <binarymodelserialiser.h>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>

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
    BinaryModelSerialiser serialiser(sourceModel);
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

void tst_BinaryModelSerialiser::basicSaveLoadData()
{
    QTest::addColumn<const QAbstractItemModel *>("sourceModel");
    QTest::addColumn<QAbstractItemModel *>("destinationModel");
    QTest::newRow("List Single Role") << createStringModel(this) << static_cast<QAbstractItemModel *>(new QStringListModel(this));
#ifdef QT_GUI_LIB
    QTest::newRow("Table Single Role") << createComplexModel(false, false, this) << static_cast<QAbstractItemModel *>(new QStandardItemModel(this));
    QTest::newRow("Table Multi Roles") << createComplexModel(false, true, this) << static_cast<QAbstractItemModel *>(new QStandardItemModel(this));
    QTest::newRow("Tree Single Role") << createComplexModel(true, false, this) << static_cast<QAbstractItemModel *>(new QStandardItemModel(this));
    QTest::newRow("Tree Multi Roles") << createComplexModel(true, true, this) << static_cast<QAbstractItemModel *>(new QStandardItemModel(this));
#endif
}
