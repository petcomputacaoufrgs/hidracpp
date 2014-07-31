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

QVector<Register *> Machine::getRegisters() const
{
    return registers;
}

void Machine::setRegisters(const QVector<Register *> &value)
{
    registers = value;
}

QVector<Bit *> Machine::getFlags() const
{
    return flags;
}

void Machine::setFlags(const QVector<Bit *> &value)
{
    flags = value;
}
