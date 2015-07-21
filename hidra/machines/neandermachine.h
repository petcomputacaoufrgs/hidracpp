#ifndef NEANDERMACHINE_H
#define NEANDERMACHINE_H

#include "core/machine.h"

class NeanderMachine : public Machine
{
public:
    NeanderMachine();

    static const int MEM_SIZE = 256;
    static const int MAX_VALUE = 255;
    static const int MAX_SIGNED_VALUE = 127; // Maximum positive number for 8-bits in two's complement
};

#endif // NEANDERMACHINE_H
