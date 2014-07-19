#ifndef NEANDERMACHINE_H
#define NEANDERMACHINE_H

#include "machine.h"
#include <iostream>

class NeanderMachine : public Machine
{
public:
    NeanderMachine();

    void printStatusDebug();

    virtual void load(QString filename);
    virtual void save(QString filename);

    virtual void step();
    virtual void run();
    virtual void assemble(QString filename);

    virtual const Instruction* getInstructionFromValue(int);

private:
    Register* AC;
};

#endif // NEANDERMACHINE_H
