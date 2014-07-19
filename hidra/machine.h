#ifndef MACHINE_H
#define MACHINE_H

#include <QObject>
#include <QVector>
#include "bit.h"
#include "byte.h"
#include "register.h"
#include "instruction.h"

class Machine : public QObject
{
    Q_OBJECT
public:
    explicit Machine(QObject *parent = 0);

    virtual void load(QString filename) = 0;
    virtual void save(QString filename) = 0;

    virtual void step() = 0;
    virtual void run() = 0;
    virtual void assemble(QString filename) = 0;

    virtual const Instruction* getInstructionFromValue(int) = 0;

protected:
    QVector<Register*> registers;
    Register* PC;
    QVector<Byte*> memory;
    QVector<Bit*> flags;
    QVector<Instruction*> instructions;


signals:

public slots:

};

#endif // MACHINE_H
