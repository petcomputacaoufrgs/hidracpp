#ifndef NEANDERMACHINE_H
#define NEANDERMACHINE_H

#include "machine.h"

class NeanderMachine : public Machine
{
public:
    NeanderMachine();

private:
    Register* acumulator;
};

#endif // NEANDERMACHINE_H
