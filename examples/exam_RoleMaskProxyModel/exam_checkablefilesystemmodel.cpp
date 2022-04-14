// This example will demonstrate how to use RoleMaskProxyModel to leverage roles not supported by the source model.
// In the specific we will use Qt::CheckStateRole on a QFileSystemModel to allow selecting multiple files

#include <RoleMaskProxyModel>
#include <QFileSystemModel>
#include <QStandardPaths>
#include <QTreeView>
#include <QApplication>
#include <QDebug>

// we create a small subclass of RoleMaskProxyModel
class CheckableProxy : public RoleMaskProxyModel
{
public:
    explicit CheckableProxy(QObject *parent = nullptr)
        : RoleMaskProxyModel(parent)
    {
        // we will intercept all data in Qt::CheckStateRole and handle it in the proxy rather than passing it on to the source model
        addMaskedRole(Qt::CheckStateRole);

        // If we wanted checkboxes in all columns it would just be sufficient to disable the transparency of the proxy model
        // and assign Qt::Unchecked as the default value to make the checkbox appear
        // setTransparentIfEmpty(false);
        // setMaskedRoleDefaultValue(Qt::CheckStateRole, Qt::Unchecked);
    }
    // since we want the checkbox only on the first column we can reimplement data to return the default Qt::Unchecked if
    // no check state is set.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        const QVariant baseData = RoleMaskProxyModel::data(index, role);
        if (role == Qt::CheckStateRole && index.isValid() && index.column() == 0 && !baseData.isValid())
            return Qt::Unchecked;
        return baseData;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    // same as above but using multiData in Qt6
    void multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const override
    {
        RoleMaskProxyModel::multiData(index, roleDataSpan);
        for (QModelRoleData &roleData : roleDataSpan) {
            if (roleData.role() == Qt::CheckStateRole && index.column() == 0 && !roleData.data().isValid())
                roleData.setData(Qt::Unchecked);
        }
    }
#endif
    // we override the flags method to make the user able to interact with the checkbox
    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        if (index.isValid() && index.column() == 0)
            return RoleMaskProxyModel::flags(index) | Qt::ItemIsUserCheckable;
        return RoleMaskProxyModel::flags(index);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // Create the source model
    QFileSystemModel baseModel;

    // create the proxy model
    CheckableProxy checkableProxy;
    // set the source model
    checkableProxy.setSourceModel(&baseModel);

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
