#include <QtTest>

#include "simulationtestcase.h"
#include "cesarmachine.h"

class CesarMachineTest : public QObject
{

    Q_OBJECT

private slots:
    void init();

    // Execution tests

    // GROUP_NOP
    // GROUP_CONDITIONAL_CODES
    // GROUP_CONDITIONAL_BRANCHES
    // GROUP_JUMP
    // GROUP_LOOP_CONTROL
    // GROUP_JUMP_SUBROUTINE
    // GROUP_RETURN_SUBROUTINE
    // GROUP_ONE_OPERAND
    // GROUP_TWO_OPERAND
    void test_MOV();


    private:
    CesarMachine testedMachine;

};

void CesarMachineTest::init()
{
    testedMachine.clearFlags();
    testedMachine.setPCValue(0);
    testedMachine.setRegisterValue("R0", 0);
    testedMachine.setRegisterValue("R1", 0);
    testedMachine.setRegisterValue("R2", 0);
    testedMachine.setRegisterValue("R3", 0);
    testedMachine.setRegisterValue("R4", 0);
    testedMachine.setRegisterValue("R5", 0);
    testedMachine.setRegisterValue("SP", 0);
    testedMachine.setRegisterValue("PC", 0);
    testedMachine.setRunning(true);
}

void CesarMachineTest::test_MOV()
{
    testedMachine.setRegisterValue("R0", 5);
    testedMachine.setMemoryValue(0, 0b10010000);
    testedMachine.setMemoryValue(1, 0b00000001);
    testedMachine.step();
    QCOMPARE(testedMachine.getRegisterValue("R1"), 5);

}

#include "tst_cesar.moc"
QTEST_APPLESS_MAIN(CesarMachineTest)