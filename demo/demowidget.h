#ifndef DEMOWIDGET_H
#define DEMOWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class DemoWidget; }
QT_END_NAMESPACE

class DemoWidget : public QWidget
{
    Q_OBJECT

public:
    DemoWidget(QWidget *parent = nullptr);
    ~DemoWidget();

private:
    Ui::DemoWidget *ui;
};
#endif // DEMOWIDGET_H
