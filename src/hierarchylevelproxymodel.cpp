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
#include "private/hierarchylevelproxymodel_p.h"
#include "hierarchylevelproxymodel.h"

int HierarchyLevelProxyModelPrivate::rootOf(QModelIndex sourceIndex) const
{
    Q_ASSERT(sourceIndex.isValid());
    Q_ASSERT(sourceIndex.model() == q_func()->sourceModel());
    do{
        sourceIndex=sourceIndex.parent();
        for(int i=0;i<m_roots.size();++i){
            if(m_roots.at(i).root==sourceIndex)
                return i;
        }
    }while(sourceIndex.isValid());
    return -1;
}

void HierarchyLevelProxyModelPrivate::rebuildMapping()
{
    m_roots.clear();
    m_maxCol=0;
    Q_Q(HierarchyLevelProxyModel);
    if(!q->sourceModel())
        return;
    if(m_targetLevel==0){
        m_roots.append(HierarchyRootData(QModelIndex(),0));
        m_maxCol = q->sourceModel()->columnCount();
        return;
    }
    int rootsRowCount=0;
    rebuildMappingBranch(QModelIndex(),0,rootsRowCount);
}

void HierarchyLevelProxyModelPrivate::rebuildMappingBranch(const QModelIndex& parent, int levl, int& rootsRowCount)
{
    Q_Q(HierarchyLevelProxyModel);
    for(int i=0, iMax = q->sourceModel()->rowCount(parent);i<iMax;++i){
        for(int j=0, jMax = q->sourceModel()->columnCount(parent);j<jMax;++j){
            const QModelIndex currIdx = q->sourceModel()->index(i,j,parent);
            if(levl==m_targetLevel-1){
                m_roots.append(HierarchyRootData(currIdx,rootsRowCount));
                rootsRowCount+=q->sourceModel()->rowCount(currIdx);
                m_maxCol = qMax(m_maxCol,q->sourceModel()->columnCount(currIdx));
            }
            else{
                rebuildMappingBranch(currIdx,levl+1,rootsRowCount);
            }
        }
    }

}

void HierarchyLevelProxyModelPrivate::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_Q(HierarchyLevelProxyModel);
    Q_ASSERT(topLeft.isValid() && bottomRight.isValid() && topLeft.model()==q->sourceModel() &&  topLeft.model()==bottomRight.model() && bottomRight.parent()==topLeft.parent());
    const QModelIndex mappedTopLeft = q->mapFromSource(topLeft);
    if(mappedTopLeft.isValid())
        emit q->dataChanged(mappedTopLeft,q->mapFromSource(bottomRight),roles);
}

void HierarchyLevelProxyModelPrivate::onHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::onColumnsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::onColumnsAboutToBeMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::onColumnsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::onColumnsInserted(const QModelIndex &parent, int first, int last)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::onColumnsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::onColumnsRemoved(const QModelIndex &parent, int first, int last)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    Q_Q(HierarchyLevelProxyModel);
    for(auto i=m_roots.begin(), iEnd=m_roots.end();i!=iEnd;++i){
        if(i->root==parent){
            q->beginInsertRows(QModelIndex(),i->cachedCumRowCount+first,i->cachedCumRowCount+last);
            return;
        }
    }
    const QModelIndex mappedParent = q->mapFromSource(parent);
    if(mappedParent.isValid()){
        q->beginInsertRows(mappedParent,first,last);
    }
    //#TODO row added to parent that didn't have one
}

void HierarchyLevelProxyModelPrivate::onRowsAboutToBeMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    //#TODO
}

void HierarchyLevelProxyModelPrivate::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_Q(HierarchyLevelProxyModel);
    for(auto i=m_roots.begin(), iEnd=m_roots.end();i!=iEnd;++i){
        if(i->root==parent){

            for(++i;i!=iEnd;++i)
                i->cachedCumRowCount+=last-first+1;
            q->endInsertRows();
            return;
        }
    }
}

void HierarchyLevelProxyModelPrivate::onRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{

}

void HierarchyLevelProxyModelPrivate::onRowsRemoved(const QModelIndex &parent, int first, int last)
{

}

