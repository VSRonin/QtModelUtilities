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
