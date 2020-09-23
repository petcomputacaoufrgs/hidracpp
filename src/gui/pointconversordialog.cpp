#include "pointconversordialog.h"
#include "ui_pointconversor.h"

PointConversorDialog::PointConversorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PointConversor)
{
    ui->setupUi(this);
    input = 0.0;
    output = 0.0;
}

PointConversorDialog::~PointConversorDialog()
{
    delete ui;
}

void PointConversorDialog::on_pushButtonInvert_clicked()
{
    QString input = ui->lineEditInput->text();
    ui->lineEditInput->setText(ui->lineEditOutput->text());
    ui->lineEditOutput->setText(input);
}

void PointConversorDialog::parseInput()
{
    QString inputFormat = ui->comboBoxInput->currentText();
    if (inputFormat == "Notação Humana")
    {
        // TODO: check if OK is actually OK.
        bool ok = 0;
        input = ui->lineEditInput->text().toDouble(&ok);
    }
    else if (inputFormat == "Float 16 Bits")
    {
        // Half precision floating point format.
        parseFloat16();
    }
    else if (inputFormat == "Float 32 Bits")
    {
        // Evil bit manipulation here. If you know a better way, improve this,
        // please.
        uint32_t bits = parseBin();
        input = *reinterpret_cast<float*>(&bits);
    }
    else if (inputFormat == "Float 64 Bits")
    {
        // Evil bit manipulation here again. If you know a better way, improve
        // this, please.
        uint64_t bits = parseBin();
        input = *reinterpret_cast<double*>(&bits);
    }
    else if (inputFormat == "Ponto Fixo 8 Bits")
    {

    }
    else if (inputFormat == "Ponto Fixo 16 Bits")
    {

    }
    else if (inputFormat == "Ponto Fixo 32 Bits")
    {

    }
    else if (inputFormat == "Ponto Fixo 64 Bits")
    {

    }
}

uint64_t PointConversorDialog::parseBin()
{
    uint64_t bits = 0;
    QString inputText = ui->lineEditInput->text().trimmed();

    // TODO: check for errors.
    foreach (QChar digit, inputText) {
        bits <<= 1;
        bits |= digit.toLatin1() - '0';
    }

    return bits;
}

void PointConversorDialog::parseFloat16()
{
    // Parse a binary unsigned number.
    uint16_t bits = parseBin();
    // Get bits [10, 14] (starting to count from 0). Remember:
    // - 0x1F = 0001 1111
    uint64_t exponent = (bits >> 10) & 0x1F;

    // Test for maximum exponent.
    if (exponent == 0x1F)
    {
        // Maximum exponent, meaning NaN or Infinity.
        exponent = 0x7FF;
    }
    // Only do this if exponent is not minimum. If minimum, the number is
    // either zero or subnormal.
    else if (exponent != 0)
    {
        // Remember:
        // - 0x1F = 0001 1111
        // - 0x3FF = 0011 1111 1111
        // - 0x3FF is double's encoding for an exponent zero.
        // - 0xF is half's encoding for an exponent zero.
        exponent = 0x3FF - 0xF + exponent;
    }
    // Get the mantissa (the digits). Remember:
    // - 0x3FF = 0011 1111 1111
    // Get bits [0, 9] (starting to count from 0).
    uint64_t mantissa = bits & 0x3FF;
    // Shift mantisssa to fit it in double representation
    // (filling with zeroes to the right).
    mantissa = mantissa << 42;
    // Get the sign bit.
    uint64_t sign = bits >> 15;
    // Mount everything in an encoded double precision floating point number.
    uint64_t bitsDouble = sign << 63 | exponent << 52 | mantissa;
    input = *reinterpret_cast<double*>(&bitsDouble);
}
