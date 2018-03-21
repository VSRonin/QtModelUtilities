
#ifndef binarymodelserialiser_p_h__
#define binarymodelserialiser_p_h__

#include "private/abstractmultiroleserialiser_p.h"
#include "binarymodelserialiser.h"
#include <QModelIndex>
class BinaryModelSerialiserPrivate : public AbstractMultiRoleSerialiserPrivate
{
    Q_DISABLE_COPY(BinaryModelSerialiserPrivate);
    Q_DECLARE_PUBLIC(BinaryModelSerialiser)
protected:
    BinaryModelSerialiserPrivate(BinaryModelSerialiser* q);
    bool writeBinary(QDataStream& writer) const;
    bool readBinary(QDataStream& reader);
    void writeBinaryElement(QDataStream& destination, const QModelIndex& parent = QModelIndex()) const;
    bool readBinaryElement(QDataStream& source, const QModelIndex& parent = QModelIndex());

#ifdef MS_DECLARE_STREAM_OPERATORS
    friend QDataStream& operator<<(QDataStream & stream, const QAbstractItemModel& model);
    friend QDataStream& operator>>(QDataStream & stream, QAbstractItemModel& model);
#endif
};
#endif // binarymodelserialiser_p_h__
