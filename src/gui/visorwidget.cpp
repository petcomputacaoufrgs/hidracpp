#include "visorwidget.h"
#include "ui_visorwidget.h"
#include <QtGui>

VisorWidget::VisorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisorWidget)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose, false);
    this->setWindowFlags(Qt::FramelessWindowHint);
}

VisorWidget::~VisorWidget()
{
    delete ui;
}

// get old position when mouse is clicked on window
void VisorWidget::mousePressEvent(QMouseEvent *evt)
{
    old_pos = evt->globalPos();
}

// update position of visor widget 
void  VisorWidget::mouseMoveEvent(QMouseEvent *evt)
{
    const QPoint pos_variation = evt->globalPos() - old_pos;
    move(x()+pos_variation.x(), y()+pos_variation.y());
    old_pos = evt->globalPos();
}

