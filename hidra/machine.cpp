#include "machine.h"

Machine::Machine(QObject *parent) :
    QObject(parent)
{
}

QVector<Byte *> Machine::getMemory() const
{
    return memory;
}

void Machine::setMemory(const QVector<Byte *> &value)
{
    memory = value;
}
