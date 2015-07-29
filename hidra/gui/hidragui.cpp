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

    currentFilename = "";

    buildSuccessful = true;
    showHexValues = false;

    sourceAndMemoryInSync = false;
    machine = NULL;

    ui->layoutRegisters->setAlignment(Qt::AlignTop);
    ui->scrollAreaRegisters->setFrameShape(QFrame::NoFrame);

    ui->tableViewMemoryInstructions->setEditTriggers(false);
    ui->tableViewMemoryData->setEditTriggers(false);
    ui->tableViewMemoryData->setVisible(false);

    //instructionAccessCounter.setStyleSheet("QLabel { color : gray; }");
    ui->statusBar->addPermanentWidget(&instructionAccessCounter, 0.25);

    // Escolhe a máquina Neander e atualiza a interface
    selectMachine("Neander");

    modifiedFile = false;
    forceSaveAs = true;
}

HidraGui::~HidraGui()
{
    delete ui;
}



//////////////////////////////////////////////////
// Initialize/update methods
//////////////////////////////////////////////////

void HidraGui::selectMachine(QString machineName)
{
    if (currentMachineName != machineName)
    {
        delete machine;

        if (machineName == "Ahmes")
            machine = new AhmesMachine();
        else if (machineName == "Ramses")
            machine = new RamsesMachine();
        else
            machine = new NeanderMachine(); // Default to Neander

        connect(machine, SIGNAL(buildErrorDetected(QString)), this, SLOT(addError(QString)));

        ui->comboBoxMachine->setCurrentText(machineName);

        sourceAndMemoryInSync = false;
        forceSaveAs = true;

        codeEditor->disableLineHighlight();
        initializeMachineInterface();

        currentMachineName = machineName;
    }
}

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
    initializeInstructionsList();
}

