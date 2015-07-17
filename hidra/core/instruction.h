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
        JMP, JN, JZ, JC, JSR,
        NEG, SHR,
        HLT,
    };

    Instruction();
    Instruction(int numBytes, QString bitPattern, InstructionCode instructionCode, QString assemblyFormat);
    bool matchByte(int byte);

    QString getMnemonic() const;
    QStringList getArguments() const;
    int getByteValue();
    int getNumBytes() const;

    InstructionCode getInstructionCode() const;
    QString getBytePattern() const;

private:
    int numBytes;

    QString bitPattern;
    QRegExp byteRegExp;
    QString mnemonic;
    QStringList arguments;

    InstructionCode instructionCode;
};

#endif // INSTRUCTION_H
