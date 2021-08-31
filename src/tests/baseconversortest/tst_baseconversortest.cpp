#include <QtTest>

#include "baseconversor.h"
#include "invalidconversorinput.h"

class BaseConversorTest : public QObject
{
    Q_OBJECT

public:
    BaseConversorTest();
    ~BaseConversorTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_positiveToSignMagnitudeBaseTwo();
    void test_positiveToOnesComplBaseTwo();
    void test_positiveToTwosComplBaseTwo();

    void test_signMagnitudeToPositiveBaseTwo();
    void test_signMagnitudeToOnesComplBaseTwo();
    void test_signMagnitudeToTwosComplBaseTwo();

    void test_onesComplToPositiveBaseTwo();
    void test_onesComplToTwosComplBaseTwo();
    void test_onesComplToSignMagnitudeBaseTwo();

    void test_twosComplToPositiveBaseTwo();
    void test_twosComplToOnesComplBaseTwo();
    void test_twosComplToSignMagnitudeBaseTwo();

    void test_positiveBaseTwoToDec();
    void test_positiveBaseTwoToThree();
    void test_positiveBaseHexToFive();

    void test_signMagnitudeBaseTenToTwelve();
    void test_signMagnitudeBaseTwoToFive();

    void test_onesComplBaseFourToSix();
    void test_onesComplBaseSevenToTen();

    void test_twosComplBaseThreeToFive();
    void test_twosComplBaseEightToEleven();

    void test_onesComplLimitsEven();
    void test_onesComplLimitsOdd();

    void test_twosComplLimitsEven();
    void test_twosComplLimitsOdd();

    void test_signMagnitBinToTwosComplThirteen();
    void test_onesComplThirteenToSignMagnitBin();

    void test_badBase();
    void test_badDigit();
    void test_empty();

    void test_badSize();
    void test_badSizeToCompl();

    void test_signMagToOnesComplWidth();
    void test_signMagToTwosComplWidth();
private:
    BaseConversor conversor;
};

BaseConversorTest::BaseConversorTest()
{

}

BaseConversorTest::~BaseConversorTest()
{

}

void BaseConversorTest::initTestCase()
{
}

void BaseConversorTest::cleanupTestCase()
{
}

void BaseConversorTest::test_positiveToSignMagnitudeBaseTwo()
{
    QString output = conversor.inputPositive("100", 2).outputSignMagnitude(2, 4);
    QCOMPARE(output, "00100");
}

void BaseConversorTest::test_positiveToOnesComplBaseTwo()
{
    QString output = conversor.inputPositive("101", 2).outputOnesComplement(2, 4);
    QCOMPARE(output, "0101");
}

void BaseConversorTest::test_positiveToTwosComplBaseTwo()
{
    QString output = conversor.inputPositive("101", 2).outputOnesComplement(2, 4);
    QCOMPARE(output, "0101");
}

void BaseConversorTest::test_signMagnitudeToOnesComplBaseTwo()
{
    QString output = conversor.inputSignMagnitude("1010", 2).outputOnesComplement(2, 3);
    QCOMPARE(output, "101");

    output = conversor.inputSignMagnitude("0010", 2).outputOnesComplement(2, 3);
    QCOMPARE(output, "010");

    output = conversor.inputSignMagnitude("0001", 2).outputOnesComplement(2, 5);
    QCOMPARE(output, "00001");

    output = conversor.inputSignMagnitude("1001", 2).outputOnesComplement(2, 5);
    QCOMPARE(output, "11110");
}

void BaseConversorTest::test_signMagnitudeToTwosComplBaseTwo()
{
    QString output = conversor.inputSignMagnitude("1001", 2).outputTwosComplement(2, 3);
    QCOMPARE(output, "111");

    output = conversor.inputSignMagnitude("0001", 2).outputTwosComplement(2, 3);
    QCOMPARE(output, "001");

    output = conversor.inputSignMagnitude("0001", 2).outputTwosComplement(2, 5);
    QCOMPARE(output, "00001");

    output = conversor.inputSignMagnitude("1001", 2).outputTwosComplement(2, 5);
    QCOMPARE(output, "11111");
}

