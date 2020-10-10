#ifndef BASECONVERSOR_H
#define BASECONVERSOR_H

#include <QString>
#include <iostream>
#include <math.h>
#include <string>

class BaseConversor
{
public:
    BaseConversor();
    BaseConversor& input(QString digits, int base);
    long long unsigned getBits();
    QString output(int base);
    QString inputValidation(int baseIn, int baseOut, QString digits);
    int mapInput(char i);

private:
    long long unsigned bits;

};

#endif // BASECONVERSOR_H
