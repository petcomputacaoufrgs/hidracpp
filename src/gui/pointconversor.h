#ifndef POINTCONVERSOR_H
#define POINTCONVERSOR_H

#include <QDialog>

namespace Ui {
class PointConversor;
}

class PointConversor : public QDialog
{
    Q_OBJECT

public:
    explicit PointConversor(QWidget *parent = nullptr);
    ~PointConversor();

private:
    Ui::PointConversor *ui;
};

#endif // POINTCONVERSOR_H
