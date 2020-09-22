#include "baseconversor.h"
#include "ui_baseconversor.h"

BaseConversor::BaseConversor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BaseConversor)
{
    ui->setupUi(this);
}

BaseConversor::~BaseConversor()
{
    delete ui;
}
