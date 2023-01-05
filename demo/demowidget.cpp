#include "demowidget.h"
#include "./ui_demowidget.h"

DemoWidget::DemoWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DemoWidget)
{
    ui->setupUi(this);
}

DemoWidget::~DemoWidget()
{
    delete ui;
}
