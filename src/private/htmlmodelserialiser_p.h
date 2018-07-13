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

#ifndef htmlmodelserialiser_p_h__
#define htmlmodelserialiser_p_h__

#include "private/abstractstringserialiser_p.h"
#include "htmlmodelserialiser.h"
#include <QModelIndex>
class QXmlStreamWriter;
class QXmlStreamReader;
class HtmlModelSerialiserPrivate : public AbstractStringSerialiserPrivate
{
    Q_DISABLE_COPY(HtmlModelSerialiserPrivate);
    Q_DECLARE_PUBLIC(HtmlModelSerialiser)
protected:
    HtmlModelSerialiserPrivate(HtmlModelSerialiser* q);
    void writeHtmlElement(QXmlStreamWriter& destination, const QModelIndex& parent = QModelIndex()) const;
    bool readHtmlElement(QXmlStreamReader& source, const QModelIndex& parent = QModelIndex());
    bool writeHtml(QXmlStreamWriter& writer) const;
    bool readHtml(QXmlStreamReader& reader);
    static Q_DECL_CONSTEXPR bool isImageType(int val) Q_DECL_NOEXCEPT{
        return val == QMetaType::QImage
        || val == QMetaType::QPixmap
        || val == QMetaType::QBitmap
        ;
    }
    static void writeHtmlVariant(QXmlStreamWriter& writer, const QVariant& val);
    static QVariant readHtmlVariant(QXmlStreamReader& reader, int valType);
    bool m_printStartDocument;
};
#endif // htmlmodelserialiser_p_h__
