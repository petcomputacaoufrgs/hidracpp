#include <QtTest>

#include "neandermachine.h"

class NeanderMachineTest : public QObject
{

    Q_OBJECT

private slots:
    void init();
    void initTestCase(); //Carrega o programa teste
    
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
    NeanderMachine TestedMachine;
    const QString memfile_filename = "TestNeander.mem";

};

void NeanderMachineTest::initTestCase()
{
    TestedMachine.importMemory(memfile_filename, 0, 256, 0);
}

void NeanderMachineTest::init()
{
    TestedMachine.clearFlags();
}

void NeanderMachineTest::test_STA()
{
    //Move PC para posição do STA
    //Chama step para executar
    //Checa se valor na memória está certo
    int memValue;
    
    TestedMachine.setPCValue(0);
    TestedMachine.setMemoryValue(130,5);
    TestedMachine.step();
    memValue = TestedMachine.getMemoryValue(130);
    QCOMPARE(memValue, 0);
}
void NeanderMachineTest::test_LDA()
{
    int regValue;
    // Set PC to a position with the instruction STA
    TestedMachine.setMemoryValue(132, 5);
    TestedMachine.setPCValue(4);
    // Step of the simulation
    TestedMachine.step();
    // Get register value from a memory address
    regValue = TestedMachine.getRegisterValue("AC");
    // Compare actual value with expected value
    QCOMPARE(regValue, 5);
}
void NeanderMachineTest::test_ADD()
{
    int regValue;
    // Set PC to a position with the instruction ADD
    TestedMachine.setMemoryValue(132, 5);
    TestedMachine.setPCValue(8);
    // Step of the simulation
    TestedMachine.step();
    // Get register value after the sum
    regValue = TestedMachine.getRegisterValue("AC");
    // Compare actual value with expected value
    QCOMPARE(regValue, 10);
}

void NeanderMachineTest::test_OR()
{
    int regValue;
    // Set Pc to a position with the instruction OR
    TestedMachine.setMemoryValue(132, 5);
    TestedMachine.setPCValue(12);
    // Step of the simultation
    TestedMachine.step();
    // Get register value after the step
    regValue = TestedMachine.getRegisterValue("AC");
    // Compare the value of the register with the expected value 
    QCOMPARE(regValue, 15);
}

void NeanderMachineTest::test_AND()
{
    int regValue;
    // Set Pc to a position with the instruction AND
    TestedMachine.setMemoryValue(132, 5);
    TestedMachine.setPCValue(16);
    // Step of the simulation
    TestedMachine.step();
    // Get register value after the step
    regValue = TestedMachine.getRegisterValue("AC");
    // Compare the value of the register with the expected value
    QCOMPARE(regValue, 5);
}

void NeanderMachineTest::test_NOT()
{   
    int regValue;
    // Set PC value to a position with instruction NOT
    TestedMachine.setMemoryValue(132, 5);
    TestedMachine.setPCValue(20);
    // Step of the simulation
    TestedMachine.step();
    // Get register value after the logical operation NOT
    regValue = TestedMachine.getRegisterValue("AC");
    // Compare actual value with expected value
    QCOMPARE (regValue, 250);
}

void NeanderMachineTest::test_JMP()
{   
    int pcValue;
    // Set PC value to a position with instruction JMP
    TestedMachine.setPCValue(23);
    // Step of the simulation
    TestedMachine.step();
    // Get PC value
    pcValue = TestedMachine.getPCValue();
    // Compare the PC value with the expected value
    QCOMPARE (pcValue, 18);

}

void NeanderMachineTest::test_JN()
{   
    int pcValue;
    TestedMachine.setFlagValue("N", 1);
    // Set PC value to a position with instructon JN
    TestedMachine.setPCValue(27);
    // Step of the simulation
    TestedMachine.step();
    // Get PC value after jump
    pcValue = TestedMachine.getPCValue();
    // Compare actual value with expected value
    QCOMPARE (pcValue, 21);

}

void NeanderMachineTest::test_JZ()
{
    int memValue;
    TestedMachine.setFlagValue("Z", 1);
    // Set PC value to the instruction JZ
    TestedMachine.setPCValue(31);
    // Step of the simulation
    TestedMachine.step();
    // Get PC value 
    memValue = TestedMachine.getPCValue();
    // Compare PC Value with the expected value
    QCOMPARE (memValue, 24);

}

void NeanderMachineTest::test_HLT()
{
    
    // Set PC value to a position with instruction HLT
    TestedMachine.setPCValue(35);
    // Do a step of the simulation
    TestedMachine.step();
    // Compare actual value with expected value
    QCOMPARE(TestedMachine.isRunning(), false);
}

void NeanderMachineTest::test_NOP()
{
    // Set PC value to a position with instruction NOP
    TestedMachine.setPCValue(39);
    // Do a step of the simulation
    TestedMachine.step();
    // Compare actual value with expected value
    //QCOMPARE( , );
}

#include "tst_neander.moc"
QTEST_APPLESS_MAIN(NeanderMachineTest)

