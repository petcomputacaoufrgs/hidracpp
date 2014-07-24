#ifndef NEANDERMACHINE_H
#define NEANDERMACHINE_H

#include "machine.h"
#include <iostream>

#include <QFile>

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
    Bit *N, *Z;
    static const int MEM_SIZE = 256;
    static const int MAX_VALUE = 255;
    static const int MAX_VALUE_SIGN = 127;   //127 = 0x7F que eh o maior numero positico em 8 bits em complemente B-1
};

#endif // NEANDERMACHINE_H