void BaseConversorTest::test_signMagnitudeToPositiveBaseTwo()
{
    QString output = conversor.inputSignMagnitude("00100", 2).outputPositive(2, 4);
    QCOMPARE(output, "0100");
}

void BaseConversorTest::test_onesComplToTwosComplBaseTwo()
{
    QString output = conversor.inputOnesComplement("1100", 2).outputTwosComplement(2, 6);
    QCOMPARE(output, "111101");

    output = conversor.inputOnesComplement("0100", 2).outputTwosComplement(2, 6);
    QCOMPARE(output, "000100");

    output = conversor.inputOnesComplement("1110", 2).outputTwosComplement(2, 3);
    QCOMPARE(output, "111");

    output = conversor.inputOnesComplement("000010", 2).outputTwosComplement(2, 3);
    QCOMPARE(output, "010");
}

void BaseConversorTest::test_onesComplToSignMagnitudeBaseTwo()
{
    QString output = conversor.inputOnesComplement("101", 2).outputSignMagnitude(2, 4);
    QCOMPARE(output, "10010");

    output = conversor.inputOnesComplement("010", 2).outputSignMagnitude(2, 4);
    QCOMPARE(output, "00010");

    output = conversor.inputOnesComplement("00010", 2).outputSignMagnitude(2, 4);
    QCOMPARE(output, "00010");

    output = conversor.inputOnesComplement("11101", 2).outputSignMagnitude(2, 4);
    QCOMPARE(output, "10010");
}

void BaseConversorTest::test_onesComplToPositiveBaseTwo()
{
    QString output = conversor.inputOnesComplement("00110", 2).outputPositive(2, 3);
    QCOMPARE(output, "110");
}

void BaseConversorTest::test_twosComplToSignMagnitudeBaseTwo()
{
    QString output = conversor.inputTwosComplement("111", 2).outputSignMagnitude(2, 4);
    QCOMPARE(output, "10001");

    output = conversor.inputTwosComplement("001", 2).outputSignMagnitude(2, 4);
    QCOMPARE(output, "00001");

    output = conversor.inputTwosComplement("00001", 2).outputSignMagnitude(2, 4);
    QCOMPARE(output, "00001");

    output = conversor.inputTwosComplement("11111", 2).outputSignMagnitude(2, 4);
    QCOMPARE(output, "10001");
}

void BaseConversorTest::test_twosComplToOnesComplBaseTwo()
{
    QString output = conversor.inputTwosComplement("111101", 2).outputOnesComplement(2, 4);
    QCOMPARE(output, "1100");

    output = conversor.inputTwosComplement("000100", 2).outputOnesComplement(2, 4);
    QCOMPARE(output, "0100");

    output = conversor.inputTwosComplement("111", 2).outputOnesComplement(2, 4);
    QCOMPARE(output, "1110");

    output = conversor.inputOnesComplement("010", 2).outputTwosComplement(2, 6);
    QCOMPARE(output, "000010");
}

void BaseConversorTest::test_twosComplToPositiveBaseTwo()
{
    QString output = conversor.inputOnesComplement("011", 2).outputPositive(2, 5);
    QCOMPARE(output, "00011");
}

void BaseConversorTest::test_positiveBaseTwoToDec()
{
    QString output = conversor.inputPositive("11011", 2).outputPositive(10, 2);
    QCOMPARE(output, "27");

    output = conversor.inputPositive("11010", 2).outputPositive(10, 6);
    QCOMPARE(output, "000026");
}

void BaseConversorTest::test_positiveBaseTwoToThree()
{
    QString output = conversor.inputPositive("100", 2).outputPositive(3, 5);
    QCOMPARE(output, "00011");

    output = conversor.inputPositive("00101", 2).outputPositive(3, 2);
    QCOMPARE(output, "12");
}

