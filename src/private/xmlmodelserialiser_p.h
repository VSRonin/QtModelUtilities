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

#ifndef XMLMODELSERIALISER_P_H
#define XMLMODELSERIALISER_P_H

#include "private/abstractstringserialiser_p.h"
#include "xmlmodelserialiser.h"
#include <QModelIndex>
class XmlModelSerialiserPrivate : public AbstractStringSerialiserPrivate
{
    Q_DISABLE_COPY(XmlModelSerialiserPrivate);
    Q_DECLARE_PUBLIC(XmlModelSerialiser)
protected:
    XmlModelSerialiserPrivate(XmlModelSerialiser *q);
    bool writeXml(QXmlStreamWriter &writer) const;
    bool readXml(QXmlStreamReader &reader);
    void writeXmlElement(QXmlStreamWriter &destination, const QModelIndex &parent = QModelIndex()) const;
    bool readXmlElement(QXmlStreamReader &source, const QModelIndex &parent = QModelIndex());
    bool m_printStartDocument;
#ifdef MS_DECLARE_STREAM_OPERATORS
    friend QXmlStreamWriter &operator<<(QXmlStreamWriter &stream, const QAbstractItemModel &model);
    friend QXmlStreamReader &operator>>(QXmlStreamReader &stream, QAbstractItemModel &model);
#endif
};

#endif // XMLMODELSERIALISER_P_H
