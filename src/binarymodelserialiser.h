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

#ifndef binarymodelserialiser_h__
#define binarymodelserialiser_h__
#include "modelutilities_global.h"
#include "abstractmodelserialiser.h"
class BinaryModelSerialiserPrivate;
class MODELUTILITIES_EXPORT BinaryModelSerialiser : public AbstractModelSerialiser
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(BinaryModelSerialiser)
    Q_DISABLE_COPY(BinaryModelSerialiser)
public:
    BinaryModelSerialiser(QAbstractItemModel *model = Q_NULLPTR, QObject *parent = Q_NULLPTR);
    BinaryModelSerialiser(const QAbstractItemModel *model, QObject *parent = Q_NULLPTR);
    ~BinaryModelSerialiser();
    virtual bool saveModel(QDataStream &stream) const;
    Q_INVOKABLE bool saveModel(QIODevice *destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE bool saveModel(QByteArray *destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE bool loadModel(QIODevice *source) Q_DECL_OVERRIDE;
    Q_INVOKABLE bool loadModel(const QByteArray &source) Q_DECL_OVERRIDE;
    virtual bool loadModel(QDataStream &stream);

protected:
    BinaryModelSerialiser(BinaryModelSerialiserPrivate &d, QObject *parent = Q_NULLPTR);

#ifdef MS_DECLARE_STREAM_OPERATORS
    friend QDataStream &operator<<(QDataStream &stream, const QAbstractItemModel &model);
    friend QDataStream &operator>>(QDataStream &stream, QAbstractItemModel &model);
#endif
};
#ifdef MS_DECLARE_STREAM_OPERATORS
QDataStream &operator<<(QDataStream &stream, const QAbstractItemModel &model);
QDataStream &operator>>(QDataStream &stream, QAbstractItemModel &model);
#endif
#endif // binarymodelserialiser_h__
