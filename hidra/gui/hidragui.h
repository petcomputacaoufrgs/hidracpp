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
    void on_pushButtonStep_clicked();
    void on_pushButtonRun_clicked();

    void initializeMachineInterface();
    void updateMachineInterface();

    void clearErrorsField();
    void addError(QString);

private slots:
    void sourceCodeChanged();

    void on_actionPasso_triggered();
    void on_actionRodar_triggered();
    void on_actionMontar_triggered();
    void on_actionSaveAs_triggered();
    void on_comboBoxMachine_currentIndexChanged(int index);
    void on_action_Save_triggered();
    void on_actionClose_triggered();
    void on_actionManual_triggered();
    void on_actionRelatar_um_problema_triggered();
    void on_actionOpen_triggered();
    void on_actionCarregar_triggered();
    void on_actionSaveMem_triggered();
    void on_actionZerarMemoria_triggered();
    void on_actionZerar_registradores_triggered();
    void on_pushButtonMontar_clicked();
    void on_actionHexadecimal_toggled(bool arg1);

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

    void closeEvent(QCloseEvent *event);
};

#endif // HIDRAGUI_H
