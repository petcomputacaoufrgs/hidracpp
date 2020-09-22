#ifndef BASECONVERSOR_H
#define BASECONVERSOR_H

#include <QDialog>

namespace Ui {
class BaseConversor;
}

class BaseConversor : public QDialog
{
    Q_OBJECT

public:
    explicit BaseConversor(QWidget *parent = nullptr);
    ~BaseConversor();

private:
    Ui::BaseConversor *ui;
};

#endif // BASECONVERSOR_H
