#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <QString>
#include <QStringList>
#include <QRegExp>

class Instruction
{
public:
    enum InstructionCode
    {
        NOP,  CESAR_NOP, CESAR_HLT,
        /*cesar TWO OPERANDS*/ 
        CESAR_ADD,CESAR_SUB,CESAR_AND,CESAR_OR,CESAR_CMP,
        /*cesar ONE OPERAND*/
        CESAR_CLR,CESAR_NOT,CESAR_INC,CESAR_DEC,CESAR_NEG,CESAR_TST,
        CESAR_ROR,CESAR_ROL,CESAR_ASR,CESAR_ASL,CESAR_ADC,CESAR_SBC,
        /*cesar FLOW CONTROL*/
        CESAR_JMP,CESAR_BR,CESAR_BNE,CESAR_BEQ,CESAR_BPL,CESAR_BMI,CESAR_BVC,CESAR_BVS,CESAR_BCC,CESAR_BCS,CESAR_BGE,
        CESAR_BLT,CESAR_BGT,CESAR_BLE,CESAR_BHI,CESAR_BLS,CESAR_JSR,CESAR_RTS,
        /*cesar SPECIAL INSTRUCTIONS*/
        CESAR_CCC,CESAR_SCC,CESAR_SOB,

        STR, LDR, MOV,
        ADD, OR, AND, NOT, SUB, INC, DEC,
        JMP, JN, JP, JV, JNV, JZ, JNZ, JC, JNC, JB, JNB, JSR,
        NEG, SHR, SHL, ROR, ROL,
        HLT,

        REG_IF,

        VOLTA_NOP,
        VOLTA_ADD, VOLTA_SUB, VOLTA_AND,  VOLTA_OR, VOLTA_CLR, VOLTA_NOT, VOLTA_NEG,
        VOLTA_INC, VOLTA_DEC, VOLTA_ASR, VOLTA_ASL, VOLTA_ROR, VOLTA_ROL,
        VOLTA_SZ,  VOLTA_SNZ, VOLTA_SPL, VOLTA_SMI, VOLTA_SPZ, VOLTA_SMZ,
        VOLTA_SEQ, VOLTA_SNE, VOLTA_SGR, VOLTA_SLS, VOLTA_SGE, VOLTA_SLE,
        VOLTA_RTS, VOLTA_PSH, VOLTA_POP, VOLTA_JMP, VOLTA_JSR, VOLTA_HLT
    };

    enum InstructionGroup
    {
        NO_GROUP,
        //Cesar
        GROUP_NOP, 
        GROUP_CONDITIONAL_CODES, 
        GROUP_CONDITIONAL_BRANCHES,
        GROUP_JUMP,
        GROUP_LOOP_CONTROL,
        GROUP_JUMP_SUBROUTINE,
        GROUP_RETURN_SUBROUTINE,
        GROUP_ONE_OPERAND,
        GROUP_TWO_OPERAND,
        GROUP_HLT

    };
 
    Instruction();
    Instruction(int numBytes, QString bitPattern, InstructionCode instructionCode, QString assemblyFormat, InstructionGroup instructionGroup = InstructionGroup::NO_GROUP);
    bool matchByte(int byte);

    QString getMnemonic() const;
    QStringList getArguments() const;
    QString getAssemblyFormat() const;
    int getByteValue();
    int getNumBytes() const; // 0 if variable
    int getNumberOfArguments() const;

    InstructionCode getInstructionCode() const;
    QString getBitPattern() const;
    InstructionGroup getInstructionGroup() const;

private:
    int numBytes;

    QString bitPattern;
    QRegExp byteRegExp;
    QString mnemonic;
    QString assemblyFormat;
    QStringList arguments;
    InstructionGroup instructionGroup;

    InstructionCode instructionCode;
};

#endif // INSTRUCTION_H
