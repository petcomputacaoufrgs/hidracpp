#ifndef BYTE_H
#define BYTE_H

class Byte
{
public:
    Byte();
    Byte(unsigned char);

    unsigned char getValue() const;
    void setValue(unsigned char value);

private:
    unsigned char value;
};

#endif // BYTE_H
