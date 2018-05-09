

#ifndef xmlmodelserialiser_p_h__
#define xmlmodelserialiser_p_h__

#include "private/abstractmultiroleserialiser_p.h"
#include "xmlmodelserialiser.h"
#include <QModelIndex>
class XmlModelSerialiserPrivate : public AbstractMultiRoleSerialiserPrivate
{
    Q_DISABLE_COPY(XmlModelSerialiserPrivate);
    Q_DECLARE_PUBLIC(XmlModelSerialiser)
protected:
    XmlModelSerialiserPrivate(XmlModelSerialiser* q);
    bool writeXml(QXmlStreamWriter& writer) const;
    bool readXml(QXmlStreamReader& reader);
    void writeXmlElement(QXmlStreamWriter& destination, const QModelIndex& parent = QModelIndex()) const;
    bool readXmlElement(QXmlStreamReader& source, const QModelIndex& parent = QModelIndex());
    bool m_printStartDocument;
#ifdef MS_DECLARE_STREAM_OPERATORS
    friend QXmlStreamWriter&  operator<<(QXmlStreamWriter& stream, const QAbstractItemModel& model);
    friend QXmlStreamReader& operator>>(QXmlStreamReader& stream, QAbstractItemModel& model);
#endif
};


#endif // xmlmodelserialiser_p_h__
