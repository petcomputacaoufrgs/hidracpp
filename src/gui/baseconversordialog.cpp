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


void BaseConversorDialog::on_convertButton_clicked()
{
    BaseConversor baseConversor;
    QString stringOut = "Etrada original";
    int inputTypeIndex = ui->inputType->currentIndex();
    int outputTypeIndex = ui->outputType->currentIndex();

    // Getting inputs
    QString inputValorStringQ = ui->lineEditInputValor->text();
    QString baseInQ = ui->lineEditInputBase->text();
    QString baseOutQ = ui->lineEditOutputBase->text();
    // converting bases from QString to int
    int baseOut = baseOutQ.toInt();
    int baseIn = baseInQ.toInt();

    // Input validation


    switch (inputTypeIndex) {
        case TYPE_POSITIVE:
            ui->labelError->setText(
                    baseConversor.validate(baseIn, baseOut, inputValorStringQ));
            baseConversor = baseConversor.inputPositive(inputValorStringQ, baseIn);
            break;
        case TYPE_SIGN_MAGNITUDE:
            ui->labelError->setText(
                    baseConversor.validateSignMagnitude(baseIn, baseOut, inputValorStringQ));
            baseConversor = baseConversor.inputSignMagnitude(inputValorStringQ, baseIn);
            break;

        case TYPE_ONES_COMPLEMENT:
            ui->labelError->setText(
                    baseConversor.validate(baseIn, baseOut, inputValorStringQ));
            baseConversor = baseConversor.inputOnesComplement(inputValorStringQ, baseIn);
            break;

        case TYPE_TWOS_COMPLEMENT:
            baseConversor = baseConversor.inputTwosComplement(inputValorStringQ, baseIn);
            break;
    }

    switch (outputTypeIndex) {
        case TYPE_POSITIVE:
            if (baseConversor.getNegativeSignal())
                ui->labelError->setText("Valor negativo, não há representação em inteiro positivo");
            stringOut = baseConversor.outputPositive(baseOut);
            break;
        case TYPE_SIGN_MAGNITUDE:
            stringOut = baseConversor.outputSignMagnitude(baseOut);
            break;
        case TYPE_ONES_COMPLEMENT:
            stringOut = baseConversor.outputOnesComplement(baseOut);
            break;
        case TYPE_TWOS_COMPLEMENT:
            stringOut = baseConversor.outputTwosComplement(baseOut);
            break;
    }

    ui->lineEditOutputValor->setText(stringOut);

}

void BaseConversorDialog::on_invertButton_clicked()
{
    QString inputBaseStringQ = ui->lineEditInputBase->text();
    ui->lineEditInputBase->setText(ui->lineEditOutputBase->text());
    ui->lineEditOutputBase->setText(inputBaseStringQ);
}
