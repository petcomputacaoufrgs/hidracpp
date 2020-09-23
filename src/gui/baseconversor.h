#ifndef BASECONVERSOR_H
#define BASECONVERSOR_H

#include <QDialog>
#include <iostream>
#include <math.h>
#include <string>

namespace Ui {
class BaseConversor;
}

class BaseConversor : public QDialog
{
    Q_OBJECT

public:
    explicit BaseConversor(QWidget *parent = nullptr);
    ~BaseConversor();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::BaseConversor *ui;
};


using namespace std;



#endif // BASECONVERSOR_H
