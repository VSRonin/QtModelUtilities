#ifndef tst_htmlmodelserialiser_h__
#define tst_htmlmodelserialiser_h__
#include <QObject>
class QAbstractItemModel;
class tst_HtmlModelSerialiser : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void basicSaveLoad();
    void basicSaveLoad_data();
};
#endif
