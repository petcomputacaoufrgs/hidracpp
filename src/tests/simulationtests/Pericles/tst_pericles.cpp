#include <QtTest>

#include "simulationtestcase.h"
#include "periclesmachine.h"

class PericlesMachineTest : public QObject
{

    Q_OBJECT

private slots:
    void test_calculateBytesToReserve();
    void test_assemblerReserve();

private:
    PericlesMachine testedMachine;
};

void PericlesMachineTest::test_calculateBytesToReserve()
{
    //Instruction with fixed size of 1 bytes
    Instruction i1 = Instruction(1, "0000....", Instruction::NOP, "nop");
    //Instruction with variable size
    Instruction i2 = Instruction(0, "0001....", Instruction::STR, "str r a");
    //Last parameter determines byte size
    //Immediate takes two bytes, while other addressing modes take three
    QString param1 = "#128";
    QString param2 = ",i";
    QStringList args1 = QStringList(param1);
    QStringList args2 = QStringList(param2);
    
    QCOMPARE(testedMachine.calculateBytesToReserve(&i1, args1), 1);
    QCOMPARE(testedMachine.calculateBytesToReserve(&i1, args2), 1);
    QCOMPARE(testedMachine.calculateBytesToReserve(&i2, args1), 2);
    QCOMPARE(testedMachine.calculateBytesToReserve(&i2, args2), 3);
}

//TO-DO: Add this and other assembler related tests to all machine test suites (maybe through inheritance?)
void PericlesMachineTest::test_assemblerReserve()
{
    QString testSourceCode1 = "STR A 128";
    QString testSourceCode2 = "STR A #128";
    QString testSourceCode3 = "STR A 128,i";
    QString testSourceCode4 = "STR A 128,x";

    testedMachine.assemble(testSourceCode1);
    QCOMPARE(testedMachine.isAssemblerMemoryReserved(0), true);
    QCOMPARE(testedMachine.isAssemblerMemoryReserved(1), true);
    QCOMPARE(testedMachine.isAssemblerMemoryReserved(2), true);

    testedMachine.assemble(testSourceCode2);
    QCOMPARE(testedMachine.isAssemblerMemoryReserved(0), true);
    QCOMPARE(testedMachine.isAssemblerMemoryReserved(1), true);
    QCOMPARE(testedMachine.isAssemblerMemoryReserved(2), false);

    testedMachine.assemble(testSourceCode3);
    QCOMPARE(testedMachine.isAssemblerMemoryReserved(0), true);
    QCOMPARE(testedMachine.isAssemblerMemoryReserved(1), true);
    QCOMPARE(testedMachine.isAssemblerMemoryReserved(2), true);

    testedMachine.assemble(testSourceCode4);
    QCOMPARE(testedMachine.isAssemblerMemoryReserved(0), true);
    QCOMPARE(testedMachine.isAssemblerMemoryReserved(1), true);
    QCOMPARE(testedMachine.isAssemblerMemoryReserved(2), true);

}

#include "tst_pericles.moc"
QTEST_APPLESS_MAIN(PericlesMachineTest)