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
#endif
