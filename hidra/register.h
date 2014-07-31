#ifndef REGISTER_H
#define REGISTER_H

class Register
{
public:
    Register();

    int getValue() const;
    void setValue(int value);
    void incrementValue();

    int getNumOfBits() const;
    void setNumOfBits(int value);

private:
    int value;
    int numOfBits;
};

#endif // REGISTER_H
