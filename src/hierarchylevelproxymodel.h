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
    Q_PROPERTY(InsertBehaviour insertBehaviour READ insertBehaviour WRITE setInsertBehaviour NOTIFY insertBehaviourChanged)
    Q_DISABLE_COPY_MOVE(HierarchyLevelProxyModel)
    Q_DECLARE_PRIVATE_D(m_dptr, HierarchyLevelProxyModel)
    HierarchyLevelProxyModelPrivate *m_dptr;

public:
    enum InsertBehaviour {
        InsertToPrevious
        , InsertToPreviousNonEmpty
        , InsertToNext
    };
    Q_ENUM(InsertBehaviour)
    explicit HierarchyLevelProxyModel(QObject *parent = nullptr);
    ~HierarchyLevelProxyModel();
    void setSourceModel(QAbstractItemModel *newSourceModel) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    void multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override;
    bool clearItemData(const QModelIndex &index) override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    /*bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent,
                  int destinationChild) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent,
                     int destinationChild) override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    */
    QModelIndex buddy(const QModelIndex &index) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;
    QModelIndexList match(const QModelIndex &start, int role, const QVariant &value, int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    QSize span(const QModelIndex &index) const override;
    int hierarchyLevel() const;
    void setHierarchyLevel(int hierarchyLvl);
    InsertBehaviour insertBehaviour() const;
    void setInsertBehaviour(InsertBehaviour behave);
protected:
    HierarchyLevelProxyModel(HierarchyLevelProxyModelPrivate &dptr, QObject *parent);
Q_SIGNALS:
    void hierarchyLevelChanged(int hierarchyLvl);
    void insertBehaviourChanged(InsertBehaviour behave);
};
#endif // HIERARCHYLEVELPROXY_H
