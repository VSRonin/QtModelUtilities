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

#ifndef INSERTPROXY_H
#define INSERTPROXY_H
#include <modelutilities_global.h>
#include <QAbstractProxyModel>
#include <QVariant>
class InsertProxyModelPrivate;
class MODELUTILITIES_EXPORT InsertProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
    Q_PROPERTY(InsertDirections insertDirection READ insertDirection WRITE setInsertDirection NOTIFY insertDirectionChanged)
    Q_PROPERTY(bool mergeDisplayEdit READ mergeDisplayEdit WRITE setMergeDisplayEdit NOTIFY mergeDisplayEditChanged)
    Q_DISABLE_COPY(InsertProxyModel)
    Q_DECLARE_PRIVATE_D(m_dptr, InsertProxyModel)
    InsertProxyModelPrivate *m_dptr;
public:
    enum InsertDirection { NoInsert = 0x0, InsertRow = 0x1, InsertColumn = 0x2 };
    Q_DECLARE_FLAGS(InsertDirections, InsertDirection)
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    Q_FLAG(InsertDirections)
#else
    Q_FLAGS(InsertDirections)
#endif
    explicit InsertProxyModel(QObject *parent = Q_NULLPTR);
    ~InsertProxyModel();
    void setSourceModel(QAbstractItemModel *newSourceModel) Q_DECL_OVERRIDE;
    QModelIndex buddy(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) Q_DECL_OVERRIDE;
    QMap<int, QVariant> itemData(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent,
                  int destinationChild) Q_DECL_OVERRIDE;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent,
                     int destinationChild) Q_DECL_OVERRIDE;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) Q_DECL_OVERRIDE;
    InsertDirections insertDirection() const;
    void setInsertDirection(const InsertDirections &direction);
    virtual QVariant dataForCorner(int role = Qt::DisplayRole) const;
    virtual void setDataForCorner(const QVariant &value, int role = Qt::EditRole);
    bool mergeDisplayEdit() const;
    void setMergeDisplayEdit(bool val);
protected:
    virtual bool validRow() const;
    virtual bool validColumn() const;
    virtual Qt::ItemFlags flagForExtra(bool isRow, int section) const;
    InsertProxyModel(InsertProxyModelPrivate &dptr, QObject *parent);
public Q_SLOTS:
    virtual bool commitRow();
    virtual bool commitColumn();
Q_SIGNALS:
    void dataForCornerChanged(const QVector<int> &roles);
    void extraDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void mergeDisplayEditChanged(bool separate);
    void insertDirectionChanged(InsertDirections direction);
};
Q_DECLARE_METATYPE(InsertProxyModel::InsertDirections)
Q_DECLARE_OPERATORS_FOR_FLAGS(InsertProxyModel::InsertDirections)
#endif // INSERTPROXY_H
