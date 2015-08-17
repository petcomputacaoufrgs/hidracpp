#include "registerwidget.h"
#include "ui_registerwidget.h"

RegisterWidget::RegisterWidget(QWidget *parent, QString registerName) :
    QWidget(parent),
    ui(new Ui::RegisterWidget)
{
    ui->setupUi(this);
    ui->registerName->setText(registerName);
    setValue(0);
}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}

void RegisterWidget::setValue(int value)
{
    ui->registerValue->display(value);
    ui->registerValue->setToolTip(QString("%3").arg(value, 8, 2, QChar('0')));
}

void RegisterWidget::setMode(bool hexMode)
{
    if (hexMode)
        ui->registerValue->setHexMode();
    else
        ui->registerValue->setDecMode();
}
