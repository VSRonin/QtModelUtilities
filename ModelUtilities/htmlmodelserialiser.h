#ifndef htmlmodelserialiser_h__
#define htmlmodelserialiser_h__

#include "modelutilities_global.h"
#include "abstractmultiroleserialiser.h"
class HtmlModelSerialiserPrivate;
class MODELUTILITIES_EXPORT HtmlModelSerialiser : public AbstractMultiRoleSerialiser
{
    Q_GADGET
    Q_PROPERTY(bool printStartDocument READ printStartDocument WRITE setPrintStartDocument)
    Q_DECLARE_PRIVATE(HtmlModelSerialiser)
public:
    HtmlModelSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    HtmlModelSerialiser(const QAbstractItemModel* model);
    HtmlModelSerialiser(const HtmlModelSerialiser& other);
    HtmlModelSerialiser& operator=(const HtmlModelSerialiser& other);
#ifdef Q_COMPILER_RVALUE_REFS
    HtmlModelSerialiser(HtmlModelSerialiser&& other) Q_DECL_NOEXCEPT;
    HtmlModelSerialiser& operator=(HtmlModelSerialiser&& other);
#endif // Q_COMPILER_RVALUE_REFS
    virtual ~HtmlModelSerialiser() Q_DECL_OVERRIDE;
    bool printStartDocument() const;
    void setPrintStartDocument(bool val);
    Q_INVOKABLE virtual bool saveModel(QIODevice* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool saveModel(QByteArray* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool saveModel(QString* destination) const;
    Q_INVOKABLE virtual bool loadModel(QIODevice* source) Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool loadModel(const QByteArray& source) Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool loadModel(QString* source);
protected:
    HtmlModelSerialiser(HtmlModelSerialiserPrivate& d);
};
Q_DECLARE_METATYPE(HtmlModelSerialiser)
#endif // htmlmodelserialiser_h__