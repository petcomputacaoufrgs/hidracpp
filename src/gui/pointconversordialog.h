#ifndef POINTCONVERSOR_H
#define POINTCONVERSOR_H

#include <QDialog>
#include <cstdint>

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

private:
    Ui::PointConversorDialog *ui;
    double input;
    double output;

    void parseInput();
    uint64_t parseBin();
    void parseFloat16();
};

#endif // POINTCONVERSOR_H
