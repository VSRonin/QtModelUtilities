
#ifndef binarymodelserialiser_h__
#define binarymodelserialiser_h__

#include "modelutilities_global.h"
#include "abstractmultiroleserialiser.h"
class BinaryModelSerialiserPrivate;
class QDataStream;
class MODELUTILITIES_EXPORT BinaryModelSerialiser : public AbstractMultiRoleSerialiser
{
    Q_GADGET
    Q_DECLARE_PRIVATE(BinaryModelSerialiser)
    Q_DISABLE_COPY(BinaryModelSerialiser)
public:
    BinaryModelSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    BinaryModelSerialiser(const QAbstractItemModel* model);
    ~BinaryModelSerialiser();
    Q_INVOKABLE bool saveModel(QIODevice* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE bool saveModel(QByteArray* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE bool loadModel(QIODevice* source) Q_DECL_OVERRIDE;
    Q_INVOKABLE bool loadModel(const QByteArray& source) Q_DECL_OVERRIDE;
protected:
    BinaryModelSerialiser(BinaryModelSerialiserPrivate& d);

#ifdef MS_DECLARE_STREAM_OPERATORS
    friend QDataStream& operator<<(QDataStream & stream, const QAbstractItemModel& model);
    friend QDataStream& operator>>(QDataStream & stream, QAbstractItemModel& model);
#endif
};
#ifdef MS_DECLARE_STREAM_OPERATORS
QDataStream& operator<<(QDataStream & stream, const QAbstractItemModel& model);
QDataStream& operator>>(QDataStream & stream, QAbstractItemModel& model);
#endif
#endif // binarymodelserialiser_h__