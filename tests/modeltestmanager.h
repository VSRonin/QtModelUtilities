#ifndef MODELTESTMANAGER_H
#define MODELTESTMANAGER_H

#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
#    ifdef MOC_MODEL_TEST
class ModelTest : public QObject
{
    Q_DISABLE_COPY(ModelTest)
public:
    ModelTest(QAbstractItemModel *model, QObject *parent = Q_NULLPTR)
        : QObject(parent)
    {
        Q_UNUSED(model)
    }
};
#    else
#        include "modeltest.h"
#    endif
#else
#    include <QAbstractItemModelTester>
class ModelTest : public QAbstractItemModelTester
{
    Q_DISABLE_COPY(ModelTest)
public:
    ModelTest(QAbstractItemModel *model, QObject *parent)
        : QAbstractItemModelTester(model, QAbstractItemModelTester::FailureReportingMode::QtTest, parent)
    { }
};
#endif

#ifndef COMPLEX_MODEL_SUPPORT
#if defined(QT_GUI_LIB)
#define COMPLEX_MODEL_SUPPORT
#ifndef SKIP_QTBUG_45114
#if (QT_VERSION < QT_VERSION_CHECK(5, 11, 0))
#define SKIP_QTBUG_45114
#endif
#endif
#include <QStandardItemModel>
using ComplexModel = QStandardItemModel;
#elif defined(QTMODELUTILITIES_GENERICMODEL)
#define COMPLEX_MODEL_SUPPORT
#include <genericmodel.h>
using ComplexModel = GenericModel;
#endif
#endif

#ifndef SKIP_QTBUG_92220
#if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0) || (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) && QT_VERSION <= QT_VERSION_CHECK(6, 0, 3)))
#define SKIP_QTBUG_92220
#endif
#endif

#ifndef SKIP_QTBUG_67511
#if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
#define SKIP_QTBUG_67511
#endif
#endif

#ifndef SKIP_QTBUG_92886
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 4) || QT_VERSION == QT_VERSION_CHECK(6, 1, 0))
#define SKIP_QTBUG_92886
#endif
#endif


#endif



