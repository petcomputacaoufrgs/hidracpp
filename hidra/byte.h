#ifndef BYTE_H
#define BYTE_H

class Byte
{
public:
    Byte();

    unsigned char getValue() const;
    void setValue(unsigned char value);

private:
    unsigned char value;
};

#endif // BYTE_H
