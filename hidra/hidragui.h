#ifndef HIDRAGUI_H
#define HIDRAGUI_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include "hidracodeeditor.h"
#include "hidrahighlighter.h"
#include "neandermachine.h"
#include "ahmesmachine.h"
#include "ramsesmachine.h"

namespace Ui {
class HidraGui;
}

class HidraGui : public QMainWindow
{
    Q_OBJECT

public:
    explicit HidraGui(QWidget *parent = 0);
    ~HidraGui();

    void cleanMachines();
    void save();
    void saveAs();
public slots:
    void on_commandLinkButtonStep_clicked();
    void on_commandLinkButtonRun_clicked();

    void updateMemoryMap();
    void updateFlagsLeds();
    void updateLCDDisplay();
    void cleanErrorsField();

    void addError(QString);
private slots:
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

    void on_textEditSouceCode_textChanged();

    void on_actionCarregar_triggered();

    void on_actionSaveMem_triggered();

    void on_actionZerarMemoria_triggered();

    void on_actionZerar_registradores_triggered();

    void on_commandLinkButtonMontar_clicked();

private:
    Ui::HidraGui *ui;
    Machine *machine;
    QString currentFile;
    bool savedFile, buildSuccessful;
    QStandardItemModel *model;
    HidraHighlighter *highlighter;
    HidraCodeEditor *codeEditor;
};

#endif // HIDRAGUI_H
