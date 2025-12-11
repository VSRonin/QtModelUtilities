/****************************************************************************\
   Copyright 2025 Luca Beldi
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

#ifndef SUBTOTALPROXY_H
#define SUBTOTALPROXY_H
#include <modelutilities_global.h>
#include <QIdentityProxyModel>
class SubtotalProxyModelPrivate;
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
using SubtotalProxyModelBaseClass = QAbstractProxyModel;
#else
using SubtotalProxyModelBaseClass = QIdentityProxyModel;
#endif
class MODELUTILITIES_EXPORT SubtotalProxyModel : public SubtotalProxyModelBaseClass
{
    Q_OBJECT
    Q_PROPERTY(bool overwriteParentData READ overwriteParentData WRITE setOverwriteParentData NOTIFY overwriteParentDataChanged)
    Q_PROPERTY(bool recursive READ recursive WRITE setRecursive NOTIFY recursiveChanged)
    Q_PROPERTY(SubtotalLocations totalLocations READ totalLocations WRITE setTotalLocations NOTIFY totalLocationsChanged)
    Q_DISABLE_COPY(SubtotalProxyModel)
    Q_DECLARE_PRIVATE_D(m_dptr, SubtotalProxyModel)
public:
    using AggregateFunction = std::function<void(QVariant &, const QVariant &)>;
    enum SubtotalLocation { slNone = 0, slTop = 0x1, slBottom = 0x2, slOnParent = 0x4 };
    Q_DECLARE_FLAGS(SubtotalLocations, SubtotalLocation)
    explicit SubtotalProxyModel(QObject *parent = Q_NULLPTR);
    ~SubtotalProxyModel();
    virtual void setSubTotalColumn(int column, AggregateFunction aggregator, const QVariant &initialValue = QVariant(), int role = Qt::DisplayRole);
    virtual void removeSubtotalColumn(int column);
    virtual void removeSubtotalColumn(int column, int role);
    virtual bool overwriteParentData() const;
    virtual void setOverwriteParentData(bool overwrite);
    virtual bool recursive() const;
    virtual void setRecursive(bool recur);
    virtual const SubtotalLocations &totalLocations() const;
    virtual void setTotalLocations(const SubtotalLocations &locations);
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    void setSourceModel(QAbstractItemModel *sourceModel) override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    void multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const override;
    bool clearItemData(const QModelIndex &index) override;
#endif
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QModelIndex buddy(const QModelIndex &index) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool hasChildren(const QModelIndex &parent) const override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;
    bool moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent,
                     int destinationChild) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
Q_SIGNALS:
    void overwriteParentDataChanged(bool overwrite);
    void recursiveChanged(bool rec);
    void totalLocationsChanged(SubtotalProxyModel::SubtotalLocations locations);

protected:
    virtual Qt::ItemFlags flagsForTotalRow(const QModelIndex &index) const;
    SubtotalProxyModel(SubtotalProxyModelPrivate &dptr, QObject *parent);
    SubtotalProxyModelPrivate *m_dptr;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(SubtotalProxyModel::SubtotalLocations)
#endif // SUBTOTALPROXY_H
