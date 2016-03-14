#include "byte.h"



//////////////////////////////////////////////////
// Conversions
//////////////////////////////////////////////////

int Conversion::stringToValue(QString valueString)
{
    int value = 0;

    for (int i=0; i<8; i++)
        value += (valueString.at(7-i) == '1') ? (1 << i) : 0; // Don't care bits evaluate to zero

    return value;
}

QString Conversion::valueToString(int value)
{
    QString str;

    for (int i=7; i>=0; i--)
        str += ((value & (1 << i)) != 0) ? '1' : '0';

    return str;
}



//////////////////////////////////////////////////
// Byte class
//////////////////////////////////////////////////

Byte::Byte()
{
    this->value = 0;
}

Byte::Byte(int value)
{
    this->value = (unsigned char)(value & 0xFF);
}

int Byte::getValue() const
{
    return value;
}

void Byte::setValue(int value)
{
    this->value = (unsigned char)(value & 0xFF);
}
