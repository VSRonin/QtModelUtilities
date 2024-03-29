#ifndef TST_HTMLMODELSERIALISER_H
#define TST_HTMLMODELSERIALISER_H
#include <QObject>
#include <tst_serialiserscommon.h>
class QAbstractItemModel;
class QNetworkAccessManager;
class tst_HtmlModelSerialiser : public QObject, public tst_SerialiserCommon
{
    Q_OBJECT
private Q_SLOTS:
    void autoParent();
    void initTestCase();
    void basicSaveLoadByteArray();
    void basicSaveLoadFile();
    void basicSaveLoadString();
    void basicSaveLoadNested();
    void basicSaveLoadByteArray_data() { basicSaveLoadData(this); }
    void basicSaveLoadFile_data() { basicSaveLoadData(this); }
    void basicSaveLoadString_data() { basicSaveLoadData(this); }
    void basicSaveLoadNested_data() { basicSaveLoadData(this); }
    void validateHtmlOutput();
    void validateHtmlOutput_data();

protected:
    QNetworkAccessManager *qnam;
};
#endif
