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
    void test_MOV_data();
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

void CesarMachineTest::test_MOV_data()
{
    QTest::addColumn<int>("src");
    QTest::addColumn<int>("dst");
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("dst_value");
    QTest::addColumn<int>("expected_src");
    QTest::addColumn<int>("expected_dst");
    QTest::addColumn<int>("checked_address");
    QTest::addColumn<int>("expected_address_value");
    QTest::addColumn<int>("addressing_mode_1");
    QTest::addColumn<int>("addressing_mode_2");
                                        // src  dst    src_v    dst_v   exp_src exp_dst ch_add  exp_add am1     am2
    QTest::newRow("Register-Register")   << 0   << 1    << 5    << 0    << 5    << 5    << -1   << -1   << 0    << 0;
    //MOV R0 (R1)+ (R0 = 5, R1 = 0) ->
    //MEM(R1) = REG(R0); R1+=2;
    QTest::newRow("Register-Post-Inc")   << 0   << 1    << 5    << 14   << 5    << 16   << 15   << 5    << 0    << 1;
    //MOV R0 -(R1) (R0 = 5, R1 = 0) -> 
    //R1-=2; MEM(R1) = REG(R0);
    QTest::newRow("Register-Pre-Dec")    << 0   << 1    << 5    << 16   << 5    << 14   << 15   << 5    << 0    << 2;

}

void CesarMachineTest::test_MOV()
{
    QFETCH(int, src);
    QFETCH(int, dst);
    QFETCH(int, src_value);
    QFETCH(int, dst_value);
    QFETCH(int, expected_src);
    QFETCH(int, expected_dst);
    QFETCH(int, checked_address);
    QFETCH(int, expected_address_value);   
    QFETCH(int, addressing_mode_1);
    QFETCH(int, addressing_mode_2);
    
    testedMachine.setRegisterValue(src, src_value);
    testedMachine.setRegisterValue(dst, dst_value);
    int opcode = 0b1001000000000000;
    opcode |= src << 6;
    opcode |= addressing_mode_1 << 9;
    opcode |= dst;
    opcode |= addressing_mode_2 << 3;
    testedMachine.memoryWriteTwoByte(0, opcode);
    testedMachine.step();
    
    QCOMPARE(testedMachine.getRegisterValue(src), expected_src);
    
    if(checked_address == -1)
    {
        QCOMPARE(testedMachine.getRegisterValue(dst), expected_dst);
    }
    else{
        QCOMPARE(testedMachine.getMemoryValue(checked_address), expected_address_value);
    }
    

}

#include "tst_cesar.moc"
QTEST_APPLESS_MAIN(CesarMachineTest)