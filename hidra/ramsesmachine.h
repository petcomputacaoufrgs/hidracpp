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
    virtual void run();

    virtual int getMemorySize();

    virtual Machine::ErrorCode mountInstruction(QString mnemonic, QString arguments, QHash<QString, int> &labelPCMap);

    virtual Instruction* getInstructionFromValue(int);
    virtual Instruction* getInstructionFromMnemonic(QString);

private:
    Register *RA, *RB, *RX;
    Bit *N, *Z, *C;
    static const int MEM_SIZE = 256;
    static const int MAX_VALUE = 255;
    static const int MAX_VALUE_SIGN = 127;
};

#endif // RAMSESMACHINE_H
