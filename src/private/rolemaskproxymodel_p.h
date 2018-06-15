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
#ifndef rolemaskproxymodel_p_h__
#define rolemaskproxymodel_p_h__
#include <QPersistentModelIndex>
#include <QHash>
#include <QSet>
#include <QMetaObject>
#include <QVector>
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
    QVector<QMetaObject::Connection> m_sourceConnections;
    void clearUnusedMaskedRoles(const QSet<int>& roles);
    bool removeRole(const QPersistentModelIndex& idx, int role);
    bool removeRole(const QHash<QPersistentModelIndex, RolesContainer >::iterator& idxIter, int role);
    void signalAllChanged(const QVector<int>& roles = QVector<int>(), const QModelIndex& parent = QModelIndex());
    void interceptDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
};

#endif // rolemaskproxymodel_p_h__