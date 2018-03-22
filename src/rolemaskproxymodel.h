#ifndef rolemaskproxymodel_h__
#define rolemaskproxymodel_h__
#include "modelutilities_global.h"
#include <QIdentityProxyModel>
#include <QList>
class RoleMaskProxyModelPrivate;
class MODELUTILITIES_EXPORT RoleMaskProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool transparentIfEmpty READ transparentIfEmpty WRITE setTransparentIfEmpty NOTIFY transparentIfEmptyChanged)
    Q_PROPERTY(bool mergeDisplayEdit READ mergeDisplayEdit WRITE setMergeDisplayEdit NOTIFY mergeDisplayEditChanged)
    Q_PROPERTY(QList<int> maskedRoles READ maskedRoles WRITE setMaskedRoles NOTIFY maskedRolesChanged RESET clearMaskedRoles)
    Q_DISABLE_COPY(RoleMaskProxyModel)
    Q_DECLARE_PRIVATE(RoleMaskProxyModel)
public:
    explicit RoleMaskProxyModel(QObject* parent = Q_NULLPTR);
    ~RoleMaskProxyModel();
    QList<int> maskedRoles() const;
    void setMaskedRoles(const QList<int>& roles);
    void clearMaskedRoles();
    Q_SIGNAL void maskedRolesChanged();
    void addMaskedRole(int role);
    void removeMaskedRole(int role);
    void setSourceModel(QAbstractItemModel *sourceModel) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    bool transparentIfEmpty() const;
    void setTransparentIfEmpty(bool val);
    Q_SIGNAL void transparentIfEmptyChanged(bool val);
    bool mergeDisplayEdit() const;
    void setMergeDisplayEdit(bool val);
    Q_SIGNAL void mergeDisplayEditChanged(bool val);
private:
    RoleMaskProxyModelPrivate* d_ptr;
};
#endif // rolemaskproxymodel_h__
