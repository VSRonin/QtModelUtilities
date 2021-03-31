#ifndef tst_binarymodelserialiser_h__
#define tst_binarymodelserialiser_h__
#include <QObject>
class QAbstractItemModel;
class tst_BinaryModelSerialiser : public QObject{
    Q_OBJECT
private Q_SLOTS:
    void basicSaveLoad();
    void basicSaveLoad_data();
};
#endif
