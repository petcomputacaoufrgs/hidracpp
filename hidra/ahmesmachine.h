#ifndef AHMESMACHINE_H
#define AHMESMACHINE_H

#include "machine.h"

class AhmesMachine : public Machine
{
public:
    AhmesMachine();

    virtual void printStatusDebug();

    virtual void load(QString filename);
    virtual void save(QString filename);

    virtual void step();
    virtual void run();

    virtual Instruction* getInstructionFromValue(int);
    virtual Instruction* getInstructionFromMnemonic(QString);

    virtual int getMemorySize();

    virtual Machine::ErrorCode mountInstruction(QString mnemonic, QString arguments, QHash<QString, int> &labelPCMap);

    static const int MEM_SIZE = 256;
    static const int MAX_VALUE = 255;
    static const int MAX_SIGNED_VALUE = 127; // Maximum positive number for 8-bits in two's complement


private:
    void updateFlags(unsigned char previousAC, unsigned char operand, bool addition, bool subtraction, bool shiftRotateLeft, bool shiftRotateRight);

    Register *AC;
    Bit *N, *Z, *V, *C, *B;
};

#endif // AHMESMACHINE_H
