#include "pointconversor.h"
#include "ui_pointconversor.h"

PointConversor::PointConversor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PointConversor)
{
    ui->setupUi(this);
}

PointConversor::~PointConversor()
{
    delete ui;
}
