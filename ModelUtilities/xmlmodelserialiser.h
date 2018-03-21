

#ifndef xmlmodelserialiser_h__
#define xmlmodelserialiser_h__

#include "modelutilities_global.h"
#include "abstractmultiroleserialiser.h"
class XmlModelSerialiserPrivate;
class QXmlStreamWriter;
class QXmlStreamReader;
class MODELUTILITIES_EXPORT XmlModelSerialiser : public AbstractMultiRoleSerialiser
{
    Q_GADGET
    Q_PROPERTY(bool printStartDocument READ printStartDocument WRITE setPrintStartDocument)
    Q_DECLARE_PRIVATE(XmlModelSerialiser)
public:
    XmlModelSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    XmlModelSerialiser(const QAbstractItemModel* model);
    XmlModelSerialiser(const XmlModelSerialiser& other);
    XmlModelSerialiser& operator=(const XmlModelSerialiser& other);
#ifdef Q_COMPILER_RVALUE_REFS
    XmlModelSerialiser(XmlModelSerialiser&& other) Q_DECL_NOEXCEPT;
    XmlModelSerialiser& operator=(XmlModelSerialiser&& other);
#endif // Q_COMPILER_RVALUE_REFS
    virtual ~XmlModelSerialiser() Q_DECL_OVERRIDE;
    bool printStartDocument() const;
    void setPrintStartDocument(bool val);
    Q_INVOKABLE virtual bool saveModel(QIODevice* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool saveModel(QByteArray* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool saveModel(QString* destination) const;
    Q_INVOKABLE virtual bool loadModel(QIODevice* source) Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool loadModel(const QByteArray& source) Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool loadModel(QString* source);
protected:
    XmlModelSerialiser(XmlModelSerialiserPrivate& d);

#ifdef MS_DECLARE_STREAM_OPERATORS
    friend QXmlStreamWriter&  operator<<(QXmlStreamWriter & stream, const QAbstractItemModel& model);
    friend QXmlStreamReader& operator>>(QXmlStreamReader & stream, QAbstractItemModel& model);
#endif
};
Q_DECLARE_METATYPE(XmlModelSerialiser)
#ifdef MS_DECLARE_STREAM_OPERATORS
QXmlStreamWriter&  operator<<(QXmlStreamWriter & stream, const QAbstractItemModel& model);
QXmlStreamReader& operator>>(QXmlStreamReader & stream, QAbstractItemModel& model);
#endif
#endif // xmlmodelserialiser_h__