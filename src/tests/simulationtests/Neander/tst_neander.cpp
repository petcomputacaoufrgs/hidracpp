#include <QtTest>

#include "simulationtestcase.h"
#include "neandermachine.h"

class NeanderMachineTest : public QObject
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
    void test_JMP();
    void test_JN();
    void test_JZ();
    void test_HLT();
    void test_NOP();

    private:
    NeanderMachine testedMachine;

};

void NeanderMachineTest::init()
{
    testedMachine.clearFlags();
    testedMachine.setPCValue(0);
    testedMachine.setRegisterValue("AC", 0);
    testedMachine.setRunning(true);
}

void NeanderMachineTest::test_STA()
{
    int value; 
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 16, 128);
    testedMachine.setMemoryValue(128,5);
    testedMachine.step();
    value = testedMachine.getMemoryValue(128);
    QCOMPARE(value, 0);
}
void NeanderMachineTest::test_LDA()
{
    int value; 
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 32, 128);
    testedMachine.setMemoryValue(128,5);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 5);
}
void NeanderMachineTest::test_ADD()
{
    int value; 
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 48, 128);
    testedMachine.setRegisterValue("AC", 7);
    testedMachine.setMemoryValue(128,5);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 12);
}

void NeanderMachineTest::test_OR()
{
    int value; 
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 64, 128);
    testedMachine.setRegisterValue("AC", 7);
    testedMachine.setMemoryValue(128,5);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 7);
}

void NeanderMachineTest::test_AND()
{
     int value; 
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 80, 128);
    testedMachine.setRegisterValue("AC", 7);
    testedMachine.setMemoryValue(128, 5);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 5);
}

void NeanderMachineTest::test_NOT()
{   
     int value; 
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 96, 0);
    testedMachine.setRegisterValue("AC", 7);
    testedMachine.step();
    value = testedMachine.getRegisterValue("AC");
    QCOMPARE(value, 248);
}

void NeanderMachineTest::test_JMP()
{   
    int value; 
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 128, 30);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 30);
}

void NeanderMachineTest::test_JN()
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

void NeanderMachineTest::test_JZ()
{
    int value; 
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 160, 30);
    testedMachine.setRegisterValue("AC", 7);
    testedMachine.updateFlags(7);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 2);

    testedMachine.setRegisterValue("PC", 0);
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 160, 30);
    testedMachine.setRegisterValue("AC", 0);
    testedMachine.updateFlags(0);
    testedMachine.step();
    value = testedMachine.getRegisterValue("PC");
    QCOMPARE(value, 30);

}

void NeanderMachineTest::test_HLT()
{
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 240, 0);
    testedMachine.step();
    QCOMPARE(testedMachine.isRunning(), false);
}

void NeanderMachineTest::test_NOP()
{
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 0, 0);
    testedMachine.step();
    QCOMPARE(testedMachine.isRunning(), true);
}

#include "tst_neander.moc"
QTEST_APPLESS_MAIN(NeanderMachineTest)

