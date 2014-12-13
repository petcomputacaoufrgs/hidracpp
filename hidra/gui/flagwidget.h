#ifndef FLAGWIDGET_H
#define FLAGWIDGET_H

#include <QWidget>

namespace Ui {
class FlagWidget;
}

class FlagWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FlagWidget(QWidget *parent = 0, QString flagName = "", bool flagValue = false);
    ~FlagWidget();
    void setValue(bool value);

private:
    Ui::FlagWidget *ui;
};

#endif // FLAGWIDGET_H
