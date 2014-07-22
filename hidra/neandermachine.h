#ifndef NEANDERMACHINE_H
#define NEANDERMACHINE_H

#include "machine.h"
#include <iostream>

class NeanderMachine : public Machine
{
public:
    NeanderMachine();

    virtual void printStatusDebug();

    virtual void load(QString filename);
    virtual void save(QString filename);

    virtual void step();
    virtual void run();
    virtual void assemble(QString filename);

    virtual const Instruction* getInstructionFromValue(int);
    virtual const Instruction* getInstructionFromMnemonic(QString);
    virtual const bool validateInstructions(QStringList);

private:
    Register* AC;
    static const int MEM_SIZE = 256;
};

#endif // NEANDERMACHINE_H
