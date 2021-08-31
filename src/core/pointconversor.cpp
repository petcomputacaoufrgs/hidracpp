#include "pointconversor.h"
#include "invalidconversorinput.h"

// TODO: implement human notation WIHTOUT double float (or any float).
// With float, there are some 64 bits numbers which cannot be represented
// because double float only really has 52 bits for mantissa.

PointConversor::PointConversor():
    bits(0),
    exponent(0),
    sign(false),
    normality(PointConversor::NORMAL)
{
}

// Raw input functions

PointConversor& PointConversor::inputHalfFloatRaw(uint16_t number)
{
    return this->inputGenericFloatRaw(number, 10, 5);
}

PointConversor& PointConversor::inputSingleFloatRaw(float number)
{
    union { float number; uint32_t bits; } bitCast { number };
    return this->inputGenericFloatRaw(bitCast.bits, 23, 8);
}

PointConversor& PointConversor::inputDoubleFloatRaw(double number)
{
    union { double number; uint64_t bits; } bitCast { number };
    return this->inputGenericFloatRaw(bitCast.bits, 52, 11);
}

PointConversor& PointConversor::inputFixed8Raw(uint8_t number, int16_t pointPos, Signedness signedness)
{
    return this->inputGenericFixedRaw(number, 8, pointPos, signedness);
}

PointConversor& PointConversor::inputFixed16Raw(uint16_t number, int16_t pointPos, Signedness signedness)
{
    return this->inputGenericFixedRaw(number, 16, pointPos, signedness);
}

PointConversor& PointConversor::inputFixed32Raw(uint32_t number, int16_t pointPos, Signedness signedness)
{
    return this->inputGenericFixedRaw(number, 32, pointPos, signedness);
}

PointConversor& PointConversor::inputFixed64Raw(uint64_t number, int16_t pointPos, Signedness signedness)
{
    return this->inputGenericFixedRaw(number, 64, pointPos, signedness);
}

// String input functions

PointConversor& PointConversor::inputHalfFloat(QString const& number)
{
    return this->inputGenericFloat(number, 10, 5);
}

PointConversor& PointConversor::inputSingleFloat(QString const& number)
{
    return this->inputGenericFloat(number, 23, 8);
}

PointConversor& PointConversor::inputDoubleFloat(QString const& number)
{
    return this->inputGenericFloat(number, 52, 11);
}

PointConversor& PointConversor::inputFixed8(QString const& number, Signedness signedness)
{
    return this->inputGenericFixed(number, 8, signedness);
}

PointConversor& PointConversor::inputFixed16(QString const& number, Signedness signedness)
{
    return this->inputGenericFixed(number, 16, signedness);
}

PointConversor& PointConversor::inputFixed32(QString const& number, Signedness signedness)
{
    return this->inputGenericFixed(number, 32, signedness);
}

PointConversor& PointConversor::inputFixed64(QString const& number, Signedness signedness)
{
    return this->inputGenericFixed(number, 64, signedness);
}

PointConversor& PointConversor::inputHumanNotation(QString const& number)
{
    // For now, we are decoding human notation via double float. This means we
    // are losing some precision, since double has only 52 bits of significant
    // digits, and our class stores 64 bits of significant digits, but that's
    // perhaps acceptable.
    bool ok = true;
    double asDoubleFloat = number.toDouble(&ok);
    if (!ok) {
        throw InvalidConversorInput("Número não está na notação humana adequada");
    }
    return this->inputDoubleFloatRaw(asDoubleFloat);
}

// Raw output functions

uint16_t PointConversor::outputHalfFloatRaw() const
{
    return this->outputGenericFloatRaw(10, 5);
}

float PointConversor::outputSingleFloatRaw() const
{
    uint32_t number = this->outputGenericFloatRaw(23, 8);
    union { uint32_t bits; float number; } bitCast { number };
    return bitCast.number;
}

double PointConversor::outputDoubleFloatRaw() const
{
    uint64_t number = this->outputGenericFloatRaw(52, 11);
    union { uint64_t bits; double number; } bitCast { number };
    return bitCast.number;
}

uint8_t PointConversor::outputFixed8Raw(int16_t pointPos, Signedness signedness) const
{
    return this->outputGenericFixedRaw(8, pointPos, signedness);
}

