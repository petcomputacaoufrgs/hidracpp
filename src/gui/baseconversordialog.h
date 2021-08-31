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

    /// Helper to convert user input into method calls based on signedness.
    enum Signedness
    {
        /// Positive integers.
        TYPE_POSITIVE = 0,
        /// Sign magnitude format ([+-] and digits).
        TYPE_SIGN_MAGNITUDE = 1,
        /// B-1's complement (or 1's complement): B**n - 1 - y
        TYPE_ONES_COMPLEMENT = 2,
        /// B's complement (or 2's complement): B**n - y
        TYPE_TWOS_COMPLEMENT = 3
    };
};


using namespace std;



#endif // BASECONVERSORDIALOG_H
