
#ifndef csvmodelserialiser_h__
#define csvmodelserialiser_h__

#include "modelutilities_global.h"
#include "abstractsingleroleserialiser.h"
class CsvModelSerialiserPrivate;
class QTextStream;
class MODELUTILITIES_EXPORT CsvModelSerialiser : public AbstractSingleRoleSerialiser
{
    Q_GADGET
    Q_DECLARE_PRIVATE(CsvModelSerialiser)
    Q_PROPERTY(QString csvSeparator READ csvSeparator WRITE setCsvSeparator)
    Q_PROPERTY(bool firstRowIsHeader READ firstRowIsHeader WRITE setFirstRowIsHeader)
    Q_PROPERTY(bool firstColumnIsHeader READ firstColumnIsHeader WRITE setFirstColumnIsHeader)
public:
    CsvModelSerialiser(QAbstractItemModel* model = Q_NULLPTR);
    CsvModelSerialiser(const QAbstractItemModel* model);
    CsvModelSerialiser(const CsvModelSerialiser& other);
    CsvModelSerialiser& operator=(const CsvModelSerialiser& other);
#ifdef Q_COMPILER_RVALUE_REFS
    CsvModelSerialiser(CsvModelSerialiser&& other) Q_DECL_NOEXCEPT;
    CsvModelSerialiser& operator=(CsvModelSerialiser&& other);
#endif // Q_COMPILER_RVALUE_REFS
    virtual ~CsvModelSerialiser() Q_DECL_OVERRIDE;
    const QString& csvSeparator() const;
    void setCsvSeparator(const QString& val);
    bool firstRowIsHeader();
    bool firstColumnIsHeader();
    void setFirstRowIsHeader(bool val);
    void setFirstColumnIsHeader(bool val);
    Q_INVOKABLE virtual bool saveModel(QIODevice* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool saveModel(QByteArray* destination) const Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool saveModel(QString* destination) const;
    Q_INVOKABLE virtual bool loadModel(QIODevice* source) Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool loadModel(const QByteArray& source) Q_DECL_OVERRIDE;
    Q_INVOKABLE virtual bool loadModel(QString* source);
protected:
    CsvModelSerialiser(CsvModelSerialiserPrivate& d);

#ifdef MS_DECLARE_STREAM_OPERATORS
    friend QTextStream& operator<<(QTextStream & stream, const QAbstractItemModel& model);
    friend QTextStream& operator>>(QTextStream & stream, QAbstractItemModel& model);
#endif
};
Q_DECLARE_METATYPE(CsvModelSerialiser)
#ifdef MS_DECLARE_STREAM_OPERATORS
QTextStream& operator<<(QTextStream & stream, const QAbstractItemModel& model);
QTextStream& operator>>(QTextStream & stream, QAbstractItemModel& model);
#endif
#endif // csvmodelserialiser_h__