#include "hidragui.h"
#include "ui_hidragui.h"

HidraGui::HidraGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HidraGui)
{
    ui->setupUi(this);
    cleanMachines();
    ui->comboBoxMachine->setCurrentIndex(0);

    model = new QStandardItemModel(256,2, this);

    model->setHeaderData(0, Qt::Horizontal, "End");
    model->setHeaderData(1, Qt::Horizontal, "Valor");

    ui->tableViewMemory->setModel(model);
    ui->tableViewMemory->resizeColumnsToContents();
    ui->tableViewMemory->resizeRowsToContents();

    ui->tableViewMemory->setEditTriggers(0);
}

HidraGui::~HidraGui()
{
    delete ui;
}

void HidraGui::cleanMachines()
{
    ui->frameAhmes->setVisible(false);
    ui->frameRamses->setVisible(false);
    ui->frameCesar->setVisible(false);
    ui->frameNeander->setVisible(false);
}

void HidraGui::on_commandLinkButtonStep_clicked(){
    this->ui->textEditSouceCode->setPlainText("Passo");
}

void HidraGui::on_commandLinkButtonRun_clicked(){
    this->ui->textEditSouceCode->setPlainText("Rodar");
}

//void HidraGui::comboBoxMachine(){//arrumar aqui
//    if (machine == neander){
//        Machine=NeanderMachine;
//    }else
//        if (machine == ahmes){
//            Machine=AhmesMachine;
//        }else
//            if (machine == ramses){
//                Machine=RamsesMachine;
//            }
//}

//void HidraGui::check

void HidraGui::on_actionPasso_triggered()
{
    machine->step();
}

void HidraGui::on_actionRodar_triggered()
{
    machine->run();
}

void HidraGui::on_actionMontar_triggered()
{
    machine->assemble(actualFile);
}

void HidraGui::on_actionSaveAs_triggered()
{

}

void HidraGui::on_comboBoxMachine_currentIndexChanged(int index)
{
    cleanMachines();
    switch (index) {
    case 0:
        ui->frameNeander->setVisible(true);
        break;
    case 1:
        ui->frameAhmes->setVisible(true);
        break;
    case 2:
        ui->frameRamses->setVisible(true);
        break;
    case 3:
        ui->frameCesar->setVisible(true);
        break;
    default:
        break;
    }
}

void HidraGui::on_action_Save_triggered()
{

}

void HidraGui::on_actionClose_triggered()
{
    this->close();
}

void HidraGui::on_actionManual_triggered()
{

}

void HidraGui::on_actionRelatar_um_problema_triggered()
{

}
