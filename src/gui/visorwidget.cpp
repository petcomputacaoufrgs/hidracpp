#include "visorwidget.h"
#include "ui_visorwidget.h"
#include <QtGui>
#include <QLabel>
#include <QChar>
#include <iostream>


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
// Position the QLabel widgets manually
    
    char toPrint = char(value);
    
    QChar qchar = QChar(toPrint);
        
   

// Update the text of the QLabel widgets as needed
    
    labels[position]->setText(qchar);
}

void VisorWidget::initializeVisorArray()
{
    // QPixmap pixmap(":/Images/visor.png");
   // Set up a monospaced font
    QFont font("Led Board", 12); // 12 is a suitable font size for most displays
    font.setStyleHint(QFont::Monospace);

    int x = 10; // Starting X position
    int y = 10; // Starting Y position
    

    // Create QLabel widgets for each position
    for (int i = 0; i < 36; i++) 
    {
        QLabel *label = new QLabel(" ", this); // Set initial text to a space character
        label->setFont(font);
        labels.append(label);
        label->setStyleSheet("QLabel { background-color : white; color : black; }");
        // label->setPixmap(pixmap);

        QLabel *label_number = new QLabel(" ", this); // Set initial text to a space character
        label_number->setFont(font);
        labels_number.append(label_number);
    }

    labels[0]->setGeometry(10,y,width(), height());
    labels_number[0]->setGeometry(10,y+20,width(), height());
     
    for (int i = 0; i < labels.size(); i++)
    {
        labels[i]->setGeometry((i*35/1.15)+10, y, width(), height()); // Set size to 20x20 (adjust as needed)
        labels_number[i]->setGeometry((i*35/1.15)+10, y+20, width(), height()); 
        // Increase X position for the next label

        QString qchar = QString::number(i);
        labels_number[i]->setText(qchar);
    }

}