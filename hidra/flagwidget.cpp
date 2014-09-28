#include "flagwidget.h"
#include "ui_flagwidget.h"

FlagWidget::FlagWidget(QWidget *parent, QString flagName, bool flagValue) :
    QWidget(parent),
    ui(new Ui::FlagWidget)
{
    ui->setupUi(this);
    ui->flagCheckBox->setText(flagName);
    ui->flagCheckBox->setChecked(flagValue);
}

FlagWidget::~FlagWidget()
{
    delete ui;
}

void FlagWidget::setValue(bool value)
{
    ui->flagCheckBox->setChecked(value);
}