uint16_t PointConversor::outputFixed16Raw(int16_t pointPos, Signedness signedness) const
{
    return this->outputGenericFixedRaw(16, pointPos, signedness);
}

uint32_t PointConversor::outputFixed32Raw(int16_t pointPos, Signedness signedness) const
{
    return this->outputGenericFixedRaw(32, pointPos, signedness);
}

uint64_t PointConversor::outputFixed64Raw(int16_t pointPos, Signedness signedness) const
{
    return this->outputGenericFixedRaw(64, pointPos, signedness);
}

QString PointConversor::outputHalfFloat() const
{
    return this->outputGenericFloat(10, 5);
}

QString PointConversor::outputSingleFloat() const
{
    return this->outputGenericFloat(23, 8);
}

QString PointConversor::outputDoubleFloat() const
{
    return this->outputGenericFloat(52, 11);
}

// String output function

QString PointConversor::outputFixed8(int16_t pointPos, Signedness signedness) const
{
    return this->outputGenericFixed(8, pointPos, signedness);
}

QString PointConversor::outputFixed16(int16_t pointPos, Signedness signedness) const
{
    return this->outputGenericFixed(16, pointPos, signedness);
}

QString PointConversor::outputFixed32(int16_t pointPos, Signedness signedness) const
{
    return this->outputGenericFixed(32, pointPos, signedness);
}

QString PointConversor::outputFixed64(int16_t pointPos, Signedness signedness) const
{
    return this->outputGenericFixed(64, pointPos, signedness);
}

QString PointConversor::outputHumanNotation() const
{
    double asDoubleFloat = this->outputDoubleFloatRaw();
    QString rendered = QString::number(asDoubleFloat, 'f', 50);

    if (rendered.contains('.')) {
        int trailingPos = rendered.length() - 1;
        while (trailingPos > 2 && rendered[trailingPos] == '0') {
            trailingPos--;
        }
        rendered.truncate(trailingPos + 1);
    }

    return rendered;
}

// Generic input functions

PointConversor& PointConversor::inputGenericFloatRaw(uint64_t number, uint16_t mantissaSize, uint16_t exponentSize)
{
    uint64_t mantissaMask = 0;
    int16_t exponentMask = 0;
    uint64_t finalBit = 0;

    validateFloatSpec(mantissaSize, exponentSize);

    // Initialize here because mantissa size or exponent size might be invalid.
    mantissaMask = ((uint64_t) 1 << mantissaSize) - 1;
    exponentMask = ((uint64_t) 1 << exponentSize) - 1;
    finalBit = ((uint64_t) 1 << mantissaSize);

    // Sign is the last bit.
    this->sign = number >> (exponentSize + mantissaSize);
    // We store the mantissa bits (lower bits) as the significant bits.
    this->bits = number & mantissaMask;
    // Exponent is between the mantissa and the sign.
    this->exponent = (number >> mantissaSize) & exponentMask;
    if (this->exponent != exponentMask) {
        // Exponent being `< maximum` exponent means it is normal.
        if (this->exponent == 0) {
            // But if the exponent is zero, it is actually subnormal.
            this->exponent++;
        } else {
            // Otherwise, `0 < exponent < max` means it is really normal.
            //
            // There is an extra high bit, because a normal number is in this
            // format:
            //
            // 1.xxxxxxx... * 2**e
            //
            // And this "1" is not stored, only the xxxxxx...
            //
            // So, we need to introduce it.
            this->bits |= finalBit;
        }
        this->normality = PointConversor::NORMAL;
        // The exponent uses an encoding which we need to translate to C++'s signed numbers.
        this->exponent -= mantissaSize + (exponentMask >> 1);
    } else if (this->bits == 0) {
        // exponent == MAX and bits are zeroed, therefore, Infinity.
        this->normality = PointConversor::INFINITY_NAN;
    } else {
        // exponent == MAX and bits not zeroed, therefore, regular NAN.
        this->normality = PointConversor::NOT_A_NUMBER;
    }

    return *this;
}

