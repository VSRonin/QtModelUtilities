#ifndef rolemaskproxymodel_p_h__
#define rolemaskproxymodel_p_h__
#include <QPersistentModelIndex>
#include <QHash>
#include <QSet>
#include <QMetaObject>
#include "private/modelutilities_common_p.h"
#include "rolemaskproxymodel.h"
class RoleMaskProxyModelPrivate
{
    Q_DECLARE_PUBLIC(RoleMaskProxyModel)
    RoleMaskProxyModel* q_ptr;
    RoleMaskProxyModelPrivate(RoleMaskProxyModel* q);
    QSet<int> m_maskedRoles;
    QHash<QPersistentModelIndex, RolesContainer > m_maskedData;
    bool m_transparentIfEmpty;
    bool m_mergeDisplayEdit;
    QMetaObject::Connection m_sourceDataChangedConnection;
    void clearUnusedMaskedRoles(const QSet<int>& roles);
    bool removeRole(const QPersistentModelIndex& idx, int role);
    bool removeRole(const QHash<QPersistentModelIndex, RolesContainer >::iterator& idxIter, int role);
    void signalAllChanged(const QVector<int>& roles = QVector<int>(), const QModelIndex& parent = QModelIndex());
    void interceptDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
};

#endif // rolemaskproxymodel_p_h__