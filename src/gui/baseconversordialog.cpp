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


    switch (inputTypeIndex){
        case 0: ui->labelError->setText(baseConversor.inputValidation
                                                (baseIn, baseOut, inputValorStringQ));
                baseConversor = baseConversor.inputPositive(inputValorStringQ, baseIn);
                break;
        case 1: ui->labelError->setText(baseConversor.inputValidationSignalMagnitude
                                                (baseIn, baseOut, inputValorStringQ));
                baseConversor = baseConversor.inputSignalMagnitude(inputValorStringQ, baseIn);
                break;

        case 2: ui->labelError->setText(baseConversor.inputValidation
                                        (baseIn, baseOut, inputValorStringQ));
                baseConversor = baseConversor.inputOnesComplement(inputValorStringQ, baseIn);
                break;

        case 3: baseConversor = baseConversor.inputTwosComplement(inputValorStringQ, baseIn);
            break;
    }

    switch (outputTypeIndex){
        case 0: if(baseConversor.getNegativeSignal() == true)
                    ui->labelError->setText("Valor negativo, não há representação em inteiro positivo");
                stringOut = baseConversor.outputPositive(baseOut);
                break;
        case 1: stringOut = baseConversor.outputSignalMagnitude(baseOut);
                break;
    }

    ui->lineEditOutputValor->setText(stringOut);

}

void BaseConversorDialog::on_pushButton_2_clicked()
{
    QString inputBaseStringQ = ui->lineEditInputBase->text();
    ui->lineEditInputBase->setText(ui->lineEditOutputBase->text());
    ui->lineEditOutputBase->setText(inputBaseStringQ);
}