PointConversor& PointConversor::inputGenericFixedRaw(uint64_t number, int16_t width, int16_t pointPos, Signedness signedness)
{
    uint64_t signMask = 0;
    uint64_t extendMask = 0;

    validateFixedSpec(width, pointPos, signedness);

    // Initialize here because width might be invalid.
    //
    // Last bit.
    signMask = (uint64_t) 1 << (width - 1);
    // Cut off extension mask, 0000....011111...1111
    extendMask = ~((uint64_t) 0) >> (MAX_WIDTH - width);

    // Fixed point is always normal.
    this->normality = PointConversor::NORMAL;

    switch (signedness) {
    case UNSIGNED:
        this->sign = false;
        break;
    case TWOS_COMPL:
        this->sign = (number & signMask) != 0;
        break;
    }

    // Bits b with point p means the number is b * 2**(-p)
    this->exponent = -pointPos;

    this->bits = number;
    if (this->sign) {
        // Complementing negativeness.
        this->bits = (~this->bits + 1) & extendMask;
    }

    return *this;
}


PointConversor& PointConversor::inputGenericFloat(QString const &number, uint16_t mantissaSize, uint16_t exponentSize)
{
    uint64_t numBits = 0;
    bool hasChars = false;
    int count = 0;

    for (QChar numChar : number) {
        if (numChar == '0' || numChar == '1') {
            numBits <<= 1;
            // By now we know it is zero or one.
            numBits |= numChar.digitValue();
            if (numBits != 0) {
                // Only start to count when bits not zero because leading zeros are ignored.
                count++;
            }
            if (count > mantissaSize + exponentSize + 1) {
                // Note that count ignores leading zeros.
                throw InvalidConversorInput("Entrada é muito grande");
            }
            hasChars = true;
        } else if (numChar != ' ') {
            // Space is ignored, but other chars are an error!
            throw InvalidConversorInput("Entrada precisa conter apenas 0 ou 1");
        }
    }

    if (!hasChars) {
        throw InvalidConversorInput("Entrada não pode ser vazia");
    }

    return this->inputGenericFloatRaw(numBits, mantissaSize, exponentSize);
}

PointConversor& PointConversor::inputGenericFixed(QString const &number, int16_t width, Signedness signedness)
{
    uint64_t numBits = 0;
    bool hasChars = false;
    bool pointFound = false;
    int16_t pointPos = 0;
    int count = 0;

    for (QChar numChar : number) {
        if (numChar == '0' || numChar == '1') {
            numBits <<= 1;
            // By now we know it is zero or one.
            numBits |= numChar.digitValue();
            if (numBits != 0 || pointFound) {
                // Only start to count when bits not zero because leading zeros are ignored.
                //
                // However, once a point has been found, we cannot ignore anymore.
                count++;
            }
            if (count > width && numChar != '0') {
                // Note that count ignores leading zeros (before point!).
                throw InvalidConversorInput("Entrada é muito grande");
            }
            hasChars = true;
        } else if (numChar == '.') {
            if (pointFound) {
                throw InvalidConversorInput("Entrada pode conter apenas um ponto");
            }
            pointFound = true;
            // Saves point position, because count still has to be incremented for overflow checks,
            // and for knowing the total width.
            pointPos = count;
        } else if (numChar != ' ') {
            // Space is ignored, but other chars are an error!
            throw InvalidConversorInput("Entrada precisa conter apenas 0 ou 1 (ou ponto)");
        }
    }

    if (!hasChars) {
        throw InvalidConversorInput("Entrada não pode ser vazia");
    }
    if (!pointFound) {
        throw InvalidConversorInput("O número em ponto fixo deve conter um ponto.");
    }
    if (pointPos > width || count - pointPos > width) {
        throw InvalidConversorInput("O ponto não pode estar fora dos limites.");
    }

    // Actual point position is reversed because humans usually write numbers in big-endian:
    // complement it with the found width to reverse it back..
    return this->inputGenericFixedRaw(numBits, width, count - pointPos, signedness);
}

// Generic output functions

