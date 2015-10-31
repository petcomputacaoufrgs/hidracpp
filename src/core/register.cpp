#include "register.h"

#include "byte.h"

Register::Register(QString name, QString bitPattern, int numOfBits, bool isData)
{
    this->name = name;
    this->bitPattern = bitPattern;
    this->numOfBits = numOfBits;
    this->isDataFlag = isData;

    this->value = 0;
    this->valueMask = (1 << numOfBits) - 1; // 0xFF for 8-bit registers
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

int Register::getSignedValue() const
{
    int signBitMask = 1 << (numOfBits - 1); // 0x80 for 8-bit registers

    if ((value & signBitMask) != 0) // If signed
        return value - (1 << numOfBits); // value - 256 for 8-bit registers
    else
        return value;
}

void Register::setValue(int value)
{
    this->value = value & valueMask;
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

bool Register::isData() const
{
    return isDataFlag;
}
