#include "instruction.h"
#include "byte.h"

Instruction::Instruction()
{
}

Instruction::Instruction(int numBytes, QString bitPattern, InstructionCode instructionCode, QString assemblyFormat)
{
    this->numBytes = numBytes;
    this->bitPattern = bitPattern;
    this->byteRegExp = QRegExp(bitPattern);
    this->instructionCode = instructionCode;

    QStringList assemblyFormatList = assemblyFormat.split(" ");

    this->mnemonic = assemblyFormatList.first();
    assemblyFormatList.removeFirst();
    this->arguments = assemblyFormatList; // Mnemonic not included
    this->assemblyFormat = assemblyFormat;
}

bool Instruction::matchByte(int byte)
{
    return byteRegExp.exactMatch(Byte(byte).toString());
}

QString Instruction::getMnemonic() const
{
    return mnemonic;
}

QStringList Instruction::getArguments() const
{
    return arguments;
}

QString Instruction::getAssemblyFormat() const
{
    return assemblyFormat;
}

int Instruction::getByteValue()
{
    return Byte(bitPattern).getValue();
}

int Instruction::getNumBytes() const
{
    return numBytes;
}

Instruction::InstructionCode Instruction::getInstructionCode() const
{
    return instructionCode;
}

QString Instruction::getBitPattern() const
{
    return bitPattern;
}


