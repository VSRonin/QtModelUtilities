#ifndef serialisers_common_h__
#define serialisers_common_h__
#include <QAbstractItemModel>
#include <QStringListModel>
namespace SerialiserCommon{
bool modelEqual(const QAbstractItemModel* a, const QAbstractItemModel* b, const QModelIndex& aParent = QModelIndex(), const QModelIndex& bParent = QModelIndex()){
    Q_ASSERT(a);
    Q_ASSERT(b);
    Q_ASSERT(!aParent.isValid() || aParent.model()==a);
    Q_ASSERT(!bParent.isValid() || bParent.model()==b);
    const int rowC = a->rowCount(aParent);
    if(b->rowCount(bParent)!=rowC)
        return false;
    const int colC = a->columnCount(aParent);
    if(b->columnCount(bParent)!=colC)
        return false;
    for(int i=0;i<rowC;++i){
        for(int j=0;j<colC;++j){
            const QModelIndex aIdx = a->index(i,j,aParent);
            const QModelIndex bIdx = b->index(i,j,bParent);
            if(a->itemData(aIdx)!=b->itemData(bIdx))
                return false;
            const bool hasChildren = a->hasChildren(aIdx);
            if(b->hasChildren(bIdx)!=hasChildren)
                return false;
            if(hasChildren){
                if(!modelEqual(a,b,aIdx,bIdx))
                    return false;
            }
        }
    }
    return true;
}
QAbstractItemModel* createStringModel(QObject* parent = nullptr){
    const QStringList euStates {
        QStringLiteral("Austria")
        , QStringLiteral("Belgium")
        , QStringLiteral("Bulgaria")
        , QStringLiteral("Croatia")
        , QStringLiteral("Republic of Cyprus")
        , QStringLiteral("Czech Republic")
        , QStringLiteral("Denmark")
        , QStringLiteral("Estonia")
        , QStringLiteral("Finland")
        , QStringLiteral("France")
        , QStringLiteral("Germany")
        , QStringLiteral("Greece")
        , QStringLiteral("Hungary")
        , QStringLiteral("Ireland")
        , QStringLiteral("Italy")
        , QStringLiteral("Latvia")
        , QStringLiteral("Lithuania")
        , QStringLiteral("Luxembourg")
        , QStringLiteral("Malta")
        , QStringLiteral("Netherlands")
        , QStringLiteral("Poland")
        , QStringLiteral("Portugal")
        , QStringLiteral("Romania")
        , QStringLiteral("Slovakia")
        , QStringLiteral("Slovenia")
        , QStringLiteral("Spain")
        , QStringLiteral("Sweden")
    };
    return new QStringListModel(euStates,parent);
}
}
#endif
