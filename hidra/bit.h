#ifndef BIT_H
#define BIT_H

class Bit
{
public:
    Bit();

    bool getValue() const;
    void setValue(bool value);

private:
    bool value;
};

#endif // BIT_H
