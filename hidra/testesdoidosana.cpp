#include "hidragui.h"
#include "ui_hidragui.h"

HidraGui::HidraGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HidraGui)
{
    ui->setupUi(this);
}

HidraGui::~HidraGui()
{
    delete ui;
}

void HidraGui::commandLinkButtonStep_click();{
     this->ui->lineEdit->setText("Aehoo")
}
