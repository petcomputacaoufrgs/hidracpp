#ifndef BASECONVERSOR_H
#define BASECONVERSOR_H

#include <QDialog>
#include <iostream>
#include <math.h>
#include <string>

namespace Ui {
class BaseConversorDialog;
}

class BaseConversorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BaseConversorDialog(QWidget *parent = nullptr);
    ~BaseConversorDialog();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::BaseConversorDialog *ui;
};


using namespace std;



#endif // BASECONVERSOR_H
