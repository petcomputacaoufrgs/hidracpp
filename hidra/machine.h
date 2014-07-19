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

    virtual void load(QString filename);
    virtual void save(QString filename);

    virtual void step();
    virtual void run();
    virtual void assemble(QString filename);

protected:
    QVector<Register*> registers;
    Register* PC;
    QVector<Byte*> memory;
    QVector<Bit*> flags;
    QVector<Instruction*> instructions;

    const Instruction* getInstructionFromValue(int);

signals:

public slots:

};

#endif // MACHINE_H
