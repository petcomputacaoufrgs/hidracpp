#include "hidragui.h"
#include "ui_hidragui.h"
#include <QSizeGrip>

#define DEBUG_INT(value) qDebug(QString::number(value).toStdString().c_str());
#define DEBUG_STRING(value) qDebug(value.toStdString().c_str());

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

    connect(ui->statusBar, SIGNAL(messageChanged(QString)), this, SLOT(statusBarMessageChanged(QString)));

    buildSuccessful = true;

    // View options
    showHexValues = false;
    showSignedData = false;
    fastExecute = false;
    followPC = false;
    previousPCValue = 0;

    sourceAndMemoryInSync = false;
    machine = nullptr;

    ui->scrollAreaRegisters->setFrameShape(QFrame::NoFrame);

    // Select Neander machine and update interface
    selectMachine("Neander");

    modifiedFile = false;
    modifiedSinceBackup = false;
    forceSaveAs = true;
    updateWindowTitle();

    // Load dropped files
    codeEditor->setAcceptDrops(false);
    setAcceptDrops(true);

    // Open recovery file (if existing)
    /*if (QFile::exists("__Recovery__.txt"))
    {
        load("__Recovery__.txt");
        modifiedFile = true;
    }

    // Set backup timer
    backupTimer.setInterval(3*60000); // Save backup every N minutes
    connect(&backupTimer, SIGNAL(timeout()), this, SLOT(saveBackup()));
    backupTimer.start(); */ // TODO: Only open recovery once
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
        Machine *previousMachine = machine;

        if (machineName == "Neander")
            machine = new NeanderMachine();
        else if (machineName == "Ahmes")
            machine = new AhmesMachine();
        else if (machineName == "Ramses")
            machine = new RamsesMachine();
        else if (machineName == "Cromag")
            machine = new CromagMachine();
        else if (machineName == "Queops")
            machine = new QueopsMachine();
        else if (machineName == "Pitagoras")
            machine = new PitagorasMachine();
        else if (machineName == "Pericles")
            machine = new PericlesMachine();
        else if (machineName == "REG")
            machine = new RegMachine();
        else if (machineName == "Volta")
            machine = new VoltaMachine();
        else
            return; // Error

        delete previousMachine;
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

void HidraGui::updateMachineInterface(bool force = false)
{
    updateMachineInterfaceComponents(force);
}



//////////////////////////////////////////////////
// Internal initialize methods
//////////////////////////////////////////////////

void HidraGui::initializeMachineInterfaceComponents()
{
    initializeMemoryTable();
    initializeStackTable();
    initializeFlagWidgets();
    initializeRegisterWidgets();
    initializeHighlighter();
    initializeInstructionsList();
    initializeAddressingModesList();
}

