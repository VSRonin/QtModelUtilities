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
#ifndef GENERICMODEL_P_H
#define GENERICMODEL_P_H
#include "genericmodel.h"
#include "private/modelutilities_common_p.h"
#include <QVector>
#include <QSize>
class GenericModelItem
{
public:
    GenericModelItem(GenericModel *model);
    GenericModelItem(GenericModelItem *par);
    virtual ~GenericModelItem();
    GenericModelItem *childAt(int row, int col) const;
    GenericModelItem *parent;
    RolesContainer data;
    Qt::ItemFlags flags;
    int columnCount() const;
    int rowCount() const;
    void insertColumns(int column, int count);
    void removeColumns(int column, int count);
    void insertRows(int row, int count);
    void removeRows(int row, int count);
    QVector<GenericModelItem *> takeRows(int row, int count);
    void insertRows(int row, QVector<GenericModelItem *> rows);
    QVector<GenericModelItem *> takeCols(int col, int count);
    void insertCols(int col, QVector<GenericModelItem *> cols);
    int row() const;
    int column() const;
    void setMergeDisplayEdit(bool val);
    QSize span() const;
    void setSpan(const QSize &sz);
    void sortChildren(int column, int role, Qt::SortOrder order, bool recursive, QVector<RolesContainer>* headersToSort);
    void moveChildRows(int sourceRow, int count, int destinationChild);
    void moveChildColumns(int sourceCol, int count, int destinationChild);
    void setRow(int r);
    void setColumn(int c);

private:
    int m_colCount;
    int m_rowCount;
    int m_row;
    int m_column;
    int m_rowSpan;
    int m_colSpan;
    GenericModel *m_model;
    QVector<GenericModelItem *> children;
    void sortChildren(int column, int role, Qt::SortOrder order, bool recursive, const QModelIndexList &persistentIndexes, QVector<RolesContainer>* headersToSort);
};

class GenericModelPrivate
{
    Q_DECLARE_PUBLIC(GenericModel)
    Q_DISABLE_COPY(GenericModelPrivate)
    GenericModelPrivate(GenericModel *q);
    virtual ~GenericModelPrivate();
    QString mimeDataName() const;
    GenericModelItem *itemForIndex(const QModelIndex &idx) const;
    QModelIndex indexForItem(GenericModelItem *item) const;
    void insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    void insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    void removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void moveRowsSameParent(const QModelIndex &sourceParent, int sourceRow, int count, int destinationChild);
    void moveRowsDifferentParent(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent,
                                 int destinationChild);
    void moveColumnsSameParent(const QModelIndex &sourceParent, int sourceCol, int count, int destinationChild);
    void moveColumnsDifferentParent(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent,
                                    int destinationChild);
    void setMergeDisplayEdit(bool val);
    void signalAllChanged(const QVector<int> &roles = QVector<int>(), const QModelIndex &parent = QModelIndex());
    GenericModel *q_ptr;
    GenericModelItem *root;
    QVector<RolesContainer> vHeaderData;
    QVector<RolesContainer> hHeaderData;
    bool m_mergeDisplayEdit;
    int sortRole;
    QHash<int, QByteArray> m_roleNames;

public:
    static void setMergeDisplayEdit(bool val, RolesContainer &container);
    static bool isVariantLessThan(const QVariant &left, const QVariant &right);
};

#endif // GENERICMODEL_P_H