void HierarchyLevelProxyModelPrivate::beforeLayoutChange(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{

}

void HierarchyLevelProxyModelPrivate::afetrLayoutChange(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{

}

void HierarchyLevelProxyModelPrivate::afterReset()
{
    rebuildMapping();
    Q_Q(HierarchyLevelProxyModel);
    q->endResetModel();
}

HierarchyLevelProxyModelPrivate::HierarchyLevelProxyModelPrivate(HierarchyLevelProxyModel *q)
    : q_ptr(q)
    , m_maxCol(0)
    , m_targetLevel(0)
{
    Q_ASSERT(q_ptr);
}

/*!
Constructs a new proxy model with the given \a parent.
*/
HierarchyLevelProxyModel::HierarchyLevelProxyModel(QObject *parent)
    : HierarchyLevelProxyModel(*new HierarchyLevelProxyModelPrivate(this), parent)
{ }

/*!
\internal
*/
HierarchyLevelProxyModel::HierarchyLevelProxyModel(HierarchyLevelProxyModelPrivate &dptr, QObject *parent)
    : QAbstractProxyModel(parent)
    , m_dptr(&dptr)
{ }

/*!
Destructor
*/
HierarchyLevelProxyModel::~HierarchyLevelProxyModel()
{
    delete m_dptr;
}

/*!
\reimp
*/
void HierarchyLevelProxyModel::setSourceModel(QAbstractItemModel *newSourceModel)
{
    if (newSourceModel == sourceModel())
        return;
    Q_D(HierarchyLevelProxyModel);
    beginResetModel();
    for (auto discIter = d->m_sourceConnections.cbegin(); discIter != d->m_sourceConnections.cend(); ++discIter)
        QObject::disconnect(*discIter);
     d->m_sourceConnections.clear();
    QAbstractProxyModel::setSourceModel(newSourceModel);
    d->rebuildMapping();
    if (sourceModel()) {
        using namespace std::placeholders;
        d->m_sourceConnections
                << QObject::connect(sourceModel(), &QAbstractItemModel::dataChanged, this, std::bind(&HierarchyLevelProxyModelPrivate::onDataChanged,d,_1,_2,_3))
                << QObject::connect(sourceModel(), &QAbstractItemModel::headerDataChanged,this, std::bind(&HierarchyLevelProxyModelPrivate::onHeaderDataChanged,d,_1,_2,_3))
                << QObject::connect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged,this, std::bind(&HierarchyLevelProxyModelPrivate::beforeLayoutChange,d,_1,_2))
                << QObject::connect(sourceModel(), &QAbstractItemModel::layoutChanged,this, std::bind(&HierarchyLevelProxyModelPrivate::afetrLayoutChange,d,_1,_2))
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeInserted, this, std::bind(&HierarchyLevelProxyModelPrivate::onColumnsAboutToBeInserted,d,_1,_2,_3))
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeMoved, this, std::bind(&HierarchyLevelProxyModelPrivate::onColumnsAboutToBeMoved,d,_1,_2,_3,_4,_5))
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsAboutToBeRemoved, this, std::bind(&HierarchyLevelProxyModelPrivate::onColumnsAboutToBeRemoved,d,_1,_2,_3))
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsInserted, this, std::bind(&HierarchyLevelProxyModelPrivate::onColumnsInserted,d,_1,_2,_3))
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsRemoved,this, std::bind(&HierarchyLevelProxyModelPrivate::onColumnsRemoved,d,_1,_2,_3))
                << QObject::connect(sourceModel(), &QAbstractItemModel::columnsMoved,this, std::bind(&HierarchyLevelProxyModelPrivate::onColumnsMoved,d,_1,_2,_3,_4,_5))
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted,this, std::bind(&HierarchyLevelProxyModelPrivate::onRowsAboutToBeInserted,d,_1,_2,_3))
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeMoved,this,std::bind(&HierarchyLevelProxyModelPrivate::onRowsAboutToBeMoved,d,_1,_2,_3,_4,_5))
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, this, std::bind(&HierarchyLevelProxyModelPrivate::onRowsAboutToBeRemoved,d,_1,_2,_3))
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsInserted,this, std::bind(&HierarchyLevelProxyModelPrivate::onRowsInserted,d,_1,_2,_3))
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsRemoved,this, std::bind(&HierarchyLevelProxyModelPrivate::onRowsRemoved,d,_1,_2,_3))
                << QObject::connect(sourceModel(), &QAbstractItemModel::rowsMoved,this,std::bind(&HierarchyLevelProxyModelPrivate::onRowsMoved,d,_1,_2,_3,_4,_5))
                << QObject::connect(sourceModel(), &QAbstractItemModel::modelAboutToBeReset,this,&HierarchyLevelProxyModel::beginResetModel)
                << QObject::connect(sourceModel(), &QAbstractItemModel::modelReset, this,std::bind(&HierarchyLevelProxyModelPrivate::afterReset,d));
    }
    endResetModel();
}



/*!
\reimp
*/
int HierarchyLevelProxyModel::rowCount(const QModelIndex &parent) const
{
    if (!sourceModel())
        return 0;
    Q_D(const HierarchyLevelProxyModel);
    if (!parent.isValid()){
        if(d->m_roots.isEmpty())
            return 0;
        const auto& lastRoot = d->m_roots.last();
        return sourceModel()->rowCount(lastRoot.root)+lastRoot.cachedCumRowCount;
    }
    return sourceModel()->rowCount(mapToSource(parent));
}

/*!
\reimp
*/
int HierarchyLevelProxyModel::columnCount(const QModelIndex &parent) const
{
    if (!sourceModel())
        return 0;
    Q_D(const HierarchyLevelProxyModel);
    if (!parent.isValid())
        return d->m_maxCol;
    return sourceModel()->columnCount(mapToSource(parent));
}

