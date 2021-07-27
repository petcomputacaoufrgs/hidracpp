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
    enum Signedness {
        TWOS_COMPL,
        UNSIGNED,
    };

    PointConversor();
    PointConversor& inputHalfFloatRaw(uint16_t number);
    PointConversor& inputSingleFloatRaw(float number);
    PointConversor& inputDoubleFloatRaw(double number);
    PointConversor& inputFixed8Raw(uint8_t number, int16_t pointPos, Signedness signedness);
    PointConversor& inputFixed16Raw(uint16_t number, int16_t pointPos, Signedness signedness);
    PointConversor& inputFixed32Raw(uint32_t number, int16_t pointPos, Signedness signedness);
    PointConversor& inputFixed64Raw(uint64_t number, int16_t pointPos, Signedness signedness);

    PointConversor& inputHalfFloat(QString const& number);
    PointConversor& inputSingleFloat(QString const& number);
    PointConversor& inputDoubleFloat(QString const& number);
    PointConversor& inputFixed8(QString const& number, Signedness signedness);
    PointConversor& inputFixed16(QString const& number, Signedness signedness);
    PointConversor& inputFixed32(QString const& number, Signedness signedness);
    PointConversor& inputFixed64(QString const& number, Signedness signedness);

    uint16_t outputHalfFloatRaw();
    float outputSingleFloatRaw();
    double outputDoubleFloatRaw();
    uint8_t outputFixed8Raw(int16_t pointPos, Signedness signedness);
    uint16_t outputFixed16Raw(int16_t pointPos, Signedness signedness);
    uint32_t outputFixed32Raw(int16_t pointPos, Signedness signedness);
    uint64_t outputFixed64Raw(int16_t pointPos, Signedness signedness);

    QString outputHalfFloat();
    QString outputSingleFloat();
    QString outputDoubleFloat();
    QString outputFixed8(int16_t pointPos, Signedness signedness);
    QString outputFixed16(int16_t pointPos, Signedness signedness);
    QString outputFixed32(int16_t pointPos, Signedness signedness);
    QString outputFixed64(int16_t pointPos, Signedness signedness);

private:
    uint64_t digits;
    int16_t exponent;
    bool sign;
    Normality normality;

    PointConversor& inputGenericFloatRaw(uint64_t number, uint16_t mantissaSize, uint16_t exponentSize);
    PointConversor& inputGenericFixedRaw(uint64_t number, int16_t width, int16_t pointPos, Signedness signedness);

    PointConversor& inputGenericFloat(QString const &number, uint16_t mantissaSize, uint16_t exponentSize);
    PointConversor& inputGenericFixed(QString const &number, int16_t width, Signedness signedness);

    uint64_t outputGenericFloatRaw(uint16_t mantissaSize, uint16_t exponentSize);
    uint64_t outputGenericFixedRaw(int16_t width, int16_t pointPos, Signedness signedness);

    QString outputGenericFloat(uint16_t mantissaSize, uint16_t exponentSize);
    QString outputGenericFixed(int16_t width, int16_t pointPos, Signedness signedness);

    static const int MAX_BIT_COUNT = 64;
};

#endif // POINTCONVERSOR_H