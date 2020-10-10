#include "baseconversordialog.h"
#include "ui_baseconversordialog.h"

BaseConversorDialog::BaseConversorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BaseConversorDialog)
{
    ui->setupUi(this);
}

BaseConversorDialog::~BaseConversorDialog()
{
    delete ui;
}


void BaseConversorDialog::on_pushButton_clicked()
{
    BaseConversor baseConversor;
    QString inputValorStringQ = ui->lineEditInputValor->text();
    QString baseInQ = ui->lineEditInputBase->text();
    QString baseOutQ = ui->lineEditOutputBase->text();

    ui->label_5->setText(baseConversor.inputValidation(baseInQ.toInt(),
                                                      baseOutQ.toInt(),
                                                      inputValorStringQ));

    QString stringOut = BaseConversor()
        .input(inputValorStringQ, baseInQ.toInt())
        .output(baseOutQ.toInt());

    ui->lineEditOutputValor->setText(stringOut);

}

void BaseConversorDialog::on_pushButton_2_clicked()
{
    QString inputBaseStringQ = ui->lineEditInputBase->text();
    ui->lineEditInputBase->setText(ui->lineEditOutputBase->text());
    ui->lineEditOutputBase->setText(inputBaseStringQ);
}

