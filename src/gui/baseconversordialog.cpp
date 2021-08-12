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
    int baseIn = ui->inputBase->value();
    int baseOut = ui->outputBase->value();

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
    } catch (InvalidConversorInput const &err) {
        ui->labelError->setText(err.getMessage());
    }

    ui->lineEditOutputValor->setText(stringOut);
}

void BaseConversorDialog::on_invertButton_clicked()
{
    int inputBase = ui->inputBase->value();
    QString inputNumber = ui->lineEditInputValor->text();
    int inputFormat = ui->inputType->currentIndex();

    ui->inputBase->setValue(ui->outputBase->value());
    ui->outputBase->setValue(inputBase);
    ui->lineEditInputValor->setText(ui->lineEditOutputValor->text());
    ui->lineEditOutputValor->setText(inputNumber);
    ui->inputType->setCurrentIndex(ui->outputType->currentIndex());
    ui->outputType->setCurrentIndex(inputFormat);
    ui->minDigits->setValue(inputNumber.length());
}
