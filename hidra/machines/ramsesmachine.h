#ifndef RAMSESMACHINE_H
#define RAMSESMACHINE_H

#include "core/machine.h"

class RamsesMachine : public Machine
{
public:
    RamsesMachine();

    virtual AddressingMode extractAddressingMode(int byteArray[]);
    virtual int extractRegisterId(int byteArray[]);

    virtual void setCarry(bool state);
    virtual void setBorrowOrCarry(bool state);

    static const int MEM_SIZE = 256;
    static const int MAX_VALUE = 255; // Maximum value allowed (inclusive)
    static const int MAX_SIGNED_VALUE = 127; // Maximum positive number for 8-bits in two's complement
};

#endif // RAMSESMACHINE_H
