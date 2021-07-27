#include <QtTest>

#include "pointconversor.h"

class PointConversorTest : public QObject
{
    Q_OBJECT

private:
    PointConversor conversor;

public:
    PointConversorTest();
    ~PointConversorTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_fixedSamePoint();
    void test_fixedSamePointTwosCompl();

    void test_fixedOtherPoint();
    void test_fixedOtherPointTwosCompl();
    void test_fixedZerosRight();
    void test_fixedZerosLeft();

    void test_floatToFloat();
    void test_floatToFloatSigned();

    void test_floatToFixed();
    void test_fixedToFloat();

    void test_subnormalToFloat();
    void test_subnormalToFixed();

    void test_floatOverflow();
    void test_floatUnderflow();

    void test_fixedOverflow();
    void test_fixedUnderflow();

    void test_infiniteToFloat();
    void test_infiniteToFixed();

    void test_nanToFloat();
};

PointConversorTest::PointConversorTest()
{

}

PointConversorTest::~PointConversorTest()
{

}

void PointConversorTest::initTestCase()
{
}

void PointConversorTest::cleanupTestCase()
{
}

void PointConversorTest::test_fixedSamePoint()
{
    QCOMPARE(conversor.inputFixed8("0101.0011", PointConversor::UNSIGNED).outputFixed64(4, PointConversor::UNSIGNED),
             "000000000000000000000000000000000000000000000000000000000101.0011");

    QCOMPARE(conversor.inputFixed64("0101.0011", PointConversor::UNSIGNED).outputFixed8(4, PointConversor::UNSIGNED),
             "0101.0011");
}

void PointConversorTest::test_fixedSamePointTwosCompl()
{
    QCOMPARE(conversor.inputFixed8("0101.0011", PointConversor::TWOS_COMPL).outputFixed64(4, PointConversor::TWOS_COMPL),
             "000000000000000000000000000000000000000000000000000000000101.0011");

    QCOMPARE(conversor.inputFixed64("0101.0011", PointConversor::TWOS_COMPL).outputFixed8(4, PointConversor::TWOS_COMPL),
             "0101.0011");

    QCOMPARE(conversor.inputFixed8("1010.1101", PointConversor::TWOS_COMPL).outputFixed64(4, PointConversor::TWOS_COMPL),
             "111111111111111111111111111111111111111111111111111111111010.1101");
}

void PointConversorTest::test_fixedOtherPoint()
{
    QCOMPARE(conversor.inputFixed8("0101.0011", PointConversor::UNSIGNED).outputFixed64(5, PointConversor::UNSIGNED),
             "00000000000000000000000000000000000000000000000000000000101.00110");
}

void PointConversorTest::test_fixedOtherPointTwosCompl()
{
    QCOMPARE(conversor.inputFixed8("0101.0011", PointConversor::TWOS_COMPL).outputFixed64(5, PointConversor::TWOS_COMPL),
             "00000000000000000000000000000000000000000000000000000000101.00110");

    QCOMPARE(conversor.inputFixed8("1010.1101", PointConversor::TWOS_COMPL).outputFixed64(5, PointConversor::TWOS_COMPL),
             "11111111111111111111111111111111111111111111111111111111010.11010");
}

void PointConversorTest::test_fixedZerosRight()
{
    QCOMPARE(conversor.inputFixed64("0000000000000000000000000000000000000000000000000000101.001100000", PointConversor::UNSIGNED).outputFixed8(4, PointConversor::UNSIGNED),
             "0101.0011");
}

void PointConversorTest::test_fixedZerosLeft()
{
    QCOMPARE(conversor.inputFixed8("00000.01010011", PointConversor::UNSIGNED).outputFixed64(9, PointConversor::UNSIGNED),
             "0000000000000000000000000000000000000000000000000000000.010100110");
}

void PointConversorTest::test_floatToFloat()
{
    QCOMPARE(conversor.inputDoubleFloat("0011111110100110000000000000000000000000000000000000000000000000").outputSingleFloat(),
             "00111101001100000000000000000000");

    QCOMPARE(conversor.inputSingleFloat("00111101001010010000000000000000").outputDoubleFloat(),
             "0011111110100101001000000000000000000000000000000000000000000000");

    QCOMPARE(conversor.inputSingleFloat("01000010010000000110000000000000").outputHalfFloat(),
             "0101001000000011");
}

void PointConversorTest::test_floatToFloatSigned()
{
    QCOMPARE(conversor.inputDoubleFloat("1011111110100110000000000000000000000000000000000000000000000000").outputSingleFloat(),
             "10111101001100000000000000000000");

    QCOMPARE(conversor.inputSingleFloat("10111101001010010000000000000000").outputDoubleFloat(),
             "1011111110100101001000000000000000000000000000000000000000000000");

    QCOMPARE(conversor.inputSingleFloat("11000010010000000110000000000000").outputHalfFloat(),
             "1101001000000011");
}

