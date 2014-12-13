#ifndef BYTE_H
#define BYTE_H

class Byte
{
public:
    Byte();
    Byte(int);

    int getValue() const;
    void setValue(int value);

private:
    unsigned char value;
};

#endif // BYTE_H
