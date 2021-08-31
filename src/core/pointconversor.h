#ifndef POINTCONVERSOR_H
#define POINTCONVERSOR_H

#include <cstdint>
#include <qstring.h>

class PointConversor
{
private:
    /// Normality of a number, used to capture some non-normal floating point states.
    enum Normality
    {
        /// Normal number. Corresponds to any fixed point number, and to normal and
        /// subnormal float states.
        NORMAL,
        /// Not a (valid) number, not even infinity. Corresponds to float's NAN (but not INFINITY).
        NOT_A_NUMBER,
        /// Infinity, be it positive or negative. Corresponds to float's infinity.
        INFINITY_NAN
    };
public:
    /// Signedness of a fixed point number.
    enum Signedness
    {
        /// Unsigned.
        UNSIGNED = 0,
        /// Two's complement.
        TWOS_COMPL = 1,
    };

    PointConversor();

    /// Inputs a half precision floating point number encoded in a 16-bit unsigned integer.
    PointConversor& inputHalfFloatRaw(uint16_t number);
    /// Inputs a single precision floating point number encoded in a regular float.
    PointConversor& inputSingleFloatRaw(float number);
    /// Inputs a double precision floating point number encoded in a regular double.
    PointConversor& inputDoubleFloatRaw(double number);
    /// Inputs an 8-bit fixed point with its bits as an unsigned integer, point position, and signedness.
    ///
    /// Throws an error if point position is `< 1` or `> 7`.
    PointConversor& inputFixed8Raw(uint8_t number, int16_t pointPos, Signedness signedness);
    /// Inputs a 16-bit fixed point with its bits as an unsigned integer, point position, and signedness.
    ///
    /// Throws an error if point position is `< 1` or `> 15`.
    PointConversor& inputFixed16Raw(uint16_t number, int16_t pointPos, Signedness signedness);
    /// Inputs a 32-bit fixed point with its bits as an unsigned integer, point position, and signedness.
    ///
    /// Throws an error if point position is `< 1` or `> 31`.
    PointConversor& inputFixed32Raw(uint32_t number, int16_t pointPos, Signedness signedness);
    /// Inputs a 64-bit fixed point with its bits as an unsigned integer, point position, and signedness.
    ///
    /// Throws an error if point position is `< 1` or `> 63`.
    PointConversor& inputFixed64Raw(uint64_t number, int16_t pointPos, Signedness signedness);

    /// Inputs a half precision floating point number encoded in a binary string ('0' or '1').
    ///
    /// Throws an error if the string contains any other character than '0', '1' or space,
    /// or if there are too many digits (more than 16).
    PointConversor& inputHalfFloat(QString const& number);
    /// Inputs a single precision floating point number encoded in a binary string ('0' or '1').
    ///
    /// Throws an error if the string contains any other character than '0', '1' or space,
    /// or if there are too many digits (more than 32).
    PointConversor& inputSingleFloat(QString const& number);
    /// Inputs a double precision floating point number encoded in a binary string ('0' or '1').
    ///
    /// Throws an error if the string contains any other character than '0', '1' or space,
    /// or if there are too many digits (more than 64).
    PointConversor& inputDoubleFloat(QString const& number);

    /// Inputs an 8-bit fixed point number encoded in a binary string ('0' or '1')
    /// with its signedness.
    ///
    /// Throws an error if:
    /// - the string contains any other character than '0', '1', '.', or space;
    /// - or if there are too many digits (more than 8);
    /// - or if there is not exactly one point ('.');
    /// - or if the point is in position `< 1` or `> 7`.
    PointConversor& inputFixed8(QString const& number, Signedness signedness);
    /// Inputs an 16-bit fixed point number encoded in a binary string ('0' or '1')
    /// with its signedness.
    ///
    /// Throws an error if:
    /// - the string contains any other character than '0', '1', '.', or space;
    /// - or if there are too many digits (more than 16);
    /// - or if there is not exactly one point ('.');
    /// - or if the point is in position `< 1` or `> 15`.
    PointConversor& inputFixed16(QString const& number, Signedness signedness);
    /// Inputs an 32-bit fixed point number encoded in a binary string ('0' or '1')
    /// with its signedness.
    ///
    /// Throws an error if:
    /// - the string contains any other character than '0', '1', '.', or space;
    /// - or if there are too many digits (more than 32);
    /// - or if there is not exactly one point ('.');
    /// - or if the point is in position `< 1` or `> 31`.
    PointConversor& inputFixed32(QString const& number, Signedness signedness);
    /// Inputs an 64-bit fixed point number encoded in a binary string ('0' or '1')
    /// with its signedness.
    ///
    /// Throws an error if:
    /// - the string contains any other character than '0', '1', '.', or space;
    /// - or if there are too many digits (more than 64);
    /// - or if there is not exactly one point ('.');
    /// - or if the point is in position `< 1` or `> 63`.
    PointConversor& inputFixed64(QString const& number, Signedness signedness);
    /// Inputs a fractional number in human notation.
    ///
    /// Throws an error if the string contains any other character than decimal digits or space.
    PointConversor& inputHumanNotation(QString const& number);

