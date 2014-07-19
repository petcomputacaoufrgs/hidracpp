#include "byte.h"

Byte::Byte()
{
    this->value = 0;
}

Byte::Byte(unsigned char value)
{
    this->value = value;
}
unsigned char Byte::getValue() const
{
    return value;
}

void Byte::setValue(unsigned char value)
{
    this->value = value;
}

