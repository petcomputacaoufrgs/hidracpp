#include "register.h"

Register::Register()
{
}
int Register::getValue() const
{
    return value;
}

void Register::setValue(int value)
{
    value = value;
}
int Register::getNumOfBits() const
{
    return numOfBits;
}

void Register::setNumOfBits(int value)
{
    numOfBits = value;
}


