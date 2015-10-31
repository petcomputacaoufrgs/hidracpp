#ifndef PERICLESMACHINE_H
#define PERICLESMACHINE_H

#include "core/machine.h"

class PericlesMachine : public Machine
{
public:
    PericlesMachine();

    virtual void decodeInstruction(int fetchedValue, Instruction *&instruction, AddressingMode::AddressingModeCode &addressingMode, QString &registerId, int &immediateAddress);
    virtual int calculateBytesToReserve(QString addressArgument);
    virtual int memoryGetOperandAddress(int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode); // increments accessCount
    virtual void getNextOperandAddress(int &intermediateAddress, int &intermediateAddress2, int &finalOperandAddress);

    int memoryReadTwoByteAddress(int address);
    int getMemoryTwoByteAddress(int address);
};

#endif // PERICLESMACHINE_H
