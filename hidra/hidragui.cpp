#include "hidragui.h"
#include "ui_hidragui.h"

HidraGui::HidraGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HidraGui)
{
    ui->setupUi(this);

    //CODIGO PARA BETA VERSION
    codeEditor = new HidraCodeEditor();
    ui->layoutSourceCodeHolder->addWidget(codeEditor);
    //ui->textEditSouceCode = new HidraCodeEditor();
    //ui->textEditSouceCode->show();
    ui->comboBoxMachine->removeItem(3);
    ui->comboBoxMachine->removeItem(2);

    highlighter = new HidraHighlighter(codeEditor->document());

    //FIM DO BETA CODE
    currentFile = "";
    savedFile = false;
    buildSuccessful = true;
    model = NULL;
    machine = NULL;

    // limpa a interface, e seta a maquina selecionada como o neander
    ui->comboBoxMachine->setCurrentIndex(0);

    updateMemoryMap();
    updateFlagsLeds();
    updateLCDDisplay();
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
    out << codeEditor->toPlainText();
    file.close();
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
    ui->actionPasso->trigger();
}

void HidraGui::on_commandLinkButtonRun_clicked(){
    ui->actionRodar->trigger();
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
        ui->checkBoxN_3->setChecked(machine->getFlags().at(0)->getValue());
        ui->checkBoxZ_3->setChecked(machine->getFlags().at(1)->getValue());
        ui->checkBoxV->setChecked(machine->getFlags().at(2)->getValue());
        ui->checkBoxC_3->setChecked(machine->getFlags().at(3)->getValue());
        ui->checkBoxB_3->setChecked(machine->getFlags().at(4)->getValue());
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

void HidraGui::updateLCDDisplay()
{
    switch (ui->comboBoxMachine->currentIndex()) {
    case 0:
        ui->lcdNumberAC_Neander->display(machine->getRegisters().at(0)->getValue());
        ui->lcdNumberPC_Neander->display(machine->getRegisters().at(1)->getValue());
        break;
    case 1:
        ui->lcdNumberAC_Ahmes->display(machine->getRegisters().at(0)->getValue());
        ui->lcdNumberPC_Ahmes->display(machine->getRegisters().at(1)->getValue());
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

void HidraGui::cleanErrorsField()
{
    ui->textEditError->clear();
}

void HidraGui::addError(QString errorString)
{
    ui->textEditError->setPlainText(ui->textEditError->toPlainText() + errorString + "\n");
    buildSuccessful = false;
}

void HidraGui::on_actionPasso_triggered()
{
    machine->step();
    updateMemoryMap();
    updateFlagsLeds();
    updateLCDDisplay();
}

void HidraGui::on_actionRodar_triggered()
{
    machine->run();
    updateMemoryMap();
    updateFlagsLeds();
    updateLCDDisplay();
}

void HidraGui::on_actionMontar_triggered()
{
    if(!savedFile) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, "Salvar arquivo", "O arquivo näo esta salvo, deseja salva-lo antes de montar?");
        if (reply == QMessageBox::Ok){
            ui->action_Save->trigger();
        }
    }
    if(savedFile) {
        cleanErrorsField();
        machine->assemble(currentFile);
        if(buildSuccessful) {
            machine->load(currentFile.split(".")[0].append(".mem"));
        }
        buildSuccessful = true;
        updateMemoryMap();
        updateFlagsLeds();
        updateLCDDisplay();
    }
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
    connect(machine, SIGNAL(buildErrorDetected(QString)), this, SLOT(addError(QString)));
    highlighter->setTargetMachine(machine);
    updateMemoryMap();
    updateFlagsLeds();
    updateLCDDisplay();
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
        codeEditor->setPlainText(in.readAll());
        savedFile = true;
        file.close();
    }
}

void HidraGui::on_textEditSouceCode_textChanged()
{
    savedFile = false;
}

void HidraGui::on_actionCarregar_triggered()
{

}

void HidraGui::on_actionSaveMem_triggered()
{

}

void HidraGui::on_actionZerarMemoria_triggered()
{
    QVector<Byte *> regs = machine->getMemory();
    foreach (Byte* tmp, regs) {
        tmp->setValue((unsigned char)0);
    }
    updateMemoryMap();
    updateFlagsLeds();
    updateLCDDisplay();
}

void HidraGui::on_actionZerar_registradores_triggered()
{
    QVector<Register *> regs = machine->getRegisters();
    foreach (Register* tmp, regs) {
        tmp->setValue(0);
    }
    updateMemoryMap();
    updateFlagsLeds();
    updateLCDDisplay();
}

void HidraGui::on_commandLinkButtonMontar_clicked()
{
    ui->actionMontar->trigger();
}
