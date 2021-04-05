#ifndef tst_jsonmodelserialiser_h__
#define tst_jsonmodelserialiser_h__
#include <QObject>
#include <tst_serialiserscommon.h>
class QAbstractItemModel;
class tst_JsonModelSerialiser : public QObject, public tst_SerialiserCommon
{
    Q_OBJECT
private Q_SLOTS:
    void basicSaveLoadByteArray();
    void basicSaveLoadFile();
    void basicSaveLoadObject();
    void basicSaveLoadString();
    void basicSaveLoadByteArray_data() { basicSaveLoadData(this); }
    void basicSaveLoadFile_data() { basicSaveLoadData(this); }
    void basicSaveLoadObject_data() { basicSaveLoadData(this); }
    void basicSaveLoadString_data() { basicSaveLoadData(this); }
    void validateJsonOutput();
    void validateJsonOutput_data();
};
#endif
