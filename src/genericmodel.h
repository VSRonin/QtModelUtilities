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

#ifndef GENERICMODEL_H
#define GENERICMODEL_H
#include <modelutilities_global.h>
#include <QAbstractItemModel>
#include <QVariant>
class GenericModelPrivate;
class MODELUTILITIES_EXPORT GenericModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(bool mergeDisplayEdit READ mergeDisplayEdit WRITE setMergeDisplayEdit NOTIFY mergeDisplayEditChanged)
    Q_PROPERTY(int sortRole READ sortRole WRITE setSortRole NOTIFY sortRoleChanged)
    Q_DISABLE_COPY(GenericModel)
    Q_DECLARE_PRIVATE_D(m_dptr, GenericModel)
    friend class GenericModelItem;

public:
    explicit GenericModel(QObject *parent = Q_NULLPTR);
    ~GenericModel();
    void setRoleNames(const QHash<int, QByteArray> &rNames);
    QHash<int, QByteArray> roleNames() const override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual bool setFlags(const QModelIndex &index, Qt::ItemFlags flags);
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    // QMimeData *mimeData(const QModelIndexList &indexes) const override;
    QStringList mimeTypes() const override;
    bool moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent,
                     int destinationChild) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
    QModelIndex parent(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    void sort(int column, const QModelIndex &parent, Qt::SortOrder order = Qt::AscendingOrder, bool recursive = true);
    QSize span(const QModelIndex &index) const override;
    bool setSpan(const QModelIndex &index, const QSize &size);
    Qt::DropActions supportedDragActions() const override;
    Qt::DropActions supportedDropActions() const override;
    bool mergeDisplayEdit() const;
    void setMergeDisplayEdit(bool val);
    int sortRole() const;
    void setSortRole(int role);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    void multiData(const QModelIndex &index, QModelRoleDataSpan roleDataSpan) const override;
    bool clearItemData(const QModelIndex &index) override;
#else
    bool clearItemData(const QModelIndex &index);
#endif
Q_SIGNALS:
    void mergeDisplayEditChanged(bool val);
    void sortRoleChanged(int val);

protected:
    GenericModel(GenericModelPrivate &dptr, QObject *parent);
    GenericModelPrivate *m_dptr;
};
#endif // GENERICMODEL_H
