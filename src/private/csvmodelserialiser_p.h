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
#ifndef CSVMODELSERIALISER_P_H
#define CSVMODELSERIALISER_P_H

#include "private/abstractsingleroleserialiser_p.h"
#include "csvmodelserialiser.h"
#include <QModelIndex>
class CsvModelSerialiserPrivate : public AbstractSingleRoleSerialiserPrivate
{
    Q_DISABLE_COPY(CsvModelSerialiserPrivate);
    Q_DECLARE_PUBLIC(CsvModelSerialiser)
protected:
    CsvModelSerialiserPrivate(CsvModelSerialiser *q);
    bool m_firstRowIsHeader;
    bool m_firstColumnIsHeader;
    QString m_csvSeparator;
    bool writeCsv(QTextStream &writer) const;
    bool readCsv(QTextStream &reader);
    QString escapedCSV(QString unexc) const;
    QString unescapedCSV(QString exc) const;
    static int guessVarType(const QString &val);
#ifdef MS_DECLARE_STREAM_OPERATORS
    friend QTextStream &operator<<(QTextStream &stream, const QAbstractItemModel &model);
    friend QTextStream &operator>>(QTextStream &stream, QAbstractItemModel &model);
#endif
};
#endif // CSVMODELSERIALISER_P_H
