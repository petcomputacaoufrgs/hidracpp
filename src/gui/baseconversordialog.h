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
    void on_convertButton_clicked();

    void on_invertButton_clicked();

private:
    Ui::BaseConversorDialog *ui;

    static const int TYPE_POSITIVE = 0;
    static const int TYPE_SIGN_MAGNITUDE = 1;
    static const int TYPE_ONES_COMPLEMENT = 2;
    static const int TYPE_TWOS_COMPLEMENT = 3;
};


using namespace std;



#endif // BASECONVERSORDIALOG_H
