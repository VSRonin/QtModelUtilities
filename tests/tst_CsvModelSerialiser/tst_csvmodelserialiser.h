#ifndef tst_csvmodelserialiser_h__
#define tst_csvmodelserialiser_h__
#include <QObject>
class QAbstractItemModel;
class tst_CsvModelSerialiser : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void basicSaveLoad();
    void basicSaveLoad_data();
};
#endif
