#include <QtTest>

#include "simulationtestcase.h"
#include "cesarmachine.h"

#define MAX_POSITIVE 32767
#define MIN_NEGATIVE 32768

class CesarMachineTest : public QObject
{

    Q_OBJECT

private slots:
    void init();
    
    // Execution tests

    // GROUP_NOP
     void test_CESAR_NOP();
    // // GROUP_CONDITIONAL_CODES
    void test_CESAR_CCC();
    void test_CESAR_CCC_data();
    void test_CESAR_SCC();
    void test_CESAR_SCC_data(); 
    // // GROUP_CONDITIONAL_BRANCHES
    // void test_CESAR_BR();
    // void test_CESAR_BNE();
    // void test_CESAR_BEQ();
    // void test_CESAR_BPL();
    // void test_CESAR_BVC();
    // void test_CESAR_BVS();
    // void test_CESAR_BCC();
    // void test_CESAR_BCS();
    // void test_CESAR_BGE();
    // void test_CESAR_BLT();
    // void test_CESAR_BGT();
    // void test_CESAR_BLE();
    // void test_CESAR_BHI();
    // void test_CESAR_BLS();
    // // GROUP_JUMP
    // void test_CESAR_JMP();
    // // GROUP_LOOP_CONTROL
    // void test_CESAR_SOB();
    // // GROUP_JUMP_SUBROUTINE
    // void test_CESAR_JSR();
    // // GROUP_RETURN_SUBROUTINE
    // void test_CESAR_RTS();
    // // GROUP_ONE_OPERAND
    void test_CESAR_CLR_data();
    void test_CESAR_CLR();
    // void test_CESAR_NOT();
    void test_CESAR_INC_data();
    void test_CESAR_INC();
    void test_CESAR_DEC_data();
    void test_CESAR_DEC();
    // void test_CESAR_NEG();
    void test_CESAR_TST_data();
    void test_CESAR_TST();
    void test_CESAR_ROR();
    void test_CESAR_ROR_data();
    void test_CESAR_ROL();
    void test_CESAR_ROL_data();
    void test_CESAR_ASR();
    void test_CESAR_ASR_data();
    void test_CESAR_ASL();
    void test_CESAR_ASL_data();
    void test_CESAR_ADC();
    void test_CESAR_ADC_data();
    void test_CESAR_SBC();
    void test_CESAR_SBC_data();
    //gale end
    // GROUP_TWO_OPERAND
    void test_MOV_data();
    void test_MOV();
    void test_CESAR_ADD_data();
    void test_CESAR_ADD();
    void test_CESAR_SUB_data();
    void test_CESAR_SUB();
    void test_CESAR_AND_data();
    void test_CESAR_AND();
    void test_CESAR_OR_data();
    void test_CESAR_OR();
    void test_CESAR_CMP_data();
    void test_CESAR_CMP();

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
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("dst_value");
    QTest::addColumn<int>("dst_result");
    QTest::addColumn<int>("flag_N");
    QTest::addColumn<int>("flag_Z");
    QTest::addColumn<int>("flag_V");
    QTest::addColumn<int>("flag_C");
                                        
                            /*     src_val           dst_val  dst_res              N    Z    V   C         */
    QTest::newRow("No Flags")       << 5              << 0    << 5              << 0 << 0 << 0 << 0;
    QTest::newRow("N")              << MIN_NEGATIVE   << 0    << MIN_NEGATIVE   << 1 << 0 << 0 << 0;
    QTest::newRow("Z")              << 0              << 1    << 0              << 0 << 1 << 0 << 0;

}

