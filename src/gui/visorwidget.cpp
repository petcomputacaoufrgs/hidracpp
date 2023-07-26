#include "visorwidget.h"
#include "ui_visorwidget.h"
#include <QtGui>
#include <QLabel>
#include <QChar>
#include <iostream>

#define MIN_CHAR_VALUE 32
#define MAX_CHAR_VALUE 126

#define FONT_SIZE_CHAR 40
#define FONT_SIZE_POSITION 12
#define VISOR_SIZE 36


VisorWidget::VisorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisorWidget)
{
    ui->setupUi(this);
    
    this->setAttribute(Qt::WA_QuitOnClose, false);
    this->setWindowFlags(Qt::FramelessWindowHint);
    initializeVisorArray(); 
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

void VisorWidget::setValue(int position, int value)
{   
    QChar qchar;

    if (value >= MIN_CHAR_VALUE and value <= MAX_CHAR_VALUE)
    {        
        qchar = QChar(value);
    }
    else
    {        
        qchar = QChar(' ');
    }

    labels[position]->setText(qchar);
}


void VisorWidget::initializeVisorArray()
{
    // Get custom font
    // TODO: change font path
    int font_id = QFontDatabase::addApplicationFont("../src/gui/fonts/LEDCalculator.ttf");
    QFont font_char = QFontDatabase::applicationFontFamilies(font_id).at(0);
    QFont font_position = QFontDatabase::applicationFontFamilies(font_id).at(0);
    font_char.setPointSize(FONT_SIZE_CHAR);
    font_position.setPointSize(FONT_SIZE_POSITION);

    char_width = width()/ VISOR_SIZE;

    int x = 10; // Starting X position
    int y = 10; // Starting Y position

    // Create vectors of QLabels
    // Each visor position has its own Qlabel
    // Position in visor is accessed by its respective position in QLabels vector (example: nth position in visor = label[nth])
    for (int i = 0; i < VISOR_SIZE; i++) 
    {
        QLabel *label = new QLabel(" ", this); // Set initial text to a space character
        label->setFont(font_char);
        labels.append(label);
        label->setStyleSheet("QLabel { background-color : black; color : lime; }");

        QLabel *label_number = new QLabel(" ", this); // Set initial text to a space character
        label_number->setFont(font_position);
        labels_number.append(label_number);
        label_number->setStyleSheet("QLabel { background-color : black; color : white; }");
    }

     
    for (int i = 0; i < labels.size(); i++)
    {
        labels[i]->setGeometry((i*width())/VISOR_SIZE, 0, char_width + 1, 70);
        labels_number[i]->setGeometry(i*width()/VISOR_SIZE + 7, 60, char_width + 1, 30); 

        QString qchar = QString::number(i);
        labels_number[i]->setText(qchar);
    }


    // chinelagem, label to paint balck empty space in labels_number (because of the offset of + 7)
    offset = new QLabel(" ", this);

    offset->setGeometry(0, 60, 7, 30); 
    offset->setStyleSheet("QLabel { background-color : black; color : lime; }");

}