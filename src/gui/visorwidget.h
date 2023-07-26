#ifndef VISORWIDGET_H
#define VISORWIDGET_H

#include <QWidget>
#include <QLabel>

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
    void initializeVisorArray();
    void setValue(int position, int value);

    QPoint old_pos;
    int char_width {0};
    QList<QLabel*> labels;
    QList<QLabel*> labels_number;
    QLabel *offset;


private:
    Ui::VisorWidget *ui;
};

#endif // VISORWIDGET_H