#ifndef TST_CSVMODELSERIALISER_H
#define TST_CSVMODELSERIALISER_H
#include <QObject>
#include <tst_serialiserscommon.h>
class QAbstractItemModel;
class CsvModelSerialiser;
class tst_CsvModelSerialiser : public QObject, public tst_SerialiserCommon
{
    Q_OBJECT
private Q_SLOTS:
    void autoParent();
    void basicSaveLoadByteArray();
    void basicSaveLoadFile();
    void basicSaveLoadString();
    void basicSaveLoadStream();
    void basicSaveLoadByteArray_data() { basicSaveLoadData(); }
    void basicSaveLoadFile_data() { basicSaveLoadData(); }
    void basicSaveLoadString_data() { basicSaveLoadData(); }
    void basicSaveLoadStream_data() { basicSaveLoadData(); }

    void basicSaveLoadByteArrayNoHeader();
    void basicSaveLoadFileNoHeader();
    void basicSaveLoadStringNoHeader();
    void basicSaveLoadStreamNoHeader();
    void basicSaveLoadByteArrayNoHeader_data() { basicSaveLoadData(); }
    void basicSaveLoadFileNoHeader_data() { basicSaveLoadData(); }
    void basicSaveLoadStringNoHeader_data() { basicSaveLoadData(); }
    void basicSaveLoadStreamNoHeader_data() { basicSaveLoadData(); }

    void basicSaveLoadByteArrayCustomSeparator();
    void basicSaveLoadFileCustomSeparator();
    void basicSaveLoadStringCustomSeparator();
    void basicSaveLoadStreamCustomSeparator();
    void basicSaveLoadByteArrayCustomSeparator_data() { basicSaveLoadData(); }
    void basicSaveLoadFileCustomSeparator_data() { basicSaveLoadData(); }
    void basicSaveLoadStringCustomSeparator_data() { basicSaveLoadData(); }
    void basicSaveLoadStreamCustomSeparator_data() { basicSaveLoadData(); }

protected:
    void basicSaveLoadData(QObject *parent = nullptr) override;
};
#endif
