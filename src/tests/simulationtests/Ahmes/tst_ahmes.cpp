#include <QtTest>

#include "simulationtestcase.h"
#include "ahmesmachine.h"

class AhmesMachineTest : public QObject
{

    Q_OBJECT

private slots:

    void init();

    // Execution tests
    void test_STA();
    void test_LDA();
    void test_ADD();
    void test_OR();
    void test_AND();
    void test_NOT();
    void test_SUB();
    void test_JMP();
    void test_JN();
    void test_JP();
    void test_JV();
    void test_JNV();
    void test_JZ();
    void test_JNZ();
    void test_JC();
    void test_JNC();
    void test_JB();
    void test_JNB();
    void test_SHR();
    void test_SHL();
    void test_ROR();
    void test_ROL();
    void test_HLT();
    void test_NOP();

private:
    AhmesMachine testedMachine;
};

void AhmesMachineTest::init()
{
    testedMachine.clearFlags();
    testedMachine.setPCValue(0);
    testedMachine.setRegisterValue("AC", 0);
    testedMachine.setRunning(true);
}

void AhmesMachineTest::test_STA()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 16, 128);
    testedMachine.setMemoryValue(128, 5);
    testedMachine.step();
    value = testedMachine.getMemoryValue(128);
    QCOMPARE(value, 0);
}

void AhmesMachineTest::test_LDA()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 32, 128);
    testedMachine.setMemoryValue(128, 5);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 5);
}
void AhmesMachineTest::test_ADD()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 48, 128);
    testedMachine.setRegisterValue("AC", 7);
    testedMachine.setMemoryValue(128, 5);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 12);
}

void AhmesMachineTest::test_OR()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 64, 128);
    testedMachine.setRegisterValue("AC", 7);
    testedMachine.setMemoryValue(128, 5);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 7);
}

void AhmesMachineTest::test_AND()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 80, 128);
    testedMachine.setRegisterValue("AC", 7);
    testedMachine.setMemoryValue(128, 5);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 5);
}

void AhmesMachineTest::test_NOT()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 96, 0);
    testedMachine.setRegisterValue("AC", 7);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 248);
}

void AhmesMachineTest::test_JMP()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 128, 30);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 30);
}

void AhmesMachineTest::test_JN()
{
    int value; 
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 144, 30);
    testedMachine.setRegisterValue("AC", 7);
    testedMachine.updateFlags(7);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 2);

    testedMachine.setRegisterValue("PC", 0);
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 144, 30);
    testedMachine.setRegisterValue("AC", 248);
    testedMachine.updateFlags(248);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 30);

}

void AhmesMachineTest::test_HLT()
{
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 240, 0);
    testedMachine.step();
    QCOMPARE(testedMachine.isRunning(), false);
}

void AhmesMachineTest::test_NOP()
{
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 0, 0);
    testedMachine.step();
    QCOMPARE(testedMachine.isRunning(), true);
}

void AhmesMachineTest::test_SUB()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 112, 132);
    testedMachine.setMemoryValue(132, 5);
    testedMachine.setRegisterValue("AC", 5);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 0);
}

void AhmesMachineTest::test_JP()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 148, 4);
    testedMachine.setRegisterValue("AC", 7);
    testedMachine.updateFlags(7);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 4);

    testedMachine.setRegisterValue("PC", 0);
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 148, 4);
    testedMachine.setRegisterValue("AC", 255);
    testedMachine.updateFlags(255);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 2);
}

void AhmesMachineTest::test_JV() // teste FODA    -v, j 01/11/2022
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 152, 4);
    testedMachine.setRegisterValue("AC", 7);
    testedMachine.setFlagValue("V", 1);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 4);
}

void AhmesMachineTest::test_JNV()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 156, 4);
    testedMachine.setRegisterValue("AC", 256);
    testedMachine.setFlagValue("V", 0);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 4);
}

void AhmesMachineTest::test_JZ()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 160, 4);
    testedMachine.setRegisterValue("AC", 256);
    testedMachine.setFlagValue("Z", 1);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 4);
}

void AhmesMachineTest::test_JNZ()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 164, 4);
    testedMachine.setRegisterValue("AC", 256);
    testedMachine.setFlagValue("Z", 0);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 4);
}

void AhmesMachineTest::test_JC()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 176, 4);
    testedMachine.setRegisterValue("AC", 256);
    testedMachine.setFlagValue("C", 1);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 4);
}

void AhmesMachineTest::test_JNC()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 180, 4);
    testedMachine.setRegisterValue("AC", 256);
    testedMachine.setFlagValue("C", 0);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 4);
}

void AhmesMachineTest::test_JB()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 184, 4);
    testedMachine.setRegisterValue("AC", 256);
    testedMachine.setFlagValue("B", 1);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 4);
}

void AhmesMachineTest::test_JNB()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 188, 4);
    testedMachine.setRegisterValue("AC", 256);
    testedMachine.setFlagValue("B", 0);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 4);
}

void AhmesMachineTest::test_SHR()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 224, 0);
    testedMachine.setRegisterValue("AC", 5);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 2);
}

void AhmesMachineTest::test_SHL()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 225, 0);
    testedMachine.setRegisterValue("AC", 5);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 10);
}

void AhmesMachineTest::test_ROR()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 226, 0);
    testedMachine.setRegisterValue("AC", 128);
    testedMachine.setFlagValue("C", 1);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 192);
}

void AhmesMachineTest::test_ROL()
{
    int value;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 227, 0);
    testedMachine.setRegisterValue("AC", 128);
    testedMachine.setFlagValue("C", 1);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 1);
}

#include "tst_ahmes.moc"
QTEST_APPLESS_MAIN(AhmesMachineTest)