void CesarMachineTest::test_MOV()
{
    QFETCH(int, src_value);
    QFETCH(int, dst_value);
    QFETCH(int, dst_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);

    
    testedMachine.setRegisterValue("R0", src_value);
    testedMachine.setRegisterValue("R1", dst_value);
    int opcode = 0b1001000000000000;
    opcode |= 1;
    testedMachine.memoryWriteTwoByte(0, opcode);
    testedMachine.step();
    
    QCOMPARE(testedMachine.getRegisterValue("R1"), dst_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);

}
void CesarMachineTest::test_CESAR_ADD_data()
{
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("dst_value");
    QTest::addColumn<int>("dst_result");
    QTest::addColumn<int>("flag_N");
    QTest::addColumn<int>("flag_Z");
    QTest::addColumn<int>("flag_V");
    QTest::addColumn<int>("flag_C");
                                /*   src_val  dst_val   dst_res     N      Z     V       C       */                              
    QTest::newRow("No Flags")       << 15      << 5     << 20    << 0   << 0   << 0    << 0;
    QTest::newRow("N")              << 40000   << 1000  << 41000 << 1   << 0   << 0    << 0;
    QTest::newRow("Z")              << 0       << 0     << 0     << 0   << 1   << 0    << 0;
    QTest::newRow("V_pos")          << 20000   << 20000 << 40000 << 1   << 0   << 1    << 0;
    QTest::newRow("C")              << 65535   << 5     << 4     << 0   << 0   << 0    << 1;
    QTest::newRow("V_neg")          << 33000   << 33000 << 464   << 0   << 0   << 1    << 1;
    
}

void CesarMachineTest::test_CESAR_ADD()
{
    QFETCH(int, src_value);
    QFETCH(int, dst_value);
    QFETCH(int, dst_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);
     
    int opcode = 0b1010000000000000;
    opcode |= 1;
    
    testedMachine.setRegisterValue("R0", src_value);
    testedMachine.setRegisterValue("R1", dst_value);
    testedMachine.memoryWriteTwoByte(0, opcode);
    testedMachine.step();

    QCOMPARE(testedMachine.getRegisterValue("R1"), dst_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);
}

void CesarMachineTest::test_CESAR_SUB_data()
{
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("dst_value");
    QTest::addColumn<int>("dst_result");
    QTest::addColumn<int>("flag_N");
    QTest::addColumn<int>("flag_Z");
    QTest::addColumn<int>("flag_V");
    QTest::addColumn<int>("flag_C");
                                    /*   src_val  dst_val   dst_res     N      Z     V       C       */                              
    QTest::newRow("Register-Register")   << 15      << 5     << 10    << 0  << 0   << 0    << 0;
    
} 

void CesarMachineTest::test_CESAR_SUB(){
    QFETCH(int, src_value);
    QFETCH(int, dst_value);
    QFETCH(int, dst_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);
    
    int opcode = 0b1011000000000000;
    opcode |= 1;
    
    testedMachine.setRegisterValue("R0", src_value);
    testedMachine.setRegisterValue("R1", dst_value);
    testedMachine.memoryWriteTwoByte(0, opcode);
    testedMachine.step();
    
    QCOMPARE(testedMachine.getRegisterValue("R1"), dst_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);
}

void CesarMachineTest::test_CESAR_CMP_data()
{
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("dst_value");
    QTest::addColumn<int>("dst_result");
    QTest::addColumn<int>("flag_N");
    QTest::addColumn<int>("flag_Z");
    QTest::addColumn<int>("flag_V");
    QTest::addColumn<int>("flag_C");
                                    /*   src_val  dst_val   dst_res     N      Z     V       C       */                              
    QTest::newRow("Register-Register")   << 1      << 1     << 1    << 0  << 1   << 0    << 0;
    
} 

void CesarMachineTest::test_CESAR_CMP(){
    QFETCH(int, src_value);
    QFETCH(int, dst_value);
    QFETCH(int, dst_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);

    int opcode = 0b1100000000000001;

    testedMachine.setRegisterValue("R0", src_value);
    testedMachine.setRegisterValue("R1", dst_value);
    testedMachine.memoryWriteTwoByte(0, opcode);
    testedMachine.step();

    QCOMPARE(testedMachine.getRegisterValue("R1"), dst_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);
}

void CesarMachineTest::test_CESAR_AND_data()
{
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("dst_value");
    QTest::addColumn<int>("dst_result");
    QTest::addColumn<int>("flag_N");
    QTest::addColumn<int>("flag_Z");
    QTest::addColumn<int>("flag_V");
    QTest::addColumn<int>("flag_C");
                                    /*   src_val  dst_val   dst_res     N      Z     V       C       */                              
    QTest::newRow("Register-Register")   << 128      << 1     << 0    << 0  << 1   << 0    << 0;
    
}

void CesarMachineTest::test_CESAR_AND()
{
    QFETCH(int, src_value);
    QFETCH(int, dst_value);
    QFETCH(int, dst_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);

    int opcode = 0b1101000000000000;
    opcode |= 1;

    testedMachine.setRegisterValue("R0", src_value);
    testedMachine.setRegisterValue("R1", dst_value);
    testedMachine.memoryWriteTwoByte(0, opcode);
    testedMachine.step();

    QCOMPARE(testedMachine.getRegisterValue("R1"), dst_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);

}

