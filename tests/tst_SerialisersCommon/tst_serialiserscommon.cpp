#include "tst_serialiserscommon.h"
#include <abstractmodelserialiser.h>
#include <abstractstringserialiser.h>
#include <QTemporaryFile>
#include <algorithm>
#include <QtTest/QTest>
void tst_SerialiserCommon::saveLoadByteArray(AbstractModelSerialiser *serialiser, const QAbstractItemModel *sourceModel,
                                             QAbstractItemModel *destinationModel, bool multiRole, bool checkHeaders) const
{
    serialiser->setModel(sourceModel);
    if (multiRole)
        serialiser->addRoleToSave(Qt::UserRole + 1);
    QByteArray dataArray;
    QVERIFY(serialiser->saveModel(&dataArray));
    serialiser->setModel(destinationModel);
    QVERIFY(serialiser->loadModel(dataArray));
    checkModelEqual(sourceModel, destinationModel, QModelIndex(), QModelIndex(), checkHeaders);
}

void tst_SerialiserCommon::saveLoadFile(AbstractModelSerialiser *serialiser, const QAbstractItemModel *sourceModel,
                                        QAbstractItemModel *destinationModel, bool multiRole, bool checkHeaders) const
{
    serialiser->setModel(sourceModel);
    if (multiRole)
        serialiser->addRoleToSave(Qt::UserRole + 1);
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QVERIFY(serialiser->saveModel(&tempFile));
    QVERIFY(tempFile.seek(0));
    serialiser->setModel(destinationModel);
    QVERIFY(serialiser->loadModel(&tempFile));
    checkModelEqual(sourceModel, destinationModel, QModelIndex(), QModelIndex(), checkHeaders);
}

void tst_SerialiserCommon::saveLoadString(AbstractStringSerialiser *serialiser, const QAbstractItemModel *sourceModel,
                                          QAbstractItemModel *destinationModel, bool multiRole, bool checkHeaders) const
{
    serialiser->setModel(sourceModel);
    if (multiRole)
        serialiser->addRoleToSave(Qt::UserRole + 1);
    QString dataString;
    QVERIFY(serialiser->saveModel(&dataString));
    serialiser->setModel(destinationModel);
    QVERIFY(serialiser->loadModel(dataString));
    checkModelEqual(sourceModel, destinationModel, QModelIndex(), QModelIndex(), checkHeaders);
}

void tst_SerialiserCommon::checkModelEqual(const QAbstractItemModel *a, const QAbstractItemModel *b, const QModelIndex &aParent,
                                           const QModelIndex &bParent, bool checkHeaders) const
{
    Q_ASSERT(a);
    Q_ASSERT(b);
    const bool aParentValid = aParent.isValid();
    Q_ASSERT(!aParentValid || aParent.model() == a);
    Q_ASSERT(!bParent.isValid() || bParent.model() == b);
    Q_ASSERT(aParentValid == bParent.isValid());
    const int rowC = a->rowCount(aParent);
    QCOMPARE(b->rowCount(bParent), rowC);
    const int colC = a->columnCount(aParent);
    QCOMPARE(b->columnCount(bParent), colC);
    if (!aParentValid && checkHeaders) {
        for (int i = 0; i < rowC; ++i) {
            QCOMPARE(a->headerData(i, Qt::Vertical), b->headerData(i, Qt::Vertical));
            QCOMPARE(a->headerData(i, Qt::Vertical, Qt::UserRole), b->headerData(i, Qt::Vertical, Qt::UserRole));
        }
        for (int i = 0; i < colC; ++i) {
            QCOMPARE(a->headerData(i, Qt::Horizontal), b->headerData(i, Qt::Horizontal));
            QCOMPARE(a->headerData(i, Qt::Horizontal, Qt::UserRole), b->headerData(i, Qt::Horizontal, Qt::UserRole));
        }
    }
    for (int i = 0; i < rowC; ++i) {
        for (int j = 0; j < colC; ++j) {
            const QModelIndex aIdx = a->index(i, j, aParent);
            const QModelIndex bIdx = b->index(i, j, bParent);
            const auto aItemData = a->itemData(aIdx);
            const auto bItemData = b->itemData(bIdx);
            QCOMPARE(bItemData.keys(), aItemData.keys());
            QCOMPARE(bItemData, aItemData);
            const bool hasChildren = a->hasChildren(aIdx);
            QCOMPARE(b->hasChildren(bIdx), hasChildren);
            if (hasChildren) {
                checkModelEqual(a, b, aIdx, bIdx, checkHeaders);
            }
        }
    }
}

