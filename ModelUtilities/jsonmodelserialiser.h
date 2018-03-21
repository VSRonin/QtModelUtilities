#ifndef jsonmodelserialiser_h__
#define jsonmodelserialiser_h__

#include "modelutilities_global.h"
#include "abstractmultiroleserialiser.h"
#include <QJsonObject>
class JsonModelSerialiserPrivate;
class MODELUTILITIES_EXPORT JsonModelSerialiser : public AbstractMultiRoleSerialiser
{
    Q_GADGET
    Q_DECLARE_PRIVATE(JsonModelSerialiser)
public:
    JsonModelSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    JsonModelSerialiser(const QAbstractItemModel* model);
    JsonModelSerialiser(const JsonModelSerialiser& other);
    JsonModelSerialiser& operator=(const JsonModelSerialiser& other);
#ifdef Q_COMPILER_RVALUE_REFS
    JsonModelSerialiser(JsonModelSerialiser&& other) Q_DECL_NOEXCEPT;
    JsonModelSerialiser& operator=(JsonModelSerialiser&& other);
#endif // Q_COMPILER_RVALUE_REFS
    virtual ~JsonModelSerialiser() Q_DECL_OVERRIDE;

    Q_INVOKABLE virtual bool saveModel(QIODevice* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool saveModel(QByteArray* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool saveModel(QString* destination) const;
    Q_INVOKABLE virtual QJsonObject toJsonObject() const;
    Q_INVOKABLE virtual bool loadModel(QIODevice* source) Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool loadModel(const QByteArray& source) Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool loadModel(QString* source);
    Q_INVOKABLE virtual bool fromJsonObject(const QJsonObject& source);
protected:
    JsonModelSerialiser(JsonModelSerialiserPrivate& d);

};
#endif // jsonmodelserialiser_h__