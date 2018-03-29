#include "tst_insertproxymodel.h"
#include <QStringListModel>
#include <insertproxymodel.h>
#ifdef QT_GUI_LIB
#include <QStandardItemModel>
#endif
#include <QtTest/QTest>
void tst_InsertProxyModel::initTestCase()
{
    m_models.append(new QStringListModel(QStringList() << QStringLiteral("1") << QStringLiteral("2") << QStringLiteral("3") << QStringLiteral("4") << QStringLiteral("5"), this));
#ifdef QT_GUI_LIB
    m_models.append(new QStandardItemModel(this));
    m_models.last()->insertRows(0, 5);
    m_models.last()->insertColumns(0, 3);
    for (int i = 0; i < m_models.last()->rowCount(); ++i) {
        for (int j = 0; j < m_models.last()->columnCount(); ++j) {
            m_models.last()->setData(m_models.last()->index(i, j), QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            m_models.last()->setData(m_models.last()->index(i, j), i, Qt::UserRole);
            m_models.last()->setData(m_models.last()->index(i, j), j, Qt::UserRole + 1);
        }
    }

    m_models.append(new QStandardItemModel(this));
    m_models.last()->insertRows(0, 5);
    m_models.last()->insertColumns(0, 3);
    for (int i = 0; i < m_models.last()->rowCount(); ++i) {
        for (int j = 0; j < m_models.last()->columnCount(); ++j) {
            m_models.last()->setData(m_models.last()->index(i, j), QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            m_models.last()->setData(m_models.last()->index(i, j), i, Qt::UserRole);
            m_models.last()->setData(m_models.last()->index(i, j), j, Qt::UserRole + 1);
        }
        const QModelIndex parIdx = m_models.last()->index(i, 0);
        m_models.last()->insertRows(0, 5, parIdx);
        m_models.last()->insertColumns(0, 3, parIdx);
        for (int k = 0; k < m_models.last()->rowCount(parIdx); ++k) {
            for (int h = 0; h < m_models.last()->columnCount(parIdx); ++h) {
                m_models.last()->setData(m_models.last()->index(k, h, parIdx), QStringLiteral("%1,%2,%3").arg(i).arg(k).arg(h), Qt::EditRole);
                m_models.last()->setData(m_models.last()->index(k, h, parIdx), h, Qt::UserRole);
                m_models.last()->setData(m_models.last()->index(k, h, parIdx), k, Qt::UserRole + 1);
            }
        }
    }
#endif
}

void tst_InsertProxyModel::cleanupTestCase()
{
    while (!m_models.isEmpty())
        m_models.takeLast()->deleteLater();
}

void tst_InsertProxyModel::testBenchMark()
{
    QBENCHMARK{
        QStringLiteral("lohvbhjvbBIDBVHAFBVHbhjgdvcjghBVSBFDBa").toUpper();
    }
}
