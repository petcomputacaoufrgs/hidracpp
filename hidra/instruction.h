#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <QString>

class Instruction
{
public:
    Instruction();

private:
    QString mnemonic;
    int value;
    int numberOfArguments;
};

#endif // INSTRUCTION_H
