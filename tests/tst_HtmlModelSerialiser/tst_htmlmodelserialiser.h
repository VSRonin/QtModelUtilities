#ifndef tst_htmlmodelserialiser_h__
#define tst_htmlmodelserialiser_h__
#include <QObject>
#include <tst_serialiserscommon.h>
class QAbstractItemModel;
class tst_HtmlModelSerialiser : public QObject, public tst_SerialiserCommon
{
    Q_OBJECT
private Q_SLOTS:
    void basicSaveLoadByteArray();
    void basicSaveLoadFile();
    void basicSaveLoadString();
    void basicSaveLoadNested();
    void basicSaveLoadByteArray_data() { basicSaveLoadData(); }
    void basicSaveLoadFile_data() { basicSaveLoadData(); }
    void basicSaveLoadString_data() { basicSaveLoadData(); }
    void basicSaveLoadNested_data() { basicSaveLoadData(); }

protected:
    void basicSaveLoadData();
};
#endif