void CesarMachineTest::test_CESAR_OR_data()
{
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("dst_value");
    QTest::addColumn<int>("dst_result");
    QTest::addColumn<int>("flag_N");
    QTest::addColumn<int>("flag_Z");
    QTest::addColumn<int>("flag_V");
    QTest::addColumn<int>("flag_C");
                                    /*   src_val  dst_val   dst_res     N      Z     V       C       */                              
    QTest::newRow("Register-Register")   << 15   << 240     << 255    << 0  << 0   << 0    << 0;
    
}


void CesarMachineTest::test_CESAR_OR()
{
    QFETCH(int, src_value);
    QFETCH(int, dst_value);
    QFETCH(int, dst_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);

    int opcode = 0b1110000000000000;
    opcode |= 1;

    testedMachine.setRegisterValue("R0", src_value);
    testedMachine.setRegisterValue("R1", dst_value);
    testedMachine.memoryWriteTwoByte(0, opcode);
    testedMachine.step();

    QCOMPARE(testedMachine.getRegisterValue("R1"), dst_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);
    
}


void CesarMachineTest::test_CESAR_CLR_data()
{
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("dst_result");

                            /*      src_val dst_res  */
    QTest::newRow("Register")       << 15   << 0;

}


void CesarMachineTest::test_CESAR_CLR()
{
    QFETCH(int, src_value);
    QFETCH(int, dst_result);

    int opcode = 0b1000000000000000;
    testedMachine.setRegisterValue("R0", src_value);
    testedMachine.memoryWriteTwoByte(0, opcode);

    testedMachine.step();

    QCOMPARE(testedMachine.getRegisterValue("R0"), dst_result);
}

void CesarMachineTest::test_CESAR_INC_data()
{
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("dst_result");
    QTest::addColumn<int>("flag_N");
    QTest::addColumn<int>("flag_Z");
    QTest::addColumn<int>("flag_V");
    QTest::addColumn<int>("flag_C");

                            /*      src_val   dst_res    N     Z     V     C     */
    QTest::newRow("Register")       << 20      << 21    << 0  << 0  << 0  << 0;
    QTest::newRow("N")              << 40000   << 40001 << 1  << 0  << 0  << 0;
    QTest::newRow("V")              << MAX_POSITIVE   << MIN_NEGATIVE << 1  << 0  << 1  << 0;
    QTest::newRow("Z and C")        << 65535   << 0     << 0  << 1  << 0  << 1;


}

void CesarMachineTest::test_CESAR_INC()
{
    QFETCH(int, src_value);
    QFETCH(int, dst_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);

    int opcode = 0b1000001000000000;
    testedMachine.setRegisterValue("R0", src_value);
    testedMachine.memoryWriteTwoByte(0, opcode);

    testedMachine.step();

    QCOMPARE(testedMachine.getRegisterValue("R0"), dst_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);

}

void CesarMachineTest::test_CESAR_DEC_data()
{
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("src_result");
    QTest::addColumn<int>("flag_N");
    QTest::addColumn<int>("flag_Z");
    QTest::addColumn<int>("flag_V");
    QTest::addColumn<int>("flag_C");
                                    /*   src_val           src_res          N    Z     V      C       */                              
    QTest::newRow("Register")           << 20            << 19           << 0  << 0  << 0  << 0;
    QTest::newRow("N")                  << 40000         << 39999        << 1  << 0  << 0  << 0;
    QTest::newRow("V")                  << MIN_NEGATIVE  << MAX_POSITIVE << 0  << 0  << 1  << 0;
    QTest::newRow("C")                  << 0            << 65535         << 1  << 0  << 0  << 1;
    QTest::newRow("Neg No Borrow")      << 0xFFFF       << 0xFFFE        << 1  << 0  << 0  << 0;
}

void CesarMachineTest::test_CESAR_DEC()
{
    QFETCH(int, src_value);
    QFETCH(int, src_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);
    
    int opcode = 0b1000001100000000;

    testedMachine.setRegisterValue("R0", src_value);
    testedMachine.memoryWriteTwoByte(0, opcode);

    testedMachine.step();

    QCOMPARE(testedMachine.getRegisterValue("R0"), src_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);
}



