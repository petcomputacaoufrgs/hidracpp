#ifndef RAMSESMACHINE_H
#define RAMSESMACHINE_H

#include "machine.h"
#include <iostream>

class RamsesMachine : public Machine
{
public:
    RamsesMachine();

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
    Register *RA, *RB, *RX;
    Bit *N, *Z, *C;
    static const int MEM_SIZE = 256;
    static const int MAX_VALUE = 255;
    static const int MAX_VALUE_SIGN = 127;
};

#endif // RAMSESMACHINE_H