    /// Outputs a half precision floating point number encoded in a 16-bit unsigned integer.
    uint16_t outputHalfFloatRaw() const;
    /// Outputs a single precision floating point number encoded in a regular float.
    float outputSingleFloatRaw() const;
    /// Outputs a double precision floating point number encoded in a regular double.
    double outputDoubleFloatRaw() const;
    /// Outputs an 8-bit fixed point (as an unsigned integer) with given point position, and signedness.
    ///
    /// Throws an error if:
    /// - point position is `< 1` or `> 8`;
    /// - or if the stored number is NAN;
    /// - or if the number is negative but the given signedness ask for unsigned output.
    uint8_t outputFixed8Raw(int16_t pointPos, Signedness signedness) const;
    /// Outputs a 16-bit fixed point (as an unsigned integer) with given point position, and signedness.
    ///
    /// Throws an error if:
    /// - point position is `< 1` or `> 15`;
    /// - or if the stored number is NAN;
    /// - or if the number is negative but the given signedness ask for unsigned output.
    uint16_t outputFixed16Raw(int16_t pointPos, Signedness signedness) const;
    /// Outputs a 32-bit fixed point (as an unsigned integer) with given point position, and signedness.
    ///
    /// Throws an error if:
    /// - point position is `< 1` or `> 31`;
    /// - or if the stored number is NAN;
    /// - or if the number is negative but the given signedness ask for unsigned output.
    uint32_t outputFixed32Raw(int16_t pointPos, Signedness signedness) const;
    /// Outputs a 64-bit fixed point (as an unsigned integer) with given point position, and signedness.
    ///
    /// Throws an error if:
    /// - point position is `< 1` or `> 63`;
    /// - or if the stored number is NAN;
    /// - or if the number is negative but the given signedness ask for unsigned output.
    uint64_t outputFixed64Raw(int16_t pointPos, Signedness signedness) const;

    /// Outputs a half precision floating point number in a binary string.
    QString outputHalfFloat() const;
    /// Outputs a single precision floating point number in a binary string.
    QString outputSingleFloat() const;
    /// Outputs a double precision floating point number in a binary string.
    QString outputDoubleFloat() const;
    /// Outputs an 8-bit fixed point number in a binary string with the given signedness.
    ///
    /// Throws an error if:
    /// - the point position is `< 1` or `> 7`;
    /// - or the number is NAN;
    /// - or if the number is negative but unsigned output is requested.
    QString outputFixed8(int16_t pointPos, Signedness signedness) const;
    /// Outputs an 16-bit fixed point number in a binary string with the given signedness.
    ///
    /// Throws an error if:
    /// - the point position is `< 1` or `> 15`;
    /// - or the number is NAN;
    /// - or if the number is negative but unsigned output is requested.
    QString outputFixed16(int16_t pointPos, Signedness signedness) const;
    /// Outputs an 32-bit fixed point number in a binary string with the given signedness.
    ///
    /// Throws an error if:
    /// - the point position is `< 1` or `> 31`;
    /// - or the number is NAN;
    /// - or if the number is negative but unsigned output is requested.
    QString outputFixed32(int16_t pointPos, Signedness signedness) const;
    /// Outputs an 64-bit fixed point number in a binary string with the given signedness.
    ///
    /// Throws an error if:
    /// - the point position is `< 1` or `> 63`;
    /// - or the number is NAN;
    /// - or if the number is negative but unsigned output is requested.
    QString outputFixed64(int16_t pointPos, Signedness signedness) const;
    /// Outputs a string in the decimal system encoding this fractional number in human notation.
    QString outputHumanNotation() const;

private:
    /// Significant bits of the stored number. This is `b` in `b * 2**e`. With exponent `e == 0`,
    /// this is an integer value and if you'd place a point, it would be exactly after the last
    /// bit/digit, i.e. `1234 -> 1234.0` and `500 -> 500.0`, etc.
    uint64_t bits;
    /// Exponent of the stored number. This is `e` in the formula `b * 2**e`. This changes the
    /// position of the point.
    int16_t exponent;
    /// Sign of the stored number, more specifically, whether it is negative.
    bool sign;
    /// Normality of the number. Bits are ignored when not `NORMAL`.
    Normality normality;

