#ifndef POINTCONVERSOR_H
#define POINTCONVERSOR_H

#include <cstdint>
#include <qstring.h>

class PointConversor
{
private:
    enum Normality {
        NORMAL,
        NOT_A_NUMBER,
        INFINITY_NAN
    };
public:
    PointConversor();
    PointConversor& inputHalfFloatRaw(uint16_t number);
    PointConversor& inputSingleFloatRaw(float number);
    PointConversor& inputDoubleFloatRaw(double number);
    PointConversor& inputFixed8Raw(uint8_t number, int16_t pointPos);
    PointConversor& inputFixed16Raw(uint16_t number, int16_t pointPos);
    PointConversor& inputFixed32Raw(uint32_t number, int16_t pointPos);
    PointConversor& inputFixed64Raw(uint64_t number, int16_t pointPos);

    PointConversor& inputHalfFloat(QString const& number);
    PointConversor& inputSingleFloat(QString const& number);
    PointConversor& inputDoubleFloat(QString const& number);
    PointConversor& inputFixed8(QString const& number);
    PointConversor& inputFixed16(QString const& number);
    PointConversor& inputFixed32(QString const& number);
    PointConversor& inputFixed64(QString const& number);

    uint16_t outputHalfFloatRaw();
    float outputSingleFloatRaw();
    double outputDoubleFloatRaw();
    uint8_t outputFixed8Raw(int16_t pointPos);
    uint16_t outputFixed16Raw(int16_t pointPos);
    uint32_t outputFixed32Raw(int16_t pointPos);
    uint64_t outputFixed64Raw(int16_t pointPos);

    QString outputHalfFloat();
    QString outputSingleFloat();
    QString outputDoubleFloat();
    QString outputFixed8(int16_t pointPos);
    QString outputFixed16(int16_t pointPos);
    QString outputFixed32(int16_t pointPos);
    QString outputFixed64(int16_t pointPos);

private:
    uint64_t digits;
    int16_t exponent;
    bool sign;
    Normality normality;

    PointConversor& inputGenericFloatRaw(uint64_t number, uint16_t mantissaSize, uint16_t exponentSize);
    PointConversor& inputGenericFixedRaw(uint64_t number, uint16_t width, int16_t pointPos);

    PointConversor& inputGenericFloat(QString const &number, uint16_t mantissaSize, uint16_t exponentSize);
    PointConversor& inputGenericFixed(QString const &number, uint16_t width);

    uint64_t outputGenericFloatRaw(uint16_t mantissaSize, uint16_t exponentSize);
    uint64_t outputGenericFixedRaw(uint16_t width, int16_t pointPos);

    QString outputGenericFloat(uint16_t mantissaSize, uint16_t exponentSize);
    QString outputGenericFixed(uint16_t width, int16_t pointPos);

};

#endif // POINTCONVERSOR_H
