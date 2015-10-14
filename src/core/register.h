#ifndef REGISTER_H
#define REGISTER_H

#include <QDebug>
#include <QString>
#include <cmath>

class Register
{
public:
    static const int NO_BIT_CODE = -1;

    Register(QString name, QString bitPattern, int numOfBits);

    QString getName() const;
    QString getBitPattern() const;
    int getBitCode() const; // Returns NO_BIT_CODE if register isn't directly accessible
    int getValue() const;
    void setValue(int value);
    void incrementValue();
    bool matchByte(int byte);

    int getNumOfBits() const;

private:
    QString name;
    QString bitPattern; // Empty string if not directly accessible

    int value;
    int numOfBits;
    int valueMask;
};

#endif // REGISTER_H
