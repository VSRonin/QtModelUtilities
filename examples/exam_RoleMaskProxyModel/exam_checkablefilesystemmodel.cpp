// This example will demonstrate how to use RoleMaskProxyModel to leverage roles not supported by the source model.
// In the specific we will use Qt::CheckStateRole on a QFileSystemModel to allow selecting multiple files
// We will also demonstrate how to override the flags() of the source model

#include <RoleMaskProxyModel>
#include <QFileSystemModel>
#include <QStandardPaths>
#include <QTreeView>
#include <QApplication>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // Create the source model
    QFileSystemModel baseModel;

    // create the proxy model
    RoleMaskProxyModel checkableProxy;
    // set the source model
    checkableProxy.setSourceModel(&baseModel);
    // we will intercept all data in Qt::CheckStateRole and handle it in the proxy rather than passing it on to the source model
    checkableProxy.addMaskedRole(Qt::CheckStateRole);

    QObject::connect(&baseModel, &QFileSystemModel::directoryLoaded, [&checkableProxy, &baseModel](const QString &path) {
        // when the source model loads a new folder we iterate over all the files and make them checkable by the user
        // first of all we get the parent index of the folder
        const QModelIndex sourceParent = baseModel.index(path);
        // we iterate over the children
        for (int i = 0, iEnd = baseModel.rowCount(sourceParent); i != iEnd; ++i) {
            const QModelIndex sourceChild = baseModel.index(i, 0, sourceParent);
            // if it's a directory we skip it
            if (baseModel.isDir(sourceChild))
                continue;
            // map the index to the proxy
            const QModelIndex proxyChild = checkableProxy.mapFromSource(sourceChild);
            // make the checkbox appear
            checkableProxy.setData(proxyChild, Qt::Unchecked, Qt::CheckStateRole);
            // make the user able to interact with the checkbox
            checkableProxy.setMaskedFlags(proxyChild, baseModel.flags(sourceChild) | Qt::ItemIsUserCheckable);
        }
    });

    // we load a folder in the QFileSystemModel
    const QStringList docsLocations = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    baseModel.setRootPath(docsLocations.first());

    // we display the proxy model in a tree view
    QTreeView mainView;
    mainView.setWindowTitle(QStringLiteral("RoleMaskProxyModel Example - Checkable QFileSystemModel"));
    mainView.setModel(&checkableProxy);
    mainView.show();
    return app.exec();
}
