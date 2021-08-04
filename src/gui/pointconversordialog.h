#ifndef POINTCONVERSORDIALOG_H
#define POINTCONVERSORDIALOG_H

#include <QDialog>
#include <cstdint>

#include "../core/pointconversor.h"

#include <QComboBox>

namespace Ui {
class PointConversorDialog;
}

class PointConversorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PointConversorDialog(QWidget *parent = nullptr);
    ~PointConversorDialog();

    enum Notation {
        HUMAN = 0,
        FLOAT16 = 1,
        FLOAT32 = 2,
        FLOAT64 = 3,
        FIXED8 = 4,
        FIXED16 = 5,
        FIXED32 = 6,
        FIXED64 = 7
    };

private slots:
    void on_convertButton_clicked();
    void on_invertButton_clicked();

    void on_inputFormat_currentIndexChanged(int index);

    void on_outputFormat_currentIndexChanged(int index);

private:
    Ui::PointConversorDialog *ui;

    PointConversor::Signedness signednessFromUser(QComboBox *input);
};

#endif // POINTCONVERSOR_H
