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
#include "private/rootindexproxymodel_p.h"
#include "rootindexproxymodel.h"
#include <functional>
#include <QSize>
RootIndexProxyModelPrivate::RootIndexProxyModelPrivate(RootIndexProxyModel *q)
    : q_ptr(q)
{
    Q_ASSERT(q_ptr);
    QObject::connect(q,&RootIndexProxyModel::sourceModelChanged,q,std::bind(&RootIndexProxyModelPrivate::resetRootOnModelChange,this));
}

bool RootIndexProxyModelPrivate::isDescendant(QModelIndex idx) const
{
    Q_ASSERT(idx.isValid());
    Q_ASSERT(m_rootIndex.isValid());
    Q_ASSERT(idx.model()==m_rootIndex.model());
    for(idx=idx.parent();idx.isValid();idx=idx.parent()) {
        if(idx==m_rootIndex)
            return true;
    }
    return false;
}

void RootIndexProxyModelPrivate::resetRootOnModelChange()
{
    m_rootIndex = QModelIndex();
}


/*!
Constructs a new proxy model with the given \a parent.
*/
RootIndexProxyModel::RootIndexProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
    , m_dptr(new RootIndexProxyModelPrivate(this))
{ }

/*!
\internal
*/
RootIndexProxyModel::RootIndexProxyModel(RootIndexProxyModelPrivate &dptr, QObject *parent)
    : QIdentityProxyModel(parent)
    , m_dptr(&dptr)
{

}

/*!
Destructor
*/
RootIndexProxyModel::~RootIndexProxyModel()
{
    delete m_dptr;
}

/*!
\property RootIndexProxyModel::rootIndex
\accessors %rootIndex(), setRootIndex()
\notifier rootIndexChanged()
\brief This property holds the root index of the model
\details The root index is an index of the source model used as root by all elements of the proxy model
*/

QModelIndex RootIndexProxyModel::rootIndex() const
{
    Q_D(const RootIndexProxyModel);
    return d->m_rootIndex;
}

void RootIndexProxyModel::setRootIndex(const QModelIndex &root)
{
    Q_ASSERT_X(!root.isValid() || root.model()==sourceModel(),"RootIndexProxyModel::setRootIndex","The root index must be an index of the source model");
    Q_D(RootIndexProxyModel);
    beginResetModel();
    d->m_rootIndex=root;
    endResetModel();
    rootIndexChanged();
}


/*!
\reimp
*/
QModelIndex RootIndexProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceModel())
        return QModelIndex();
    if (!sourceIndex.isValid())
        return QModelIndex();
    Q_D(const RootIndexProxyModel);
    if(!d->m_rootIndex.isValid())
        return QIdentityProxyModel::mapFromSource(sourceIndex);
    if(!d->isDescendant(sourceIndex))
        return QModelIndex();
    if(sourceIndex.parent()==d->m_rootIndex)
        return createIndex(sourceIndex.row(),sourceIndex.column());
    return QIdentityProxyModel::mapFromSource(sourceIndex);
}

/*!
\reimp
*/
QVariant RootIndexProxyModel::data(const QModelIndex &index, int role) const{
    if(!index.isValid())
        return QVariant();
    return QIdentityProxyModel::data(index,role);
}

/*!
\reimp
*/
Qt::ItemFlags RootIndexProxyModel::flags(const QModelIndex &index) const{
    if(!index.isValid())
        return Qt::NoItemFlags;
    return QIdentityProxyModel::flags(index);
}

/*!
\reimp
*/
QMap<int, QVariant> RootIndexProxyModel::itemData(const QModelIndex &index) const{
    if(!index.isValid())
        return QMap<int, QVariant>();
    return QIdentityProxyModel::itemData(index);
}

/*!
\reimp
*/
bool RootIndexProxyModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if(!index.isValid())
        return false;
    return QIdentityProxyModel::setData(index,value,role);
}

/*!
\reimp
*/
bool RootIndexProxyModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles){
    if(!index.isValid())
        return false;
    return QIdentityProxyModel::setItemData(index,roles);
}

/*!
\reimp
*/
QSize RootIndexProxyModel::span(const QModelIndex &index) const{
    if(!index.isValid())
        return QSize();
    return QIdentityProxyModel::span(index);
}

/*!
\reimp
*/
QModelIndex RootIndexProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!sourceModel())
        return QModelIndex();
    Q_D(const RootIndexProxyModel);
    if (!proxyIndex.isValid())
        return d->m_rootIndex;
    if(proxyIndex.internalPointer())
        return QIdentityProxyModel::mapToSource(proxyIndex);
    return sourceModel()->index(proxyIndex.row(),proxyIndex.column(),d->m_rootIndex);
}


/*!
\class RootIndexProxyModel
\brief This proxy model will display only the portion of a tree model with the common ancestor \a rootIndex

The functionality is similar to QAbstractItemView::setRootIndex but on the model side
*/

