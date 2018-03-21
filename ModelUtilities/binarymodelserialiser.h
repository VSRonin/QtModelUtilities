
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
public:
    BinaryModelSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    BinaryModelSerialiser(const QAbstractItemModel* model);
    BinaryModelSerialiser(const BinaryModelSerialiser& other);
    BinaryModelSerialiser& operator=(const BinaryModelSerialiser& other);
#ifdef Q_COMPILER_RVALUE_REFS
    BinaryModelSerialiser(BinaryModelSerialiser&& other) Q_DECL_NOEXCEPT;
    BinaryModelSerialiser& operator=(BinaryModelSerialiser&& other);
#endif // Q_COMPILER_RVALUE_REFS
    virtual ~BinaryModelSerialiser() Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool saveModel(QIODevice* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool saveModel(QByteArray* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool loadModel(QIODevice* source) Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool loadModel(const QByteArray& source) Q_DECL_OVERRIDE;
protected:
    BinaryModelSerialiser(BinaryModelSerialiserPrivate& d);

#ifdef MS_DECLARE_STREAM_OPERATORS
    friend QDataStream& operator<<(QDataStream & stream, const QAbstractItemModel& model);
    friend QDataStream& operator>>(QDataStream & stream, QAbstractItemModel& model);
#endif
};
Q_DECLARE_METATYPE(BinaryModelSerialiser)
#ifdef MS_DECLARE_STREAM_OPERATORS
QDataStream& operator<<(QDataStream & stream, const QAbstractItemModel& model);
QDataStream& operator>>(QDataStream & stream, QAbstractItemModel& model);
#endif
#endif // binarymodelserialiser_h__