void CesarMachineTest::test_CESAR_TST_data()
{
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("src_result");
    QTest::addColumn<int>("flag_N");
    QTest::addColumn<int>("flag_Z");
    QTest::addColumn<int>("flag_V");
    QTest::addColumn<int>("flag_C");
                                    /*   src_val    src_res     N     Z     V      C       */                              
    QTest::newRow("No_flags")             << 15     << 15    <<  0  << 0   << 0  << 0;
    QTest::newRow("N_flags")              << 40000  << 40000 <<  1  << 0   << 0  << 0;
    QTest::newRow("Z_flags")              << 0      << 0     <<  0  << 1   << 0  << 0;
    
}


void CesarMachineTest::test_CESAR_TST()
{
    QFETCH(int, src_value);
    QFETCH(int, src_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);
    
    int opcode = 0b1000010100000000;

    testedMachine.setRegisterValue("R0", src_value);
    testedMachine.memoryWriteTwoByte(0, opcode);

    testedMachine.step();

    QCOMPARE(testedMachine.getRegisterValue("R0"), src_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);
}

void CesarMachineTest::test_CESAR_NOP()
{
    int opcode = 0b00001011;
    testedMachine.setRegisterValue("R0", MAX_POSITIVE);
    testedMachine.setPCValue(0xFFFF);
    testedMachine.setFlagValue("N", 1);
    testedMachine.setFlagValue("Z", 1);
    testedMachine.setFlagValue("V", 1);
    testedMachine.setFlagValue("C", 1);

    testedMachine.memoryWriteTwoByte(0, opcode);

    testedMachine.step();

    QCOMPARE(testedMachine.getRegisterValue("R0"), MAX_POSITIVE);
    QCOMPARE(testedMachine.getPCValue(), 0);
    QCOMPARE(testedMachine.getFlagValue("N"), 1);
    QCOMPARE(testedMachine.getFlagValue("Z"), 1);
    QCOMPARE(testedMachine.getFlagValue("V"), 1);
    QCOMPARE(testedMachine.getFlagValue("C"), 1);
}

void CesarMachineTest::test_CESAR_ROR_data() //NOT DONE
{
   QTest::addColumn<int>("src_value");
   QTest::addColumn<int>("src_result");
   QTest::addColumn<int>("flag_N");
   QTest::addColumn<int>("flag_Z");
   QTest::addColumn<int>("flag_V");
   QTest::addColumn<int>("flag_C");
   QTest::addColumn<int>("Input_C");
                                   /*   src_val    src_res     N     Z     V      C   INPUT_C    */                              
   QTest::newRow("C_IN_flags")           << 3     << 1    <<  0    << 0  << 1  <<  1    << 0;
   QTest::newRow("C_OUT_flags")          << 0      << 32768 <<  1  << 0   << 1  << 0   << 1; 
   QTest::newRow("Z_flags")              << 1      << 0     <<  0  << 1   << 1  << 1   << 0;
   
}

void CesarMachineTest::test_CESAR_ROR()
{
    QFETCH(int, src_value);
    QFETCH(int, src_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);
    QFETCH(int, Input_C);

    int opcode = 0b1000011000000000;
    testedMachine.memoryWriteTwoByte(0, opcode);
    testedMachine.setRegisterValue("R0", src_value);   
    testedMachine.setFlagValue("C", Input_C);
    testedMachine.step();
    QCOMPARE(testedMachine.getRegisterValue("R0"), src_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);

}

void CesarMachineTest::test_CESAR_ROL_data() //NOT DONE
{
   QTest::addColumn<int>("src_value");
   QTest::addColumn<int>("src_result");
   QTest::addColumn<int>("flag_N");
   QTest::addColumn<int>("flag_Z");
   QTest::addColumn<int>("flag_V");
   QTest::addColumn<int>("flag_C");
   QTest::addColumn<int>("Input_C");
                                   /*   src_val             src_res         N     Z     V      C   INPUT_C    */                              
   QTest::newRow("C_IN_flags")           << 49152<<         MIN_NEGATIVE << 1   << 0 << 0  <<  1     << 0;
   QTest::newRow("C_OUT_flags")          << 0      <<       1          <<   0  <<  0   << 0  << 0   << 1; 
   QTest::newRow("Z_flags")              << MIN_NEGATIVE << 0     <<       0  <<  1   << 1  << 1   << 0;
}

