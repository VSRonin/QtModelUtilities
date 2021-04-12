/****************************************************************************\
   Copyright 2021 Luca Beldi
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

#ifndef ROOTINDEXPROXY_H
#define ROOTINDEXPROXY_H
#include "modelutilities_global.h"
#include <QIdentityProxyModel>
class RootIndexProxyModelPrivate;
class MODELUTILITIES_EXPORT RootIndexProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QModelIndex rootIndex READ rootIndex WRITE setRootIndex NOTIFY rootIndexChanged)
    Q_DISABLE_COPY(RootIndexProxyModel)
    Q_DECLARE_PRIVATE_D(m_dptr, RootIndexProxyModel)
public:
    explicit RootIndexProxyModel(QObject *parent = Q_NULLPTR);
    ~RootIndexProxyModel();
    QModelIndex rootIndex() const;
    void setRootIndex(const QModelIndex &root);
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override;
    QSize span(const QModelIndex &index) const override;
    void setSourceModel(QAbstractItemModel *sourceModel) override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override;
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool hasChildren(const QModelIndex &parent) const override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;
    bool moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent, int destinationChild) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
Q_SIGNALS:
    void rootIndexChanged();

protected:
    RootIndexProxyModel(RootIndexProxyModelPrivate &dptr, QObject *parent);
    RootIndexProxyModelPrivate *m_dptr;
};
#endif // ROOTINDEXPROXY_H
