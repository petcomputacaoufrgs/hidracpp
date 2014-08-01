#ifndef HIDRAGUI_H
#define HIDRAGUI_H

#include <QMainWindow>
#include <QStandardItemModel>
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
public slots:
    void on_commandLinkButtonStep_clicked();
    void on_commandLinkButtonRun_clicked();
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

private:
    Ui::HidraGui *ui;
    Machine *machine;
    QString actualFile;
    bool savedFile;
    QStandardItemModel *model;
};

#endif // HIDRAGUI_H
