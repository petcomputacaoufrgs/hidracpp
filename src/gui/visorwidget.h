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
    
    void mousePressEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);

    QPoint old_pos;


private:
    Ui::VisorWidget *ui;
};

#endif // VISORWIDGET_H