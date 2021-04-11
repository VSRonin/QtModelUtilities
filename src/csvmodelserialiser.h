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

#ifndef CSVMODELSERIALISER_H
#define CSVMODELSERIALISER_H

#include "modelutilities_global.h"
#include "abstractsingleroleserialiser.h"
class CsvModelSerialiserPrivate;
class QTextStream;
class MODELUTILITIES_EXPORT CsvModelSerialiser : public AbstractSingleRoleSerialiser
{
    Q_OBJECT
    Q_PROPERTY(QString csvSeparator READ csvSeparator WRITE setCsvSeparator)
    Q_PROPERTY(bool firstRowIsHeader READ firstRowIsHeader WRITE setFirstRowIsHeader)
    Q_PROPERTY(bool firstColumnIsHeader READ firstColumnIsHeader WRITE setFirstColumnIsHeader)
    Q_DECLARE_PRIVATE(CsvModelSerialiser)
    Q_DISABLE_COPY(CsvModelSerialiser)
public:
    explicit CsvModelSerialiser(QObject *parent = Q_NULLPTR);
    CsvModelSerialiser(QAbstractItemModel *model, QObject *parent);
    CsvModelSerialiser(const QAbstractItemModel *model, QObject *parent);
    const QString &csvSeparator() const;
    void setCsvSeparator(const QString &val);
    bool firstRowIsHeader();
    bool firstColumnIsHeader();
    void setFirstRowIsHeader(bool val);
    void setFirstColumnIsHeader(bool val);
    virtual bool saveModel(QTextStream &stream) const;
    bool saveModel(QIODevice *destination) const Q_DECL_OVERRIDE;
    bool saveModel(QByteArray *destination) const Q_DECL_OVERRIDE;
    bool saveModel(QString *destination) const Q_DECL_OVERRIDE;
    bool loadModel(QString *source) Q_DECL_OVERRIDE;
    bool loadModel(QIODevice *source) Q_DECL_OVERRIDE;
    bool loadModel(const QByteArray &source) Q_DECL_OVERRIDE;
    virtual bool loadModel(QTextStream &stream);

protected:
    CsvModelSerialiser(CsvModelSerialiserPrivate &d, QObject *parent);

#ifdef MS_DECLARE_STREAM_OPERATORS
    friend QTextStream &operator<<(QTextStream &stream, const QAbstractItemModel &model);
    friend QTextStream &operator>>(QTextStream &stream, QAbstractItemModel &model);
#endif
};
#ifdef MS_DECLARE_STREAM_OPERATORS
QTextStream &operator<<(QTextStream &stream, const QAbstractItemModel &model);
QTextStream &operator>>(QTextStream &stream, QAbstractItemModel &model);
#endif
#endif // CSVMODELSERIALISER_H
