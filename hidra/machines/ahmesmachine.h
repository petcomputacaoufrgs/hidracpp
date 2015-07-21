#ifndef AHMESMACHINE_H
#define AHMESMACHINE_H

#include "core/machine.h"

class AhmesMachine : public Machine
{
public:
    AhmesMachine();

    static const int MEM_SIZE = 256;
    static const int MAX_VALUE = 255;
    static const int MAX_SIGNED_VALUE = 127; // Maximum positive number for 8-bits in two's complement

    void setCarry(bool state);
    void setBorrowOrCarry(bool state);
    void setOverflow(bool state);
};

#endif // AHMESMACHINE_H
