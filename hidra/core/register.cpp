#include "register.h"

#include "byte.h"

Register::Register(QString name, QString bitPattern, int numOfBits)
{
    this->name = name;
    this->bitPattern = bitPattern;

    this->numOfBits = numOfBits;
    this->value = 0;
}

QString Register::getName() const
{
    return name;
}

QString Register::getBitPattern() const
{
    return bitPattern;
}

int Register::getBitCode() const
{
    if (bitPattern == "")
        return -1; // No bit code
    else
        return Byte(bitPattern).getValue();
}

int Register::getValue() const
{
    return value;
}

void Register::setValue(int value)
{
    this->value = value & ((1 << numOfBits) - 1);
}

void Register::incrementValue()
{
    setValue(value + 1);
}

int Register::getNumOfBits() const
{
    return numOfBits;
}
