#ifndef VISORWIDGET_H
#define VISORWIDGET_H

#include <QWidget>

namespace Ui {
class VisorWidget;
}

class VisorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VisorWidget(QWidget *parent = 0);
    ~VisorWidget();

private:
    Ui::VisorWidget *ui;
};

#endif // VISORWIDGET_H