#ifndef tst_binarymodelserialiser_h__
#define tst_binarymodelserialiser_h__
#include <QObject>
#include <tst_serialiserscommon.h>
class QAbstractItemModel;
class tst_BinaryModelSerialiser : public QObject, public tst_SerialiserCommon
{
    Q_OBJECT
private Q_SLOTS:
    void basicSaveLoadByteArray();
    void basicSaveLoadFile();
    void basicSaveLoadStream();
    void basicSaveLoadByteArray_data() { basicSaveLoadData(this); }
    void basicSaveLoadFile_data() { basicSaveLoadData(this); }
    void basicSaveLoadStream_data() { basicSaveLoadData(this); }
};
#endif
