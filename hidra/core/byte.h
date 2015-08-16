#ifndef BYTE_H
#define BYTE_H

#include <QString>

namespace Conversion
{
    int stringToValue(QString valueString);
    QString valueToString(int value);
}

class Byte
{
public:
    Byte();
    Byte(int value);

    int getValue() const;
    void setValue(int value);

private:
    unsigned char value;
};

#endif // BYTE_H
