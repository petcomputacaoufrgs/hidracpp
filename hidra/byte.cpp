#include "byte.h"

Byte::Byte()
{
    this->value = 0;
}

Byte::Byte(int value)
{
    this->value = (unsigned char)(value % 256);
}

int Byte::getValue() const
{
    return value;
}

void Byte::setValue(int value)
{
    this->value = (unsigned char)(value % 256);
}

