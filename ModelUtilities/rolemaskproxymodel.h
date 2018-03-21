/**********************************************************************************\

Copyright 2018 Luca Beldi

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

\**********************************************************************************/
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