    /// Generic float input, with bits encoded as an unsigned integer, and given mantissa and exponent sizes for
    /// the desired float spec.
    ///
    /// Throws an error if:
    /// - mantissaSize + exponentSize >= MAX_WIDTH;
    /// - mantissaSize < MIN_MANTISSA_SIZE;
    /// - exponentSize < MIN_MANTISSA_SIZE.
    PointConversor& inputGenericFloatRaw(uint64_t number, uint16_t mantissaSize, uint16_t exponentSize);
    /// Generic fixed-point input, with bits encoded as an unsigned integer, given width, point position and
    /// signedness for the desired fixed-point spec.
    ///
    /// Throws an error if:
    /// - width < MIN_FIXED_WIDTH;
    /// - width > MAX_WIDTH;
    /// - pointPos < 1;
    /// - pointPos >= MAX_WIDTH.
    PointConversor& inputGenericFixedRaw(uint64_t number, int16_t width, int16_t pointPos, Signedness signedness);

    /// Generic float input, with bits encoded as binary string, and given mantissa and exponent sizes for
    /// the desired float spec.
    ///
    /// Throws an error if:
    /// - mantissaSize + exponentSize >= MAX_WIDTH;
    /// - mantissaSize < MIN_MANTISSA_SIZE;
    /// - exponentSize < MIN_MANTISSA_SIZE;
    /// - the given string contains any character that is not `0`, `1` or space.
    PointConversor& inputGenericFloat(QString const &number, uint16_t mantissaSize, uint16_t exponentSize);
    /// Generic fixed-point input, with bits encoded as a binary string, given width, point position and
    /// signedness for the desired fixed-point spec.
    ///
    /// Throws an error if:
    /// - width < MIN_FIXED_WIDTH;
    /// - width > MAX_WIDTH;
    /// - the given string contains any character that is not `0`, `1`, '.' or space.
    /// - there is not exactly one point ('.').
    /// - point position < 1;
    /// - point position >= MAX_WIDTH;
    PointConversor& inputGenericFixed(QString const &number, int16_t width, Signedness signedness);

    /// Outputs a generic float with bits encoded as an unsigned integer.
    ///
    /// Throws an error if:
    /// - mantissaSize + exponentSize >= MAX_WIDTH;
    /// - mantissaSize < MIN_MANTISSA_SIZE;
    /// - exponentSize < MIN_MANTISSA_SIZE.
    uint64_t outputGenericFloatRaw(uint16_t mantissaSize, uint16_t exponentSize) const;
    /// Outputs a generic fixed-point with bits encoded as an unsigned integer.
    ///
    /// Throws an error if:
    /// - width < MIN_FIXED_WIDTH;
    /// - width > MAX_WIDTH;
    /// - pointPos < 1;
    /// - pointPos >= MAX_WIDTH;
    /// - number is NAN.
    /// - signedness == UNSIGNED and number is negative.
    uint64_t outputGenericFixedRaw(int16_t width, int16_t pointPos, Signedness signedness) const;

    /// Outputs a generic float with bits encoded as a binary string.
    ///
    /// Throws an error if:
    /// - mantissaSize + exponentSize >= MAX_WIDTH;
    /// - mantissaSize < MIN_MANTISSA_SIZE;
    /// - exponentSize < MIN_MANTISSA_SIZE.
    QString outputGenericFloat(uint16_t mantissaSize, uint16_t exponentSize) const;
    /// Outputs a generic fixed-point with bits encoded as a binary string.
    ///
    /// Throws an error if:
    /// - width < MIN_FIXED_WIDTH;
    /// - width > MAX_WIDTH;
    /// - pointPos < 1;
    /// - pointPos >= MAX_WIDTH;
    /// - number is NAN.
    /// - signedness == UNSIGNED and number is negative.
    QString outputGenericFixed(int16_t width, int16_t pointPos, Signedness signedness) const;

    /// Validates the the given spec of floating point numbers.
    ///
    /// Throws an error if:
    /// - mantissaSize + exponentSize >= MAX_WIDTH;
    /// - mantissaSize < MIN_MANTISSA_SIZE;
    /// - exponentSize < MIN_MANTISSA_SIZE.
    void validateFloatSpec(uint16_t mantissaSize, uint16_t exponentSize) const;
    /// Validates the the given spec of floating point numbers.
    ///
    /// Throws an error if:
    /// - width < MIN_FIXED_WIDTH;
    /// - width > MAX_WIDTH;
    /// - pointPos < 1;
    /// - pointPos >= MAX_WIDTH.
    void validateFixedSpec(int16_t width, int16_t pointPos, Signedness signedness) const;

    /// Maximum width in bits.
    static const int MAX_WIDTH = 64;

    /// Minimum width for fixed point (internal usage only).
    static const int MIN_FIXED_WIDTH = 2;
    /// Minimum exponent size for floats (internal usage only).
    static const int MIN_EXPONENT_SIZE = 1;
    /// Minimum mantissa size for floats (internal usage only).
    static const int MIN_MANTISSA_SIZE = 1;
};

#endif // POINTCONVERSOR_H
