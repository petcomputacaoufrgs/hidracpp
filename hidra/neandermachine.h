#ifndef NEANDERMACHINE_H
#define NEANDERMACHINE_H

#include "machine.h"
#include <iostream>

#include <QFile>
#include <QHash>
#include <QPair>

class NeanderMachine : public Machine
{
public:
    NeanderMachine();

    void printStatusDebug();

private:
    Register* AC;
};

#endif // NEANDERMACHINE_H
