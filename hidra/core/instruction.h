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
        ADD, OR, AND, NOT, SUB,
        JMP, JN, JP, JV, JNV, JZ, JNZ, JC, JNC, JB, JNB, JSR,
        NEG, SHR, SHL, ROR, ROL,
        HLT,
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
