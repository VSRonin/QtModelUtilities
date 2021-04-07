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

#ifndef abstractmultiroleserialiser_h__
#define abstractmultiroleserialiser_h__

#include "modelutilities_global.h"
#include <QObject>
#include <QScopedPointer>
#include <QAbstractItemModel>
#include <QDataStream>
class AbstractModelSerialiserPrivate;
class QIODevice;
class MODELUTILITIES_EXPORT AbstractModelSerialiser : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<int> rolesToSave READ rolesToSave WRITE setRoleToSave RESET resetRoleToSave)
    Q_PROPERTY(QAbstractItemModel *model READ model WRITE setModel)
    Q_PROPERTY(const QAbstractItemModel *constModel READ constModel WRITE setModel)
    Q_DECLARE_PRIVATE(AbstractModelSerialiser)
    Q_DISABLE_COPY(AbstractModelSerialiser)
public:
    AbstractModelSerialiser(QAbstractItemModel *model = Q_NULLPTR, QObject *parent = Q_NULLPTR);
    AbstractModelSerialiser(const QAbstractItemModel *model, QObject *parent = Q_NULLPTR);
    virtual ~AbstractModelSerialiser() = 0;
    virtual const QList<int> &rolesToSave() const;
    virtual void setRoleToSave(const QList<int> &val);
    Q_INVOKABLE virtual void addRoleToSave(int role);
    Q_INVOKABLE virtual void removeRoleToSave(int role);
    Q_INVOKABLE virtual void clearRoleToSave();
    virtual void resetRoleToSave();
    static QList<int> modelDefaultRoles();
    virtual QAbstractItemModel *model() const;
    virtual const QAbstractItemModel *constModel() const;
    void setModel(QAbstractItemModel *val);
    void setModel(const QAbstractItemModel *val);
    Q_INVOKABLE QDataStream::Version streamVersion() const;
    Q_INVOKABLE virtual bool saveModel(QIODevice *destination) const = 0;
    Q_INVOKABLE virtual bool saveModel(QByteArray *destination) const = 0;
    Q_INVOKABLE virtual bool loadModel(QIODevice *source) = 0;
    Q_INVOKABLE virtual bool loadModel(const QByteArray &source) = 0;
public Q_SLOTS:
    void setStreamVersion(QDataStream::Version ver);

protected:
    AbstractModelSerialiser(AbstractModelSerialiserPrivate &d, QObject *parent);
    QScopedPointer<AbstractModelSerialiserPrivate> d_ptr;
};
#endif // abstractmultiroleserialiser_h__
