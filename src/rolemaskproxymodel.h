/****************************************************************************\
   Copyright 2018 Luca Beldi
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
\****************************************************************************/
#ifndef rolemaskproxymodel_h__
#define rolemaskproxymodel_h__
#include "modelutilities_global.h"
#include <QIdentityProxyModel>
#include <QList>
#include <QSet>
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
    void addMaskedRole(int role);
    void removeMaskedRole(int role);
    void setSourceModel(QAbstractItemModel *sourceModel) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) Q_DECL_OVERRIDE;
    QMap<int, QVariant> itemData(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QMap<int, QVariant> maskedItemData(const QModelIndex &index) const;
    void clearMaskedData(const QModelIndex &index);
    bool transparentIfEmpty() const;
    void setTransparentIfEmpty(bool val);
    bool mergeDisplayEdit() const;
    void setMergeDisplayEdit(bool val);
Q_SIGNALS:
    void mergeDisplayEditChanged(bool val);
    void transparentIfEmptyChanged(bool val);
    void maskedRolesChanged();
    void maskedDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int>& roles);
protected:
    const QSet<int>& maskedRolesSets() const;
    RoleMaskProxyModel(RoleMaskProxyModelPrivate& dptr, QObject* parent);
private:
    RoleMaskProxyModelPrivate* d_ptr;
};
#endif // rolemaskproxymodel_h__
