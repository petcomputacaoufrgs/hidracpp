#include "register.h"

Register::Register()
{
    this->value = 0;
    this->numOfBits = 8;
}

int Register::getValue() const
{
    return value;
}

void Register::setValue(int value)
{
    this->value = value;
}
int Register::getNumOfBits() const
{
    return numOfBits;
}

void Register::setNumOfBits(int value)
{
    this->numOfBits = value;
}


