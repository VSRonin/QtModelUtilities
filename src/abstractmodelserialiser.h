
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
    Q_DISABLE_COPY(AbstractModelSerialiser)
public:
    AbstractModelSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    AbstractModelSerialiser(const QAbstractItemModel* model);
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