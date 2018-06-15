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

#ifndef abstractmodelserialiser_h__
#define abstractmodelserialiser_h__
#include "modelutilities_global.h"
#include <QObject>
class AbstractModelSerialiserPrivate;
class QAbstractItemModel;
class QIODevice;
class MODELUTILITIES_EXPORT AbstractModelSerialiser
{
    Q_GADGET
    Q_PROPERTY(QAbstractItemModel* model READ model WRITE setModel)
    Q_PROPERTY(const QAbstractItemModel* constModel READ constModel WRITE setModel)
    Q_DECLARE_PRIVATE(AbstractModelSerialiser)
    Q_DISABLE_COPY(AbstractModelSerialiser)
public:
    AbstractModelSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    AbstractModelSerialiser(const QAbstractItemModel* model);
    virtual ~AbstractModelSerialiser() = 0;
    virtual QAbstractItemModel* model() const;
    virtual const QAbstractItemModel* constModel() const;
    void setModel(QAbstractItemModel* val);
    void setModel(const QAbstractItemModel* val);
    Q_INVOKABLE virtual bool saveModel(QIODevice* destination) const = 0;
    Q_INVOKABLE virtual bool saveModel(QByteArray* destination) const = 0;
    Q_INVOKABLE virtual bool loadModel(QIODevice* source) = 0;
    Q_INVOKABLE virtual bool loadModel(const QByteArray& source) = 0;
protected:
    AbstractModelSerialiserPrivate* d_ptr;
    AbstractModelSerialiser(AbstractModelSerialiserPrivate& d);
};

#endif // abstractmodelserialiser_h__