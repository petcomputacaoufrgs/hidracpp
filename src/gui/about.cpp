#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    ui->textAbout->setHtml(ui->textAbout->toHtml().replace("__DATE__", QString(__DATE__)));
}

About::~About()
{
    delete ui;
}

void About::on_pushButton_clicked()
{
    this->close();
}
