#ifndef AHMESMACHINE_H
#define AHMESMACHINE_H

#include "machine.h"
#include <iostream>

class AhmesMachine : public Machine
{
public:
    AhmesMachine();

    virtual void load(QString filename) = 0;
    virtual void save(QString filename) = 0;

    virtual void step() = 0;
    virtual void run() = 0;
    virtual void assemble(QString filename) = 0;

    virtual const Instruction* getInstructionFromValue(int);

private:
    Register *AC;
};

#endif // AHMESMACHINE_H
