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
    connect(codeEditor, SIGNAL(textChanged()), this, SLOT(sourceCodeChanged()));
    ui->layoutSourceCodeHolder->addWidget(codeEditor);

    highlighter = new HidraHighlighter(codeEditor->document());

    //FIM DO BETA CODE
    currentFile = "";
    modifiedFile = false;
    sourceAndMemoryInSync = false;

    fileSaved = false;
    buildSuccessful = true;
    showHexValues = false;

    machine = NULL;

    ui->layoutRegisters->setAlignment(Qt::AlignTop);
    ui->scrollAreaRegisters->setFrameShape(QFrame::NoFrame);
    ui->tableViewMemoryInstructions->setEditTriggers(false);

    // Escolhe a máquina Neander e atualiza a interface
    ui->comboBoxMachine->setCurrentIndex(0);
    updateMachineInterface();
}

HidraGui::~HidraGui()
{
    delete ui;
}

void HidraGui::initializeFlagWidgets()
{
    for (int i=0; i < machine->getNumberOfFlags(); i++)
    {
        FlagWidget *newFlag = new FlagWidget(this, machine->getFlagName(i), machine->getFlagValue(i));
        ui->layoutFlags->addWidget(newFlag);
        flagWidgets.append(newFlag);
    }
}

void HidraGui::initializeRegisterWidgets()
{
    for (int i=0; i < machine->getNumberOfRegisters(); i++)
    {
        RegisterWidget *newRegister = new RegisterWidget(this, machine->getRegisterName(i));
        ui->layoutRegisters->addWidget(newRegister, i/2, i%2); // Two per line, alternates left and right columns with i%2
        registerWidgets.append(newRegister);
    }
}

void HidraGui::initializeMachineInterface()
{
    clearMachineInterface();
    initializeMemoryMap();
    initializeFlagWidgets();
    initializeRegisterWidgets();
}

void HidraGui::clearFlagWidgets()
{
    while(ui->layoutFlags->count() > 0)
        delete ui->layoutFlags->takeAt(0)->widget();

    flagWidgets.clear();
}

void HidraGui::clearRegisterWidgets()
{
    while(ui->layoutRegisters->count() > 0)
        delete ui->layoutRegisters->takeAt(0)->widget();

    registerWidgets.clear();
}

void HidraGui::clearMachineInterface()
{
    clearMemoryMap();
    clearRegisterWidgets();
    clearFlagWidgets();
}

void HidraGui::clearMemoryMap()
{
    model.clear();
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
        save(); // Resets fileModified to false if successful
    }
}

void HidraGui::initializeMemoryMap()
{
    QVector<Byte*> memory = machine->getMemory();
    model.setRowCount(memory.size());
    model.setColumnCount(3);

    model.setHeaderData(0, Qt::Horizontal, " ");
    model.setHeaderData(1, Qt::Horizontal, "End");
    model.setHeaderData(2, Qt::Horizontal, "Valor");

    ui->tableViewMemoryInstructions->setModel(&model);

    ui->tableViewMemoryInstructions->resizeColumnsToContents();
    ui->tableViewMemoryInstructions->resizeRowsToContents();
    ui->tableViewMemoryInstructions->verticalHeader()->hide();
}

void HidraGui::updateMemoryMap()
{
    QVector<Byte*> memory = machine->getMemory();
    int pcValue = machine->getPCValue();
    int base = showHexValues? 16 : 10;

    QModelIndex index;

    for (int byteAddress=0; byteAddress<memory.size(); byteAddress++)
    {
        // Column 0: PC Arrow
        index = model.index(byteAddress, 0);
        model.setData(index, byteAddress == pcValue ? QString::fromUtf8("\u2192") : ""); // Unicode arrow / blank

        // Column 1: Byte address
        index = model.index(byteAddress, 1);
        model.setData(index, QString::number(byteAddress, base).toUpper());

        // Column 2: Byte value
        index = model.index(byteAddress, 2);
        model.setData(index, QString::number(memory[byteAddress]->getValue(), base).toUpper());
    }

    // TODO Binary tooltip/status bar

    // Update all cells
    emit model.dataChanged(model.index(0, 0), model.index(memory.size(), 0));
}

