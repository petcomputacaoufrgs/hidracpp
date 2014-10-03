#include "instruction.h"

Instruction::Instruction()
{
}

Instruction::Instruction(QString mnemonic, int value, int arguments, int size)
{
    this->mnemonic = mnemonic;
    this->value = value;
    this->numberOfArguments = arguments;
    this->size = size;
}

QString Instruction::getMnemonic() const
{
    return mnemonic;
}

void Instruction::setMnemonic(const QString &value)
{
    this->mnemonic = value;
}
int Instruction::getValue() const
{
    return value;
}

void Instruction::setValue(int value)
{
    this->value = value;
}

int Instruction::getNumberOfArguments() const
{
    return numberOfArguments;
}

void Instruction::setNumberOfArguments(int value)
{
    this->numberOfArguments = value;
}

int Instruction::getSize() const
{
    return size;
}

void Instruction::setSize(int size)
{
    this->size = size;
}