void HidraGui::initializeMemoryTable()
{
    int memorySize = machine->getMemorySize();

    ui->tableViewMemoryInstructions->setModel(&memoryModel);
    ui->tableViewMemoryData->setModel(&memoryModel);

    // Set table size
    memoryModel.setRowCount(memorySize);
    memoryModel.setColumnCount(NumColumns);

    previousRowColor = QVector<QColor>(memorySize);
    previousLabel = QVector<QString>(memorySize);

    // Initialize items
    for (int row = 0; row < memorySize; row++)
    {
        for (int column = 0; column < memoryModel.columnCount(); column++)
            memoryModel.setData(memoryModel.index(row, column), "");

        previousRowColor[row] = Qt::white;
        previousLabel[row] = "";
    }

    // Set table headers
    memoryModel.setHeaderData(ColumnPC,               Qt::Horizontal, " ");
    memoryModel.setHeaderData(ColumnAddress,          Qt::Horizontal, " End ");
    memoryModel.setHeaderData(ColumnInstructionValue, Qt::Horizontal, "Valor");
    memoryModel.setHeaderData(ColumnDataValue,        Qt::Horizontal, "Dado");
    memoryModel.setHeaderData(ColumnLabel,            Qt::Horizontal, "  Label  ");

    // Adjust table settings
    ui->tableViewMemoryInstructions->verticalHeader()->hide();
    ui->tableViewMemoryInstructions->resizeRowsToContents();
    ui->tableViewMemoryInstructions->resizeColumnsToContents();
    ui->tableViewMemoryInstructions->setMouseTracking(true);
    ui->tableViewMemoryInstructions->setEditTriggers(false);
    ui->tableViewMemoryData->verticalHeader()->hide();
    ui->tableViewMemoryData->resizeRowsToContents();
    ui->tableViewMemoryData->resizeColumnsToContents();
    ui->tableViewMemoryData->setMouseTracking(true);
    ui->tableViewMemoryData->setEditTriggers(false);

    // Hide columns
    ui->tableViewMemoryInstructions->hideColumn(ColumnLabel);
    ui->tableViewMemoryInstructions->hideColumn(ColumnDataValue);
    ui->tableViewMemoryData->hideColumn(ColumnPC);
    ui->tableViewMemoryData->hideColumn(ColumnInstructionValue);

    // Scroll to appropriate position
    ui->tableViewMemoryInstructions->scrollTo(memoryModel.index(0, ColumnAddress), QAbstractItemView::PositionAtTop);
    ui->tableViewMemoryData->scrollTo(memoryModel.index((memorySize < 4096) ? 128 : 1024, ColumnAddress), QAbstractItemView::PositionAtTop);
}

void HidraGui::initializeStackTable()
{
    VoltaMachine *voltaMachine = dynamic_cast<VoltaMachine*>(machine);
    bool isVoltaMachine = (voltaMachine != nullptr);

    // If machine is Volta, show stack. Otherwise, show data memory.
    ui->tableViewStack->setVisible(isVoltaMachine);
    ui->tableViewMemoryData->setVisible(!isVoltaMachine);
    ui->tableViewMemoryInstructions->setColumnHidden(ColumnLabel, !isVoltaMachine); // Show labels in Volta's memory table

    if (!isVoltaMachine)
        return; // No stack to initialize

    int stackSize = voltaMachine->getStackSize();

    ui->tableViewStack->setModel(&stackModel);

    // Set table size
    stackModel.setRowCount(stackSize);
    stackModel.setColumnCount(NumColumnsStack);

    // Initialize items
    for (int row = 0; row < stackSize; row++)
    {
        for (int column = 0; column < NumColumnsStack; column++)
            stackModel.setData(stackModel.index(row, column), "");
    }

    // Set table headers
    stackModel.setHeaderData(ColumnStackSP, Qt::Horizontal, " SP ");
    stackModel.setHeaderData(1, Qt::Horizontal, " End ");
    stackModel.setHeaderData(2, Qt::Horizontal, "Valor");

    // Adjust table settings
    ui->tableViewStack->verticalHeader()->hide();
    ui->tableViewStack->resizeRowsToContents();
    ui->tableViewStack->resizeColumnsToContents();
    ui->tableViewStack->setMouseTracking(true);
    ui->tableViewStack->setEditTriggers(false);

    // Scroll to appropriate position
    ui->tableViewStack->scrollTo(stackModel.index(voltaMachine->getStackSize() - 1, 0), QAbstractItemView::PositionAtBottom);
}

void HidraGui::initializeFlagWidgets()
{
    ui->areaFlags->setVisible(machine->getNumberOfFlags() > 0); // Only show flags area if there are flags to be displayed

    for (int i=0; i < machine->getNumberOfFlags(); i++)
    {
        FlagWidget *newFlag = new FlagWidget(this, machine->getFlagName(i), machine->getFlagValue(i));
        flagWidgets.append(newFlag);
        ui->layoutFlags->addWidget(newFlag);
    }
}

