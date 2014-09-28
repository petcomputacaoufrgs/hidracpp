#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>

namespace Ui {
class RegisterWidget;
}

class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget *parent = 0, QString registerName = "");
    ~RegisterWidget();    
    void setValue(int value);

private:
    Ui::RegisterWidget *ui;
};

#endif // REGISTERWIDGET_H
