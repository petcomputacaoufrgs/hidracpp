#include "pointconversor.h"

PointConversor::PointConversor()
{
    digits = 0;
    sign = false;
    exponent = 0;
    normality = PointConversor::NORMAL;
}

PointConversor& PointConversor::inputHalfFloat(uint16_t number)
{
    return this->inputGenericFloat(number, 10, 5);
}

PointConversor& PointConversor::inputSingleFloat(float number)
{
    union { float number; uint32_t bits; } bitCast { number };
    return this->inputGenericFloat(bitCast.bits, 23, 8);
}

PointConversor& PointConversor::inputDoubleFloat(double number)
{
    union { double number; uint64_t bits; } bitCast { number };
    return this->inputGenericFloat(bitCast.bits, 52, 11);
}

PointConversor& PointConversor::inputFixed8(uint8_t number, int16_t pointPos)
{
    return this->inputGenericFixed(number, 8, pointPos);
}

PointConversor& PointConversor::inputFixed16(uint16_t number, int16_t pointPos)
{
    return this->inputGenericFixed(number, 16, pointPos);
}

PointConversor& PointConversor::inputFixed32(uint32_t number, int16_t pointPos)
{
    return this->inputGenericFixed(number, 32, pointPos);
}

PointConversor& PointConversor::inputFixed64(uint64_t number, int16_t pointPos)
{
    return this->inputGenericFixed(number, 64, pointPos);
}

uint16_t PointConversor::outputHalfFloat()
{
    return this->outputGenericFloat(10, 5);
}

float PointConversor::outputSingleFloat()
{
    uint32_t number = this->outputGenericFloat(23, 8);
    union { uint32_t bits; float number; } bitCast { number };
    return bitCast.number;
}

double PointConversor::outputDoubleFloat()
{
    uint64_t number = this->outputGenericFloat(52, 10);
    union { uint64_t bits; double number; } bitCast { number };
    return bitCast.number;
}

uint8_t PointConversor::outputFixed8(int16_t pointPos)
{
    return this->outputGenericFixed(8, pointPos);
}

uint16_t PointConversor::outputFixed16(int16_t pointPos)
{
    return this->outputGenericFixed(16, pointPos);
}

uint32_t PointConversor::outputFixed32(int16_t pointPos)
{
    return this->outputGenericFixed(32, pointPos);
}

uint64_t PointConversor::outputFixed64(int16_t pointPos)
{
    return this->outputGenericFixed(64, pointPos);
}

PointConversor& PointConversor::inputGenericFloat(uint64_t number, uint16_t mantissaSize, uint16_t exponentSize)
{
    uint64_t mantissaMask = ((uint64_t) 1 << mantissaSize) - 1;
    int16_t exponentMask = ((uint64_t) 1 << exponentSize) - 1;
    uint64_t finalBit = ((uint64_t) 1 << (exponentSize + mantissaSize));

    sign = number >> (exponentSize + mantissaSize);
    digits = number & mantissaMask;
    exponent = (number >> mantissaSize) & exponentMask;
    if (exponent != exponentMask) {
        if (exponent != 0) {
            digits |= finalBit;
            normality = PointConversor::NORMAL;
        }
        exponent -= exponentMask >> 1;
    } else if (digits == 0) {
        normality = PointConversor::INFINITY;
    } else {
        normality = PointConversor::NAN;
    }

    return *this;
}

PointConversor& PointConversor::inputGenericFixed(uint64_t number, uint16_t width, int16_t pointPos)
{
    uint64_t sign_mask = (uint64_t) 1 << (width - 1);

    normality = PointConversor::NORMAL;
    sign = (number & sign_mask) != 0;
    exponent = -pointPos;

    digits = number;
    if (sign) {
        digits = ~digits + 1;
    }

    return *this;
}

uint64_t PointConversor::outputGenericFloat(uint16_t mantissaSize, uint16_t exponentSize)
{
    int16_t exponentMask = ((uint64_t) 1 << exponentSize) - 1;
    uint64_t finalBit = ((uint64_t) 1 << (exponentSize + mantissaSize));
    int16_t numExponent;
    uint64_t numDigits;

    switch (normality) {
    case PointConversor::NORMAL:
        numExponent = exponent + (exponentMask >> 1);
        numDigits = digits;
        while (numDigits < finalBit && numDigits > 0) {
            numDigits <<= 1;
            numExponent -= 1;
        }
        while (numDigits > finalBit) {
            numDigits >>= 1;
            numExponent += 1;
        }
        while (numExponent >= exponentMask) {
            numDigits >>= 1;
            numExponent -= 1;
        }
        if (numDigits == 0) {
            numExponent = 0;
        } else if (numExponent >= exponentMask) {
            numExponent = exponentMask;
            numDigits = 0;
        } else if (numExponent <= 0) {
            numExponent += 1;
            numDigits >>= 1;
        }
        break;
    case PointConversor::INFINITY:
        numExponent = exponentMask;
        numDigits = 0;
        break;
    case PointConversor::NAN:
        numExponent = exponentMask;
        numDigits = 1;
        break;
    }

    uint64_t number = numDigits;
    number |= (uint64_t) numExponent << mantissaSize;
    number |= (uint64_t) sign << (mantissaSize + exponentSize);
    return number;
}

uint64_t PointConversor::outputGenericFixed(uint16_t width, int16_t pointPos)
{
    uint64_t number = digits;
    int16_t numExponent = exponent;
    while (numExponent > -pointPos) {
        numExponent -= 1;
        number <<= 1;
    }
    while (numExponent < -pointPos) {
        numExponent += 1;
        number >>= 1;
    }
    if (sign) {
        number = ~number + 1;
    }
    return number;
}
