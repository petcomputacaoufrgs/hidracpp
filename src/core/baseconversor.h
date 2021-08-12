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

    // Input functions
    BaseConversor& inputPositive(QString digits, uint64_t base);
    BaseConversor& inputSignMagnitude(QString digits, uint64_t base);
    BaseConversor& inputOnesComplement(QString digits, uint64_t base);
    BaseConversor& inputTwosComplement(QString digits, uint64_t base);

    // Output functions
    QString outputPositive(uint64_t base, uint64_t width);
    QString outputSignMagnitude(uint64_t base, uint64_t width);
    QString outputOnesComplement(uint64_t base, uint64_t width);
    QString outputTwosComplement(uint64_t base, uint64_t width);

private:  
    uint64_t bits;
    bool sign;

    uint64_t mapInput(QChar character, uint64_t base);
    QChar mapOutput(uint64_t digit);

    uint64_t maxValue(uint64_t base, uint64_t *width, uint64_t atLeast);

    void decode(QString const &digitsQ, uint64_t base, uint64_t *width);
    QString encode(uint64_t bits, uint64_t base, uint64_t width, QChar fill);

    static const int MAX_WIDTH = 64;
    static const int MIN_WIDTH = 1;
};

#endif // BASECONVERSOR_H
