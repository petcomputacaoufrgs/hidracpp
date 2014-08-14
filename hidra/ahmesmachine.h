#ifndef AHMESMACHINE_H
#define AHMESMACHINE_H

#include "machine.h"
#include "neandermachine.h"

class AhmesMachine : public NeanderMachine
{
public:
    AhmesMachine();

    virtual void printStatusDebug();

    virtual void load(QString filename);
    virtual void save(QString filename);

    virtual void step();
    virtual void run();
//    virtual void assemble(QString filename);

    virtual Instruction* getInstructionFromValue(int);
    virtual Instruction* getInstructionFromMnemonic(QString);

private:
    void updateFlags(unsigned char previous_AC, unsigned char operand, bool addition, bool subtraction, bool shift_rotate_left, bool shift_rotate_right);

    Register *AC;
    Bit *N, *Z, *V, *C, *B;
};

#endif // AHMESMACHINE_H
