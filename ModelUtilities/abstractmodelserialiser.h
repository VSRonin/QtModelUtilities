
#ifndef abstractmodelserialiser_h__
#define abstractmodelserialiser_h__
#include "modelutilities_global.h"
#include <QObject>
class AbstractModelSerialiserPrivate;
class QAbstractItemModel;
class QIODevice;
class MODELUTILITIES_EXPORT AbstractModelSerialiser
{
    Q_GADGET
    Q_PROPERTY(QAbstractItemModel* model READ model WRITE setModel)
    Q_PROPERTY(const QAbstractItemModel* constModel READ constModel WRITE setModel)
    Q_DECLARE_PRIVATE(AbstractModelSerialiser)
public:
    AbstractModelSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    AbstractModelSerialiser(const QAbstractItemModel* model);
    AbstractModelSerialiser(const AbstractModelSerialiser& other);
    AbstractModelSerialiser& operator=(const AbstractModelSerialiser& other);
#ifdef Q_COMPILER_RVALUE_REFS
    AbstractModelSerialiser(AbstractModelSerialiser&& other) Q_DECL_NOEXCEPT;
    AbstractModelSerialiser& operator=(AbstractModelSerialiser&& other);
#endif // Q_COMPILER_RVALUE_REFS
    virtual ~AbstractModelSerialiser() = 0;
    virtual QAbstractItemModel* model() const;
    virtual const QAbstractItemModel* constModel() const;
    void setModel(QAbstractItemModel* val);
    void setModel(const QAbstractItemModel* val);
    Q_INVOKABLE virtual bool saveModel(QIODevice* destination) const = 0;
    Q_INVOKABLE virtual bool saveModel(QByteArray* destination) const = 0;
    Q_INVOKABLE virtual bool loadModel(QIODevice* source) = 0;
    Q_INVOKABLE virtual bool loadModel(const QByteArray& source) = 0;
protected:
    AbstractModelSerialiserPrivate* d_ptr;
    AbstractModelSerialiser(AbstractModelSerialiserPrivate& d);
};

#endif // abstractmodelserialiser_h__