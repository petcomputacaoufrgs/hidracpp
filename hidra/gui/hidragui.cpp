#include "hidragui.h"
#include "ui_hidragui.h"
#include <QSizeGrip>

HidraGui::HidraGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HidraGui)
{
    ui->setupUi(this);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    codeEditor  = new HidraCodeEditor();
    highlighter = new HidraHighlighter(codeEditor->document());
    ui->layoutSourceCodeHolder->addWidget(codeEditor);
    connect(codeEditor, SIGNAL(textChanged()), this, SLOT(sourceCodeChanged()));

    currentFilename = "";

    buildSuccessful = true;
    showHexValues = false;
    previousPCValue = 0;

    sourceAndMemoryInSync = false;
    machine = NULL;

    ui->layoutRegisters->setAlignment(Qt::AlignTop);
    ui->scrollAreaRegisters->setFrameShape(QFrame::NoFrame);
    ui->scrollAreaRegisters->setWidgetResizable(true);

    ui->tableViewMemoryInstructions->setEditTriggers(false);
    ui->tableViewMemoryData->setEditTriggers(false);
    ui->tableViewMemoryData->setVisible(false);

    // Select Neander machine and update interface
    selectMachine("Neander");

    // Show data table
    ui->actionDisplayDataTable->trigger();

    modifiedFile = false;
    modifiedSinceBackup = false;
    forceSaveAs = true;

    // Open recovery file (if existing)
    /*if (QFile::exists("__Recovery__.txt"))
    {
        load("__Recovery__.txt");
        modifiedFile = true;
    }*/ // TODO: Automatic loading only if single instance

    // Set backup timer
    backupTimer.setInterval(3*60000); // Save backup every N minutes
    connect(&backupTimer, SIGNAL(timeout()), this, SLOT(saveBackup()));
    backupTimer.start();
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
    updateMachineInterfaceComponents(true);
}

