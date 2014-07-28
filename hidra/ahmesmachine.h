#ifndef AHMESMACHINE_H
#define AHMESMACHINE_H

#include "machine.h"
#include <QFile>
#include <QHash>
#include <QPair>
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
    void updateFlags(unsigned char preAC, unsigned char operand, bool sour, bool sub);

    Register *AC;
    Bit *N, *Z, *V, *C, *B;
};

#endif // AHMESMACHINE_H
