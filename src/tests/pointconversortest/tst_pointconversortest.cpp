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
    void test_fixedOtherPoint();
    void test_fixedZerosRight();
    void test_fixedZerosLeft();

    void test_floatToFloat();
    void test_floatToFixed();
    void test_fixedToFloat();

    void test_subnormalToFloat();
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

void PointConversorTest::test_fixedOtherPoint()
{
    QCOMPARE(conversor.inputFixed8("0101.0011", PointConversor::UNSIGNED).outputFixed64(5, PointConversor::UNSIGNED),
             "00000000000000000000000000000000000000000000000000000000101.00110");
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

QTEST_APPLESS_MAIN(PointConversorTest)

#include "tst_pointconversortest.moc"
