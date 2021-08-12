#include "baseconversor.h"
#include "invalidconversorinput.h"

BaseConversor::BaseConversor()
{
    bits = 0;
    sign = false;
}

uint64_t BaseConversor::mapInput(QChar character, uint64_t base)
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

QChar BaseConversor::mapOutput(uint64_t digit)
{
    char map[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (digit >= sizeof(map)) {
        throw InvalidConversorInput("Base inválida");
    }

    return map[digit];
}

uint64_t BaseConversor::maxValue(uint64_t base, uint64_t *width, uint64_t atLeast)
{
    uint64_t result = 0;
    uint64_t tmpResult = 0;
    uint64_t i = 0;

    while (i < *width || result < atLeast) {
        tmpResult = result * base;
        if (tmpResult < result) {
            throw InvalidConversorInput("Entrada é muito grande.");
        }
        result = tmpResult;
        tmpResult = result + (base - 1);
        if (tmpResult < result) {
            throw InvalidConversorInput("Entrada é muito grande.");
        }
        result = tmpResult;
        i++;
    }

    *width = i;
    return result;
}

// Input functions

void BaseConversor::decode(QString const &digitsQ, uint64_t base, uint64_t *width)
{
    uint64_t result;
    validateBase(base);

    bits = 0;
    *width = 0;

    for (QChar ch : digitsQ) {
        if (ch != ' ') {
            result = bits * base;
            if (result < bits) {
                throw InvalidConversorInput("Entrada é muito grande.");
            }
            bits = result;
            result = bits + mapInput(ch, base);
            if (result < bits) {
                throw InvalidConversorInput("Entrada é muito grande.");
            }
            bits = result;
            (*width)++;
        }
    }

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
}

QString BaseConversor::encode(uint64_t inputBits, uint64_t base, uint64_t width, QChar fill)
{
    QString output;
    uint64_t current = 0;

    validateBase(base);

    while (current < width || inputBits > 0) {
        output.append(mapOutput(inputBits % base));
        inputBits /= base;
        current++;
    }

    while (current < width) {
        output.append(fill);
        current++;
    }

    std::reverse(output.begin(), output.end());
    return output;
}

BaseConversor& BaseConversor::inputPositive(QString digitsQ, uint64_t base)
{
    uint64_t width;
    decode(digitsQ, base, &width);
    sign = false;
    return *this;
}

BaseConversor& BaseConversor::inputSignMagnitude(QString digitsQ, uint64_t base)
{
    uint64_t width;
    QString digitsTrimmed = digitsQ.trimmed();

    if (digitsTrimmed.length() == 0) {
        throw InvalidConversorInput("Sinal magnitude precisa de um dígito para sinal.");
    }

    switch (digitsTrimmed[0].unicode()) {
    case '0':
        sign = false;
        break;
    case '1':
        sign = true;
        break;
    default:
        throw InvalidConversorInput("Dígito inválido para a base");
        break;
    }

    digitsTrimmed.remove(0, 1);
    decode(digitsTrimmed, base, &width);
    return *this;
}

BaseConversor& BaseConversor::inputOnesComplement(QString digitsQ, uint64_t base)
{
    uint64_t width;
    uint64_t maxValue;
    decode(digitsQ, base, &width);

    maxValue = this->maxValue(base, &width, 0);

    sign = bits >= (maxValue / 2 + maxValue % 2);
    if (sign) {
        bits = maxValue - bits;
    }

    return *this;
}

BaseConversor& BaseConversor::inputTwosComplement(QString digitsQ, uint64_t base)
{
    uint64_t width;
    uint64_t maxValue;
    decode(digitsQ, base, &width);

    maxValue = this->maxValue(base, &width, 0);

    sign = bits > maxValue / 2;
    if (sign) {
        bits = maxValue - bits + 1;
    }
    return *this;
}

// Output functions

QString BaseConversor::outputPositive(uint64_t base, uint64_t width)
{
    if (sign) {
        throw InvalidConversorInput("Inteiro positivo não admite valores negativos.");
    }

    return this->encode(bits, base, width, '0');
}

QString BaseConversor::outputSignMagnitude(uint64_t base, uint64_t width)
{
    QString output = this->encode(bits, base, width, '0');
    output.prepend(sign ? '1' : '0');
    return output;
}

QString BaseConversor::outputOnesComplement(uint64_t base, uint64_t width)
{
    uint64_t outputWidth = width;
    uint64_t outputBits = bits;
    uint64_t maxValue, atLeast;
    QChar fill = '0';

    if (sign) {
        maxValue = this->maxValue(base, &outputWidth, outputBits);
        outputBits = maxValue - outputBits;
        fill = mapOutput(base - 1);
    } else {
        atLeast = outputBits * 2;
        if (atLeast < outputBits) {
            throw InvalidConversorInput("Entrada é muito grande.");
        }
        this->maxValue(base, &outputWidth, outputBits * 2);
    }

    return this->encode(outputBits, base, outputWidth, fill);
}


QString BaseConversor::outputTwosComplement(uint64_t base, uint64_t width)
{
    uint64_t outputWidth = width;
    uint64_t outputBits = bits;
    uint64_t maxValue, atLeast;
    QChar fill = '0';

    if (sign) {
        maxValue = this->maxValue(base, &outputWidth, outputBits);
        outputBits = maxValue - outputBits + 1;
        fill = mapOutput(base - 1);
    } else {
        atLeast = outputBits * 2;
        if (atLeast < outputBits) {
            throw InvalidConversorInput("Entrada é muito grande.");
        }
        this->maxValue(base, &outputWidth, outputBits * 2);
    }

    return this->encode(outputBits, base, outputWidth, fill);
}

void BaseConversor::validateBase(u_int64_t base)
{
    if (base < MIN_BASE) {
        throw InvalidConversorInput(QString("Base precisa ser no mínimo ") + QString::number(MIN_BASE));
    }

    if (base > MAX_BASE) {
        throw InvalidConversorInput(QString("Base precisa ser no máximo ") + QString::number(MAX_BASE));
    }
}
