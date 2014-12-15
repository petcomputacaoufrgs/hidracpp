#include "hidragui.h"
#include "ui_hidragui.h"
#include <QSizeGrip>

HidraGui::HidraGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HidraGui)
{
    ui->setupUi(this);

    codeEditor  = new HidraCodeEditor();
    highlighter = new HidraHighlighter(codeEditor->document());
    ui->layoutSourceCodeHolder->addWidget(codeEditor);
    connect(codeEditor, SIGNAL(textChanged()), this, SLOT(sourceCodeChanged()));

    currentFile = "";

    fileSaved = false;
    buildSuccessful = true;
    showHexValues = false;

    sourceAndMemoryInSync = false;
    machine = NULL;

    ui->layoutRegisters->setAlignment(Qt::AlignTop);
    ui->scrollAreaRegisters->setFrameShape(QFrame::NoFrame);
    ui->tableViewMemoryInstructions->setEditTriggers(false);

    // Escolhe a máquina Neander e atualiza a interface
    ui->comboBoxMachine->setCurrentIndex(0);

    modifiedFile = false;
}

HidraGui::~HidraGui()
{
    delete ui;
}



//////////////////////////////////////////////////
// Initialize/update methods
//////////////////////////////////////////////////

void HidraGui::initializeMachineInterface()
{
    clearMachineInterfaceComponents();
    initializeMachineInterfaceComponents();
    updateMachineInterfaceComponents();
}

void HidraGui::updateMachineInterface()
{
    updateMachineInterfaceComponents();
}



//////////////////////////////////////////////////
// Initialization internal methods
//////////////////////////////////////////////////

void HidraGui::initializeMachineInterfaceComponents()
{
    initializeMemoryTable();
    initializeFlagWidgets();
    initializeRegisterWidgets();
    initializeHighlighter();
}