void HidraGui::initializeMemoryTable()
{
    ui->tableViewMemoryInstructions->setModel(&instructionsTableModel);
    ui->tableViewMemoryData->setModel(&dataTableModel);

    // Set table size
    instructionsTableModel.setRowCount(machine->getMemorySize());
    instructionsTableModel.setColumnCount(3);
    dataTableModel.setRowCount(machine->getMemorySize());
    dataTableModel.setColumnCount(2);

    // Initialize items
    for (int row = 0; row < machine->getMemorySize(); row++)
    {
        instructionsTableModel.setData(instructionsTableModel.index(row, 0), "");
        instructionsTableModel.setData(instructionsTableModel.index(row, 1), "");
        instructionsTableModel.setData(instructionsTableModel.index(row, 2), "");
        dataTableModel.setData(dataTableModel.index(row, 0), "");
        dataTableModel.setData(dataTableModel.index(row, 1), "");
    }

    // Set table headers
    instructionsTableModel.setHeaderData(0, Qt::Horizontal, " ");
    instructionsTableModel.setHeaderData(1, Qt::Horizontal, "End  ");
    instructionsTableModel.setHeaderData(2, Qt::Horizontal, "Valor");
    dataTableModel.setHeaderData(0, Qt::Horizontal, "End  ");
    dataTableModel.setHeaderData(1, Qt::Horizontal, "Valor");

    // Adjust table settings
    ui->tableViewMemoryInstructions->resizeColumnsToContents();
    ui->tableViewMemoryInstructions->resizeRowsToContents();
    ui->tableViewMemoryInstructions->verticalHeader()->hide();
    ui->tableViewMemoryData->resizeColumnsToContents();
    ui->tableViewMemoryData->resizeRowsToContents();
    ui->tableViewMemoryData->verticalHeader()->hide();

    // Scroll to appropriate position
    ui->tableViewMemoryInstructions->scrollTo(instructionsTableModel.index(0, 0));
    ui->tableViewMemoryData->scrollTo(dataTableModel.index(128, 0));
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

void HidraGui::initializeInstructionsList()
{
    int i = 0;

    foreach (Instruction *instruction, machine->getInstructions())
    {
        QLabel *instructionText = new QLabel(this);
        instructionText->setText(instruction->getMnemonic().toUpper());
        instructionText->setTextFormat(Qt::RichText);

        // Description in tooltip
        QString toolTip;
        toolTip += "<b>" + instruction->getMnemonic().toUpper() + "</b> ";
        toolTip += instruction->getArguments().join(" ") + "<br>";
        toolTip += machine->getDescription(instruction->getAssemblyFormat());
        instructionText->setToolTip(toolTip);

        instructionText->setEnabled(false); // Grayed out
        ui->layoutInstructions->addWidget(instructionText, i/6, i%6);
        i += 1;
    }
}



//////////////////////////////////////////////////
// Clearing internal methods
//////////////////////////////////////////////////

void HidraGui::clearMachineInterfaceComponents()
{
    clearMemoryTable();
    clearRegisterWidgets();
    clearFlagWidgets();
    clearInstructionsList();
    clearErrorsField();
}

void HidraGui::clearMemoryTable()
{
    instructionsTableModel.clear();
    dataTableModel.clear();
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

void HidraGui::clearInstructionsList()
{
    while(ui->layoutInstructions->count() > 0)
        delete ui->layoutInstructions->takeAt(0)->widget();
}



//////////////////////////////////////////////////
// Updating internal methods
//////////////////////////////////////////////////

void HidraGui::updateMachineInterfaceComponents()
{
    updateMemoryTable();
    updateFlagWidgets();
    updateRegisterWidgets();
    updateCodeEditor();
    updateButtons();
    updateStatusBar();
}

void HidraGui::updateMemoryTable()
{
    int memorySize = machine->getMemorySize();
    int pcValue = machine->getPCValue();
    int base = showHexValues? 16 : 10;

    QModelIndex index;
    int currentLine = machine->getPCCorrespondingLine();

    for (int byteAddress=0; byteAddress<memorySize; byteAddress++)
    {
        QColor rowColor;
        int value = machine->getMemoryValue(byteAddress);

        // Column 0: PC Arrow (instructions only)
        instructionsTableModel.item(byteAddress, 0)->setText(byteAddress == pcValue ? QString::fromUtf8("\u2192") : ""); // Unicode arrow / blank

        // Column 1: Byte address
        instructionsTableModel.item(byteAddress, 1)->setText(QString::number(byteAddress, base).toUpper());
        dataTableModel.item(        byteAddress, 0)->setText(QString::number(byteAddress, base).toUpper());

        // Column 2: Byte value
        instructionsTableModel.item(byteAddress, 2)->setText(QString::number(value, base).toUpper());
        dataTableModel.item(        byteAddress, 1)->setText(QString::number(value, base).toUpper());

        // Highlight current instruction's row
        if (currentLine == machine->getAddressCorrespondingLine(byteAddress) && currentLine >= 0)
            rowColor = QColor(255, 240, 0); // Yellow
        else
            rowColor =  Qt::white;

        // Set yellow background (instructions only)
        for (int column=0; column<3; column++)
        {
            instructionsTableModel.item(byteAddress, column)->setBackground(rowColor);
        }

        // Set binary tooltip
        instructionsTableModel.item(byteAddress, 2)->setToolTip(QString("%1").arg(value, 8, 2, QChar('0')));
        dataTableModel.item(byteAddress, 1)->setToolTip(QString("%1").arg(value, 8, 2, QChar('0')));
    }

    // Update all cells
    emit instructionsTableModel.dataChanged(instructionsTableModel.index(0, 0), instructionsTableModel.index(memorySize, 0));
    emit dataTableModel.dataChanged(dataTableModel.index(0, 0), dataTableModel.index(memorySize, 0));
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

void HidraGui::updateStatusBar()
{
    QString instructionAccessMessage;
    instructionAccessMessage  = "Instruções: " + QString::number(machine->getInstructionCount()) + " | ";
    instructionAccessMessage += "Acessos: "    + QString::number(machine->getAccessCount());
    instructionAccessCounter.setText(instructionAccessMessage);
}



//////////////////////////////////////////////////
// Saving/loading
//////////////////////////////////////////////////

void HidraGui::save(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(this, "Erro ao salvar arquivo",
                                 file.errorString());
        return;
    }

    QTextStream out(&file);
    out << codeEditor->toPlainText();
    file.close();

    currentFilename = filename;
    modifiedFile = false;
    forceSaveAs = false;
}

void HidraGui::saveAs()
{
    QString extension = "Fonte do Neander (*.ndr)"; // Default

    if (currentMachineName == "Ahmes")
        extension = "Fonte do Ahmes (*.ahd)";
    else if (currentMachineName == "Ramses")
        extension = "Fonte do Ramses (*.rad)";

    QString filename = QFileDialog::getSaveFileName(this,
                                                   "Salvar código-fonte", "",
                                                   extension);

    if (!filename.isEmpty())
        save(filename); // Resets fileModified to false if successful
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

void HidraGui::on_pushButtonRun_clicked()
{
    ui->actionRun->trigger();
}

void HidraGui::on_pushButtonStep_clicked()
{
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
        // Set breakpoint
        int breakpointLine = codeEditor->getBreakpointLine();
        int breakpointAddress = machine->getLineCorrespondingAddress(breakpointLine);
        machine->setBreakpoint(breakpointAddress);

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
    try
    {
        machine->step();
    }
    catch (QString error)
    {
        QMessageBox::information(this, tr("Error"), error);
    }

    updateMachineInterface();
}

void HidraGui::on_actionNew_triggered()
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

    // Se não foi cancelado, cria um novo arquivo
    if (!cancelled)
    {
        codeEditor->clear();
        machine->clear();
        initializeMachineInterface();
    }
}

void HidraGui::on_actionOpen_triggered()
{
    QString allExtensions = "Fontes do Hidra (*.ndr *.ahd *.rad)";

    currentFilename = QFileDialog::getOpenFileName(this,
                                               "Abrir código-fonte", "",
                                               allExtensions);

    if (!currentFilename.isEmpty())
    {
        QFile file(currentFilename);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::information(this, tr("Erro ao abrir arquivo"),
                                     file.errorString());
            return;
        }

        QTextStream in(&file);
        codeEditor->setPlainText(in.readAll());
        file.close();

        QString extension = currentFilename.section(".", -1);

        if (extension == "ndr")
            selectMachine("Neander");
        else if (extension == "ahd")
            selectMachine("Ahmes");
        else if (extension == "rad")
            selectMachine("Ramses");

        modifiedFile = false;
    }
}

void HidraGui::on_actionSave_triggered()
{
    if(currentFilename == "" || forceSaveAs) {
        saveAs();
    } else {
        save(currentFilename);
    }
}

void HidraGui::on_actionSaveAs_triggered()
{
    saveAs();
}



void HidraGui::on_actionImportMemory_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Importar memória", "",
                                                    "Arquivo de memória (*.mem)");

    if (!filename.isEmpty())
    {
        QString errorMessage;

        switch (machine->importMemory(filename))
        {
            case FileErrorCode::noError:
                break;

            case FileErrorCode::inputOutput:
                errorMessage = "Erro na leitura do arquivo.";
                break;

            case FileErrorCode::incorrectSize:
                errorMessage = "Arquivo de tamanho incorreto.";
                break;

            case FileErrorCode::invalidIdentifier:
                errorMessage = "Arquivo incompatível com a máquina selecionada.";
                break;

            default:
                errorMessage = "Erro não especificado.";
                break;
        }

        if (!errorMessage.isEmpty())
            QMessageBox::information(this, "Erro ao importar memória.", errorMessage);
    }

    updateMachineInterface();
}

