#include "baseconversordialog.h"
#include "ui_baseconversordialog.h"
#include "core/invalidconversorinput.h"

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

void BaseConversorDialog::on_convertButton_clicked()
{
    BaseConversor baseConversor;
    QString stringOut = "";
    int inputTypeIndex = ui->inputType->currentIndex();
    int outputTypeIndex = ui->outputType->currentIndex();

    // Getting inputs
    QString inputValorStringQ = ui->lineEditInputValor->text();
    QString baseInQ = ui->lineEditInputBase->text();
    QString baseOutQ = ui->lineEditOutputBase->text();
    // converting bases from QString to int
    int baseOut = baseOutQ.toInt();
    int baseIn = baseInQ.toInt();

    try {
        switch (inputTypeIndex) {
            case TYPE_POSITIVE:
                baseConversor.inputPositive(inputValorStringQ, baseIn);
                break;
            case TYPE_SIGN_MAGNITUDE:
                baseConversor.inputSignMagnitude(inputValorStringQ, baseIn);
                break;
            case TYPE_ONES_COMPLEMENT:
                baseConversor.inputOnesComplement(inputValorStringQ, baseIn);
                break;
            case TYPE_TWOS_COMPLEMENT:
                baseConversor.inputTwosComplement(inputValorStringQ, baseIn);
                break;
        }

        switch (outputTypeIndex) {
            case TYPE_POSITIVE:
                stringOut = baseConversor.outputPositive(baseOut, ui->minDigits->value());
                break;
            case TYPE_SIGN_MAGNITUDE:
                stringOut = baseConversor.outputSignMagnitude(baseOut, ui->minDigits->value());
                break;
            case TYPE_ONES_COMPLEMENT:
                stringOut = baseConversor.outputOnesComplement(baseOut, ui->minDigits->value());
                break;
            case TYPE_TWOS_COMPLEMENT:
                stringOut = baseConversor.outputTwosComplement(baseOut, ui->minDigits->value());
                break;
        }
    } catch (InvalidConversorInput err) {
        ui->labelError->setText(err.getMessage());
    }

    ui->lineEditOutputValor->setText(stringOut);
}

void BaseConversorDialog::on_invertButton_clicked()
{
    QString inputBaseStringQ = ui->lineEditInputBase->text();
    ui->lineEditInputBase->setText(ui->lineEditOutputBase->text());
    ui->lineEditOutputBase->setText(inputBaseStringQ);
}
