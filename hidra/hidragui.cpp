#include "hidragui.h"
#include "ui_hidragui.h"

HidraGui::HidraGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HidraGui)
{
    ui->setupUi(this);

    currentFile = "";
    savedFile = false;
    model = NULL;
    machine = NULL;
    // limpa a interface, e seta a maquina selecionada como o neander
    ui->comboBoxMachine->setCurrentIndex(0);

    updateMemoryMap();
    updateFlagsLeds();
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

void HidraGui::save()
{
    QFile file(currentFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(this, "Incapaz de abrir arquivo",
                                 file.errorString());
        return;
    }
    QTextStream out(&file);
    out << ui->textEditSouceCode->toPlainText();

    savedFile = true;
}

void HidraGui::saveAs()
{
    QString ext;
    switch (ui->comboBoxMachine->currentIndex()) {
    case 0:
        ext = "Fonte do Neander (*.ndr)";
        break;
    case 1:
        ext = "Fonte do Ahmes (*.ahd)";
        break;
    case 2:
        ext = "Fonte do Ramses (*.rms)";
        break;
    default:
        break;
    }
    currentFile = QFileDialog::getSaveFileName(this,
                                               "Salvar código-fonte", "",
                                               ext);

    if (currentFile.isEmpty())
        return;
    else {
        save();
    }
}

void HidraGui::on_commandLinkButtonStep_clicked(){
    qDebug() << "step";
    machine->step();
    updateMemoryMap();
    updateFlagsLeds();
}

void HidraGui::on_commandLinkButtonRun_clicked(){
    qDebug() << "run";
    machine->run();
    updateMemoryMap();
    updateFlagsLeds();
}

void HidraGui::updateMemoryMap()
{
    delete model;
    model = new QStandardItemModel(NeanderMachine::MEM_SIZE, 3, this);  //temp gamby, will change

    model->setHeaderData(0, Qt::Horizontal, " ");
    model->setHeaderData(1, Qt::Horizontal, "End");
    model->setHeaderData(2, Qt::Horizontal, "Valor");
    QVector<Byte *> auxMem = machine->getMemory();
    int i = 0;
    QModelIndex index;
    foreach (Byte* tmp, auxMem) {
        index = model->index(i,1);
        model->setData(index, i);

        index = model->index(i,2);
        model->setData(index, tmp->getValue());
        i++;

    }
    index = model->index(machine->getRegisters().last()->getValue(), 0);
    model->setData(index, "->");

    ui->tableViewMemory->setModel(model);
    ui->tableViewMemory->resizeColumnsToContents();
    ui->tableViewMemory->resizeRowsToContents();
    ui->tableViewMemory->verticalHeader()->hide();
}

void HidraGui::updateFlagsLeds()
{
    switch (ui->comboBoxMachine->currentIndex()) {
    case 0:
        ui->checkBoxN_4->setChecked(machine->getFlags().at(0)->getValue());
        ui->checkBoxZ_4->setChecked(machine->getFlags().at(1)->getValue());
        break;
    case 1:
        ui->frameAhmes->setVisible(true);
        machine = new AhmesMachine();
        break;
    case 2:
        ui->frameRamses->setVisible(true);
        machine = new RamsesMachine();
        break;
    case 3:
        ui->frameCesar->setVisible(true);
        //machine = new CesarMachine();
        machine = NULL; //evita o crash
        break;
    default:
        break;
    }
}

void HidraGui::on_actionPasso_triggered()
{
    qDebug() << "step";
    machine->printStatusDebug();
    machine->step();
    updateMemoryMap();
    updateFlagsLeds();
}

void HidraGui::on_actionRodar_triggered()
{
    qDebug() << "run";
    machine->run();
    updateMemoryMap();
    updateFlagsLeds();
}

void HidraGui::on_actionMontar_triggered()
{
    machine->assemble(currentFile);
    //qDebug() << currentFile.split(".").at(0).append(".mem");
    machine->load(currentFile.split(".")[0].append(".mem"));
    updateMemoryMap();
    updateFlagsLeds();
}

void HidraGui::on_actionSaveAs_triggered()
{
    saveAs();
}

void HidraGui::on_comboBoxMachine_currentIndexChanged(int index)
{
    cleanMachines();
    delete machine;
    switch (index) {
    case 0:
        ui->frameNeander->setVisible(true);
        machine = new NeanderMachine();
        machine->printStatusDebug();
        break;
    case 1:
        ui->frameAhmes->setVisible(true);
        machine = new AhmesMachine();
        break;
    case 2:
        ui->frameRamses->setVisible(true);
        machine = new RamsesMachine();
        break;
    case 3:
        ui->frameCesar->setVisible(true);
        //machine = new CesarMachine();
        machine = NULL; //evita o crash
        break;
    default:
        break;
    }
}

void HidraGui::on_action_Save_triggered()
{
    if(currentFile == "") {
        saveAs();
    } else {
        save();
    }
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

void HidraGui::on_actionOpen_triggered()
{
    QString ext;
    switch (ui->comboBoxMachine->currentIndex()) {
    case 0:
        ext = "Fonte do Neander (*.ndr)";
        break;
    case 1:
        ext = "Fonte do Ahmes (*.ahd)";
        break;
    case 2:
        ext = "Fonte do Ramses (*.rms)";
        break;
    default:
        break;
    }
    currentFile = QFileDialog::getOpenFileName(this,
                                               "Abrir código-fonte", "",
                                               ext);

    if (currentFile.isEmpty())
        return;
    else {
        QFile file(currentFile);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::information(this, tr("Incapaz de abrir arquivo"),
                                     file.errorString());
            return;
        }
        QTextStream in(&file);
        ui->textEditSouceCode->setPlainText(in.readAll());
    }
}
