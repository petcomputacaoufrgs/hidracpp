#include "visorwidget.h"
#include "ui_visorwidget.h"

VisorWidget::VisorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisorWidget)
{
    ui->setupUi(this);
    this->setAttribute( Qt::WA_QuitOnClose, false);
}

VisorWidget::~VisorWidget()
{
    delete ui;
}