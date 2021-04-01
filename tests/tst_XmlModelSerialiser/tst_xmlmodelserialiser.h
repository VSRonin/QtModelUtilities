#ifndef tst_xmlmodelserialiser_h__
#define tst_xmlmodelserialiser_h__
#include <QObject>
class QAbstractItemModel;
class tst_XmlModelSerialiser : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void basicSaveLoad();
    void basicSaveLoad_data();
};
#endif