void HidraGui::updateMachineInterface(bool force)
{
    updateMachineInterfaceComponents(force);
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
    int memorySize = machine->getMemorySize();

    ui->tableViewMemoryInstructions->setModel(&instructionsTableModel);
    ui->tableViewMemoryData->setModel(&dataTableModel);

    // Set table size
    instructionsTableModel.setRowCount(memorySize);
    instructionsTableModel.setColumnCount(3);
    dataTableModel.setRowCount(memorySize);
    dataTableModel.setColumnCount(3);

    previousRowColor = QVector<QColor>(memorySize);
    previousLabel = QVector<QString>(memorySize);

    // Initialize items
    for (int row = 0; row < memorySize; row++)
    {
        for (int column = 0; column < 3; column++)
        {
            instructionsTableModel.setData(instructionsTableModel.index(row, column), "");
            dataTableModel.setData(dataTableModel.index(row, column), "");
        }

        previousRowColor[row] = Qt::white;
        previousLabel[row] = "";
    }

    // Set table headers
    instructionsTableModel.setHeaderData(0, Qt::Horizontal, " ");
    instructionsTableModel.setHeaderData(1, Qt::Horizontal, " End ");
    instructionsTableModel.setHeaderData(2, Qt::Horizontal, "Valor");

    dataTableModel.setHeaderData(0, Qt::Horizontal, " End ");
    dataTableModel.setHeaderData(1, Qt::Horizontal, "Valor");
    dataTableModel.setHeaderData(2, Qt::Horizontal, "  Label  ");

    // Adjust table settings
    ui->tableViewMemoryInstructions->verticalHeader()->hide();
    ui->tableViewMemoryInstructions->resizeRowsToContents();
    ui->tableViewMemoryInstructions->resizeColumnsToContents();
    ui->tableViewMemoryInstructions->setMouseTracking(true);
    ui->tableViewMemoryData->verticalHeader()->hide();
    ui->tableViewMemoryData->resizeRowsToContents();
    ui->tableViewMemoryData->setMouseTracking(true);

    // Scroll to appropriate position
    ui->tableViewMemoryInstructions->scrollTo(instructionsTableModel.index(0, 0), QAbstractItemView::PositionAtTop);
    ui->tableViewMemoryData->scrollTo(dataTableModel.index(128, 0), QAbstractItemView::PositionAtTop);
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

void HidraGui::updateMachineInterfaceComponents(bool force)
{
    updateMemoryTable(force);
    updateFlagWidgets();
    updateRegisterWidgets();
    updateCodeEditor();
    updateButtons();
    updateInformation();
}

void HidraGui::updateMemoryTable(bool force)
{
    int memorySize = machine->getMemorySize();
    int base = showHexValues? 16 : 10;

    int currentLine = machine->getPCCorrespondingLine();



    //////////////////////////////////////////////////
    // Column 0: PC Arrow (instr. table)
    //////////////////////////////////////////////////

    if (instructionsTableModel.hasIndex(previousPCValue, 0))
        instructionsTableModel.item(previousPCValue, 0)->setText(""); // Clear last PC Value's arrow
    instructionsTableModel.item(machine->getPCValue(), 0)->setText("\u2192"); // Show arrow on current PC value
    previousPCValue = machine->getPCValue(); // Update last PC value



    for (int row=0; row<memorySize; row++)
    {
        int byteAddress = row;
        int value = machine->getMemoryValue(byteAddress);

        //////////////////////////////////////////////////
        // Column 1/0: Address
        //////////////////////////////////////////////////

        if (force)
        {
            instructionsTableModel.item(row, 1)->setEnabled(false);
            dataTableModel.item(row, 0)->setEnabled(false);
            instructionsTableModel.item(row, 1)->setText(QString::number(byteAddress, base).toUpper());
            dataTableModel.item(row, 0)->setText(QString::number(byteAddress, base).toUpper());
        }

        //////////////////////////////////////////////////
        // Column 2/1: Byte value
        //////////////////////////////////////////////////

        if (machine->hasByteChanged(byteAddress) || force) // Only update cell if byte value has changed
        {
            instructionsTableModel.item(row, 2)->setText(QString::number(value, base).toUpper());
            dataTableModel.item(row, 1)->setText(QString::number(value, base).toUpper());

            // Status bar tip on mouse hover
            QString statusTip = getValueDescription(value);
            instructionsTableModel.item(row, 2)->setStatusTip(statusTip);
            dataTableModel.item(row, 1)->setStatusTip(statusTip);
        }

        //////////////////////////////////////////////////
        // Column 2 (data table): Label
        //////////////////////////////////////////////////

        if ((sourceAndMemoryInSync &&
             machine->getAddressCorrespondingLabel(byteAddress) != previousLabel[byteAddress]) || force) // Only update on change
        {
            QString labelName = machine->getAddressCorrespondingLabel(byteAddress);
            dataTableModel.item(row, 2)->setText(labelName);
            previousLabel[byteAddress] = labelName; // Update previousLabel
        }
    }



    //////////////////////////////////////////////////
    // Row color (highlight current instruction)
    //////////////////////////////////////////////////

    for (int row=0; row<memorySize; row++)
    {
        QColor rowColor;

        // Get new color
        if (sourceAndMemoryInSync &&  currentLine == machine->getAddressCorrespondingLine(row) && currentLine >= 0)
            rowColor = QColor(255, 240, 0); // Yellow
        else
            rowColor = Qt::white;

        // Update color if needed
        if (previousRowColor[row] != rowColor)
        {
            instructionsTableModel.item(row, 0)->setBackground(rowColor);
            instructionsTableModel.item(row, 1)->setBackground(rowColor);
            instructionsTableModel.item(row, 2)->setBackground(rowColor);
        }

        previousRowColor[row] = rowColor;
    }

    // Update all cells
    emit instructionsTableModel.dataChanged(instructionsTableModel.index(0, 0), instructionsTableModel.index(memorySize, 0));
    emit dataTableModel.dataChanged(dataTableModel.index(0, 0), dataTableModel.index(memorySize, 0));

    ui->tableViewMemoryData->resizeColumnsToContents();
}

void HidraGui::updateRegisterWidgets()
{
    for (int i=0; i<registerWidgets.count(); i++)
    {
        int value = machine->getRegisterValue(i);
        registerWidgets.at(i)->setValue(value);
        registerWidgets.at(i)->setStatusTip(getValueDescription(value));
    }
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

void HidraGui::updateInformation()
{
    QString informationString = "Instruções: " + QString::number(machine->getInstructionCount()) + " | "
                              + "Acessos: "    + QString::number(machine->getAccessCount());
    ui->textInformation->setText(informationString);
}

QString HidraGui::getValueDescription(int value)
{
    return QString("Dec: %1 | Hex: %2 | Bin: %3")
      .arg(value)
      .arg(value, 2, 16, QChar('0'))
      .arg(value, 8, 2, QChar('0'));
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

void HidraGui::load(QString filename)
{
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(this, tr("Erro ao abrir arquivo"),
                                 file.errorString());
        return;
    }

    QTextStream in(&file);
    codeEditor->setPlainText(in.readAll());

    QString extension = filename.section(".", -1);

    if (extension == "ndr")
        selectMachine("Neander");
    else if (extension == "ahd")
        selectMachine("Ahmes");
    else if (extension == "rad")
        selectMachine("Ramses");

    currentFilename = filename;

    modifiedFile = false;
    forceSaveAs = false;
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
    modifiedSinceBackup = true;

    if (sourceAndMemoryInSync)
    {
        sourceAndMemoryInSync = false;
        codeEditor->disableLineHighlight();
    }
}

void HidraGui::saveBackup()
{
    if (modifiedFile && modifiedSinceBackup)
    {
        QFile file("__Recovery__.txt");

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return; // Ignore errors

        QTextStream out(&file);
        out << tr(";; ARQUIVO RECUPERADO. Selecione a máquina novamente.\n")
            << tr(";; --------------------------------------------------\n")
            << codeEditor->toPlainText();

        modifiedSinceBackup = false;
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

    if (machine->getBuildSuccessful())
        sourceAndMemoryInSync = true;
    else
        codeEditor->setCurrentLine(machine->getFirstErrorLine());

    machine->setPCValue(0);

    updateMachineInterface(false);
}

void HidraGui::on_actionRun_triggered()
{
    // If already running
    if (machine->isRunning())
    {
        // Stop
        machine->setRunning(false);
        updateMachineInterface(false);
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
        machine->setRunning(false);
        QMessageBox::information(this, tr("Error"), error);
    }

    updateMachineInterface(false);
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
    QString filename;

    filename = QFileDialog::getOpenFileName(this,
                                               "Abrir código-fonte", "",
                                               allExtensions);

    if (!filename.isEmpty())
        load(filename);
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

    updateMachineInterface(false);
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

void HidraGui::on_tableViewMemoryInstructions_doubleClicked(const QModelIndex &index)
{
    machine->setPCValue(index.row());
    updateMachineInterface(false);
}

void HidraGui::on_tableViewMemoryData_doubleClicked(const QModelIndex &index)
{
    int addressCorrespondingLine = machine->getAddressCorrespondingLine(index.row());

    if (addressCorrespondingLine != -1)
        codeEditor->setCurrentLine(addressCorrespondingLine);
}

void HidraGui::on_actionResetRegisters_triggered()
{
    machine->clearRegisters();
    machine->clearFlags();
    machine->clearCounters();
    updateMachineInterface(false);
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

    updateMachineInterface(true);
}

void HidraGui::on_actionDisplayDataTable_toggled(bool checked)
{
    ui->tableViewMemoryData->setVisible(checked);
}

void HidraGui::on_comboBoxMachine_currentIndexChanged(const QString machineName)
{
    selectMachine(machineName);
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

    // Exclui arquivo de backup
    if (!cancelled)
    {
        QFile::remove("__Recovery__.txt");
    }

    // Aceita ou rejeita o evento que fecha a janela
    if (!cancelled)
        event->accept();
    else
        event->ignore();
}

void HidraGui::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Sobre o Hidra",
                       "<p align='center'>Hidra (Versão " + QString(__DATE__) + ")<br><br>"
                       "Desenvolvido pelo grupo Pet Computação.<br><br>"
                       "Máquinas teóricas criadas pelos professores<br>Dr. Raul Fernando Weber e Dra. Taisy Silva Weber.</p>");
}

void HidraGui::on_actionQuickGuide_triggered()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("Hidra_GuiaRapido.pdf"));
}

void HidraGui::on_actionReference_triggered()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("Hidra_Referencia.pdf"));
}
