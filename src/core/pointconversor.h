#ifndef POINTCONVERSOR_H
#define POINTCONVERSOR_H

#include <cstdint>

class PointConversor
{
private:
    enum Normality {
        NORMAL,
        NAN,
        INFINITY
    };
public:
    PointConversor();
    PointConversor& inputHalfFloat(uint16_t number);
    PointConversor& inputSingleFloat(float number);
    PointConversor& inputDoubleFloat(double number);
    PointConversor& inputFixed8(uint8_t number, int16_t pointPos);
    PointConversor& inputFixed16(uint16_t number, int16_t pointPos);
    PointConversor& inputFixed32(uint32_t number, int16_t pointPos);
    PointConversor& inputFixed64(uint64_t number, int16_t pointPos);

    uint16_t outputHalfFloat();
    float outputSingleFloat();
    double outputDoubleFloat();
    uint8_t outputFixed8(int16_t pointPos);
    uint16_t outputFixed16(int16_t pointPos);
    uint32_t outputFixed32(int16_t pointPos);
    uint64_t outputFixed64(int16_t pointPos);

private:
    uint64_t digits;
    int16_t exponent;
    bool sign;
    Normality normality;

    PointConversor& inputGenericFloat(uint64_t number, uint16_t mantissaSize, uint16_t exponentSize);
    PointConversor& inputGenericFixed(uint64_t number, uint16_t width, int16_t pointPos);

    uint64_t outputGenericFloat(uint16_t mantissaSize, uint16_t exponentSize);
    uint64_t outputGenericFixed(uint16_t width, int16_t pointPos);
};

#endif // POINTCONVERSOR_H