QAbstractItemModel *tst_SerialiserCommon::createStringModel(QObject *parent)
{
    std::uniform_int_distribution<int> rowsDist(20, 50);
    std::uniform_int_distribution<int> itemsDist(0, 1000);

    QStringList itmesList;
    for (int i = rowsDist(generator); i > 0; --i)
        itmesList.append(QStringLiteral("Item ") + QString::number(itemsDist(generator)));
    return new QStringListModel(itmesList, parent);
}
#ifdef QT_GUI_LIB
void tst_SerialiserCommon::insertBranch(QAbstractItemModel *model, const QModelIndex &parent, bool multiRoles, int subBranches)
{
    Q_ASSERT(model);
    Q_ASSERT(!parent.isValid() || parent.model() == model);
    const QBrush randomBrushes[] = {QBrush(Qt::red), QBrush(Qt::blue), QBrush(Qt::green), QBrush(Qt::yellow), QBrush(Qt::magenta), QBrush(Qt::cyan)};
    std::uniform_int_distribution<int> colorDistribution(0, (sizeof(randomBrushes) / sizeof(randomBrushes[0])) - 1);
    std::uniform_int_distribution<int> coulmnsDist(2, 5);
    std::uniform_int_distribution<int> rowsDist(3, 6);
    Q_ASSUME(model->insertColumns(0, coulmnsDist(generator), parent));
    Q_ASSUME(model->insertRows(0, rowsDist(generator), parent));
    const QString baseString = parent.isValid() ? (parent.data(Qt::EditRole).toString() + QStringLiteral("->")) : QString();
    for (int i = 0; i < model->rowCount(parent); ++i) {
        for (int j = 0; j < model->columnCount(parent); ++j) {
            const QModelIndex idx = model->index(i, j, parent);
            model->setData(idx, baseString + QStringLiteral("%1,%2[%3]").arg(i).arg(j).arg(colorDistribution(generator)), Qt::EditRole);
            if (multiRoles) {
                model->setData(idx, randomBrushes[colorDistribution(generator)], Qt::BackgroundRole);
                model->setData(idx, randomBrushes[colorDistribution(generator)], Qt::ForegroundRole);
                model->setData(idx, i, Qt::UserRole);
                model->setData(idx, j, Qt::UserRole + 1);
            }
            if (subBranches > 0)
                insertBranch(model, idx, multiRoles, subBranches - 1);
        }
    }
}

QAbstractItemModel *tst_SerialiserCommon::createComplexModel(bool tree, bool multiRoles, QObject *parent)
{
    QStandardItemModel *result = new QStandardItemModel(parent);
    insertBranch(result, QModelIndex(), multiRoles, tree ? 2 : 0);
    for (int i = 0; i < result->rowCount(); ++i) {
        result->setHeaderData(i, Qt::Vertical, QStringLiteral("Row %1").arg(i));
        if (multiRoles)
            result->setHeaderData(i, Qt::Vertical, i, Qt::UserRole);
    }
    for (int i = 0; i < result->columnCount(); ++i) {
        result->setHeaderData(i, Qt::Horizontal, QStringLiteral("Column %1").arg(i));
        if (multiRoles)
            result->setHeaderData(i, Qt::Horizontal, i, Qt::UserRole);
    }
    return result;
}
#endif
void tst_SerialiserCommon::basicSaveLoadData(QObject *parent)
{
    QTest::addColumn<const QAbstractItemModel *>("sourceModel");
    QTest::addColumn<QAbstractItemModel *>("destinationModel");
    QTest::newRow("List Single Role") << static_cast<const QAbstractItemModel *>(createStringModel(parent))
                                      << static_cast<QAbstractItemModel *>(new QStringListModel(parent));
    QTest::newRow("List Single Role Overwrite") << static_cast<const QAbstractItemModel *>(createStringModel(parent)) << createStringModel(parent);
#ifdef QT_GUI_LIB
    QTest::newRow("Table Single Role") << static_cast<const QAbstractItemModel *>(createComplexModel(false, false, parent))
                                       << static_cast<QAbstractItemModel *>(new QStandardItemModel(parent));
    QTest::newRow("Table Multi Roles") << static_cast<const QAbstractItemModel *>(createComplexModel(false, true, parent))
                                       << static_cast<QAbstractItemModel *>(new QStandardItemModel(parent));
    QTest::newRow("Table Single Role Overwrite") << static_cast<const QAbstractItemModel *>(createComplexModel(false, false, parent))
                                                 << createComplexModel(false, false, parent);
    QTest::newRow("Table Multi Roles Overwrite") << static_cast<const QAbstractItemModel *>(createComplexModel(false, true, parent))
                                                 << createComplexModel(false, true, parent);
    QTest::newRow("Tree Single Role") << static_cast<const QAbstractItemModel *>(createComplexModel(true, false, parent))
                                      << static_cast<QAbstractItemModel *>(new QStandardItemModel(parent));
    QTest::newRow("Tree Multi Roles") << static_cast<const QAbstractItemModel *>(createComplexModel(true, true, parent))
                                      << static_cast<QAbstractItemModel *>(new QStandardItemModel(parent));
    QTest::newRow("Tree Single Role Overwrite") << static_cast<const QAbstractItemModel *>(createComplexModel(true, false, parent))
                                                << createComplexModel(true, false, parent);
    QTest::newRow("Tree Multi Roles Overwrite") << static_cast<const QAbstractItemModel *>(createComplexModel(true, true, parent))
                                                << createComplexModel(true, true, parent);
#endif
}
