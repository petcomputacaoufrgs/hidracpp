#include "flag.h"

Flag::Flag()
{

}

Flag::Flag(FlagCode flagCode, QString name)
{
    this->flagCode = flagCode;
    this->name = name;
    this->value = false;
    this->defaultValue = false;
}

Flag::Flag(FlagCode flagCode, QString name, bool defaultValue)
{
    this->flagCode = flagCode;
    this->name = name;
    this->value = defaultValue;
    this->defaultValue = defaultValue;
}

Flag::FlagCode Flag::getFlagCode() const
{
    return flagCode;
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
