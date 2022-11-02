#include <QtTest>

#include "simulationtestcase.h"
#include "ramsesmachine.h"

class RamsesMachineTest : public QObject
{

    Q_OBJECT

private slots:
    
    void init();
    
    // Execution tests
    void test_STR();
    void test_LDR();
    void test_ADD();
    void test_OR();
    void test_AND();
    void test_NOT();
    void test_SUB();
    void test_JMP();
    void test_JN();
    void test_JZ();
    void test_JC();
    void test_jump_JSR();
    void test_return_address_JSR();
    void test_return_JSR();
    void test_NEG();
    void test_SHR();
    void test_HLT();
    void test_NOP();
    
    
private:
    RamsesMachine testedMachine;

};

void RamsesMachineTest::init()
{
    testedMachine.clearFlags();
    testedMachine.setPCValue(0);
    testedMachine.setRegisterValue("A", 0);
    testedMachine.setRegisterValue("B", 0);
    testedMachine.setRegisterValue("X", 0);
    testedMachine.setRunning(true);
}


void RamsesMachineTest::test_STR()
{
    int memValue;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 16, 130);
    testedMachine.setMemoryValue(130,5);
    testedMachine.step();
    memValue = testedMachine.getMemoryValue(130);
    QCOMPARE(memValue, 0);
}
void RamsesMachineTest::test_LDR()
{
    int regValue;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 32, 132);
    testedMachine.setMemoryValue(132, 5);
    testedMachine.step();
    // Get register value from the memory address
    regValue = testedMachine.getRegisterValue("A");
    QCOMPARE(regValue, 5);
}
void RamsesMachineTest::test_ADD()
{
    int regValue;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 48, 132);
    testedMachine.setMemoryValue(132, 5);
    testedMachine.setRegisterValue("A", 5);
    testedMachine.step();
    // Get register value after the sum
    regValue = testedMachine.getRegisterValue("A");
    QCOMPARE(regValue, 10);
}

void RamsesMachineTest::test_OR()
{
    int regValue;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 64, 132);
    testedMachine.setRegisterValue("A", 7);
    testedMachine.setMemoryValue(132, 5);
    testedMachine.step();
    // Get register value after logical operation "OR"
    regValue = testedMachine.getRegisterValue("A"); 
    QCOMPARE(regValue, 7);
}

void RamsesMachineTest::test_AND()
{
    int regValue;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 80, 132);
    testedMachine.setRegisterValue("A", 7);
    testedMachine.setMemoryValue(132, 5);
    testedMachine.step();
    // Get register value after logical operation "AND"
    regValue = testedMachine.getRegisterValue("A");
    QCOMPARE(regValue, 5);
}

void RamsesMachineTest::test_NOT()
{   
    int regValue;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 96, 0);
    testedMachine.step();
    // Get register value after the logical operation NOT
    regValue = testedMachine.getRegisterValue("A");
    QCOMPARE(regValue, 255);
}

void RamsesMachineTest::test_SUB()
{
    int regValue;
    int memValue;
    
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 112, 128);
    testedMachine.setMemoryValue(128, 15);
    testedMachine.setRegisterValue("A", 30);
    testedMachine.step();
    // Get memory value from the instruction
    memValue = testedMachine.getMemoryValue(128);
    // Subtract memory value from the register
    regValue = testedMachine.getRegisterValue("A");
    QCOMPARE(regValue, 15);

}

void RamsesMachineTest::test_JMP()
{   
    int pcValue;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 128, 128);
    testedMachine.step();
    // Get PC value after jump
    pcValue = testedMachine.getPCValue();
    QCOMPARE (pcValue, 128);

}

void RamsesMachineTest::test_JN()
{   
    int pcValue;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 144, 128);
    // Set flag N to 1
    testedMachine.setFlagValue("N", 1);
    testedMachine.step();
    // Get PC value after jump
    pcValue = testedMachine.getPCValue();
    QCOMPARE (pcValue, 128);

}

void RamsesMachineTest::test_JZ()
{
    int pcValue;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 160, 128);
    // Set flag Z to 1
    testedMachine.setFlagValue("Z", 1);
    testedMachine.step();
    // Get PC value after jump
    pcValue = testedMachine.getPCValue();
    QCOMPARE (pcValue, 128);
}

void RamsesMachineTest::test_JC()
{
    int pcValue;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 176, 128);
    // Set flag C to 1
    testedMachine.setFlagValue("C", 1);
    testedMachine.step();
    // Get PC value after jump
    pcValue = testedMachine.getPCValue();
    QCOMPARE (pcValue, 128);

}

void RamsesMachineTest::test_jump_JSR()
{
    int PCValue;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 192, 75);
    testedMachine.step();
    // Get PC value (address of the start of subroutine)
    PCValue = testedMachine.getPCValue();
    QCOMPARE(PCValue, 76);
}

void RamsesMachineTest::test_return_address_JSR()
{
    int returnAddress;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 192, 75);
    testedMachine.step();
    // Get memory value (beginning of subroutine, should have the return address in the first byte)
    returnAddress = testedMachine.getMemoryValue(testedMachine.getPCValue() - 1);
    QCOMPARE(returnAddress, 2);
}

void RamsesMachineTest::test_NEG()
{
    int regValue;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 208, 0);
    testedMachine.setRegisterValue("A", 1);
    testedMachine.step();
    regValue = testedMachine.getRegisterValue("A");
    QCOMPARE(regValue, 255);
}

void RamsesMachineTest::test_SHR()
{
    int regValue;
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 224, 0);
    testedMachine.setRegisterValue("A", 4);
    testedMachine.step();
    // Get register value (after shift right)
    regValue = testedMachine.getRegisterValue("A");
    QCOMPARE(regValue, 2);
}

void RamsesMachineTest::test_HLT()
{   
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 240, 0);
    testedMachine.step();
    QCOMPARE(testedMachine.isRunning(), false);
}

void RamsesMachineTest::test_NOP()
{
    PUT_INSTRUCTION_IN_MEMORY(testedMachine, 0, 0);
    testedMachine.step();
    QCOMPARE(testedMachine.isRunning(), true);
}

#include "tst_ramses.moc"
QTEST_APPLESS_MAIN(RamsesMachineTest)