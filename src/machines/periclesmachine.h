#ifndef PERICLESMACHINE_H
#define PERICLESMACHINE_H

#include "core/machine.h"

class PericlesMachine : public Machine
{
public:
    PericlesMachine();

    virtual void decodeInstruction(int fetchedValue, Instruction *&instruction, AddressingMode::AddressingModeCode &addressingMode, QString &registerId, int &immediateAddress);

    virtual int  reserveAssemblerMemory(const Instruction *instruction, QString arguments);
    virtual bool customAddressWrite(QString argument, bool isImmediate);

    virtual int memoryGetOperandAddress(int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode); // increments accessCount

    virtual void getNextOperandAddress(int &intermediateAddress, int &finalOperandAddress);
};

#endif // PERICLESMACHINE_H
