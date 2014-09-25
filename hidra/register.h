#ifndef REGISTER_H
#define REGISTER_H

#include <QDebug>
#include <QString>
#include <cmath>

class Register
{
public:
    Register(QString name, int numOfBits);

    QString getName() const;
    int getValue() const;
    void setValue(int value);
    void incrementValue();

    int getNumOfBits() const;
    void setNumOfBits(int value);

private:
    QString name;

    int value;
    int numOfBits;
};

#endif // REGISTER_H
