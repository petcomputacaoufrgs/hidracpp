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
//#include "machines/periclesmachine.h"
#include "machines/regmachine.h"
#include "machines/voltamachine.h"

namespace Ui {
class HidraGui;
}

class HidraGui : public QMainWindow
{
    Q_OBJECT

public:
    explicit HidraGui(QWidget *parent = 0);
    ~HidraGui();

    bool eventFilter(QObject *obj, QEvent *event);

    void newFile();
    void save(QString filename);
    void saveAs();
    void saveChangesDialog(bool &cancelled);
    void load(QString filename);
    void step(bool refresh);

public slots:
    void on_pushButtonBuild_clicked();
    void on_pushButtonRun_clicked();
    void on_pushButtonStep_clicked();

    void selectMachine(QString machineName);
    void initializeMachineInterface();
    void updateMachineInterface(bool force);

    void clearErrorsField();
    void addError(QString);

private slots:
    void sourceCodeChanged();
    void saveBackup();

    void on_actionBuild_triggered();
    void on_actionRun_triggered();
    void on_actionStep_triggered();

    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();

    void on_actionImportMemory_triggered();
    void on_actionExportMemory_triggered();

    void on_tableViewMemoryInstructions_doubleClicked(const QModelIndex &index);
    void on_tableViewMemoryData_doubleClicked(const QModelIndex &index);

    void on_actionResetRegisters_triggered();
    void on_actionSetBreakpoint_triggered();
    void on_comboBoxMachine_currentIndexChanged(const QString machineName);

    void on_actionClose_triggered();
    void closeEvent(QCloseEvent *event);

    void on_actionQuickGuide_triggered();
    void on_actionReference_triggered();
    void on_actionAbout_triggered();

    void statusBarMessageChanged(QString newMessage);

    void on_actionHexadecimalMode_toggled(bool checked);
    void on_actionFastExecuteMode_toggled(bool checked);
    void on_actionFollowPCMode_toggled(bool checked);

private:

    void initializeMachineInterfaceComponents();
    void initializeMemoryTable();
    void initializeRegisterWidgets();
    void initializeFlagWidgets();
    void initializeHighlighter();
    void initializeInstructionsList();

    void clearMachineInterfaceComponents();
    void clearMemoryTable();
    void clearRegisterWidgets();
    void clearFlagWidgets();
    void clearInstructionsList();

    void updateMachineInterfaceComponents(bool force);
    void updateMemoryTable(bool force);
    void updateRegisterWidgets();
    void updateFlagWidgets();
    void updateCodeEditor();
    void updateButtons();
    void updateInformation(); // Show counters
    void updateInformation(int value); // Show value in dec/hex/bin

    virtual void dragEnterEvent(QDragEnterEvent *e);
    virtual void dropEvent(QDropEvent *e);

    QTimer backupTimer;

    QString getValueDescription(int value);

    int previousPCValue;
    QVector<QString> previousLabel;
    QVector<QColor> previousRowColor;

    Ui::HidraGui *ui;
    Machine *machine;
    QString currentFilename;
    bool modifiedFile, modifiedSinceBackup;
    bool sourceAndMemoryInSync, buildSuccessful; // Both turn false when code is changed
    bool forceSaveAs; // Set to true when Save should trigger SaveAs
    bool showHexValues;
    bool fastExecute;
    bool followPC;
    QStandardItemModel memoryModel;
    HidraHighlighter *highlighter;
    HidraCodeEditor *codeEditor;
    QString currentMachineName;

    QVector<FlagWidget*> flagWidgets;
    QVector<RegisterWidget*> registerWidgets;
};

#endif // HIDRAGUI_H