void HidraGui::updateFlagWidgets()
{
    for (int i=0; i<flagWidgets.count(); i++)
        flagWidgets.at(i)->setValue(machine->getFlagValue(i));
}

void HidraGui::updateRegisterWidgets()
{
    for (int i=0; i<registerWidgets.count(); i++)
        registerWidgets.at(i)->setValue(machine->getRegisterValue(i));
}

void HidraGui::updateMachineInterface()
{
    updateMemoryMap();
    updateFlagWidgets();
    updateRegisterWidgets();
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

void HidraGui::clearErrorsField()
{
    ui->textEditError->clear();
}

void HidraGui::addError(QString errorString)
{
    ui->textEditError->setPlainText(ui->textEditError->toPlainText() + errorString + "\n");
}

void HidraGui::on_pushButtonStep_clicked(){
    ui->actionPasso->trigger();
}

void HidraGui::on_pushButtonRun_clicked(){
    ui->actionRodar->trigger();
}

void HidraGui::on_actionPasso_triggered()
{
    machine->step();
    updateMachineInterface();
}

void HidraGui::on_actionRodar_triggered()
{
    // If already running
    if (machine->isRunning())
    {
        // Stop
        machine->setRunning(false);
        ui->pushButtonRun->setText("Rodar");
    }
    else
    {
        // Start running
        machine->setRunning(true);
        ui->pushButtonRun->setText("Parar");

        // Keep running until stopped
        while (machine->isRunning()) {
            ui->actionPasso->trigger();
            QApplication::processEvents();
        }
    }
}

void HidraGui::on_actionMontar_triggered()
{
    clearErrorsField();
    machine->assemble(codeEditor->toPlainText());
    updateMachineInterface();

    if (machine->buildSuccessful)
        sourceAndMemoryInSync = true;
}

void HidraGui::on_actionSaveAs_triggered()
{
    saveAs();
}

void HidraGui::on_comboBoxMachine_currentIndexChanged(int index)
{
    delete machine;
    switch (index) {
    case 0:
        machine = new NeanderMachine();
        break;
    case 1:
        machine = new AhmesMachine();
        break;
    case 2:
        machine = new RamsesMachine();
        break;
    case 3:
        //machine = new CesarMachine();
        machine = new RamsesMachine();  //evita  o crash
        break;
    default:
        break;
    }
    if(index != 3) {
        connect(machine, SIGNAL(buildErrorDetected(QString)), this, SLOT(addError(QString)));
        highlighter->setTargetMachine(machine);
    }

    initializeMachineInterface();
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

void HidraGui::closeEvent(QCloseEvent *event)
{
    bool cancelled = false;

    // Se o arquivo foi modificado, oferece para salvar alterações
    if (modifiedFile)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Hidra",
                                      "Deseja salvar as alterações feitas?",
                                      QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

        if (reply == QMessageBox::Cancel)
            cancelled = true;

        if (reply == QMessageBox::Yes)
        {
            ui->action_Save->trigger();

            if (modifiedFile) // Se o arquivo não foi salvo no diálogo (ainda está modificado), cancela
                cancelled = true;
        }
    }

    // Aceita ou rejeita o evento que fecha a janela
    if (!cancelled)
        event->accept();
    else
        event->ignore();
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

void HidraGui::sourceCodeChanged()
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
    updateMachineInterface();
}

void HidraGui::on_actionZerar_registradores_triggered()
{
    machine->clearRegisters();
    machine->clearFlags();
    updateMachineInterface();
}

void HidraGui::on_pushButtonMontar_clicked()
{
    ui->actionMontar->trigger();
}

void HidraGui::on_actionHexadecimal_toggled(bool checked)
{
    showHexValues = checked;

    for (int i=0; i<registerWidgets.count(); i++)
        registerWidgets.at(i)->setMode(showHexValues);

    updateMachineInterface();
}