void HidraGui::on_actionExportMemory_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Exportar memória", "",
                                                    "Arquivo de memória (*.mem)");

    if (!filename.isEmpty())
    {
        if (machine->exportMemory(filename) != FileErrorCode::noError)
            QMessageBox::information(this, "Erro ao exportar memória.", "Erro ao exportar memória.");
    }
}

void HidraGui::on_tableViewMemoryInstructions_clicked(const QModelIndex &index)
{
    int value = machine->getMemoryValue(index.row());
    ui->statusBar->showMessage(QString("Dec: %1 | Hex: %2 | Bin: %3")
                                       .arg(value)
                                       .arg(value, 2, 16, QChar('0'))
                                       .arg(value, 8, 2, QChar('0')));
}

void HidraGui::on_tableViewMemoryInstructions_doubleClicked(const QModelIndex &index)
{
    machine->setPCValue(index.row());
    updateMachineInterface();
}

void HidraGui::on_tableViewMemoryData_clicked(const QModelIndex &index)
{
    int value = machine->getMemoryValue(index.row());
    ui->statusBar->showMessage(QString("Dec: %1 | Hex: %2 | Bin: %3")
                                       .arg(value)
                                       .arg(value, 2, 16, QChar('0'))
                                       .arg(value, 8, 2, QChar('0')));
}

void HidraGui::on_actionResetRegisters_triggered()
{
    machine->clearRegisters();
    machine->clearFlags();
    machine->clearCounters();
    updateMachineInterface();
}

void HidraGui::on_actionSetBreakpoint_triggered()
{
    codeEditor->toggleBreakpointOnCursor();

    // Set breakpoint
    int breakpointLine = codeEditor->getBreakpointLine();
    int breakpointAddress = machine->getLineCorrespondingAddress(breakpointLine);
    machine->setBreakpoint(breakpointAddress);
}

void HidraGui::on_actionHexadecimalMode_toggled(bool checked)
{
    showHexValues = checked;

    for (int i=0; i<registerWidgets.count(); i++)
        registerWidgets.at(i)->setMode(showHexValues);

    updateMachineInterface();
}


void HidraGui::on_actionDisplayDataTable_toggled(bool checked)
{
    ui->tableViewMemoryData->setVisible(checked);
}

void HidraGui::on_comboBoxMachine_currentIndexChanged(const QString machineName)
{
    selectMachine(machineName);
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

