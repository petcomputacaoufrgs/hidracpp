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
    QCOMPARE(conversor.inputFixed8("0101.0011").outputFixed64(4),
             "000000000000000000000000000000000000000000000000000000000101.0011");

    QCOMPARE(conversor.inputFixed64("0101.0011").outputFixed8(4), "0101.0011");
}

void PointConversorTest::test_fixedOtherPoint()
{
    QCOMPARE(conversor.inputFixed8("0101.0011").outputFixed64(5),
             "00000000000000000000000000000000000000000000000000000000101.00110");
}

void PointConversorTest::test_fixedZerosRight()
{
    QCOMPARE(conversor.inputFixed64("0000000000000000000000000000000000000000000000000000101.001100000").outputFixed8(4),
             "0101.0011");
}

void PointConversorTest::test_fixedZerosLeft()
{
    QCOMPARE(conversor.inputFixed8("00000.01010011").outputFixed64(9),
             "0000000000000000000000000000000000000000000000000000000.010100110");
}

void PointConversorTest::test_floatToFloat()
{
    QCOMPARE(conversor.inputDoubleFloat("0011111110011010100000000000000000000000000000000000000000000000").outputSingleFloat(),
             "00111100110101000000000000000000");

    QCOMPARE(conversor.inputSingleFloat("00111101001010010000000000000000").outputSingleFloat(),
             "0011111110100101001000000000000000000000000000000000000000000000");

    QCOMPARE(conversor.inputSingleFloat("01000010010000000110000000000000").outputHalfFloat(),
             "0101001000000011");
}

QTEST_APPLESS_MAIN(PointConversorTest)

#include "tst_pointconversortest.moc"
