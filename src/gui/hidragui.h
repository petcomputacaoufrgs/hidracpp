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

#include "hidracodeeditor.h"
#include "hidrahighlighter.h"
#include "registerwidget.h"
#include "flagwidget.h"
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
        ColumnLabel,
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
    void saveChangesDialog(bool &cancelled);
    void load(QString filename);

    void step(bool refresh);
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void selectMachine(QString machineName);
    void initializeMachineInterface();
    void updateMachineInterface(bool force);

    void clearErrorsField();
    void addError(QString);

private slots:
    void sourceCodeChanged();
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
    void on_actionRun_triggered();
    void on_actionStep_triggered();
    void on_actionImportMemory_triggered();
    void on_actionExportMemory_triggered();
    void on_actionResetRegisters_triggered();
    void on_actionSetBreakpoint_triggered();

    // View menu
    void on_actionHexadecimalMode_toggled(bool checked);
    void on_actionSignedMode_toggled(bool checked);
    void on_actionFastExecuteMode_toggled(bool checked);
    void on_actionFollowPCMode_toggled(bool checked);

    // Help menu
    void on_actionQuickGuide_triggered();
    void on_actionReference_triggered();
    void on_actionAbout_triggered();

    // Interface elements
    void on_pushButtonBuild_clicked();
    void on_pushButtonRun_clicked();
    void on_pushButtonStep_clicked();
    void on_comboBoxMachine_currentIndexChanged(const QString machineName);
    void on_tableViewMemoryInstructions_doubleClicked(const QModelIndex &index);
    void on_tableViewMemoryData_doubleClicked(const QModelIndex &index);

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
    void updateMachineInterfaceComponents(bool force);
    void updateMemoryTable(bool force);
    void updateStackTable();
    void updateRegisterWidgets();
    void updateFlagWidgets();
    void updateCodeEditor();
    void updateButtons();
    void updateInformation(); // Show counters
    void updateInformation(int value); // Show value in dec/hex/bin
    void updateWindowTitle();

    virtual void dragEnterEvent(QDragEnterEvent *e);
    virtual void dropEvent(QDropEvent *e);

    QString getValueDescription(int value);

    // Interface elements
    Ui::HidraGui *ui;
    Machine *machine;
    HidraHighlighter *highlighter;
    HidraCodeEditor *codeEditor;
    QVector<FlagWidget*> flagWidgets;
    QVector<RegisterWidget*> registerWidgets;
    QString currentMachineName;

    // File handling
    QString currentFilename;
    bool modifiedFile, modifiedSinceBackup;
    bool forceSaveAs; // Set to true when Save should trigger SaveAs
    QTimer backupTimer;

    // Build status
    bool sourceAndMemoryInSync, buildSuccessful; // Both turn false when code is changed

    // Memory table
    QStandardItemModel memoryModel, stackModel;
    int previousPCValue;
    QVector<QString> previousLabel;
    QVector<QColor> previousRowColor;

    // View options
    bool showHexValues, showSignedData; // Value display modes
    bool fastExecute; // Don't update memory table on every instruction
    bool followPC;
};

#endif // HIDRAGUI_H
