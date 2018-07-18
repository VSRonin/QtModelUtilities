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
#ifndef htmlmodelserialiser_h__
#define htmlmodelserialiser_h__

#include "modelutilities_global.h"
#include "abstractstringserialiser.h"
class HtmlModelSerialiserPrivate;
class MODELUTILITIES_EXPORT HtmlModelSerialiser : public AbstractStringSerialiser
{
    Q_OBJECT
    Q_PROPERTY(bool printStartDocument READ printStartDocument WRITE setPrintStartDocument)
    Q_DECLARE_PRIVATE(HtmlModelSerialiser)
    Q_DISABLE_COPY(HtmlModelSerialiser)
public:
    HtmlModelSerialiser(QAbstractItemModel* model = Q_NULLPTR, QObject* parent=Q_NULLPTR);
    HtmlModelSerialiser(const QAbstractItemModel* model, QObject* parent = Q_NULLPTR);
    bool printStartDocument() const;
    void setPrintStartDocument(bool val);
    bool saveModel(QIODevice* destination) const Q_DECL_OVERRIDE;
    bool saveModel(QByteArray* destination) const Q_DECL_OVERRIDE;
    bool saveModel(QString* destination) const Q_DECL_OVERRIDE;
    bool loadModel(QString* source) Q_DECL_OVERRIDE;
    bool loadModel(QIODevice* source) Q_DECL_OVERRIDE;
    bool loadModel(const QByteArray& source) Q_DECL_OVERRIDE;
    
protected:
    HtmlModelSerialiser(HtmlModelSerialiserPrivate& d, QObject* parent);
};
#endif // htmlmodelserialiser_h__