void HidraGui::initializeRegisterWidgets()
{
    static int originalMaximumHeight = ui->areaRegisters->maximumHeight();
    bool separatePCFromRegisters = false;

    // If there are too many registers, don't restrict height and show PC separately
    if (machine->getNumberOfRegisters() > 4)
    {
        ui->areaRegisters->setMaximumHeight(0xFFFFFF);
        separatePCFromRegisters = true; // Show PC on its own area
        ui->areaPC->setVisible(true);
    }
    else
    {
        ui->areaRegisters->setMaximumHeight(originalMaximumHeight);
        ui->areaPC->setVisible(false);
    }

    for (int i=0; i < machine->getNumberOfRegisters(); i++)
    {
        // Create new register
        RegisterWidget *newRegister = new RegisterWidget(this, machine->getRegisterName(i));
        registerWidgets.append(newRegister);

        // Add to GUI
        if (separatePCFromRegisters && machine->getRegisterName(i) == "PC")
            ui->layoutPC->addWidget(newRegister); // Show PC on its own area
        else
            ui->layoutRegisters->addWidget(newRegister, i/2, i%2); // Two per line, alternates left and right columns with i%2
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
        instructionText->setTextFormat(Qt::RichText);
        instructionText->setEnabled(false); // Grayed out

        // Label text
        instructionText->setText(instruction->getMnemonic().toUpper());

        // Description in tooltip
        QString toolTip = "<b>" + instruction->getMnemonic().toUpper() + "</b> " +
                          instruction->getArguments().join(" ") + "<br>" +
                          machine->getDescription(instruction->getAssemblyFormat());
        instructionText->setToolTip(toolTip);

        ui->layoutInstructions->addWidget(instructionText, i/6, i%6);
        i += 1;
    }
}

void HidraGui::initializeAddressingModesList()
{
    int i = 0;

    ui->areaAddressingModes->setVisible(machine->getAddressingModes().size() > 1); // Only show if there's more than one addressing mode

    foreach (AddressingMode *addressingMode, machine->getAddressingModes())
    {
        QString acronym, name, format, description;
        machine->getAddressingModeDescription(addressingMode->getAddressingModeCode(), acronym, name, format, description);

        QLabel *addressingModeText = new QLabel(this);
        addressingModeText->setTextFormat(Qt::RichText);
        addressingModeText->setEnabled(false); // Grayed out

        // Label text
        addressingModeText->setText(acronym);

        // Description in tooltip
        QString toolTip = "<b>" + name   + "</b><br>" +
                          "<i>" + format + "</i><br>" +
                          description;
        addressingModeText->setToolTip(toolTip);

        ui->layoutAddressingModes->addWidget(addressingModeText, i/6, i%6);
        i += 1;
    }
}



//////////////////////////////////////////////////
// Internal clear methods
//////////////////////////////////////////////////

void HidraGui::clearMachineInterfaceComponents()
{
    clearMemoryTable();
    clearRegisterWidgets();
    clearFlagWidgets();
    clearInstructionsList();
    clearAddressingModesList();
    clearErrorsField();
}

void HidraGui::clearMemoryTable()
{
    memoryModel.clear();
}

