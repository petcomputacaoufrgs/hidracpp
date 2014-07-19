#ifndef AHMESMACHINE_H
#define AHMESMACHINE_H

#include "machine.h"

class AhmesMachine : public Machine
{
public:
    AhmesMachine();
    using Machine::load;
};

#endif // AHMESMACHINE_H
