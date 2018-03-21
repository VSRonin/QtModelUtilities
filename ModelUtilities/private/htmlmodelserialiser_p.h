
#ifndef htmlmodelserialiser_p_h__
#define htmlmodelserialiser_p_h__

#include "private/abstractmultiroleserialiser_p.h"
#include "HtmlModelSerialiser.h"
#include <QModelIndex>
class QXmlStreamWriter;
class QXmlStreamReader;
class HtmlModelSerialiserPrivate : public AbstractMultiRoleSerialiserPrivate
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
