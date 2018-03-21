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


#ifndef INSERTPROXY_H
#define INSERTPROXY_H
#include "modelutilities_global.h"
#include <QAbstractProxyModel>
#include <QVariant>
class InsertProxyPrivate;
class MODELUTILITIES_EXPORT InsertProxy : public  QAbstractProxyModel
{
    Q_OBJECT
    Q_PROPERTY(InsertDirections insertDirection READ insertDirection WRITE setInsertDirection NOTIFY insertDirectionChanged)
    Q_PROPERTY(bool separateEditDisplay READ separateEditDisplay WRITE setSeparateEditDisplay NOTIFY separateEditDisplayChanged)
    Q_DISABLE_COPY(InsertProxy)
    Q_DECLARE_PRIVATE_D(m_dptr, InsertProxy)
public:
    enum InsertDirection
    {
        NoInsert = 0x0
        , InsertRow = 0x1
        , InsertColumn = 0x2
    };
    Q_DECLARE_FLAGS(InsertDirections, InsertDirection)
    Q_FLAG(InsertDirections)
    explicit InsertProxy(QObject* parent = Q_NULLPTR);
    ~InsertProxy();
    void setSourceModel(QAbstractItemModel* newSourceModel) Q_DECL_OVERRIDE;
    QModelIndex buddy(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) Q_DECL_OVERRIDE;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) Q_DECL_OVERRIDE;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent, int destinationChild) Q_DECL_OVERRIDE;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) Q_DECL_OVERRIDE;
    InsertDirections insertDirection() const;
    void setInsertDirection(const InsertDirections& direction);
    Q_SIGNAL void insertDirectionChanged(const InsertDirections& direction);
    virtual QVariant dataForCorner(int role = Qt::DisplayRole) const;
    virtual void setDataForCorner(const QVariant& value, int role = Qt::DisplayRole);
    Q_SIGNAL void dataForCornerChanged(int role);
    Q_SLOT void commitRow();
    Q_SLOT void commitColumn();
    Q_SIGNAL void extraDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    bool separateEditDisplay() const;
    void setSeparateEditDisplay(bool val);
    Q_SIGNAL void separateEditDisplayChanged(bool separate);
protected:
    virtual bool validRow() const;
    virtual bool validColumn() const;
    virtual Qt::ItemFlags flagForExtra(bool isRow, int section) const;
private:
    InsertProxyPrivate* m_dptr;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(InsertProxy::InsertDirections)
#endif // INSERTPROXY_H
