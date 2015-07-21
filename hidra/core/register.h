#ifndef REGISTER_H
#define REGISTER_H

#include <QDebug>
#include <QString>
#include <cmath>

class Register
{
public:
    Register(QString name, QString bitPattern, int numOfBits);

    QString getName() const;
    QString getBitPattern() const;
    int getBitCode() const; // -1 if no bit code (not accessible)
    int getValue() const;
    void setValue(int value);
    void incrementValue();

    int getNumOfBits() const;

private:
    QString name;
    QString bitPattern; // Empty string if not directly accessible

    int value;
    int numOfBits;
};

#endif // REGISTER_H
