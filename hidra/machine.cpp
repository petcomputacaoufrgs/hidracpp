#include "machine.h"

Machine::Machine(QObject *parent) :
    QObject(parent)
{
}

bool Machine::getRunning() const
{
    return running;
}

void Machine::setRunning(bool value)
{
    running = value;
}
