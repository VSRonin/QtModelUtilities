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

#ifndef ROOTINDEXPROXY_H
#define ROOTINDEXPROXY_H
#include "modelutilities_global.h"
#include <QIdentityProxyModel>
class RootIndexProxyModelPrivate;
class MODELUTILITIES_EXPORT RootIndexProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QModelIndex rootIndex READ rootIndex WRITE setRootIndex NOTIFY rootIndexChanged)
    Q_DISABLE_COPY(RootIndexProxyModel)
    Q_DECLARE_PRIVATE_D(m_dptr, RootIndexProxyModel)
public:
    explicit RootIndexProxyModel(QObject *parent = Q_NULLPTR);
    ~RootIndexProxyModel();
    QModelIndex rootIndex() const;
    void setRootIndex(const QModelIndex& root);
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
Q_SIGNALS:
    void rootIndexChanged();
protected:
    RootIndexProxyModel(RootIndexProxyModelPrivate &dptr, QObject *parent);
    RootIndexProxyModelPrivate *m_dptr;
};
#endif // ROOTINDEXPROXY_H
