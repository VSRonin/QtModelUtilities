/****************************************************************************\
   Copyright 2018 Luca Beldi
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
\****************************************************************************/

#ifndef binarymodelserialiser_p_h__
#define binarymodelserialiser_p_h__

#include "private/abstractmodelserialiser_p.h"
#include "binarymodelserialiser.h"
#include <QModelIndex>
class BinaryModelSerialiserPrivate : public AbstractModelSerialiserPrivate
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
