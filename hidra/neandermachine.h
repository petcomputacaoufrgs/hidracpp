#ifndef NEANDERMACHINE_H
#define NEANDERMACHINE_H

#include "machine.h"
#include <iostream>

class NeanderMachine : public Machine
{
public:
    NeanderMachine();

    void printStatusDebug();

private:
    Register* acumulator;
};

#endif // NEANDERMACHINE_H