void PointConversorTest::test_floatToFixed()
{
    QCOMPARE(conversor.inputSingleFloat("01000001110000010100100000000000").outputFixed16(10, PointConversor::UNSIGNED),
             "011000.0010100100");

    QCOMPARE(conversor.inputDoubleFloat("0011111101010100110000000000000000000000000000000000000000000000").outputFixed64(16, PointConversor::UNSIGNED),
             "000000000000000000000000000000000000000000000000.0000000001010011");
}

void PointConversorTest::test_fixedToFloat()
{
    QCOMPARE(conversor.inputFixed32("100001010.00010101000", PointConversor::UNSIGNED).outputDoubleFloat(),
             "0100000001110000101000010101000000000000000000000000000000000000");

    QCOMPARE(conversor.inputFixed16("01100010101100000.0", PointConversor::UNSIGNED).outputHalfFloat(),
             "0111101000101011");
}

void PointConversorTest::test_subnormalToFloat()
{
    QCOMPARE(conversor.inputSingleFloat("00000000000000000000000000000000010").outputDoubleFloat(),
             "0011011010110000000000000000000000000000000000000000000000000000");

    QCOMPARE(conversor.inputDoubleFloat("0011011010110000000000000000000000000000000000000000000000000000").outputSingleFloat(),
             "00000000000000000000000000000010");

    QCOMPARE(conversor.inputHalfFloat("0000000000010011").outputSingleFloat(),
             "00110101100110000000000000000000");

    QCOMPARE(conversor.inputSingleFloat("00110101100110000000000000000000").outputHalfFloat(),
             "0000000000010011");
}

void PointConversorTest::test_subnormalToFixed()
{
    QCOMPARE(conversor.inputHalfFloat("0000000000010011").outputFixed32(26, PointConversor::UNSIGNED),
             "000000.00000000000000000001001100");
}

void PointConversorTest::test_floatOverflow()
{
    QCOMPARE(conversor.inputDoubleFloat("0100001000000010011001011000000010010111010100000111110101110001").outputSingleFloat(),
             "01010000000100110010110000000101");

    QCOMPARE(conversor.inputDoubleFloat("0100001000000010011001011000000010110111010100000111110101110001").outputSingleFloat(),
             "01010000000100110010110000000110");
}

void PointConversorTest::test_floatUnderflow()
{
    QCOMPARE(conversor.inputDoubleFloat("0011110100111011110011001100110010100110001101110001010111100100").outputSingleFloat(),
             "00101001110111100110011001100101");

    QCOMPARE(conversor.inputDoubleFloat("0011110100111011110011001100110010110110001101110001010111100100").outputSingleFloat(),
             "00101001110111100110011001100110");
}

void PointConversorTest::test_fixedOverflow()
{
    QCOMPARE(conversor.inputFixed16("111111111111111.1", PointConversor::UNSIGNED).outputFixed8(6, PointConversor::UNSIGNED),
             "11.100000");

    QCOMPARE(conversor.inputFixed16("111111111111101.1", PointConversor::UNSIGNED).outputFixed8(6, PointConversor::UNSIGNED),
             "11.100000");

    QCOMPARE(conversor.inputFixed16("111111111111001.1", PointConversor::UNSIGNED).outputFixed8(6, PointConversor::UNSIGNED),
             "11.100000");
}

void PointConversorTest::test_fixedUnderflow()
{
    QCOMPARE(conversor.inputFixed16("0.0111111111111111", PointConversor::UNSIGNED).outputFixed8(3, PointConversor::UNSIGNED),
             "00000.100");

    QCOMPARE(conversor.inputFixed16("0.0101111111111111", PointConversor::UNSIGNED).outputFixed8(3, PointConversor::UNSIGNED),
             "00000.011");

    QCOMPARE(conversor.inputFixed16("0.0110111111111111", PointConversor::UNSIGNED).outputFixed8(3, PointConversor::UNSIGNED),
             "00000.011");
}

void PointConversorTest::test_infiniteToFloat()
{
    QCOMPARE(conversor.inputHalfFloat("0111110000000000").outputSingleFloat(),
             "01111111100000000000000000000000");

    QCOMPARE(conversor.inputDoubleFloat("0111111111110000000000000000000000000000000000000000000000000000").outputHalfFloat(),
             "0111110000000000");
}

void PointConversorTest::test_infiniteToFixed()
{
    QCOMPARE(conversor.inputHalfFloat("0111110000000000").outputFixed8(5, PointConversor::UNSIGNED),
             "111.11111");
}

void PointConversorTest::test_nanToFloat()
{
    QCOMPARE(conversor.inputHalfFloat("0111110001110010").outputSingleFloat(),
             "01111111100000000000000000000001");

    QCOMPARE(conversor.inputDoubleFloat("0111111111110000000000000000000000000000000000000000000000110000").outputSingleFloat(),
             "01111111100000000000000000000001");

    QCOMPARE(conversor.inputSingleFloat("01111111100000000000000001000000").outputDoubleFloat(),
             "0111111111110000000000000000000000000000000000000000000000000001");
}

QTEST_APPLESS_MAIN(PointConversorTest)

#include "tst_pointconversortest.moc"
