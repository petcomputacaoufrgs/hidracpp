#include "hidragui.h"
#include "ui_hidragui.h"

HidraGui::HidraGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HidraGui)
{
    ui->setupUi(this);
    Machine *maquina = new NeanderMachine();
    maquina->printStatusDebug();
    maquina->run();
    maquina->printStatusDebug();
    maquina->assemble("../hidra/ndr_test.ndr");
}

HidraGui::~HidraGui()
{
    delete ui;
}
