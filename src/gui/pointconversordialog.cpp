#include "pointconversordialog.h"
#include "ui_pointconversordialog.h"
#include "../core/pointconversor.h"
#include "../core/invalidconversorinput.h"

PointConversorDialog::PointConversorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PointConversorDialog)
{
    ui->setupUi(this);
    this->on_inputFormat_currentIndexChanged(ui->inputFormat->currentIndex());
    this->on_outputFormat_currentIndexChanged(ui->outputFormat->currentIndex());
}

PointConversorDialog::~PointConversorDialog()
{
    delete ui;
}

void PointConversorDialog::on_convertButton_clicked()
{
    PointConversor conversor;
    QString input = ui->lineEditInput->text();
    QString output;

    try {
        // Translates input format into method call.
        switch (ui->inputFormat->currentIndex()) {
        case HUMAN:
            conversor.inputHumanNotation(input);
            break;
        case FLOAT16:
            conversor.inputHalfFloat(input);
            break;
        case FLOAT32:
            conversor.inputSingleFloat(input);
            break;
        case FLOAT64:
            conversor.inputDoubleFloat(input);
            break;
        case FIXED8:
            conversor.inputFixed8(input, this->signednessFromUser(ui->inputSignedness));
            break;
        case FIXED16:
            conversor.inputFixed16(input, this->signednessFromUser(ui->inputSignedness));
            break;
        case FIXED32:
            conversor.inputFixed32(input, this->signednessFromUser(ui->inputSignedness));
            break;
        case FIXED64:
            conversor.inputFixed64(input, this->signednessFromUser(ui->inputSignedness));
            break;
        }

        // Translates output format into  method call.
        switch (ui->outputFormat->currentIndex()) {
        case HUMAN:
            output = conversor.outputHumanNotation();
            break;
        case FLOAT16:
            output = conversor.outputHalfFloat();
            break;
        case FLOAT32:
            output = conversor.outputSingleFloat();
            break;
        case FLOAT64:
            output = conversor.outputDoubleFloat();
            break;
        case FIXED8:
            output = conversor.outputFixed8(ui->outputPointPos->value(), this->signednessFromUser(ui->outputSignedness));
            break;
        case FIXED16:
            output = conversor.outputFixed16(ui->outputPointPos->value(), this->signednessFromUser(ui->outputSignedness));
            break;
        case FIXED32:
            output = conversor.outputFixed32(ui->outputPointPos->value(), this->signednessFromUser(ui->outputSignedness));
            break;
        case FIXED64:
            output = conversor.outputFixed64(ui->outputPointPos->value(), this->signednessFromUser(ui->outputSignedness));
            break;
        }

        ui->errorMessage->setText("Ok");
    } catch (InvalidConversorInput exc) {
        // Shows error to the user.
        ui->errorMessage->setText(exc.getMessage());
    }

    // Even on error, this will run. In such case, output box will be cleared.
    ui->lineEditOutput->setText(output);
}

void PointConversorDialog::on_invertButton_clicked()
{
    // Inverts number.
    QString input = ui->lineEditInput->text();
    ui->lineEditInput->setText(ui->lineEditOutput->text());
    ui->lineEditOutput->setText(input);

    // Inverts format/type.
    int inputFormat = ui->inputFormat->currentIndex();
    ui->inputFormat->setCurrentIndex(ui->outputFormat->currentIndex());
    ui->outputFormat->setCurrentIndex(inputFormat);

    // Inverts signedness.
    int inputSignedness = ui->inputSignedness->currentIndex();
    ui->inputSignedness->setCurrentIndex(ui->outputSignedness->currentIndex());
    ui->outputSignedness->setCurrentIndex(inputSignedness);

    int pointPos = input.indexOf('.');
    if (pointPos >= ui->outputPointPos->minimum() && pointPos <= ui->outputPointPos->maximum()) {
        // Inverts(?) point position.
        ui->outputPointPos->setValue(pointPos);
    }
}

void PointConversorDialog::on_inputFormat_currentIndexChanged(int index)
{
    switch (index) {
    case HUMAN:
    case FLOAT16:
    case FLOAT32:
    case FLOAT64:
        // Disable fixed point specific inputs on input options.
        this->ui->inputSignedness->setEnabled(false);
        this->ui->labelInputSignedness->setEnabled(false);
        break;
    case FIXED8:
    case FIXED16:
    case FIXED32:
    case FIXED64:
        // Enable fixed point specific inputs on input options.
        this->ui->inputSignedness->setEnabled(true);
        this->ui->labelInputSignedness->setEnabled(true);
        break;
    }
}


void PointConversorDialog::on_outputFormat_currentIndexChanged(int index)
{
    switch (index) {
    case HUMAN:
    case FLOAT16:
    case FLOAT32:
    case FLOAT64:
        // Disable fixed point specific inputs on output options.
        this->ui->outputSignedness->setEnabled(false);
        this->ui->labelOutputSignedness->setEnabled(false);
        this->ui->outputPointPos->setEnabled(false);
        this->ui->labelOutputPointPos->setEnabled(false);
        break;
    case FIXED8:
    case FIXED16:
    case FIXED32:
    case FIXED64:
        // Enable fixed point specific inputs on output options.
        this->ui->outputSignedness->setEnabled(true);
        this->ui->labelOutputSignedness->setEnabled(true);
        this->ui->outputPointPos->setEnabled(true);
        this->ui->labelOutputPointPos->setEnabled(true);
        break;
    }
}

PointConversor::Signedness PointConversorDialog::signednessFromUser(QComboBox *input)
{
    switch (input->currentIndex()) {
    case PointConversor::UNSIGNED:
        return PointConversor::UNSIGNED;
    case PointConversor::TWOS_COMPL:
        return PointConversor::TWOS_COMPL;
    default:
        // Required cause we have no guarantee that input->currentIndex() is in the
        // correct range, and just not doing nothing would be UB.
        throw InvalidConversorInput("Tipo inválido de sinal");
    }
}