void HidraGui::initializeMemoryTable()
{
    ui->tableViewMemoryInstructions->setModel(&model);
    QVector<Byte*> memory = machine->getMemory();

    // Set table size
    model.setRowCount(memory.size());
    model.setColumnCount(3);

    // Set table headers
    model.setHeaderData(0, Qt::Horizontal, " ");
    model.setHeaderData(1, Qt::Horizontal, "End");
    model.setHeaderData(2, Qt::Horizontal, "Valor");

    // Adjust table settings
    ui->tableViewMemoryInstructions->resizeColumnsToContents();
    ui->tableViewMemoryInstructions->resizeRowsToContents();
    ui->tableViewMemoryInstructions->verticalHeader()->hide();
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

void HidraGui::initializeHighlighter()
{
    highlighter->initializeHighlighter(*machine);
    highlighter->rehighlight();
}



//////////////////////////////////////////////////
// Clearing internal methods
//////////////////////////////////////////////////

void HidraGui::clearMachineInterfaceComponents()
{
    clearMemoryMap();
    clearRegisterWidgets();
    clearFlagWidgets();
}

void HidraGui::clearMemoryMap()
{
    model.clear();
}

void HidraGui::clearRegisterWidgets()
{
    while(ui->layoutRegisters->count() > 0)
        delete ui->layoutRegisters->takeAt(0)->widget();

    registerWidgets.clear();
}

void HidraGui::clearFlagWidgets()
{
    while(ui->layoutFlags->count() > 0)
        delete ui->layoutFlags->takeAt(0)->widget();

    flagWidgets.clear();
}



//////////////////////////////////////////////////
// Updating internal methods
//////////////////////////////////////////////////

void HidraGui::updateMachineInterfaceComponents()
{
    updateMemoryMap();
    updateFlagWidgets();
    updateRegisterWidgets();
    updateCodeEditor();
    updateButtons();
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

void HidraGui::updateRegisterWidgets()
{
    for (int i=0; i<registerWidgets.count(); i++)
        registerWidgets.at(i)->setValue(machine->getRegisterValue(i));
}

void HidraGui::updateFlagWidgets()
{
    for (int i=0; i<flagWidgets.count(); i++)
        flagWidgets.at(i)->setValue(machine->getFlagValue(i));
}

void HidraGui::updateCodeEditor()
{
    if (sourceAndMemoryInSync)
        codeEditor->highlightPCLine(machine->getPCCorrespondingLine());
    else
        codeEditor->disableLineHighlight();
}

void HidraGui::updateButtons()
{
    if (machine->isRunning())
        ui->pushButtonRun->setText("Parar");
    else
        ui->pushButtonRun->setText("Rodar");
}



//////////////////////////////////////////////////
// Saving/loading
//////////////////////////////////////////////////

void HidraGui::save()
{
    QFile file(currentFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(this, "Erro ao salvar arquivo",
                                 file.errorString());
        fileSaved = false;
        return;
    }

    fileSaved = true;
    QTextStream out(&file);
    out << codeEditor->toPlainText();
    file.close();

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



//////////////////////////////////////////////////
// Errors field
//////////////////////////////////////////////////

void HidraGui::clearErrorsField()
{
    ui->textEditError->clear();
}

void HidraGui::addError(QString errorString)
{
    ui->textEditError->setPlainText(ui->textEditError->toPlainText() + errorString + "\n");
}



//////////////////////////////////////////////////
// Others
//////////////////////////////////////////////////

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

void HidraGui::sourceCodeChanged()
{
    modifiedFile = true;

    if (sourceAndMemoryInSync)
    {
        sourceAndMemoryInSync = false;
        codeEditor->disableLineHighlight();
    }
}



//////////////////////////////////////////////////
// Actions
//////////////////////////////////////////////////

void HidraGui::on_pushButtonBuild_clicked()
{
    ui->actionBuild->trigger();
}

void HidraGui::on_pushButtonRun_clicked(){
    ui->actionRun->trigger();
}

void HidraGui::on_pushButtonStep_clicked(){
    ui->actionStep->trigger();
}



void HidraGui::on_actionBuild_triggered()
{
    clearErrorsField();
    machine->assemble(codeEditor->toPlainText());

    if (machine->buildSuccessful)
        sourceAndMemoryInSync = true;

    machine->setPCValue(0);
    updateMachineInterface();
}

void HidraGui::on_actionRun_triggered()
{
    // If already running
    if (machine->isRunning())
    {
        // Stop
        machine->setRunning(false);
        updateMachineInterface();
    }
    else
    {
        // Start running
        machine->setRunning(true);

        // Keep running until stopped
        while (machine->isRunning()) {
            ui->actionStep->trigger();
            QApplication::processEvents();
        }
    }
}

void HidraGui::on_actionStep_triggered()
{
    machine->step();
    updateMachineInterface();
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
            QMessageBox::information(this, tr("Erro ao salvar arquivo"),
                                     file.errorString());
            return;
        }
        QTextStream in(&file);
        codeEditor->setPlainText(in.readAll());
        modifiedFile = false;
        file.close();
    }
}

void HidraGui::on_actionSave_triggered()
{
    if(currentFile == "") {
        saveAs();
    } else {
        save();
    }
}

void HidraGui::on_actionSaveAs_triggered()
{
    saveAs();
}



void HidraGui::on_actionImportMemory_triggered()
{

}

void HidraGui::on_actionExportMemory_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Exportar memória", "",
                                                    "Arquivo de memória (*.mem)");

    if (!filename.isEmpty())
    {
        // Save memory
        machine->save(filename);
    }
}



void HidraGui::on_actionClearRegisters_triggered()
{
    machine->clearRegisters();
    machine->clearFlags();
    updateMachineInterface();
}

void HidraGui::on_actionHexadecimalMode_toggled(bool checked)
{
    showHexValues = checked;

    for (int i=0; i<registerWidgets.count(); i++)
        registerWidgets.at(i)->setMode(showHexValues);

    updateMachineInterface();
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
    }

    sourceAndMemoryInSync = false;
    codeEditor->disableLineHighlight();
    initializeMachineInterface();
}



void HidraGui::on_actionReportProblem_triggered()
{

}

void HidraGui::on_actionManual_triggered()
{

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
            ui->actionSave->trigger();

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
