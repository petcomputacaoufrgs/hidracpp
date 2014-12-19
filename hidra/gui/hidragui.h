#ifndef HIDRAGUI_H
#define HIDRAGUI_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QCheckBox>
#include <QDebug>
#include <QVector>

#include "hidracodeeditor.h"
#include "hidrahighlighter.h"
#include "registerwidget.h"
#include "flagwidget.h"
#include "machines/neandermachine.h"
#include "machines/ahmesmachine.h"
#include "machines/ramsesmachine.h"

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

    void save();
    void saveAs();

public slots:
    void on_pushButtonBuild_clicked();
    void on_pushButtonRun_clicked();
    void on_pushButtonStep_clicked();

    void initializeMachineInterface();
    void updateMachineInterface();

    void clearErrorsField();
    void addError(QString);

private slots:
    void sourceCodeChanged();

    void on_actionBuild_triggered();
    void on_actionRun_triggered();
    void on_actionStep_triggered();

    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();

    void on_actionImportMemory_triggered();
    void on_actionExportMemory_triggered();

    void on_actionClearRegisters_triggered();
    void on_actionHexadecimalMode_toggled(bool checked);
    void on_comboBoxMachine_currentIndexChanged(int index);

    void on_actionManual_triggered();
    void on_actionReportProblem_triggered();
    void on_actionClose_triggered();
    void closeEvent(QCloseEvent *event);

    void on_actionSetBreakpoint_triggered();

private:

    void initializeMachineInterfaceComponents();
    void initializeMemoryTable();
    void initializeRegisterWidgets();
    void initializeFlagWidgets();
    void initializeHighlighter();

    void clearMachineInterfaceComponents();
    void clearMemoryMap();
    void clearRegisterWidgets();
    void clearFlagWidgets();

    void updateMachineInterfaceComponents();
    void updateMemoryMap();
    void updateRegisterWidgets();
    void updateFlagWidgets();
    void updateCodeEditor();
    void updateButtons();

    Ui::HidraGui *ui;
    Machine *machine;
    QString currentFile;
    bool modifiedFile, sourceAndMemoryInSync, buildSuccessful; // Both turn false when code is changed
    bool fileSaved; // Feedback from Save and SaveAs
    bool showHexValues;
    QStandardItemModel model;
    HidraHighlighter *highlighter;
    HidraCodeEditor *codeEditor;

    QVector<FlagWidget*> flagWidgets;
    QVector<RegisterWidget*> registerWidgets;
};

#endif // HIDRAGUI_H