void CesarMachineTest::test_CESAR_ROL()
{
     QFETCH(int, src_value);
    QFETCH(int, src_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);
    QFETCH(int, Input_C);

    int opcode = 0b1000011100000000;
    testedMachine.memoryWriteTwoByte(0, opcode);
    testedMachine.setRegisterValue("R0", src_value);   
    testedMachine.setFlagValue("C", Input_C);
    testedMachine.step();
    QCOMPARE(testedMachine.getRegisterValue("R0"), src_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);
}

void CesarMachineTest::test_CESAR_ASL_data() //NOT DONE
{
   QTest::addColumn<int>("src_value");
   QTest::addColumn<int>("src_result");
   QTest::addColumn<int>("flag_N");
   QTest::addColumn<int>("flag_Z");
   QTest::addColumn<int>("flag_V");
   QTest::addColumn<int>("flag_C");
                                   /*   src_val             src_res     N       Z      V     C       */                              
   QTest::newRow("No_flags")             << 1              << 2      <<  0   << 0   << 0  << 0;
   QTest::newRow("C_AND_Z_AND_V_flags")  << MIN_NEGATIVE   << 0      <<  0   << 1   << 1  << 1;
   QTest::newRow("N_flags")              << 16384     << MIN_NEGATIVE<<  1   << 0   << 1  << 0;
}

void CesarMachineTest::test_CESAR_ASL()
{
    QFETCH(int, src_value);
    QFETCH(int, src_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);
    

    int opcode = 0b1000100100000000;
    testedMachine.memoryWriteTwoByte(0, opcode);
    testedMachine.setRegisterValue("R0", src_value);   
    testedMachine.step();
    QCOMPARE(testedMachine.getRegisterValue("R0"), src_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);
}

void CesarMachineTest::test_CESAR_ASR_data() //NOT DONE 
{
   QTest::addColumn<int>("src_value");
   QTest::addColumn<int>("src_result");
   QTest::addColumn<int>("flag_N");
   QTest::addColumn<int>("flag_Z");
   QTest::addColumn<int>("flag_V");
   QTest::addColumn<int>("flag_C");
                                   /*   src_val             src_res     N       Z      V     C       */                              
   QTest::newRow("No_flags")             << 2              << 1      <<  0   << 0   << 0  << 0;
   QTest::newRow("C_AND_Z_AND_V_flags")  << 1              << 0      <<  0   << 1   << 1  << 1;
   //QTest::newRow("N_flags")              << 40960          << 0      <<  0   << 0   << 0  << 0;
}

void CesarMachineTest::test_CESAR_ASR()
{
    QFETCH(int, src_value);
    QFETCH(int, src_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);
    

    int opcode = 0b1000100000000000;
    testedMachine.memoryWriteTwoByte(0, opcode);
    testedMachine.setRegisterValue("R0", src_value);   
    testedMachine.step();
    QCOMPARE(testedMachine.getRegisterValue("R0"), src_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);
}

void CesarMachineTest::test_CESAR_ADC_data() // NOT DONE
{
   QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("src_result");
    QTest::addColumn<int>("flag_N");
    QTest::addColumn<int>("flag_Z");
    QTest::addColumn<int>("flag_V");
    QTest::addColumn<int>("flag_C");

                            /*      src_val   dst_res    N     Z     V     C     */
    QTest::newRow("Register")       << 20      << 21    << 0  << 0  << 0  << 0;
    QTest::newRow("N")              << 40000   << 40001 << 1  << 0  << 0  << 0;
    QTest::newRow("V")              << MAX_POSITIVE   << MIN_NEGATIVE << 1  << 0  << 1  << 0;
    QTest::newRow("Z and C")        << 65535   << 0     << 0  << 1  << 0  << 1;
}

void CesarMachineTest::test_CESAR_ADC()
{
    QFETCH(int, src_value);
    QFETCH(int, src_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);

    int opcode = 0b1000101000000000;
    testedMachine.memoryWriteTwoByte(0, opcode);
    testedMachine.setRegisterValue("R0", src_value);
    testedMachine.setFlagValue("C", 1);   
    testedMachine.step();
    QCOMPARE(testedMachine.getRegisterValue("R0"), src_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);


}

