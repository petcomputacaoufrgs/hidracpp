#include "baseconversor.h"
#include "invalidconversorinput.h"

BaseConversor::BaseConversor():
    bits(0),
    sign(false)
{
}

uint64_t BaseConversor::mapInput(QChar character, uint64_t base) const
{
    int codepoint = character.unicode();
    uint64_t digit = base;

    if (codepoint >= '0' && codepoint <= '9') {
        digit = codepoint - '0';
    } else if (codepoint >= 'a' && codepoint <= 'z') {
        digit = codepoint - 'a' + 10;
    } else if (codepoint >= 'A' && codepoint <= 'Z') {
        digit = codepoint - 'A' + 10;
    }

    if (digit >= base) {
        throw InvalidConversorInput("Dígito inválido para a base");
    }

    return digit;
}

QChar BaseConversor::mapOutput(uint64_t digit) const
{
    if (digit < 10) {
        return (char) digit + '0';
    }
    if (digit < MAX_BASE) {
        return (char) digit - 10 + 'A';
    }

    throw InvalidConversorInput("Base inválida");
}

uint64_t BaseConversor::maxValue(uint64_t base, uint64_t *width, uint64_t atLeast) const
{
    uint64_t result = 0;
    uint64_t tmpResult = 0;
    uint64_t i = 0;

    while (i < *width || result < atLeast) {
        tmpResult = result * base;
        if (tmpResult < result) {
            // Overflow check for result *= base.
            throw InvalidConversorInput("Entrada é muito grande.");
        }
        result = tmpResult;
        tmpResult = result + (base - 1);
        if (tmpResult < result) {
            // Overflow check for result + base - 1.
            throw InvalidConversorInput("Entrada é muito grande.");
        }
        result = tmpResult;
        i++;
    }

    // Stores the actual width of the computed number, given that atLeast might
    // be bigger than base**width.
    *width = i;
    return result;
}

// Input functions

uint64_t BaseConversor::decode(QString const &digitsQ, uint64_t base, uint64_t *width) const
{
    uint64_t result = 0;
    uint64_t numberBits = 0;
    validateBase(base);

    *width = 0;

    for (QChar ch : digitsQ) {
        // Allow spaces.
        if (ch != ' ') {
            result = numberBits * base;
            if (result < numberBits) {
                // Overflow check for bits *= base.
                throw InvalidConversorInput("Entrada é muito grande.");
            }
            numberBits = result;
            result = numberBits + mapInput(ch, base);
            if (result < numberBits) {
                // Overflow check for bits += digit.
                throw InvalidConversorInput("Entrada é muito grande.");
            }
            numberBits = result;
            (*width)++;
        }
    }

    // Checks for if the given number width is valid.
    if (*width > MAX_WIDTH) {
        throw InvalidConversorInput(
                    QString("Largura do inteiro não pode ser maior que ")
                    + QString::number(MAX_WIDTH));
    }
    if (*width < MIN_WIDTH) {
        throw InvalidConversorInput(
                    QString("Largura do inteiro não pode ser menor que ")
                    + QString::number(MIN_WIDTH));
    }

    return numberBits;
}

QString BaseConversor::encode(uint64_t inputBits, uint64_t base, uint64_t width, QChar fill) const
{
    QString output;
    uint64_t current = 0;
    validateBase(base);

    // Computes each present digit.
    while (current < width || inputBits > 0) {
        output.append(mapOutput(inputBits % base));
        inputBits /= base;
        current++;
    }

    // If not large enough, adds a fill character.
    while (current < width) {
        output.append(fill);
        current++;
    }

    // Most significant digits are computed first and are placed in the
    // beginning, but they should be at the end, then reverse the string.
    std::reverse(output.begin(), output.end());
    return output;
}

// Input functions

BaseConversor& BaseConversor::inputPositive(QString const &digitsQ, uint64_t base)
{
    uint64_t width;
    // Very simple. Just decoding.
    this->bits = decode(digitsQ, base, &width);
    // Positive, not negative.
    this->sign = false;
    return *this;
}

BaseConversor& BaseConversor::inputSignMagnitude(QString const &digitsQ, uint64_t base)
{
    uint64_t width;
    // Trimming because spaces don't matter.
    QString digitsTrimmed = digitsQ.trimmed();

    // We need at least the sign.
    if (digitsTrimmed.length() == 0) {
        throw InvalidConversorInput("Sinal magnitude precisa de um dígito para sinal.");
    }

    // Only 0 or 1 is valid for the sign.
    switch (digitsTrimmed[0].unicode()) {
    case '0':
        this->sign = false;
        break;
    case '1':
        this->sign = true;
        break;
    default:
        throw InvalidConversorInput("Dígito inválido para a base");
        break;
    }

    // Removes the sign (position 0, 1 char).
    digitsTrimmed.remove(0, 1);
    // Now we can simply decode the remaining digits.
    this->bits = decode(digitsTrimmed, base, &width);
    return *this;
}

