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

private slots:
    void on_convertButton_clicked();
    void on_invertButton_clicked();

    void on_inputFormat_currentIndexChanged(int index);

    void on_outputFormat_currentIndexChanged(int index);

private:
    Ui::PointConversorDialog *ui;

    /// Helper to convert conversion input and output formats conversor's
    /// into method calls.
    enum Notation
    {
        /// Human notation, e.g. -23.54544901
        HUMAN = 0,
        /// 16-bit float aka half precision floating point number, in a binary string.
        FLOAT16 = 1,
        /// 32-bit float aka single precision floating point number, in a binary string.
        FLOAT32 = 2,
        /// 64-bit float aka double precision floating point number, in a binary string.
        FLOAT64 = 3,
        /// 8-bit fixed point number, in a binary string with a point.
        FIXED8 = 4,
        /// 16-bit fixed point number, in a binary string with a point.
        FIXED16 = 5,
        /// 32-bit fixed point number, in a binary string with a point.
        FIXED32 = 6,
        /// 64-bit fixed point number, in a binary string with a point.
        FIXED64 = 7
    };

    /// Converts the selected index of the given signedness input widget
    /// into point conversor's signedness.
    PointConversor::Signedness signednessFromUser(QComboBox *input);
};

#endif // POINTCONVERSOR_H
