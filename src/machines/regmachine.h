#ifndef REGMACHINE_H
#define REGMACHINE_H

#include "core/machine.h"

class RegMachine : public Machine
{
public:
    RegMachine();

    virtual void executeInstruction(Instruction *&instruction, AddressingMode::AddressingModeCode, QString registerName, int immediateAddress);
};

#endif // REGMACHINE_H
