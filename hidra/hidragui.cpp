#include "hidragui.h"
#include "ui_hidragui.h"

HidraGui::HidraGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HidraGui)
{
    ui->setupUi(this);
    Machine *maquina = new AhmesMachine();
    maquina->run();
   // maquina->printStatusDebug();
}

HidraGui::~HidraGui()
{
    delete ui;
}
