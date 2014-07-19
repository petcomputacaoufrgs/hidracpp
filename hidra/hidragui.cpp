#include "hidragui.h"
#include "ui_hidragui.h"

HidraGui::HidraGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HidraGui)
{
    ui->setupUi(this);
    Machine *maquina = new NeanderMachine();
    //maquina->printStatusDebug();
    maquina->step();
    //maquina->printStatusDebug();
    maquina->step();
    //maquina->printStatusDebug();
    maquina->step();
    //maquina->printStatusDebug();
    maquina->step();
    //maquina->printStatusDebug();
    maquina->step();
    //maquina->printStatusDebug();
}

HidraGui::~HidraGui()
{
    delete ui;
}
