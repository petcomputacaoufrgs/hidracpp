#ifndef MACHINE_H
#define MACHINE_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QHash>
#include <QPair>
#include <iostream>

#include "byte.h"
#include "flag.h"
#include "register.h"
#include "instruction.h"

class Machine : public QObject
{
    Q_OBJECT
public:

    enum ErrorCode
    {
        noError = 0,
        wrongNumberOfArguments,
        invalidInstruction,
        invalidAddress,
        invalidValue,
        invalidLabel,
        invalidArgument,
        duplicatedLabel,
        memoryOverflow,
        notImplemented,
    };

    explicit Machine(QObject *parent = 0);

    virtual void printStatusDebug() = 0;

    virtual void load(QString filename) = 0;
    virtual void save(QString filename) = 0;

    virtual void step() = 0;
    //virtual void run() = 0;

    // Machine specific
    virtual Machine::ErrorCode mountInstruction(QString mnemonic, QString arguments, QHash<QString, int> &labelPCMap) = 0;

    // Assembly
    void assemble(QString sourceCode);
    Machine::ErrorCode obeyDirective(QString mnemonic, QString arguments, bool reserveOnly);
    void emitError(int lineNumber, Machine::ErrorCode errorCode);

    // Assembler memory
    void clearAssemblerMemory();
    void copyAssemblerMemoryToMemory();
    Machine::ErrorCode reserveAssemblerMemory(int sizeToReserve);

    // Assembler checks
    bool isValidValue(QString valueString, int min, int max);
    bool isValidByteValue(QString valueString);
    bool isValidAddress(QString addressString);

    virtual const Instruction* getInstructionFromValue(int) = 0;
    virtual const Instruction* getInstructionFromMnemonic(QString) = 0;

    bool isRunning() const;
    void setRunning(bool running);

    int getBreakpoint() const;
    void setBreakpoint(int value);

    QVector<Byte *> getMemory() const;
    void setMemory(const QVector<Byte *> &value);
    void clearMemory();

    int getNumberOfFlags() const;
    QString getFlagName(int id) const;
    int  getFlagValue(int id) const;
    void setFlagValue(int id, int value);
    void clearFlags();

    int getNumberOfRegisters() const;
    QString getRegisterName(int id) const;
    int  getRegisterValue(int id) const;
    void setRegisterValue(int id, int value);
    void clearRegisters();

    int getPCValue() const;
    void setPCValue(int value);

    int getPCCorrespondingLine();
    int getLineCorrespondingAddress(int line);

    QVector<Instruction *> getInstructions() const;
    void setInstructions(const QVector<Instruction *> &value);

    bool buildSuccessful;

protected:

    QVector<Register*> registers;
    Register* PC;
    QVector<Byte*> memory;
    QVector<Byte*> assemblerMemory;
    QVector<bool> reserved;
    QVector<int> correspondingLine, correspondingAddress;
    QVector<Flag*> flags;
    QVector<Instruction*> instructions;
    bool running;
    int breakpoint;

signals:
    void buildErrorDetected(QString);
public slots:

};


#endif // MACHINE_H
