#ifndef BYTE_H
#define BYTE_H

#include <QString>

class Byte
{
public:
    Byte();
    Byte(int value);
    Byte(QString valueString);

    QString toString() const;

    int getValue() const;
    void setValue(int value);

private:
    unsigned char value;
};

#endif // BYTE_H
