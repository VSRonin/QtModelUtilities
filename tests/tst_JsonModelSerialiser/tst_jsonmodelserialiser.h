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
    void basicSaveLoadByteArray_data(){basicSaveLoadData();}
    void basicSaveLoadFile_data(){basicSaveLoadData();}
    void basicSaveLoadObject_data(){basicSaveLoadData();}
    void basicSaveLoadString_data(){basicSaveLoadData();}
protected:
    void basicSaveLoadData();
};
#endif