void HidraGui::clearStackTable()
{
    stackModel.clear();
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

void HidraGui::clearAddressingModesList()
{
    while(ui->layoutAddressingModes->count() > 0)
        delete ui->layoutAddressingModes->takeAt(0)->widget();
}



//////////////////////////////////////////////////
// Internal update methods
//////////////////////////////////////////////////

void HidraGui::updateMachineInterfaceComponents(bool force)
{
    updateMemoryTable(force);
    updateStackTable();
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

    int currentLine = machine->getPCCorrespondingSourceLine();



    //////////////////////////////////////////////////
    // Column 0: PC Arrow
    //////////////////////////////////////////////////

    if (memoryModel.hasIndex(previousPCValue, ColumnPC))
        memoryModel.item(previousPCValue, ColumnPC)->setText(""); // Clear last PC Value's arrow
    memoryModel.item(machine->getPCValue(), ColumnPC)->setText("\u2192"); // Show arrow on current PC value
    previousPCValue = machine->getPCValue(); // Update last PC value



    for (int row=0; row<memorySize; row++)
    {
        int byteAddress = row;
        int value = machine->getMemoryValue(byteAddress);

        //////////////////////////////////////////////////
        // Column 1: Address
        //////////////////////////////////////////////////

        if (force)
        {
            memoryModel.item(row, ColumnAddress)->setEnabled(false);
            memoryModel.item(row, ColumnAddress)->setText(QString::number(byteAddress, base).toUpper());
        }

        //////////////////////////////////////////////////
        // Columns 2, 3: Byte value
        //////////////////////////////////////////////////

        if (machine->hasByteChanged(byteAddress) || force) // Only update cell if byte value has changed
        {
            QString instructionValueStr = QString::number(value, base).toUpper();
            QString dataValueStr        = QString::number((showSignedData) ? machine->toSigned(value) : value, base).toUpper(); // May be signed

            memoryModel.item(row, ColumnInstructionValue)->setText(instructionValueStr);
            memoryModel.item(row, ColumnDataValue       )->setText(dataValueStr);

            // On mouse-over, sends value to statusbar (with "#" prefix)
            // The information box then obtains the value and displays it in dec/hex/bin
            QString statusTip = "#" + QString::number(machine->getMemoryValue(byteAddress));
            memoryModel.item(row, ColumnInstructionValue)->setStatusTip(statusTip);
            memoryModel.item(row, ColumnDataValue       )->setStatusTip(statusTip);
        }

        //////////////////////////////////////////////////
        // Column 4: Label
        //////////////////////////////////////////////////

        if ((sourceAndMemoryInSync &&
             machine->getAddressCorrespondingLabel(byteAddress) != previousLabel[byteAddress]) || force) // Only update on change
        {
            QString labelName = machine->getAddressCorrespondingLabel(byteAddress);
            memoryModel.item(row, ColumnLabel)->setText(labelName);
            previousLabel[byteAddress] = labelName; // Update previousLabel
        }
    }



    //////////////////////////////////////////////////
    // Row color (highlight current instruction)
    //////////////////////////////////////////////////

    int intermediateAddress, intermediateAddress2, finalOperandAddress;
    machine->getNextOperandAddress(intermediateAddress, intermediateAddress2, finalOperandAddress);

    for (int row=0; row<memorySize; row++)
    {
        QColor rowColor;

        // Get new color
        if (sourceAndMemoryInSync && row == finalOperandAddress)
            rowColor = QColor(255, 202, 176); // Red
        else if (sourceAndMemoryInSync && (row == intermediateAddress || row == intermediateAddress2))
            rowColor = QColor(255, 228, 148); // Orange
        else if (sourceAndMemoryInSync && currentLine == machine->getAddressCorrespondingSourceLine(row) && currentLine >= 0)
            rowColor = QColor(255, 244, 128); // Yellow
        else
            rowColor = Qt::white;

        // Update row color if needed
        if (previousRowColor[row] != rowColor)
        {
            for (int column=0; column<NumColumns; column++)
                memoryModel.item(row, column)->setBackground(rowColor);
        }

        previousRowColor[row] = rowColor;
    }

    // Adjust size
    ui->tableViewMemoryData->resizeColumnsToContents();
}

void HidraGui::updateStackTable()
{
    VoltaMachine *voltaMachine = dynamic_cast<VoltaMachine*>(machine);

    if (voltaMachine == nullptr) // Not Volta machine
        return;

    int stackSize = voltaMachine->getStackSize();
    int spValue = voltaMachine->getSPValue();
    int base = showHexValues? 16 : 10;

    for (int row=0; row<stackSize; row++)
    {
        int stackAddress = stackSize-row-1;
        int value = voltaMachine->getStackValue(stackAddress);

        //////////////////////////////////////////////////
        // Column 0: SP Arrow
        //////////////////////////////////////////////////

        stackModel.item(row, ColumnStackSP)->setText((stackAddress == spValue) ? "\u2192" : ""); // Unicode arrow



        //////////////////////////////////////////////////
        // Column 1: Address
        //////////////////////////////////////////////////

        stackModel.item(row, ColumnStackAddress)->setEnabled(false);
        stackModel.item(row, ColumnStackAddress)->setText(QString::number(stackAddress, base).toUpper());



        //////////////////////////////////////////////////
        // Column 2: Byte value
        //////////////////////////////////////////////////

        stackModel.item(row, ColumnStackValue)->setEnabled(stackAddress <= spValue); // Items after SP (and thus inaccessible) are greyed out
        stackModel.item(row, ColumnStackValue)->setText(QString::number(value, base).toUpper());

        // On mouse-over, sends value to statusbar (with "#" prefix)
        // The information box then obtains the value and displays it in dec/hex/bin
        QString statusTip = "#" + QString::number(voltaMachine->getStackValue(stackAddress));
        stackModel.item(row, ColumnStackValue)->setStatusTip(statusTip);
    }

    // Adjust size
    ui->tableViewStack->resizeColumnsToContents();
}

void HidraGui::updateRegisterWidgets()
{
    for (int i=0; i<registerWidgets.count(); i++)
    {
        int value = machine->getRegisterValue(i, showSignedData);
        registerWidgets.at(i)->setValue(value);
        registerWidgets.at(i)->setStatusTip("#" + QString::number(value));
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
        codeEditor->highlightPCLine(machine->getPCCorrespondingSourceLine());
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

void HidraGui::updateInformation() // Show counters
{
    QString informationString = "Instruções: " + QString::number(machine->getInstructionCount()) + "  |  "
                              + "Acessos: "    + QString::number(machine->getAccessCount());
    ui->textInformation->setText(informationString);
}

void HidraGui::updateInformation(int value) // Show value in dec/hex/bin
{
    ui->textInformation->setText(getValueDescription(value));
}

void HidraGui::updateWindowTitle()
{
    QString filename = QFileInfo(currentFilename).fileName(); // Remove path
    QString title;

    title += (modifiedFile) ? "*" : "";
    title += (filename != "") ? filename : "Sem título";
    title += " - Hidra - PET Computação";

    setWindowTitle(title);
}

QString HidraGui::getValueDescription(int value)
{
    return QString("Dec: %1 | Hex: %2 | Bin: %3")
      .arg(value)
      .arg(value, 2, 16, QChar('0'))
      .arg(value, 8, 2, QChar('0'));
}



//////////////////////////////////////////////////
// File handling
//////////////////////////////////////////////////

void HidraGui::newFile()
{
    codeEditor->clear();
    machine->clear();
    initializeMachineInterface();

    currentFilename = "";
    modifiedFile = false;
    updateWindowTitle();
}

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
    updateWindowTitle();
}

void HidraGui::saveAs()
{
    QString extension = "Fonte do Neander (*.ndr)"; // Default

    if (currentMachineName == "Ahmes")
        extension = "Fonte do Ahmes (*.ahd)";
    else if (currentMachineName == "Ramses")
        extension = "Fonte do Ramses (*.rad)";
    else if (currentMachineName == "Cromag")
        extension = "Fonte do Cromag (*.cmg)";
    else if (currentMachineName == "Queops")
        extension = "Fonte do Queops (*.qps)";
    else if (currentMachineName == "Pitagoras")
        extension = "Fonte do Pitagoras (*.ptg)";
    else if (currentMachineName == "Pericles")
        extension = "Fonte do Pericles (*.prc)";
    else if (currentMachineName == "REG")
        extension = "Fonte do REG (*.rg)";
    else if (currentMachineName == "Volta")
        extension = "Fonte do Volta (*.vlt)";

    QString filename = QFileDialog::getSaveFileName(this,
                                                   "Salvar código-fonte", "",
                                                   extension);

    if (!filename.isEmpty())
        save(filename); // Resets fileModified to false if successful
}

void HidraGui::saveChangesDialog(bool &cancelled)
{
    cancelled = false;

    // If modified and not empty, offer to save changes
    if (modifiedFile && !codeEditor->document()->isEmpty())
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

            if (modifiedFile) // If modifiedFile is still true, user has cancelled
                cancelled = true;
        }
    }
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
    else if (extension == "cmg")
        selectMachine("Cromag");
    else if (extension == "qps")
        selectMachine("Queops");
    else if (extension == "ptg")
        selectMachine("Pitagoras");
    else if (extension == "prc")
        selectMachine("Pericles");
    else if (extension == "rg")
        selectMachine("REG");
    else if (extension == "vlt")
        selectMachine("Volta");

    currentFilename = filename;
    modifiedFile = false;
    forceSaveAs = false;
    updateWindowTitle();
}



