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
        NOP,
        STR, LDR,
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

    Instruction();
    Instruction(int numBytes, QString bitPattern, InstructionCode instructionCode, QString assemblyFormat);
    bool matchByte(int byte);

    QString getMnemonic() const;
    QStringList getArguments() const;
    QString getAssemblyFormat() const;
    int getByteValue();
    int getNumBytes() const;

    InstructionCode getInstructionCode() const;
    QString getBitPattern() const;

private:
    int numBytes;

    QString bitPattern;
    QRegExp byteRegExp;
    QString mnemonic;
    QString assemblyFormat;
    QStringList arguments;

    InstructionCode instructionCode;
};

#endif // INSTRUCTION_H
