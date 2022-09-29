/****************************************************************************\
   Copyright 2022 Luca Beldi
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

#ifndef HIERARCHYLEVELPROXY_H
#define HIERARCHYLEVELPROXY_H
#include <modelutilities_global.h>
#include <QAbstractProxyModel>
#include <QVariant>
class HierarchyLevelProxyModelPrivate;
class MODELUTILITIES_EXPORT HierarchyLevelProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int hierarchyLevel READ hierarchyLevel WRITE setHierarchyLevel NOTIFY hierarchyLevelChanged)
    Q_DISABLE_COPY_MOVE(HierarchyLevelProxyModel)
    Q_DECLARE_PRIVATE_D(m_dptr, HierarchyLevelProxyModel)
    HierarchyLevelProxyModelPrivate *m_dptr;

public:
    explicit HierarchyLevelProxyModel(QObject *parent = nullptr);
    ~HierarchyLevelProxyModel();
    void setSourceModel(QAbstractItemModel *newSourceModel) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;
#if (QT_VERSION < QT_VERSION_CHECK(6, 3, 0))
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override;
#endif
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    /*
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent,
                  int destinationChild) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent,
                     int destinationChild) override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    */
    int hierarchyLevel() const;
    void setHierarchyLevel(int hierarchyLvl);
protected:
    HierarchyLevelProxyModel(HierarchyLevelProxyModelPrivate &dptr, QObject *parent);
Q_SIGNALS:
    void hierarchyLevelChanged(int hierarchyLvl);
};
#endif // HIERARCHYLEVELPROXY_H