void BaseConversorTest::test_positiveBaseHexToFive()
{
    QString output = conversor.inputPositive("6b9f", 16).outputPositive(5, 9);
    QCOMPARE(output, "001340201");

    output = conversor.inputPositive("cafebabe", 16).outputPositive(5, 17);
    QCOMPARE(output, "00023433324112312");
}

void BaseConversorTest::test_signMagnitudeBaseTenToTwelve()
{
    QString output = conversor.inputSignMagnitude("0095", 10).outputSignMagnitude(12, 5);
    QCOMPARE(output, "00007B");

    output = conversor.inputSignMagnitude("1095", 10).outputSignMagnitude(12, 5);
    QCOMPARE(output, "10007B");
}

void BaseConversorTest::test_signMagnitudeBaseTwoToFive()
{
    QString output = conversor.inputSignMagnitude("0010", 2).outputSignMagnitude(5, 2);
    QCOMPARE(output, "002");

    output = conversor.inputSignMagnitude("1010", 2).outputSignMagnitude(5, 2);
    QCOMPARE(output, "102");
}

void BaseConversorTest::test_onesComplBaseFourToSix()
{
    QString output = conversor.inputOnesComplement("1302", 4).outputOnesComplement(6, 1);
    QCOMPARE(output, "0310");

    output = conversor.inputOnesComplement("2311", 4).outputOnesComplement(6, 4);
    QCOMPARE(output, "5353");
}

void BaseConversorTest::test_onesComplBaseSevenToTen()
{
    QString output = conversor.inputOnesComplement("0613", 7).outputOnesComplement(10, 5);
    QCOMPARE(output, "00304");

    output = conversor.inputOnesComplement("5162", 7).outputOnesComplement(10, 5);
    QCOMPARE(output, "99407");
}

void BaseConversorTest::test_twosComplBaseThreeToFive()
{
    QString output = conversor.inputTwosComplement("102211", 3).outputOnesComplement(5, 2);
    QCOMPARE(output, "02234");

    output = conversor.inputOnesComplement("2001", 3).outputOnesComplement(5, 5);
    QCOMPARE(output, "44344");
}

void BaseConversorTest::test_twosComplBaseEightToEleven()
{
    QString output = conversor.inputTwosComplement("307674", 8).outputTwosComplement(11, 2);
    QCOMPARE(output, "06A97A");

    output = conversor.inputTwosComplement("507674", 8).outputTwosComplement(11, 8);
    QCOMPARE(output, "AAA46195");
}

void BaseConversorTest::test_onesComplLimitsEven()
{
    QString output = conversor.inputOnesComplement("333", 4).outputSignMagnitude(10, 3);
    QCOMPARE(output, "1000");

    output = conversor.inputOnesComplement("332", 4).outputSignMagnitude(10, 3);
    QCOMPARE(output, "1001");

    output = conversor.inputOnesComplement("000", 4).outputSignMagnitude(10, 3);
    QCOMPARE(output, "0000");

    output = conversor.inputOnesComplement("133", 4).outputSignMagnitude(10, 3);
    QCOMPARE(output, "0031");

    output = conversor.inputOnesComplement("200", 4).outputSignMagnitude(10, 3);
    QCOMPARE(output, "1031");
}

void BaseConversorTest::test_onesComplLimitsOdd()
{
    QString output = conversor.inputOnesComplement("222", 3).outputSignMagnitude(10, 3);
    QCOMPARE(output, "1000");

    output = conversor.inputOnesComplement("221", 3).outputSignMagnitude(10, 3);
    QCOMPARE(output, "1001");

    output = conversor.inputOnesComplement("000", 3).outputSignMagnitude(10, 3);
    QCOMPARE(output, "0000");

    output = conversor.inputOnesComplement("110", 3).outputSignMagnitude(10, 3);
    QCOMPARE(output, "0012");

    output = conversor.inputOnesComplement("111", 3).outputSignMagnitude(10, 3);
    QCOMPARE(output, "1013");
}

