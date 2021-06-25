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
    long long unsigned getBits();
    void setBits(long long unsigned input);
    bool getNegativeSignal();
    int mapInput(char i);
    char mapOutput(int i);
    long long unsigned getDec(QString digits, int base);
    long long unsigned getComplement(QString digits, int base);

    // Input functions
    BaseConversor& inputPositive(QString digits, int base);
    BaseConversor& inputSignMagnitude(QString digits, int base);
    BaseConversor& inputOnesComplement(QString digits, int base);
    BaseConversor& inputTwosComplement(QString digits, int base);

    // Output functions
    QString outputPositive(int base);
    QString outputSignMagnitude(int base);
    QString outputOnesComplement(int base);
    QString outputTwosComplement(int base);

    // Validation functions
    QString validate(int baseIn, int baseOut, QString digits);
    QString validateSignMagnitude(int baseIn, int baseOut, QString digits);
    QString validateOnesComplement(int baseIn, int baseOut, QString digits);
    QString validateTwosComplement(int baseIn, int baseOut, QString digits);

private:
    long long unsigned bits;
    bool negativeSignal;
    bool signalMagnitude;
};

#endif // BASECONVERSOR_H
