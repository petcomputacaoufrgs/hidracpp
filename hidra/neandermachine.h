#ifndef NEANDERMACHINE_H
#define NEANDERMACHINE_H

#include "machine.h"

class NeanderMachine : public Machine
{
public:
    NeanderMachine();

    virtual void printStatusDebug();

    virtual void load(QString filename);
    virtual void save(QString filename);

    virtual void step();
    virtual void run();

    virtual Instruction* getInstructionFromValue(int);
    virtual Instruction* getInstructionFromMnemonic(QString);
    virtual bool validateInstructions(QStringList);

    virtual int getMemorySize();

    virtual Machine::ErrorCode mountInstruction(QString mnemonic, QString arguments, QHash<QString, int> &labelPCMap);

    static const int MEM_SIZE = 256;
    static const int MAX_VALUE = 255;
    static const int MAX_VALUE_SIGN = 127;   //127 = 0x7F que eh o maior numero positico em 8 bits em complemente B-1
private:
    Register* AC;
    Bit *N, *Z;

};

#endif // NEANDERMACHINE_H
