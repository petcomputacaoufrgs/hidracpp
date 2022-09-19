#ifndef HIDRAGUI_H
#define HIDRAGUI_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QCheckBox>
#include <QLabel>
#include <QDebug>
#include <QVector>
#include <QColor>
#include <QSettings>

#include "hidracodeeditor.h"
#include "hidrahighlighter.h"
#include "baseconversordialog.h"
#include "pointconversordialog.h"
#include "registerwidget.h"
#include "findreplacedialog.h"
#include "flagwidget.h"
#include "about.h"
#include "machines/neandermachine.h"
#include "machines/ahmesmachine.h"
#include "machines/ramsesmachine.h"
#include "machines/cromagmachine.h"
#include "machines/queopsmachine.h"
#include "machines/pitagorasmachine.h"
#include "machines/periclesmachine.h"
#include "machines/regmachine.h"
#include "machines/voltamachine.h"

namespace Ui {
class HidraGui;
}

class HidraGui : public QMainWindow
{
    Q_OBJECT

    enum MemoryTableColumn
    {
        ColumnPC,
        ColumnAddress,
        ColumnInstructionValue,
        ColumnDataValue,
        ColumnCharacter,
        ColumnLabel,
        ColumnInstructionString,
        NumColumns // Last column marker
    };

    enum StackTableColumn
    {
        ColumnStackSP,
        ColumnStackAddress,
        ColumnStackValue,
        NumColumnsStack // Last column marker
    };

public:

    explicit HidraGui(QWidget *parent = 0);
    ~HidraGui();

    // File handling
    void newFile();
    void save(QString filename);
    void saveAs();
    void saveChangesDialog(bool &cancelled, bool *answeredNo);
    void load(QString filename, bool showErrors);

    void step(bool refresh, bool updateInstructionStrings);
    bool eventFilter(QObject *obj, QEvent *event);

    void enableDataChangedSignal();
    void disableDataChangedSignal();
    void enableStatusBarSignal();
    void disableStatusBarSignal();

public slots:
    void selectMachine(QString machineName);
    void initializeMachineInterface();
    void updateMachineInterface(bool force, bool updateInstructionStrings);
    void scrollToCurrentLine();

    void clearErrorsField();
    void addError(QString);

private slots:
    void sourceCodeChanged();
    void memoryTableDataChanged(QModelIndex topLeft, QModelIndex bottomRight);
    void statusBarMessageChanged(QString newMessage);
    void saveBackup();

    // File menu
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionClose_triggered();
    void closeEvent(QCloseEvent *event);

    // Machine menu
    void on_actionBuild_triggered();
    void on_actionResetPC_triggered();
    void on_actionRun_triggered();
    void on_actionStep_triggered();
    void on_actionImportMemory_triggered();
    void on_actionImportMemoryPartial_triggered();
    void on_actionExportMemory_triggered();
    void on_actionResetRegisters_triggered();
    void on_actionSetBreakpoint_triggered();

    // View menu
    void on_actionHexadecimalMode_toggled(bool checked);
    void on_actionSignedMode_toggled(bool checked);
    void on_actionShowCharacters_toggled(bool checked);
    void on_actionFastExecuteMode_toggled(bool checked);
    void on_actionFollowPCMode_toggled(bool checked);
    void on_actionBaseConversor_triggered();

    // Help menu
    void on_actionQuickGuide_triggered();
    void on_actionReference_triggered();
    void on_actionAbout_triggered();

    // Interface elements
    void on_pushButtonBuild_clicked();
    void on_pushButtonResetPC_clicked();
    void on_pushButtonRun_clicked();
    void on_pushButtonStep_clicked();
    void on_comboBoxMachine_currentIndexChanged(const QString machineName);
    void on_tableViewMemoryInstructions_doubleClicked(const QModelIndex &index);
    void on_tableViewMemoryData_doubleClicked(const QModelIndex &index);

    void on_actionPointConversor_triggered();
    void on_actionDefaultValues_triggered();
    void on_actionFindReplace_triggered();

private:
    // Internal initialize methods (called by initializeMachineInterface)
    void initializeMachineInterfaceComponents();
    void initializeMemoryTable();
    void initializeStackTable();
    void initializeRegisterWidgets();
    void initializeFlagWidgets();
    void initializeHighlighter();
    void initializeInstructionsList();
    void initializeAddressingModesList();

    // Internal clearing methods (called by initializeMachineInterface)
    void clearMachineInterfaceComponents();
    void clearMemoryTable();
    void clearStackTable();
    void clearRegisterWidgets();
    void clearFlagWidgets();
    void clearInstructionsList();
    void clearAddressingModesList();

    // Internal update methods (called by initialize/updateMachineInterface)
    void updateMachineInterfaceComponents(bool force, bool updateInstructionStrings);
    void updateMemoryTable(bool force, bool updateInstructionStrings);
    void updateStackTable();
    void updateRegisterWidgets();
    void updateFlagWidgets();
    void updateCodeEditor();
    void updateButtons();
    void updateWindowTitle();
    void updateInformation(); // Update counters or tracked value

    void setInformationTrackedAddress(QString addressString); // Address tracked by information box

    virtual void dragEnterEvent(QDragEnterEvent *e);
    virtual void dropEvent(QDropEvent *e);

    QString valueToString(int value, bool isHexadecimal, bool isSigned);
    QString getValueDescription(int value, bool isSigned);

    // Config file
    void loadConfFile();
    QSettings settings;


    // Interface elements
    Ui::HidraGui *ui;
    BaseConversorDialog *baseConversor;
    PointConversorDialog *pointConversor;

    Machine *machine;
    HidraHighlighter *highlighter;
    HidraCodeEditor *codeEditor;
    FindReplaceDialog *findReplaceDialog;
    QVector<FlagWidget*> flagWidgets;
    QVector<RegisterWidget*> registerWidgets;
    About *about;
    QString currentMachineName;

    // Information box
    QString informationTrackedAddressPrefix;
    int informationTrackedAddress;

    // File handling
    QString currentFilename;
    bool modifiedFile, manuallyModifiedMemory, modifiedSinceBackup;
    bool forceSaveAs; // Set to true when Save should trigger SaveAs
    QTimer backupTimer;

    // Build status
    bool sourceAndMemoryInSync, buildSuccessful; // Both turn false when code is changed

    // Memory table
    QStandardItemModel memoryModel, stackModel;
    int previousPCValue;
    QVector<QColor> previousRowColor;    
    bool advanceToNextCell = false;
    const QBrush colorGrayedOut;

    // View options
    bool showHexValues, showSignedData, showCharacters; // Value display modes
    bool fastExecute; // Don't update memory table on every instruction
    bool followPC;
};

#endif // HIDRAGUI_H
