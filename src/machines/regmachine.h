#ifndef REGMACHINE_H
#define REGMACHINE_H

#include "core/machine.h"

class RegMachine : public Machine
{
public:
    RegMachine();
    virtual QString generateArgumentsString(int address, Instruction *instruction, AddressingMode::AddressingModeCode addressingModeCode, int &argumentsSize);
};

#endif // REGMACHINE_H
