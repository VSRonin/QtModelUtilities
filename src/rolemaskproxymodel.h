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
#ifndef ROLEMASKPROXYMODEL_H
#define ROLEMASKPROXYMODEL_H
#include <modelutilities_global.h>
#include <QIdentityProxyModel>
#include <QList>
#include <QSet>
class RoleMaskProxyModelPrivate;
class MODELUTILITIES_EXPORT RoleMaskProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool transparentIfEmpty READ transparentIfEmpty WRITE setTransparentIfEmpty NOTIFY transparentIfEmptyChanged)
    Q_PROPERTY(bool mergeDisplayEdit READ mergeDisplayEdit WRITE setMergeDisplayEdit NOTIFY mergeDisplayEditChanged)
    Q_PROPERTY(bool maskHeaderData READ maskHeaderData WRITE setMaskHeaderData NOTIFY maskHeaderDataChanged)
    Q_PROPERTY(QList<int> maskedRoles READ maskedRoles WRITE setMaskedRoles NOTIFY maskedRolesChanged RESET clearMaskedRoles)
    Q_DISABLE_COPY(RoleMaskProxyModel)
    Q_DECLARE_PRIVATE(RoleMaskProxyModel)
public:
    explicit RoleMaskProxyModel(QObject *parent = Q_NULLPTR);
    ~RoleMaskProxyModel();
    QList<int> maskedRoles() const;
    void setMaskedRoles(const QList<int> &roles);
    void clearMaskedRoles();
    void addMaskedRole(int role);
    void removeMaskedRole(int role);
    void setSourceModel(QAbstractItemModel *sourceModel) override;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    void multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const override;
    bool clearItemData(const QModelIndex &index) override;
#endif
    QMap<int, QVariant> maskedItemData(const QModelIndex &index) const;
    void clearMaskedData(const QModelIndex &index);
    bool transparentIfEmpty() const;
    void setTransparentIfEmpty(bool val);
    bool mergeDisplayEdit() const;
    void setMergeDisplayEdit(bool val);
    bool maskHeaderData() const;
    void setMaskHeaderData(bool val);
Q_SIGNALS:
    void mergeDisplayEditChanged(bool val);
    void transparentIfEmptyChanged(bool val);
    void maskHeaderDataChanged(bool val);
    void maskedRolesChanged();
    void maskedDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

protected:
    const QSet<int> &maskedRolesSets() const;
    RoleMaskProxyModel(RoleMaskProxyModelPrivate &dptr, QObject *parent);

private:
    RoleMaskProxyModelPrivate *d_ptr;
};
#endif // ROLEMASKPROXYMODEL_H