/*!
\reimp
*/
QVariant HierarchyLevelProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(!sourceModel())
        return QVariant();
    if(orientation == Qt::Horizontal)
        return sourceModel()->headerData(section, orientation,  role);
    //#TODO
    return 0;
    Q_D(const HierarchyLevelProxyModel);
    for(auto i=d->m_roots.constBegin(), iEnd=d->m_roots.constEnd();i!=iEnd;++i){
        if(i->cachedCumRowCount>section){
            --i;

        }
    }
    return 0;
}



/*!
\reimp
*/
bool HierarchyLevelProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (!sourceModel())
        return false;
    if(orientation == Qt::Horizontal)
        return sourceModel()->setHeaderData(section, orientation, value, role);
    return QAbstractProxyModel::setHeaderData(section, orientation, value, role);
}

#if (QT_VERSION < QT_VERSION_CHECK(6, 3, 0))
/*!
\reimp
*/
bool HierarchyLevelProxyModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles){
    if(!sourceModel())
        return false;
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    return sourceModel()->setItemData(mapToSource(index), roles);
}
/*!
\reimp
*/
QMap<int, QVariant> HierarchyLevelProxyModel::itemData(const QModelIndex &index) const{
    if(!sourceModel())
        return QMap<int, QVariant>();
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    return sourceModel()->itemData(mapToSource(index));
}
#endif

/*!
\reimp
*/
QModelIndex HierarchyLevelProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    Q_ASSERT(sourceIndex.isValid() ? sourceIndex.model() == sourceModel() : true);
    if(!sourceModel())
        return QModelIndex();
    if (!sourceIndex.isValid())
        return QModelIndex();
    Q_D(const HierarchyLevelProxyModel);
    const int rootIdx = d->rootOf(sourceIndex);
    if(rootIdx<0)
        return QModelIndex();
    const auto& rootData = d->m_roots.at(rootIdx);
    if(rootData.root == sourceIndex.parent())
        return createIndex(rootData.cachedCumRowCount + sourceIndex.row(),sourceIndex.column());
    return createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());
}

/*!
\reimp
*/
bool HierarchyLevelProxyModel::hasChildren(const QModelIndex &parent) const
{
    if(!sourceModel())
        return false;
    Q_D(const HierarchyLevelProxyModel);
    if(!parent.isValid() && d->m_targetLevel>0)
        return columnCount()>0 && rowCount()>0;
    return sourceModel()->hasChildren(mapToSource(parent));
}

/*!
\reimp
*/
QModelIndex HierarchyLevelProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    Q_ASSERT(proxyIndex.isValid() ? proxyIndex.model() == this : true);
    if (!sourceModel())
        return QModelIndex();
    if (!proxyIndex.isValid())
        return QModelIndex();
    Q_D(const HierarchyLevelProxyModel);
    if(proxyIndex.internalPointer())
        return createSourceIndex(proxyIndex.row(), proxyIndex.column(), proxyIndex.internalPointer());
    if(proxyIndex.row()==0) // optimisation
        return sourceModel()->index(0, proxyIndex.column(),d->m_roots.first().root);
#ifdef QT_DEBUG
    for(int i=1;i<d->m_roots.size();++i)
        Q_ASSERT(d->m_roots.at(i).cachedCumRowCount>=d->m_roots.at(i-1).cachedCumRowCount);
#endif
   for(int i=d->m_roots.size()-1;i>=0;--i){
       const auto& rootData = d->m_roots.at(i);
       if(proxyIndex.row()>=rootData.cachedCumRowCount)
           return sourceModel()->index(proxyIndex.row()-rootData.cachedCumRowCount, proxyIndex.column(),rootData.root);
   }
   Q_UNREACHABLE();
   return QModelIndex();
}

/*!
\reimp
*/
Qt::ItemFlags HierarchyLevelProxyModel::flags(const QModelIndex &index) const
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    if (!sourceModel())
        return Qt::NoItemFlags;
    return sourceModel()->flags(mapToSource(index));
}

/*!
\reimp
*/
QModelIndex HierarchyLevelProxyModel::parent(const QModelIndex &index) const
{
    Q_ASSERT(index.isValid() ? index.model() == this : true);
    return mapFromSource(mapToSource(index).parent());
}

/*!
\reimp
*/
QModelIndex HierarchyLevelProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    if(parent.isValid())
        return mapFromSource(sourceModel()->index(row,column,mapToSource(parent)));
    return createIndex(row,column);
}

int HierarchyLevelProxyModel::hierarchyLevel() const
{
    Q_D(const HierarchyLevelProxyModel);
    return d->m_targetLevel;
}

void HierarchyLevelProxyModel::setHierarchyLevel(int hierarchyLvl)
{
    Q_D(HierarchyLevelProxyModel);
    if(hierarchyLvl<0)
        hierarchyLvl=0;
    if(hierarchyLvl == d->m_targetLevel)
        return;
    if(sourceModel())
        beginResetModel();
    d->m_targetLevel=hierarchyLvl;
    if(sourceModel()){
        d->rebuildMapping();
        endResetModel();
    }
}










