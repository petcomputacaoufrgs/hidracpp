#ifndef PERICLESMACHINE_H
#define PERICLESMACHINE_H

#include "core/machine.h"

class PericlesMachine : public Machine
{
public:
    PericlesMachine();

    void decodeInstruction();
    virtual int calculateBytesToReserve(const Instruction* instruction, QStringList const& arguments);
    virtual int GetCurrentOperandAddress(); // increments accessCount
    virtual void getNextOperandAddress(int &intermediateAddress, int &intermediateAddress2, int &finalOperandAddress);
    virtual QString generateArgumentsString(int address, Instruction *instruction, AddressingMode::AddressingModeCode addressingModeCode, int &argumentsSize);

};

#endif // PERICLESMACHINE_H
