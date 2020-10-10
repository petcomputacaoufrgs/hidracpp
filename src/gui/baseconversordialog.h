#ifndef BASECONVERSORDIALOG_H
#define BASECONVERSORDIALOG_H

#include <QDialog>
#include "../core/baseconversor.h"


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



#endif // BASECONVERSORDIALOG_H
