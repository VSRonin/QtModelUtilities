#ifndef tst_xmlmodelserialiser_h__
#define tst_xmlmodelserialiser_h__
#include <QObject>
#include <tst_serialiserscommon.h>
class QAbstractItemModel;
class tst_XmlModelSerialiser : public QObject, public tst_SerialiserCommon
{
    Q_OBJECT
private Q_SLOTS:
    void basicSaveLoadByteArray();
    void basicSaveLoadFile();
    void basicSaveLoadString();
    void basicSaveLoadStream();
    void basicSaveLoadNested();
    void basicSaveLoadByteArray_data() { basicSaveLoadData(this); }
    void basicSaveLoadFile_data() { basicSaveLoadData(this); }
    void basicSaveLoadString_data() { basicSaveLoadData(this); }
    void basicSaveLoadNested_data() { basicSaveLoadData(this); }
    void basicSaveLoadStream_data() { basicSaveLoadData(this); }
};
#endif
