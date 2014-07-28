#ifndef HIDRAGUI_H
#define HIDRAGUI_H

#include <QMainWindow>
#include "ahmesmachine.h"

namespace Ui {
class HidraGui;
}

class HidraGui : public QMainWindow
{
    Q_OBJECT

public:
    explicit HidraGui(QWidget *parent = 0);
    ~HidraGui();

private:
    Ui::HidraGui *ui;
};

#endif // HIDRAGUI_H
