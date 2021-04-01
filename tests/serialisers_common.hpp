#ifndef serialisers_common_h__
#define serialisers_common_h__
#include <QAbstractItemModel>
#include <QStringListModel>
#include <QtTest/QTest>
#include <random>
#ifdef QT_GUI_LIB
#    include <QStandardItemModel>
#endif
namespace SerialiserCommon {
void modelEqual(const QAbstractItemModel *a, const QAbstractItemModel *b, const QModelIndex &aParent = QModelIndex(),
                const QModelIndex &bParent = QModelIndex())
{
    Q_ASSERT(a);
    Q_ASSERT(b);
    Q_ASSERT(!aParent.isValid() || aParent.model() == a);
    Q_ASSERT(!bParent.isValid() || bParent.model() == b);
    const int rowC = a->rowCount(aParent);
    QCOMPARE(b->rowCount(bParent), rowC);
    const int colC = a->columnCount(aParent);
    QCOMPARE(b->columnCount(bParent), colC);
    for (int i = 0; i < rowC; ++i) {
        for (int j = 0; j < colC; ++j) {
            const QModelIndex aIdx = a->index(i, j, aParent);
            const QModelIndex bIdx = b->index(i, j, bParent);
            QCOMPARE(b->itemData(bIdx), a->itemData(aIdx));
            const bool hasChildren = a->hasChildren(aIdx);
            QCOMPARE(b->hasChildren(bIdx), hasChildren);
            if (hasChildren) {
                modelEqual(a, b, aIdx, bIdx);
            }
        }
    }
}
QAbstractItemModel *createStringModel(QObject *parent = nullptr)
{
    const QStringList euStates{QStringLiteral("Austria"),
                               QStringLiteral("Belgium"),
                               QStringLiteral("Bulgaria"),
                               QStringLiteral("Croatia"),
                               QStringLiteral("Republic of Cyprus"),
                               QStringLiteral("Czech Republic"),
                               QStringLiteral("Denmark"),
                               QStringLiteral("Estonia"),
                               QStringLiteral("Finland"),
                               QStringLiteral("France"),
                               QStringLiteral("Germany"),
                               QStringLiteral("Greece"),
                               QStringLiteral("Hungary"),
                               QStringLiteral("Ireland"),
                               QStringLiteral("Italy"),
                               QStringLiteral("Latvia"),
                               QStringLiteral("Lithuania"),
                               QStringLiteral("Luxembourg"),
                               QStringLiteral("Malta"),
                               QStringLiteral("Netherlands"),
                               QStringLiteral("Poland"),
                               QStringLiteral("Portugal"),
                               QStringLiteral("Romania"),
                               QStringLiteral("Slovakia"),
                               QStringLiteral("Slovenia"),
                               QStringLiteral("Spain"),
                               QStringLiteral("Sweden")};
    return new QStringListModel(euStates, parent);
}
#ifdef QT_GUI_LIB
void insertBranch(QAbstractItemModel *model, const QModelIndex &parent, bool multiRoles, int subBranches)
{
    Q_ASSERT(model);
    Q_ASSERT(!parent.isValid() || parent.model() == model);
    const QBrush randomBrushes[] = {QBrush(Qt::red), QBrush(Qt::blue), QBrush(Qt::green), QBrush(Qt::yellow), QBrush(Qt::magenta), QBrush(Qt::cyan)};
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, (sizeof(randomBrushes) / sizeof(randomBrushes[0])) - 1);
    model->insertColumns(0, 3, parent);
    model->insertRows(0, 5, parent);
    const QString baseString = parent.isValid() ? (parent.data(Qt::EditRole).toString() + QStringLiteral("->")) : QString();
    for (int i = 0; i < model->rowCount(parent); ++i) {
        for (int j = 0; j < model->columnCount(parent); ++j) {
            const QModelIndex idx = model->index(i, j, parent);
            model->setData(idx, baseString + QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            if (multiRoles) {
                model->setData(idx, randomBrushes[distribution(generator)], Qt::BackgroundRole);
                model->setData(idx, randomBrushes[distribution(generator)], Qt::ForegroundRole);
                model->setData(idx, i, Qt::UserRole);
                model->setData(idx, j, Qt::UserRole + 1);
            }
            if (subBranches > 0)
                insertBranch(model, idx, multiRoles, subBranches - 1);
        }
    }
}
QAbstractItemModel *createComplexModel(bool tree, bool multiRoles, QObject *parent = nullptr)
{
    QStandardItemModel *result = new QStandardItemModel(parent);
    insertBranch(result, QModelIndex(), multiRoles, tree ? 2 : 0);
    return result;
}
#endif
}
#endif
