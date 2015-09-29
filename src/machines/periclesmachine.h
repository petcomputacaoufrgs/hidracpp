#ifndef PERICLESMACHINE_H
#define PERICLESMACHINE_H

#include "core/machine.h"

class PericlesMachine : public Machine
{
public:
    PericlesMachine();

    virtual int  reserveAssemblerMemory(const Instruction *instruction, QString arguments);
    virtual bool customAddressWrite(QString argument, bool isImmediate);
};

#endif // PERICLESMACHINE_H
