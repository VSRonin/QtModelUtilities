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

#ifndef COLUMNSTOROLESPROXY_H
#define COLUMNSTOROLESPROXY_H
#include <modelutilities_global.h>
#include <QIdentityProxyModel>
#include <QVariant>
class ColumnsToRolesProxyModelPrivate;
class MODELUTILITIES_EXPORT ColumnsToRolesProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
    Q_DISABLE_COPY(ColumnsToRolesProxyModel)
    Q_DECLARE_PRIVATE_D(m_dptr, ColumnsToRolesProxyModel)
    ColumnsToRolesProxyModelPrivate *m_dptr;

public:
    explicit ColumnsToRolesProxyModel(QObject *parent = Q_NULLPTR);
    ~ColumnsToRolesProxyModel();
    void setSourceModel(QAbstractItemModel *newSourceModel) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) Q_DECL_OVERRIDE;
    QMap<int, QVariant> itemData(const QModelIndex &index) const Q_DECL_OVERRIDE;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) Q_DECL_OVERRIDE;

protected:
    ColumnsToRolesProxyModel(ColumnsToRolesProxyModelPrivate &dptr, QObject *parent);
};
#endif // COLUMNSTOROLESPROXY_H