void CesarMachineTest::test_CESAR_SBC_data() //NOT DONE
{
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("src_result");
    QTest::addColumn<int>("flag_N");
    QTest::addColumn<int>("flag_Z");
    QTest::addColumn<int>("flag_V");
    QTest::addColumn<int>("flag_C");
                                    /*   src_val           src_res          N    Z     V      C       */                              
    QTest::newRow("Register")           << 20            << 19           << 0  << 0  << 0  << 0;
    QTest::newRow("N")                  << 40000         << 39999        << 1  << 0  << 0  << 0;
    QTest::newRow("V")                  << MIN_NEGATIVE  << MAX_POSITIVE << 0  << 0  << 1  << 0;
    QTest::newRow("C")                  << 0            << 65535         << 1  << 0  << 0  << 1;
    QTest::newRow("Neg No Borrow")      << 0xFFFF       << 0xFFFE        << 1  << 0  << 0  << 0;
}

void CesarMachineTest::test_CESAR_SBC()
{
    QFETCH(int, src_value);
    QFETCH(int, src_result);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);

    int opcode = 0b1000101100000000;
    testedMachine.memoryWriteTwoByte(0, opcode);
    testedMachine.setRegisterValue("R0", src_value);
    testedMachine.setFlagValue("C", 1);   
    testedMachine.step();
    QCOMPARE(testedMachine.getRegisterValue("R0"), src_result);
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);
}

void CesarMachineTest::test_CESAR_CCC_data()
{
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("flag_N");
    QTest::addColumn<int>("flag_Z");
    QTest::addColumn<int>("flag_V");
    QTest::addColumn<int>("flag_C");
                                    /*   src_val              N    Z     V      C       */                              
    QTest::newRow("NZVC")           <<    0b00011111          << 0  << 0  << 0  << 0;
    QTest::newRow("NZV")                  << 0b00011110       << 0  << 0  << 0  << 1;
    QTest::newRow("NZ")                  << 0b00011100        << 0  << 0  << 1  << 1;
    QTest::newRow("N")                  <<  0b00011000        << 0  << 1  << 1  << 1 ;
    QTest::newRow("none")                  <<  0b00010000     << 1  << 1  << 1  << 1 ;
    
}

void CesarMachineTest::test_CESAR_CCC()
{
    QFETCH(int, src_value);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);

    testedMachine.setFlagValue("N", 1);
    testedMachine.setFlagValue("Z", 1);
    testedMachine.setFlagValue("V", 1);
    testedMachine.setCarry(true);

    testedMachine.memoryWrite(0,src_value);
    testedMachine.step();
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);
}

void CesarMachineTest::test_CESAR_SCC_data()
{
    QTest::addColumn<int>("src_value");
    QTest::addColumn<int>("flag_N");
    QTest::addColumn<int>("flag_Z");
    QTest::addColumn<int>("flag_V");
    QTest::addColumn<int>("flag_C");
                                    /*   src_val              N    Z     V      C       */                              
    QTest::newRow("NZVC")                 << 0b00101111    << 1  << 1  << 1  << 1;
    QTest::newRow("NZV")                  << 0b00101110    << 1  << 1  << 1  << 0;
    QTest::newRow("NZ")                   << 0b00101100    << 1  << 1  << 0  << 0;
    QTest::newRow("N")                    << 0b00101000    << 1  << 0  << 0  << 0 ;
    QTest::newRow("none")                 << 0b00100000    << 0  << 0  << 0  << 0 ;
    
}

void CesarMachineTest::test_CESAR_SCC()
{
    QFETCH(int, src_value);
    QFETCH(int, flag_N);
    QFETCH(int, flag_Z);
    QFETCH(int, flag_V);
    QFETCH(int, flag_C);
    testedMachine.setFlagValue("N", 0);
    testedMachine.setFlagValue("Z", 0);
    testedMachine.setFlagValue("V", 0);
    testedMachine.setCarry(false);

    testedMachine.memoryWrite(0,src_value);
    testedMachine.step();
    QCOMPARE(testedMachine.getFlagValue("N"), flag_N);
    QCOMPARE(testedMachine.getFlagValue("Z"), flag_Z);
    QCOMPARE(testedMachine.getFlagValue("V"), flag_V);
    QCOMPARE(testedMachine.getFlagValue("C"), flag_C);
}



#include "tst_cesar.moc"
QTEST_APPLESS_MAIN(CesarMachineTest)