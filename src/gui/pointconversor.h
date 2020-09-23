#ifndef POINTCONVERSOR_H
#define POINTCONVERSOR_H

#include <QDialog>
#include <stdint.h>

namespace Ui {
class PointConversor;
}

class PointConversor : public QDialog
{
    Q_OBJECT

public:
    explicit PointConversor(QWidget *parent = nullptr);
    ~PointConversor();

private slots:
    void on_pushButtonInvert_clicked();

private:
    Ui::PointConversor *ui;
    double input;
    double output;

    void parseInput();
    uint64_t parseBin();
    void parseFloat16();
};

#endif // POINTCONVERSOR_H
