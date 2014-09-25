#include "register.h"

Register::Register(QString name, int numOfBits)
{
    this->name = name;
    this->value = 0;
    this->numOfBits = numOfBits;
}

QString Register::getName() const
{
    return name;
}

int Register::getValue() const
{
    return value;
}

void Register::setValue(int value)
{
    this->value = value  % (1 << numOfBits);
}

void Register::incrementValue()
{
    this->value = (value + 1)  % (1 << numOfBits);
}

int Register::getNumOfBits() const
{
    return numOfBits;
}

void Register::setNumOfBits(int value)
{
    this->numOfBits = value;
}


