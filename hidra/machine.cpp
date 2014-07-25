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

QVector<Byte *> Machine::getMemory() const
{
    return memory;
}

void Machine::setMemory(const QVector<Byte *> &value)
{
    memory = value;
}
