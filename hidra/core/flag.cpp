#include "flag.h"

Flag::Flag(QString name)
{
    this->name = name;
    this->value = false;
    this->defaultValue = false;
}

Flag::Flag(QString name, bool defaultValue)
{
    this->name = name;
    this->value = defaultValue;
    this->defaultValue = defaultValue;
}

QString Flag::getName() const
{
    return name;
}

bool Flag::getValue() const
{
    return value;
}

void Flag::setValue(bool value)
{
    this->value = value;
}

void Flag::resetValue()
{
    value = defaultValue;
}
