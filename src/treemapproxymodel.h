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
#ifndef treemapproxymodel_h__
#define treemapproxymodel_h__

#include "modelutilities_global.h"
#include <QAbstractProxyModel>
class TreeMapProxyModelPrivate;
class TreeMapProxyNode;
class MODELUTILITIES_EXPORT TreeMapProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
    Q_DISABLE_COPY(TreeMapProxyModel)
    Q_DECLARE_PRIVATE(TreeMapProxyModel)
public:
    explicit TreeMapProxyModel(QObject* parent = Q_NULLPTR);
    void setSourceModel(QAbstractItemModel* newSourceModel) Q_DECL_OVERRIDE;
    void* indexToInternalPoiner(const QModelIndex& sourceParent) const;
    QModelIndex internalPointerToIndex(void* internalPoiner) const;
    void rebuildTreeMap();
    void clearTreeMap();
    void possibleNewParent(const QModelIndex& parent);
    void possibleRemovedParent(const QModelIndex& parent);
    ~TreeMapProxyModel();
protected:
    TreeMapProxyModel(TreeMapProxyModelPrivate& dptr, QObject* parent);
    TreeMapProxyModelPrivate* d_ptr;
};

#endif // treemapproxymodel_h__
