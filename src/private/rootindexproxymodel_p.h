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
#ifndef rootindexproxy_p_h__
#define rootindexproxy_p_h__
#include "rootindexproxymodel.h"
#include "private/modelutilities_common_p.h"
class RootIndexProxyModelPrivate
{
    Q_DECLARE_PUBLIC(RootIndexProxyModel)
    Q_DISABLE_COPY(RootIndexProxyModelPrivate)
    RootIndexProxyModelPrivate(RootIndexProxyModel *q);
    QPersistentModelIndex m_rootIndex;
    RootIndexProxyModel *q_ptr;
    bool isDescendant(QModelIndex idx) const;
    void resetRootOnModelChange();
};

#endif // rootindexproxy_p_h__
