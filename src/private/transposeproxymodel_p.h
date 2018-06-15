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
#ifndef transposeproxymodel_p_h__
#define transposeproxymodel_p_h__

#include "transposeproxymodel.h"
#include "private/treemapproxymodel_p.h"
#include <QVector>
#include <QMetaObject>
class TransposeProxyModelPrivate : public TreeMapProxyModelPrivate
{
    Q_DECLARE_PUBLIC(TransposeProxyModel)
    Q_DISABLE_COPY(TransposeProxyModelPrivate)
private:
    TransposeProxyModelPrivate(TransposeProxyModel* q);
    QVector<QMetaObject::Connection> m_sourceConnections;
    QVector<QPersistentModelIndex> m_layoutChangePersistentIndexes;
    QModelIndexList m_layoutChangeProxyIndexes;
    void onLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
};

#endif // transposeproxymodel_p_h__