uint64_t PointConversor::outputGenericFloatRaw(uint16_t mantissaSize, uint16_t exponentSize) const
{
    int16_t numExponent = 0;
    uint64_t numBits = 0;
    uint64_t number = 0;

    uint64_t mantissaMask = 0;
    int16_t exponentMask = 0;
    uint64_t finalBit = 0;
    bool subnormal = false;
    uint64_t roundRight = 0;

    // Initialize here because mantissa size or exponent size might be invalid.
    validateFloatSpec(mantissaSize, exponentSize);

    mantissaMask = ((uint64_t) 1 << mantissaSize) - 1;
    exponentMask = ((uint64_t) 1 << exponentSize) - 1;
    finalBit = (uint64_t) 1 << mantissaSize;

    switch (this->normality) {
    case PointConversor::NORMAL:
        // Converts the C++'s signed integer representation of the exponent into float's one.
        numExponent = this->exponent + (exponentMask >> 1) + mantissaSize;
        numBits = this->bits;

        // Try to normalize the number if too small. Remember "really" normal floats have an implicit bit,
        // and that the significant bits they store are `xxxxx...` in the number `1.xxxxx... * 2**e`.
        // This makes that 1 be in the correct position.
        while (numBits < finalBit && numBits != 0) {
            numBits <<= 1;
            // Remember making an equivalent exponent.
            numExponent -= 1;
        }

        // Same as the loop above, but in this case the number is too big.
        while (numBits >= (finalBit << 1)) {
            // If the last eliminated bit was 1, we might round the number later.
            roundRight = numBits & 1;
            numBits >>= 1;
            // Remember making an equivalent exponent.
            numExponent += 1;
        }

        if (numExponent <= 0) {
            // In case the exponent is below zero, this will be subnormal...
            while (numExponent < 0) {
                numBits >>= 1;
                numExponent += 1;
            }
            roundRight = numBits & 1;
            // Subnormal numbers have an extra step on the exponent, so we must
            // get rid off an extra bit to compensate.
            numBits >>= 1;
        }

        // If it does not have final bit, it is subnormal. Important for below.
        subnormal = (numBits & finalBit) == 0;
        // Getting rid of that implicit "1".
        numBits &= ~finalBit;
        // Potentially rounding in the right.
        numBits += roundRight;

        // Tries to fit the exponent. Might fail and end up with infinity below.
        while (numExponent > exponentMask && numBits < (finalBit >> 1)) {
            numBits <<= 1;
            numExponent -= 1;
        }

        if (subnormal && numBits == 0) {
            // No final bit => subnormal.
            numExponent = 0;
        } else if (numExponent >= exponentMask) {
            // Exponent too big? Then infinity.
            numExponent = exponentMask;
            numBits = 0;
        }
        break;
    case PointConversor::INFINITY_NAN:
        numExponent = exponentMask;
        numBits = 0;
        break;
    case PointConversor::NOT_A_NUMBER:
        numExponent = exponentMask;
        // Could be any non-zero value.
        numBits = 1;
        break;
    }

    number = numBits & mantissaMask;
    number |= (uint64_t) (numExponent & exponentMask) << mantissaSize;
    number |= (uint64_t) this->sign << (mantissaSize + exponentSize);
    return number;
}

uint64_t PointConversor::outputGenericFixedRaw(int16_t width, int16_t pointPos, Signedness signedness) const
{
    uint64_t number = this->bits;
    int16_t numExponent = this->exponent;

    uint64_t finalBit = 0;
    uint64_t mantissaMask = 0;
    uint64_t digitsMask = 0;
    uint64_t roundLeft = 0;
    uint64_t roundRight = 0;

    validateFixedSpec(width, pointPos, signedness);

    // Initialize here because width might be invalid.
    //
    // 0000...01111..111111
    digitsMask = ~((uint64_t) 0) >> (MAX_WIDTH - width);

    // Gets final significant bit (except for sign).
    switch (signedness) {
    case UNSIGNED:
        finalBit = (uint64_t) 1 << (width - 1);
        break;
    case TWOS_COMPL:
        finalBit = (uint64_t) 1 << (width - 2);
        break;
    }

    // Significant bits mask.
    mantissaMask = (finalBit << 1) - 1;

    switch (this->normality) {
    case NORMAL:
        // We need to normalize the exponent to the point position.
        while (numExponent > -pointPos) {
            // We might need to round the number in the left.
            if (number >= finalBit) {
                roundLeft = finalBit;
            }
            numExponent -= 1;
            number <<= 1;
        }

        // If the exponent was not too big, it could be too small.
        while (numExponent < -pointPos) {
            // In this case, we might need to round by the right.
            roundRight = (number & 1) != 0;
            numExponent += 1;
            number >>= 1;
        }

        if (number > mantissaMask) {
            roundLeft = finalBit;
        }

        // Potential rounding at left position (most significant).
        number |= roundLeft;

        // Truncating the number.
        number &= mantissaMask;
        if (digitsMask != number) {
            // But that's ok because we will round right (less significant) it here if required.
            number += roundRight;
        }

        if (this->sign) {
            switch (signedness) {
            case UNSIGNED:
                throw InvalidConversorInput("Formato de saída é sem sinal, mas entrada é negativa.");
            case TWOS_COMPL:
                // Complementing via 2's complement.
                number = (~number + 1) & digitsMask;
                break;
            }
        }
        break;

    case INFINITY_NAN:
        // If infinity, let's just assign it to the greatest value (in magnitude).
        number = mantissaMask;
        if (this->sign) {
            switch (signedness) {
            case UNSIGNED:
                throw InvalidConversorInput("Formato de saída é sem sinal, mas entrada é negativa.");
            case TWOS_COMPL:
                // Negative "infinity".
                number = ((~number + 1) & digitsMask) - 1;
                break;
            }
        }
        break;

    case NOT_A_NUMBER:
        // NAN is invalid for fixed points. No way to represent?
        throw InvalidConversorInput("Entrada é um NAN (Not A Number)");
    }

    return number;
}

