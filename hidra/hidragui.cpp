#include "hidragui.h"
#include "ui_hidragui.h"

HidraGui::HidraGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HidraGui)
{
    ui->setupUi(this);
    NeanderMachine *maquina = new NeanderMachine();
    maquina->printStatusDebug();
}

HidraGui::~HidraGui()
{
    delete ui;
}
