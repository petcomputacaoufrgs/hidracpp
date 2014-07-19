#include "bit.h"

Bit::Bit()
{
    this->value = false;
}

bool Bit::getValue() const
{
    return value;
}

void Bit::setValue(bool value)
{
    this->value = value;
}