BaseConversor& BaseConversor::inputOnesComplement(QString const &digitsQ, uint64_t base)
{
    uint64_t width = 0;
    uint64_t maxValue = 0;
    uint64_t numberBits = decode(digitsQ, base, &width);

    // Computes B**width - 1 (the complement).
    maxValue = this->maxValue(base, &width, 0);

    // The actual formula is (bits >= (maxValue + 1) / 2).
    //
    // floor(B**width / 2) - 1 is positive
    //
    // floor(B**width / 2) is negative
    this->sign = numberBits >= (maxValue / 2 + maxValue % 2);
    if (this->sign) {
        numberBits = maxValue - numberBits;
    }

    this->bits = numberBits;
    return *this;
}

BaseConversor& BaseConversor::inputTwosComplement(QString const &digitsQ, uint64_t base)
{
    uint64_t width = 0;
    uint64_t maxValue = 0;
    uint64_t numberBits = decode(digitsQ, base, &width);

    // Computes B**width - 1  (the complement - 1).
    maxValue = this->maxValue(base, &width, 0);

    // floor(B**width / 2) is positive
    //
    // floor(B**width / 2) + 1 is negative
    this->sign = numberBits > maxValue / 2;
    if (this->sign) {
        numberBits = maxValue - numberBits + 1;
    }

    this->bits = numberBits;
    return *this;
}

// Output functions

QString BaseConversor::outputPositive(uint64_t base, uint64_t width) const
{
    // Very simple. Just checks for negative.
    if (this->sign) {
        throw InvalidConversorInput("Inteiro positivo não admite valores negativos.");
    }

    // Then simply encodes the given number with the regular to-string algorithm.
    return this->encode(this->bits, base, width, '0');
}

QString BaseConversor::outputSignMagnitude(uint64_t base, uint64_t width) const
{
    // Simply encodes the given number with the regular to-string algorithm.
    QString output = this->encode(this->bits, base, width, '0');
    // Then prepend the sign, which does not count for the width.
    output.prepend(this->sign ? '1' : '0');
    return output;
}

QString BaseConversor::outputOnesComplement(uint64_t base, uint64_t width) const
{
    uint64_t outputWidth = width;
    uint64_t outputBits = this->bits;
    // Dummy value.
    uint64_t maxValue = 0;
    uint64_t atLeast = 0;
    // Only if positive sign.
    QChar fill = '0';

    if (this->sign) {
        // Computes B**width - 1 (the complement). Width must be at least
        // enough to store the complement.
        maxValue = this->maxValue(base, &outputWidth, outputBits);
        // Computes the complement of the stored bits.
        outputBits = maxValue - outputBits;
        // Fill is the highest digit if negative.
        fill = mapOutput(base - 1);
    } else {
        // Width must be at least enough to store the complement.
        atLeast = outputBits * 2;
        if (atLeast < outputBits) {
            // A positive number might be too large for B-1's complement.
            // Check overflow.
            throw InvalidConversorInput("Entrada é muito grande.");
        }
        // Computes the width.
        this->maxValue(base, &outputWidth, outputBits * 2);
    }

    // If no error was thrown, just convert the complemented number to string.
    return this->encode(outputBits, base, outputWidth, fill);
}


QString BaseConversor::outputTwosComplement(uint64_t base, uint64_t width) const
{
    uint64_t outputWidth = width;
    uint64_t outputBits = this->bits;
    // Dummy value.
    uint64_t maxValue = 0;
    uint64_t atLeast = 0;
    // Only if positive sign.
    QChar fill = '0';

    if (this->sign) {
        // Computes B**width - 1 (the complement - 1). Width must be at least
        // enough to store the complement.
        maxValue = this->maxValue(base, &outputWidth, outputBits);
        // Computes the complement of the stored bits.
        outputBits = maxValue - outputBits + 1;
        // Fill is the highest digit if negative.
        fill = mapOutput(base - 1);
    } else {
        // Width must be at least enough to store the complement.
        atLeast = outputBits * 2;
        if (atLeast < outputBits) {
            // A positive number might be too large for B's complement.
            // Check overflow.
            throw InvalidConversorInput("Entrada é muito grande.");
        }
        // Computes the width.
        this->maxValue(base, &outputWidth, outputBits * 2);
    }

    // If no error was thrown, just convert the complemented number to string.
    return this->encode(outputBits, base, outputWidth, fill);
}

void BaseConversor::validateBase(uint64_t base) const
{
    if (base < MIN_BASE) {
        throw InvalidConversorInput(QString("Base precisa ser no mínimo ") + QString::number(MIN_BASE));
    }

    if (base > MAX_BASE) {
        throw InvalidConversorInput(QString("Base precisa ser no máximo ") + QString::number(MAX_BASE));
    }
}
