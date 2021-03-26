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
#include "abstractmodelserialiser.h"
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    #include <QTextCodec>
#endif
class AbstractStringSerialiserPrivate;
class MODELUTILITIES_EXPORT AbstractStringSerialiser : public AbstractModelSerialiser
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractStringSerialiser)
    Q_DISABLE_COPY(AbstractStringSerialiser)
public:
    AbstractStringSerialiser(QAbstractItemModel* model = Q_NULLPTR, QObject* parent = Q_NULLPTR);
    AbstractStringSerialiser(const QAbstractItemModel* model, QObject* parent = Q_NULLPTR);
    virtual ~AbstractStringSerialiser() = 0;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QTextCodec* textCodec() const;
    bool setTextCodec(QTextCodec* val);
#endif
    Q_INVOKABLE virtual bool saveModel(QString* destination) const =0;
    Q_INVOKABLE virtual bool loadModel(QString* source) = 0;
    Q_INVOKABLE bool loadModel(const QString& source);
protected:
    AbstractStringSerialiser(AbstractStringSerialiserPrivate& d, QObject* parent);
};

#endif // abstractmodelserialiser_h__
