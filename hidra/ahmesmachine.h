#ifndef AHMESMACHINE_H
#define AHMESMACHINE_H

#include "machine.h"
#include <iostream>

class AhmesMachine : public Machine
{
public:
    AhmesMachine();

    virtual void printStatusDebug();

    virtual void load(QString filename);
    virtual void save(QString filename);

    virtual void step();
    virtual void run();
    virtual void assemble(QString filename);

    virtual const Instruction* getInstructionFromValue(int);
    virtual const Instruction* getInstructionFromMnemonic(QString);

private:
    bool hasSameSign(Byte n1, Byte n2);
    void updateFlags(char operand);
    bool isNegative(char value);
    void ror();
    void rol();
    void shl();
    void shr();

    Register *AC;
    Bit *N, *Z, *V, *C, *B;
};

#endif // AHMESMACHINE_H
