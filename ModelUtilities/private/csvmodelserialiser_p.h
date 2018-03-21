#ifndef csvmodelserialiser_p_h__
#define csvmodelserialiser_p_h__

#include "private/abstractsingleroleserialiser_p.h"
#include "CsvModelSerialiser.h"
#include <QModelIndex>
class CsvModelSerialiserPrivate : public AbstractSingleRoleSerialiserPrivate
{
    Q_DISABLE_COPY(CsvModelSerialiserPrivate);
    Q_DECLARE_PUBLIC(CsvModelSerialiser)
protected:
    CsvModelSerialiserPrivate(CsvModelSerialiser* q);
    bool m_firstRowIsHeader;
    bool m_firstColumnIsHeader;
    QString m_csvSeparator;
    bool writeCsv(QTextStream& writer) const;
    bool readCsv(QTextStream& reader);
    QString escapedCSV(QString unexc) const;
    QString unescapedCSV(QString exc) const;
    static int guessVarType(const QString& val);
#ifdef MS_DECLARE_STREAM_OPERATORS
    friend QTextStream& operator<<(QTextStream & stream, const QAbstractItemModel& model);
    friend QTextStream& operator>>(QTextStream & stream, QAbstractItemModel& model);
#endif
};
#endif // csvmodelserialiser_p_h__