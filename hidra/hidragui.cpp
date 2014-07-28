#include "hidragui.h"
#include "ui_hidragui.h"

HidraGui::HidraGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HidraGui)
{
    ui->setupUi(this);

    Machine *maquina = new NeanderMachine();
    maquina->run();
    maquina->printStatusDebug();
    delete maquina;

    maquina = new AhmesMachine();
    maquina->run();
    maquina->printStatusDebug();
    delete maquina;

    maquina = new RamsesMachine();
    maquina->run();
    maquina->printStatusDebug();
    delete maquina;
}

HidraGui::~HidraGui()
{
    delete ui;
}
