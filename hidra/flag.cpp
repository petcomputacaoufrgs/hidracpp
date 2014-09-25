#include "flag.h"

Flag::Flag(QString name)
{
    this->name = name;
    this->value = false;
}

Flag::Flag(QString name, bool defaultValue)
{
    this->name = name;
    this->value = defaultValue;
}

bool Flag::getValue() const
{
    return value;
}

void Flag::setValue(bool value)
{
    this->value = value;
}

