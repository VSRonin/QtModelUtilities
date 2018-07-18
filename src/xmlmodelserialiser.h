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


#ifndef xmlmodelserialiser_h__
#define xmlmodelserialiser_h__

#include "modelutilities_global.h"
#include "abstractstringserialiser.h"
class XmlModelSerialiserPrivate;
class QXmlStreamWriter;
class QXmlStreamReader;
class MODELUTILITIES_EXPORT XmlModelSerialiser : public AbstractStringSerialiser
{
    Q_OBJECT
    Q_PROPERTY(bool printStartDocument READ printStartDocument WRITE setPrintStartDocument)
    Q_DECLARE_PRIVATE(XmlModelSerialiser)
    Q_DISABLE_COPY(XmlModelSerialiser)
public:
    XmlModelSerialiser(QAbstractItemModel* model = Q_NULLPTR, QObject* parent = Q_NULLPTR);
    XmlModelSerialiser(const QAbstractItemModel* model, QObject* parent = Q_NULLPTR);
    ~XmlModelSerialiser();
    bool printStartDocument() const;
    void setPrintStartDocument(bool val);
    bool saveModel(QIODevice* destination) const Q_DECL_OVERRIDE;
    bool saveModel(QByteArray* destination) const Q_DECL_OVERRIDE;
    bool saveModel(QString* destination) const Q_DECL_OVERRIDE;
    bool loadModel(QString* source) Q_DECL_OVERRIDE;
    bool loadModel(QIODevice* source) Q_DECL_OVERRIDE;
    bool loadModel(const QByteArray& source) Q_DECL_OVERRIDE;
protected:
    XmlModelSerialiser(XmlModelSerialiserPrivate& d, QObject* parent);

#ifdef MS_DECLARE_STREAM_OPERATORS
    friend QXmlStreamWriter&  operator<<(QXmlStreamWriter & stream, const QAbstractItemModel& model);
    friend QXmlStreamReader& operator>>(QXmlStreamReader & stream, QAbstractItemModel& model);
#endif
};
#ifdef MS_DECLARE_STREAM_OPERATORS
QXmlStreamWriter&  operator<<(QXmlStreamWriter & stream, const QAbstractItemModel& model);
QXmlStreamReader& operator>>(QXmlStreamReader & stream, QAbstractItemModel& model);
#endif
#endif // xmlmodelserialiser_h__