void BaseConversorTest::test_twosComplLimitsEven()
{
    QString output = conversor.inputTwosComplement("333", 4).outputSignMagnitude(10, 3);
    QCOMPARE(output, "1001");

    output = conversor.inputTwosComplement("000", 4).outputSignMagnitude(10, 3);
    QCOMPARE(output, "0000");

    output = conversor.inputTwosComplement("133", 4).outputSignMagnitude(10, 3);
    QCOMPARE(output, "0031");

    output = conversor.inputTwosComplement("200", 4).outputSignMagnitude(10, 3);
    QCOMPARE(output, "1032");
}

void BaseConversorTest::test_twosComplLimitsOdd()
{
    QString output = conversor.inputTwosComplement("222", 3).outputSignMagnitude(10, 3);
    QCOMPARE(output, "1001");

    output = conversor.inputTwosComplement("000", 3).outputSignMagnitude(10, 3);
    QCOMPARE(output, "0000");

    output = conversor.inputTwosComplement("111", 3).outputSignMagnitude(10, 3);
    QCOMPARE(output, "0013");

    output = conversor.inputTwosComplement("112", 3).outputSignMagnitude(10, 3);
    QCOMPARE(output, "1013");
}

void BaseConversorTest::test_signMagnitBinToTwosComplThirteen()
{
    QString output = conversor.inputSignMagnitude("1001011", 2).outputTwosComplement(13, 4);
    QCOMPARE(output, "CCC2");
}

void BaseConversorTest::test_onesComplThirteenToSignMagnitBin()
{
    QString output =  conversor.inputOnesComplement("CCC1", 13).outputSignMagnitude(2, 6);
    QCOMPARE(output, "1001011");
}

void BaseConversorTest::test_badBase()
{
    QVERIFY_EXCEPTION_THROWN(conversor.inputOnesComplement("1", 37), InvalidConversorInput);
    QVERIFY_EXCEPTION_THROWN(conversor.inputPositive("1", 1), InvalidConversorInput);
    QVERIFY_EXCEPTION_THROWN(conversor.inputSignMagnitude("100", 2).outputPositive(27, 2), InvalidConversorInput);
    QVERIFY_EXCEPTION_THROWN(conversor.inputTwosComplement("1", 2).outputPositive(1, 2), InvalidConversorInput);
}

void BaseConversorTest::test_badDigit()
{
    QVERIFY_EXCEPTION_THROWN(conversor.inputTwosComplement("11012", 2), InvalidConversorInput);
    QVERIFY_EXCEPTION_THROWN(conversor.inputOnesComplement("885b1f", 15), InvalidConversorInput);
}

void BaseConversorTest::test_empty()
{
    QVERIFY_EXCEPTION_THROWN(conversor.inputPositive("", 2), InvalidConversorInput);
    QVERIFY_EXCEPTION_THROWN(conversor.inputSignMagnitude("1", 10), InvalidConversorInput);
    QVERIFY_EXCEPTION_THROWN(conversor.inputSignMagnitude("0", 20), InvalidConversorInput);
}

void BaseConversorTest::test_badSize()
{
    QVERIFY_EXCEPTION_THROWN(conversor.inputPositive("zzzzzzzzzzzzzzzz", 37), InvalidConversorInput);
}

void BaseConversorTest::test_badSizeToCompl()
{
    QVERIFY_EXCEPTION_THROWN(conversor.inputPositive("ffff ffff ffff ffff", 16).outputTwosComplement(2, 64), InvalidConversorInput);
    QVERIFY_EXCEPTION_THROWN(conversor.inputPositive("ffff ffff ffff ffff", 16).outputOnesComplement(3, 64), InvalidConversorInput);
}

void BaseConversorTest::test_signMagToOnesComplWidth()
{
    QString output = conversor.inputSignMagnitude("111122", 3).outputOnesComplement(2, 3);
    QCOMPARE(output, "10000010");
}

void BaseConversorTest::test_signMagToTwosComplWidth()
{
    QString output = conversor.inputSignMagnitude("111122", 3).outputTwosComplement(2, 3);
    QCOMPARE(output, "10000011");
}

QTEST_APPLESS_MAIN(BaseConversorTest)

#include "tst_baseconversortest.moc"
