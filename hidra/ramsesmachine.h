#ifndef RAMSESMACHINE_H
#define RAMSESMACHINE_H

#include "machine.h"

class RamsesMachine : public Machine
{
public:
    RamsesMachine();

    virtual void printStatusDebug();

    virtual void load(QString filename);
    virtual void save(QString filename);

    virtual void step();
    //virtual void run();

    virtual Machine::ErrorCode mountInstruction(QString mnemonic, QString arguments, QHash<QString, int> &labelPCMap);

    virtual Instruction* getInstructionFromValue(int);
    virtual Instruction* getInstructionFromMnemonic(QString);

    static const int MEM_SIZE = 256;
    static const int MAX_VALUE = 255;
    static const int MAX_SIGNED_VALUE = 127; // Maximum positive number for 8-bits in two's complement

private:
    Register *RA, *RB, *RX;
    Flag *N, *Z, *C;
};

#endif // RAMSESMACHINE_H
