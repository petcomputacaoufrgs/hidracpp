#ifndef VOLTAMACHINE_H
#define VOLTAMACHINE_H

#include "core/machine.h"

class VoltaMachine : public Machine
{
public:
    VoltaMachine();

    virtual void executeInstruction(Instruction *&instruction, AddressingMode::AddressingModeCode addressingModeCode, QString, int immediateAddress);
    void skipNextInstruction();

    virtual void generateDescriptions();

    void stackPush(int value);
    int  stackPop();

    int  getStackSize();
    void setStackSize(int size);
    int  getStackValue(int address);
    void setStackValue(int address, int value);
    void clearStack();

    int getSPValue();
    virtual void clear();

    Register *SP;

    QVector<Byte*> stack;
    QVector<bool> stackChanged;
};

#endif // VOLTAMACHINE_H
