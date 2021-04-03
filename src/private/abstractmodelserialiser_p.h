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

#ifndef abstractmultiroleserialiser_p_h__
#define abstractmultiroleserialiser_p_h__
#include "abstractmodelserialiser.h"
#define Magic_Model_Header QStringLiteral("808FC674-78A0-4682-9C17-E05B18A0CDD3") // magic string to mark models
class QAbstractItemModel;
class AbstractModelSerialiserPrivate
{
    Q_DISABLE_COPY(AbstractModelSerialiserPrivate);
    Q_DECLARE_PUBLIC(AbstractModelSerialiser)
protected:
    AbstractModelSerialiserPrivate(AbstractModelSerialiser *q);
    QDataStream::Version m_streamVersion;
    QList<int> m_rolesToSave;
    QAbstractItemModel *m_model;
    const QAbstractItemModel *m_constModel;
    AbstractModelSerialiser *q_ptr;
};
#endif // abstractmultiroleserialiser_p_h__
