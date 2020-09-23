#ifndef POINTCONVERSOR_H
#define POINTCONVERSOR_H

#include <cstdint>

class PointConversor
{
public:
    PointConversor();
    PointConversor& inputHalfFloat(uint16_t number);
    PointConversor& inputSingleFloat(float number);
    PointConversor& inputDoubleFloat(double number);
    PointConversor& inputFixed8(uint8_t number);
    PointConversor& inputFixed16(uint16_t number);
    PointConversor& inputFixed32(uint32_t number);
    PointConversor& inputFixed64(uint64_t number);

    uint16_t outputHalfFloat();
    float outputSingleFloat();
    double outputDoubleFloat();
    uint8_t outputFixed8();
    uint16_t outputFixed16();
    uint32_t outputFixed32();
    uint64_t outputFixed64();

private:
    uint64_t digits;
    uint16_t exponent;
    bool sign;
};

#endif // POINTCONVERSOR_H
