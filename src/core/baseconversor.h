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

    /// Inputs a positive (unsigned) number into the conversor in a textual
    /// representation. Max value allowed is 2**64 - 1.
    ///
    /// Throws an error if there are too many digits or invalid digits.
    BaseConversor& inputPositive(QString const &digits, uint64_t base);
    /// Inputs a sign magnitude number into the conversor in a textual
    /// representation. Max value allowed is +2**64 - 1 and minimum 2**64 - 1.
    ///
    /// Throws an error if there are too many digits, invalid digits, or sign
    /// is not 0 or 1.
    BaseConversor& inputSignMagnitude(QString const &digits, uint64_t base);
    /// Inputs a B-1's complement number into the conversor in a textual
    /// representation. Max value allowed is +2**63 - 1 and minimum 2**63 - 1.
    ///
    /// Throws an error if there are too many digits or invalid digits.
    BaseConversor& inputOnesComplement(QString const &digits, uint64_t base);
    /// Inputs a B's complement number into the conversor in a textual
    /// representation. Max value allowed is +2**63 - 1 and minimum 2**63.
    ///
    /// Throws an error if there are too many digits or invalid digits.
    BaseConversor& inputTwosComplement(QString const &digits, uint64_t base);

    /// Outputs the stored number in positive (unsigned) format as text, with
    /// at least the given width of digits.
    ///
    /// Throws an error if the stored number is negative, or the given width
    /// is invalid.
    QString outputPositive(uint64_t base, uint64_t width) const;
    /// Outputs the stored number in sign magnitude format as text, with at
    /// least the given width of digits.
    ///
    /// Throws an error if the the given width is invalid.
    QString outputSignMagnitude(uint64_t base, uint64_t width) const;
    /// Outputs the stored number in B-1's complement format as text, with at
    /// least the given width of digits.
    ///
    /// Throws an error if the the given width is invalid.
    QString outputOnesComplement(uint64_t base, uint64_t width) const;
    /// Outputs the stored number in B's complement format as text, with at
    /// least the given width of digits.
    ///
    /// Throws an error if the the given width is invalid.
    QString outputTwosComplement(uint64_t base, uint64_t width) const;

private:
    /// Base 2 representation of the conversible numbers.
    /// Holds at most 64 bits.
    uint64_t bits;
    /// Sign of the stored number. More specifically, it means whether the sign
    /// is negative. Always `false` for positive number format.
    bool sign;

    /// Maps a character to a digit in a given base.
    ///
    /// Throws an error if digit is invalid for the base.
    uint64_t mapInput(QChar character, uint64_t base) const;
    /// Maps a digit to a character in a given base.
    ///
    /// Throws an error if digit is too big for any valid base.
    QChar mapOutput(uint64_t digit) const;

    /// Gets the maximum value for a number in the given base with the given
    /// width in digits, and greater than or equals to the given `atLeast`
    /// number, even if it surpasses the expected width. The actual width is
    /// placed in the width in-out parameter.
    ///
    /// Throws an error on overflow.
    uint64_t maxValue(uint64_t base, uint64_t *width, uint64_t atLeast) const;

    /// Decodes a given string of digits in the given base into an integer.
    /// Places in the `width` out parameter the count of digits.
    ///
    /// Throws an error if an invalid digit is found or the number is too big.
    uint64_t decode(QString const &digitsQ, uint64_t base, uint64_t *width) const;
    /// Encodes into a string of digits in the given base from an integer.
    /// The total number of digits will be at least `width`, filling with the
    /// character `fill` if necessary.
    QString encode(uint64_t bits, uint64_t base, uint64_t width, QChar fill) const;

    /// Throws an exception if the given base is invalid.
    void validateBase(uint64_t base) const;

    /// Maximum width in digits.
    static const int MAX_WIDTH = 64;
    /// Minimum width in digits.
    static const int MIN_WIDTH = 1;

    /// Maximum base.
    static const int MAX_BASE = 36;
    /// Minimum base.
    static const int MIN_BASE = 2;
};

#endif // BASECONVERSOR_H
