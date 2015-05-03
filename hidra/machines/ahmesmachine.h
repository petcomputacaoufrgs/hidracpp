#ifndef AHMESMACHINE_H
#define AHMESMACHINE_H

#include "core/machine.h"

class AhmesMachine : public Machine
{
public:
    AhmesMachine();

    virtual void printStatusDebug();

    virtual void step();
    //virtual voiexportMemoryn();

    virtual Instruction* getInstructionFromValue(int);
    virtual Instruction* getInstructionFromMnemonic(QString);

    virtual void mountInstruction(QString mnemonic, QString arguments, QHash<QString, int> &labelPCMap);

    static const int MEM_SIZE = 256;
    static const int MAX_VALUE = 255;
    static const int MAX_SIGNED_VALUE = 127; // Maximum positive number for 8-bits in two's complement


private:
    int getSignedInt(int eightBitValue);

    Register *AC;
    Flag *N, *Z, *V, *C, *B;
};

#endif // AHMESMACHINE_H
