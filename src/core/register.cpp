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
        return NO_BIT_CODE;
    else
        return Conversion::stringToValue(bitPattern);
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

bool Register::matchByte(int byte)
{
    QRegExp bitPatternRegExp(bitPattern);
    return bitPatternRegExp.exactMatch(Conversion::valueToString(byte));
}

int Register::getNumOfBits() const
{
    return numOfBits;
}
