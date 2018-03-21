
#ifndef abstractmodelserialiser_p_h__
#define abstractmodelserialiser_p_h__

#include "abstractmodelserialiser.h"
#define Magic_Model_Header QStringLiteral("808FC674-78A0-4682-9C17-E05B18A0CDD3") // magic string to mark models
class AbstractModelSerialiserPrivate
{
    Q_DISABLE_COPY(AbstractModelSerialiserPrivate);
    Q_DECLARE_PUBLIC(AbstractModelSerialiser)
protected:
    QAbstractItemModel* m_model;
    const QAbstractItemModel* m_constModel;
    AbstractModelSerialiser* q_ptr;
    AbstractModelSerialiserPrivate(AbstractModelSerialiser* q);
    static QString variantToString(const QVariant& val);
    static QVariant stringToVariant(const QString& val);
#ifdef QT_GUI_LIB
    static QImage loadImageVariant(int type, const QString& val);
    static QString saveImageVariant(const QImage& val);
#endif
    static QVariant loadVariant(int type, const QString& val);
    static QString saveVariant(const QVariant& val);  
    static int guessDecimals(double val);
    static QString guessDecimalsString(double val, QLocale* loca = Q_NULLPTR);
};

#endif // abstractmodelserialiser_p_h__