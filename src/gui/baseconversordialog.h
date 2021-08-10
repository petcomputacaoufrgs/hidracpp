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

    enum Signedness
    {
        TYPE_POSITIVE = 0,
        TYPE_SIGN_MAGNITUDE = 1,
        TYPE_ONES_COMPLEMENT = 2,
        TYPE_TWOS_COMPLEMENT = 3
    };
};


using namespace std;



#endif // BASECONVERSORDIALOG_H
