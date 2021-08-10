#include <QtTest>

#include "baseconversor.h"

class BaseConversorTest : public QObject
{
    Q_OBJECT

public:
    BaseConversorTest();
    ~BaseConversorTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_positiveBaseTwoToDec();

    void test_signMagnitudeToTwosComplBaseTwo();
    void test_signMagnitudeToOnesComplBaseTwo();

    void test_onesComplToTwosComplBaseTwo();

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

void BaseConversorTest::test_positiveBaseTwoToDec()
{
    QString output = conversor.inputPositive("11011", 2).outputPositive(10, 2);
    QCOMPARE(output, "27");
}

void BaseConversorTest::test_signMagnitudeToTwosComplBaseTwo()
{
    QString output = conversor.inputSignMagnitude("1001", 2).outputTwosComplement(2, 3);
    QCOMPARE(output, "111");

    output = conversor.inputSignMagnitude("0001", 2).outputTwosComplement(2, 3);
    QCOMPARE(output, "001");
}

void BaseConversorTest::test_signMagnitudeToOnesComplBaseTwo()
{
    QString output = conversor.inputSignMagnitude("1010", 2).outputOnesComplement(2, 3);
    QCOMPARE(output, "101");
    output = conversor.inputSignMagnitude("0010", 2).outputOnesComplement(2, 3);
    QCOMPARE(output, "010");
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

QTEST_APPLESS_MAIN(BaseConversorTest)

#include "tst_baseconversortest.moc"
