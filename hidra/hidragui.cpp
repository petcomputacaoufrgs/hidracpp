#include "hidragui.h"
#include "ui_hidragui.h"
#include <QSizeGrip>
#include "qsizegrip.h"

HidraGui::HidraGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HidraGui)
{
    ui->setupUi(this);

    //CODIGO PARA BETA VERSION
    codeEditor = new HidraCodeEditor();
    connect(codeEditor, SIGNAL(textChanged()), this, SLOT(on_textEditSouceCode_textChanged()));
    ui->layoutSourceCodeHolder->addWidget(codeEditor);

    highlighter = new HidraHighlighter(codeEditor->document());

    //FIM DO BETA CODE
    currentFile = "";
    fileSaved = false;
    buildSuccessful = true;
    showHexaValues = false;
    model = NULL;
    machine = NULL;

    // limpa a interface, e seta a maquina selecionada como o neander
    ui->comboBoxMachine->setCurrentIndex(0);

    updateMachineInterface();
    ui->tableViewMemory->setEditTriggers(0);
}

HidraGui::~HidraGui()
{
    delete ui;
}

bool HidraGui::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseMove)
    {
        QSizeGrip *sg = qobject_cast<QSizeGrip*>(obj);
        if(sg)
            qDebug() << sg->parentWidget();
    }
    return false;
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
        fileSaved = false;
        return;
    }

    fileSaved = true;
    QTextStream out(&file);
    out << codeEditor->toPlainText();
    file.close();

    // Save memory
    machine->save(currentFile.section(".", 0, -2).append(".mem"));

    modifiedFile = false;
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

    if (currentFile.isEmpty()) {
        fileSaved = false;
        return;
    }
    else {
        save(); // Sets fileSaved to true if successful
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
    int base = showHexaValues? 16 : 10;
    foreach (Byte* tmp, auxMem) {
        index = model->index(i,1);
        model->setData(index, QString::number(i, base).toUpper());

        index = model->index(i,2);
        QStandardItem *item = new QStandardItem(QString::number(tmp->getValue(), base).toUpper());
        item->setToolTip(QString::number(tmp->getValue(), 2).rightJustified(8, '0'));
       // model->setData(index, item);
        model->setItem(i,2, item);
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
        ui->checkBoxN_5->setChecked(machine->getFlags().at(0)->getValue());
        ui->checkBoxZ_5->setChecked(machine->getFlags().at(1)->getValue());
        ui->checkBoxC_4->setChecked(machine->getFlags().at(2)->getValue());
        break;
    case 3:
        // TO-DO: Acertar flags Cesar
        break;
    default:
        break;
    }
}

void HidraGui::updateLCDDisplay()
{
    switch (ui->comboBoxMachine->currentIndex()) {
    case 0:
        if(showHexaValues) {    //do it to other machines iin future
            ui->lcdNumberAC_Neander->setHexMode();
            ui->lcdNumberPC_Neander->setHexMode();
        } else {
            ui->lcdNumberAC_Neander->setDecMode();
            ui->lcdNumberPC_Neander->setDecMode();
        }
        ui->lcdNumberAC_Neander->display(machine->getRegisters().at(0)->getValue());
        ui->lcdNumberPC_Neander->display(machine->getRegisters().at(1)->getValue());
        break;
    case 1:
        if(showHexaValues) {
            ui->lcdNumberAC_Ahmes->setHexMode();
            ui->lcdNumberPC_Ahmes->setHexMode();
        } else {
            ui->lcdNumberAC_Ahmes->setDecMode();
            ui->lcdNumberPC_Ahmes->setDecMode();
        }
        ui->lcdNumberAC_Ahmes->display(machine->getRegisters().at(0)->getValue());
        ui->lcdNumberPC_Ahmes->display(machine->getRegisters().at(1)->getValue());
        break;
    case 2:
    if(showHexaValues) {	//refactor to good names
            ui->lcdNumber_11->setHexMode();
            ui->lcdNumber_12->setHexMode();
            ui->lcdNumber_9->setHexMode();
            ui->lcdNumber_10->setHexMode();
        } else {
            ui->lcdNumber_11->setDecMode();
            ui->lcdNumber_12->setDecMode();
            ui->lcdNumber_9->setDecMode();
            ui->lcdNumber_10->setDecMode();
        }
        ui->lcdNumber_11->display(machine->getRegisters().at(0)->getValue());
        ui->lcdNumber_12->display(machine->getRegisters().at(1)->getValue());
        ui->lcdNumber_9->display(machine->getRegisters().at(2)->getValue());
        ui->lcdNumber_10->display(machine->getRegisters().at(3)->getValue());
        break;
    case 3:
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
}

void HidraGui::updateMachineInterface()
{
    updateMemoryMap();
    updateFlagsLeds();
    updateLCDDisplay();
}

void HidraGui::on_actionPasso_triggered()
{
    machine->step();
    updateMachineInterface();
}

void HidraGui::on_actionRodar_triggered()
{
    machine->run();
    updateMachineInterface();
}

void HidraGui::on_actionMontar_triggered()
{
    bool saveRequest = false;

    // Oferece para salvar o arquivo (não é necessário para montar):
    if (modifiedFile) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Salvar arquivo", "Deseja salvar as alterações feitas?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            ui->action_Save->trigger();
            saveRequest = true;
        }
    }

    cleanErrorsField();
    machine->assemble(codeEditor->toPlainText());
    updateMachineInterface();

    if (machine->buildSuccessful)
    {
        sourceAndMemoryInSync = true;
        if (saveRequest && fileSaved) // If file was successfully saved, save .mem too
            machine->save(currentFile.section(".", 0, -2).append(".mem"));
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
        machine = new RamsesMachine();  //evita  o crash
        break;
    default:
        break;
    }
    if(index != 3) {
        connect(machine, SIGNAL(buildErrorDetected(QString)), this, SLOT(addError(QString)));
        highlighter->setTargetMachine(machine);
        updateMachineInterface();
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
        codeEditor->setPlainText(in.readAll());
        modifiedFile = false;
        file.close();
    }
}

void HidraGui::on_textEditSouceCode_textChanged()
{
    sourceAndMemoryInSync = false;
    modifiedFile = true;
}

void HidraGui::on_actionCarregar_triggered()
{

}

void HidraGui::on_actionSaveMem_triggered()
{

}

void HidraGui::on_actionZerarMemoria_triggered()
{
    machine->clearMemory();
    updateMemoryMap();
    updateFlagsLeds();
    updateLCDDisplay();
}

void HidraGui::on_actionZerar_registradores_triggered()
{
    machine->clearRegisters();
    updateMemoryMap();
    updateFlagsLeds();
    updateLCDDisplay();
}

void HidraGui::on_commandLinkButtonMontar_clicked()
{
    ui->actionMontar->trigger();
}

void HidraGui::on_radioButtonHexa_toggled(bool checked)
{
    showHexaValues = checked;
    updateMachineInterface();
}
