#include "instruction.h"

Instruction::Instruction()
{
}
QString Instruction::getMnemonic() const
{
    return mnemonic;
}

void Instruction::setMnemonic(const QString &value)
{
    mnemonic = value;
}
int Instruction::getValue() const
{
    return value;
}

void Instruction::setValue(int value)
{
    value = value;
}
int Instruction::getNumberOfArguments() const
{
    return numberOfArguments;
}

void Instruction::setNumberOfArguments(int value)
{
    numberOfArguments = value;
}



