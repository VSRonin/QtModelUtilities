#include "tst_transposeproxymodel.h"
#include <transposeproxymodel.h>
#include <QtTest/QTest>
#ifdef QT_GUI_LIB
#include <QStandardItemModel>
#endif
void tst_TransposeProxyModel::basicTestTable()
{
#ifdef QT_GUI_LIB
    QStandardItemModel model;
    model.insertRows(0, 2);
    model.insertColumns(0, 2);
    for (int i = 0; i < model.rowCount(); ++i) {
        for (int j = 0; j < model.columnCount(); ++j) {
            model.setData(model.index(i, j), QStringLiteral("%1,%2").arg(i).arg(j), Qt::EditRole);
            model.setData(model.index(i, j), i, Qt::UserRole);
            model.setData(model.index(i, j), j, Qt::UserRole + 1);
        }
    }
    TransposeProxyModel proxy;
    proxy.setSourceModel(&model);
    QCOMPARE(proxy.hasChildren(), model.hasChildren());
    QCOMPARE(proxy.columnCount(), model.rowCount());
    QCOMPARE(proxy.rowCount(), model.columnCount());
    for (int i = 0; i < model.rowCount(); ++i) {
        for (int j = 0; j < model.columnCount(); ++j) {
            QCOMPARE(proxy.index(i, j).data(), model.index(j, i).data());
        }
    }
#else
    QSKIP("This test requires the Qt GUI module");
#endif

}

