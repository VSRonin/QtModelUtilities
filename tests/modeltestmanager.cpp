#include "modeltestmanager.h"
#ifdef QT_DEBUG
QString printModel(const QAbstractItemModel *model, const QModelIndex &root, const QString &prefix)
{
    QString result;
    for (int i = 0; i < model->rowCount(root); ++i) {
        if (i > 0)
            result += QLatin1Char('\n');
        for (int j = 0; j < model->columnCount(root); ++j) {
            result += (j > 0 ? QStringLiteral("\t") : prefix) + model->index(i, j, root).data().toString();
        }
        if (model->hasChildren(model->index(i, 0, root))) {
            result += QLatin1Char('\n');
            result += printModel(model, model->index(i, 0, root), QStringLiteral("-->") + prefix);
        }
    }
    return result;
}
#endif