//////////////////////////////////////////////////
// Others
//////////////////////////////////////////////////

void HidraGui::step(bool refresh = true)
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

    if (refresh)
    {
        updateMachineInterface();
        if (followPC)
        {
            codeEditor->setCurrentLine(machine->getPCCorrespondingSourceLine());
            ui->tableViewMemoryInstructions->scrollTo(memoryModel.index(machine->getPCValue(), ColumnAddress));
        }
        QApplication::processEvents();
    }
}


void HidraGui::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

// Accept dropped files
void HidraGui::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->urls().size() == 1)
    {
        QString filename = e->mimeData()->urls().at(0).toLocalFile();

        if (QFile::exists(filename))
        {
            bool cancelled;
            saveChangesDialog(cancelled);

            if (!cancelled)
                load(filename);
        }
    }
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

void HidraGui::sourceCodeChanged()
{
    if (!modifiedFile)
    {
        modifiedFile = true;
        updateWindowTitle();
    }

    modifiedSinceBackup = true;

    if (sourceAndMemoryInSync)
    {
        sourceAndMemoryInSync = false;
        codeEditor->disableLineHighlight();
    }
}

void HidraGui::statusBarMessageChanged(QString newMessage)
{
    if (newMessage == " ") // Ignore self-triggered change
        return;

    if (newMessage.startsWith("#")) // Steal prefixed value from statusbar
    {
        updateInformation(newMessage.remove("#").toInt()); // Display dec/hex/bin
        statusBar()->showMessage(" ");
    }
    else
    {
        updateInformation(); // Restore information to counters
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
// File menu
//////////////////////////////////////////////////

void HidraGui::on_actionNew_triggered()
{
    bool cancelled = false;
    saveChangesDialog(cancelled);

    // Se não foi cancelado, cria um novo arquivo
    if (!cancelled)
        newFile();
}

void HidraGui::on_actionOpen_triggered()
{
    QString allExtensions = "Fontes do Hidra (*.ndr *.ahd *.rad)";
    QString filename;

    filename = QFileDialog::getOpenFileName(this, "Abrir código-fonte", "", allExtensions);

    if (!filename.isEmpty())
    {
        bool cancelled;
        saveChangesDialog(cancelled);

        if (!cancelled)
            load(filename);
    }
}

void HidraGui::on_actionSave_triggered()
{
    if(currentFilename == "" || forceSaveAs)
        saveAs();
    else
        save(currentFilename);
}

void HidraGui::on_actionSaveAs_triggered()
{
    saveAs();
}

void HidraGui::on_actionClose_triggered()
{
    this->close();
}

void HidraGui::closeEvent(QCloseEvent *event)
{
    bool cancelled;
    saveChangesDialog(cancelled);

    // Delete backup file
    if (!cancelled)
        QFile::remove("__Recovery__.txt");

    // Accept/reject window close event
    if (!cancelled)
        event->accept();
    else
        event->ignore();
}



//////////////////////////////////////////////////
// Machine menu
//////////////////////////////////////////////////

void HidraGui::on_actionBuild_triggered()
{
    clearErrorsField();
    machine->assemble(codeEditor->toPlainText());

    if (machine->getBuildSuccessful())
        sourceAndMemoryInSync = true;
    else
        codeEditor->setCurrentLine(machine->getFirstErrorLine());

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
        int breakpointAddress = machine->getSourceLineCorrespondingAddress(breakpointLine);
        machine->setBreakpoint(breakpointAddress);

        // Start running
        machine->setRunning(true);

        // Keep running until stopped
        while (machine->isRunning())
        {
            int instructionsPerRefresh = (fastExecute) ? 1000 : 1; // Inside while loop, allows realtime change
            int nextInstructionCount = machine->getInstructionCount() + 1;
            bool refresh = (nextInstructionCount % instructionsPerRefresh == 0); // Refresh whenever it's a multiple of instructionsPerRefresh
            step(refresh);
        }
    }
}

void HidraGui::on_actionStep_triggered()
{
    step();
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

void HidraGui::on_actionResetRegisters_triggered()
{
    machine->setRunning(false);
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
    int breakpointAddress = machine->getSourceLineCorrespondingAddress(breakpointLine);
    machine->setBreakpoint(breakpointAddress);
}



//////////////////////////////////////////////////
// View menu
//////////////////////////////////////////////////

void HidraGui::on_actionHexadecimalMode_toggled(bool checked)
{
    showHexValues = checked;

    if (checked)
        ui->actionSignedMode->setChecked(false); // Mutually exclusive modes

    // Toggle register mode
    for (int i=0; i<registerWidgets.count(); i++)
        registerWidgets.at(i)->setMode(showHexValues);

    updateMachineInterface(true);
}

void HidraGui::on_actionSignedMode_toggled(bool checked)
{
    showSignedData = checked;

    if (checked)
        ui->actionHexadecimalMode->setChecked(false); // Mutually exclusive modes

    // Toggle register mode
    for (int i=0; i<registerWidgets.count(); i++)
        registerWidgets.at(i)->setMode(showHexValues);

    updateMachineInterface(true);
}

void HidraGui::on_actionFastExecuteMode_toggled(bool checked)
{
    fastExecute = checked;
}

void HidraGui::on_actionFollowPCMode_toggled(bool checked)
{
    followPC = checked;
}



//////////////////////////////////////////////////
// Help menu
//////////////////////////////////////////////////

void HidraGui::on_actionQuickGuide_triggered()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("Hidra_GuiaRapido.pdf"));
}

void HidraGui::on_actionReference_triggered()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("Hidra_Referencia.pdf"));
}

void HidraGui::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Sobre o Hidra",
                       "<p align='center'>Hidra v0.9 (" + QString(__DATE__) + ")<br><br>"
                       "Desenvolvido pelo grupo PET Computação.<br><br>"
                       "Máquinas teóricas criadas pelos professores<br>Raul Fernando Weber e Taisy Silva Weber.</p>");
}



//////////////////////////////////////////////////
// Interface elements
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

void HidraGui::on_tableViewMemoryInstructions_doubleClicked(const QModelIndex &index)
{
    machine->setPCValue(index.row());
    updateMachineInterface();
}

void HidraGui::on_comboBoxMachine_currentIndexChanged(const QString machineName)
{
    selectMachine(machineName);
}

void HidraGui::on_tableViewMemoryData_doubleClicked(const QModelIndex &index)
{
    int addressCorrespondingSourceLine = machine->getAddressCorrespondingSourceLine(index.row());

    if (addressCorrespondingSourceLine != -1)
        codeEditor->setCurrentLine(addressCorrespondingSourceLine);
}