QString PointConversor::outputGenericFloat(uint16_t mantissaSize, uint16_t exponentSize) const
{
    uint64_t number = this->outputGenericFloatRaw(mantissaSize, exponentSize);
    QString output;

    // Simple dummy binary conversion loop :P
    for (uint64_t i = mantissaSize + exponentSize + 1; i > 0; i--) {
        uint64_t mask = (uint64_t) 1 << (i - 1);
        output.append(number & mask ? '1' : '0');
    }
    return output;
}

QString PointConversor::outputGenericFixed(int16_t width, int16_t pointPos, Signedness signedness) const
{
    uint64_t number = this->outputGenericFixedRaw(width, pointPos, signedness);
    QString output;

    // Before the point.
    for (int i = width - 1; i >= pointPos; i--) {
        if (i >= 0) {
            // Regular bit conversion.
            uint64_t mask = (uint64_t) 1 << (uint64_t) i;
            output.append(number & mask ? '1' : '0');
        } else {
            // Append a trailing zero.
            output.append('0');
        }
    }

    // Point required because fixed-point format.
    output.append('.');

    // After the point.
    for (int i = pointPos - 1; i >= 0; i--) {
        if (i < width) {
            // Regular bit conversion.
            uint64_t mask = (uint64_t) 1 << (uint64_t) i;
            output.append(number & mask ? '1' : '0');
        } else {
            // Append a trailing zero.
            output.append('0');
        }
    }

    return output;
}

// Validation functions

void PointConversor::validateFloatSpec(uint16_t mantissaSize, uint16_t exponentSize) const
{
    if (mantissaSize < MIN_MANTISSA_SIZE) {
        throw InvalidConversorInput(QString("Tamanho da mantissa não pode ser menor que ") + QString::number(MIN_MANTISSA_SIZE));
    }
    if (exponentSize < MIN_EXPONENT_SIZE) {
        throw InvalidConversorInput(QString("Tamanho do expoente não pode ser menor que ") + QString::number(MIN_EXPONENT_SIZE));
    }
    if (mantissaSize + exponentSize > MAX_WIDTH - 1) {
        throw InvalidConversorInput(
                    QString("É necessário um bit de sinal, mantissa e expoente juntos devem ser menor que ")
                    + QString::number(MAX_WIDTH - 1));
    }
}

void PointConversor::validateFixedSpec(int16_t width, int16_t pointPos, Signedness signedness) const
{
    if (width < MIN_FIXED_WIDTH) {
        throw InvalidConversorInput(QString("Largura não pode ser menor que ") + QString::number(MIN_FIXED_WIDTH));
    }
    if (width > MAX_WIDTH) {
        throw InvalidConversorInput(QString("Largura não pode ser maior que ") +  QString::number(MAX_WIDTH));
    }
    if (pointPos > width || pointPos < 0) {
        throw InvalidConversorInput("Ponto fora da quantidade de bits disponíveis.");
    }

    if (pointPos > width || pointPos < 0) {
        throw InvalidConversorInput("Ponto fora da quantidade de bits disponíveis.");
    }
}
