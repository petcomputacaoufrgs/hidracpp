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
#include "addressingmode.h"

namespace FileErrorCode
{
    enum FileErrorCode
    {
        noError = 0,
        inputOutput,
        incorrectSize,
        invalidIdentifier,
    };
}

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
        invalidString,
        invalidLabel,
        invalidArgument,
        duplicatedLabel,
        memoryOverlap,
        notImplemented,
        undefinedError,
    };

    // Constants
    QString ALLOCATE_SYMBOL = "%";
    QString CHAR_SYMBOL = "$";
    QChar   SINGLE_QUOTE = '\'';

    explicit Machine(QObject *parent = 0);
    ~Machine();



    //////////////////////////////////////////////////
    // Import/Export memory
    //////////////////////////////////////////////////

    FileErrorCode::FileErrorCode importMemory(QString filename);
    FileErrorCode::FileErrorCode exportMemory(QString filename);



    //////////////////////////////////////////////////
    // Step
    //////////////////////////////////////////////////

    void step();
    void fetchInstruction(int byteArray[], Instruction *&instruction);
    void decodeInstruction(int byteArray[], Instruction *&instruction, AddressingMode::AddressingModeCode &addressingMode, QString &registerId, int &operandAddress);
    void executeInstruction(Instruction *&instruction, QString registerName, int operandAddress);

    AddressingMode::AddressingModeCode extractAddressingModeCode(int byteArray[]);
    QString extractRegisterName(int byteArray[]);

    void setOverflow(bool state);
    void setCarry(bool state);
    void setBorrowOrCarry(bool borrowState); // Some machines use carry as not borrow
    void updateFlags(int value); // Updates N and Z
    int toSigned(int unsignedByte);



    //////////////////////////////////////////////////
    // Memory read/write with access count
    //////////////////////////////////////////////////

    int memoryRead(int address); // Increments accessCount
    void memoryWrite(int address, int value); // Increments accessCount
    int memoryReadNext(); // Returns value pointed to by PC, then increments PC; Increments accessCount
    int memoryGetOperandAddress(int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode); // incrementsAccessCount



    //////////////////////////////////////////////////
    // Assembly
    //////////////////////////////////////////////////

    // Build
    void buildInstruction(QString mnemonic, QString arguments);

    // Assembly
    void assemble(QString sourceCode);
    void obeyDirective(QString mnemonic, QString arguments, bool reserveOnly);
    void emitError(int lineNumber, Machine::ErrorCode errorCode);

    // Assembler memory
    void clearAssemblerData();
    void copyAssemblerMemoryToMemory();
    void reserveAssemblerMemory(int sizeToReserve);

    // Assembler checks
    bool isValidValue(QString valueString, int min, int max);
    bool isValidNBytesValue(QString valueString, int numberOfBytes);
    bool isValidAddress(QString addressString);

    // Auxiliary methods
    QStringList splitArguments(QString arguments);
    void extractArgumentAddressingModeCode(QString &argument, AddressingMode::AddressingModeCode &addressingModeCode);
    int convertToUnsigned(int value, int numberOfBytes);



    //////////////////////////////////////////////////
    // Getters/setters, clear
    //////////////////////////////////////////////////

    bool isRunning() const;
    void setRunning(bool running);

    bool getBuildSuccessful();

    int getBreakpoint() const;
    void setBreakpoint(int value);

    int getMemorySize() const;
    void setMemorySize(int size);
    int getMemoryValue(int address) const;
    void setMemoryValue(int address, int value);
    bool hasByteChanged(int address); // Since last look-up
    void clearMemory();

    int getNumberOfFlags() const;
    QString getFlagName(int id) const;
    int  getFlagValue(int id) const;
    void setFlagValue(int id, int value);
    int  getFlagValue(QString flagName) const;
    void setFlagValue(QString flagName, int value);
    void setFlagValue(Flag::FlagCode flagCode, int value);
    void clearFlags();

    int getNumberOfRegisters() const;
    int getRegisterBitCode(QString registerName) const; // -1 if no code
    QString getRegisterName(int id) const;
    int  getRegisterValue(int id) const;
    void setRegisterValue(int id, int value);
    int  getRegisterValue(QString registerName) const;
    void setRegisterValue(QString registerName, int value);
    void clearRegisters();

    int getPCValue() const;
    void setPCValue(int value);
    void incrementPCValue();

    int getPCCorrespondingLine();
    int getAddressCorrespondingLine(int address);
    int getLineCorrespondingAddress(int line);

    QVector<Instruction *> getInstructions() const;
    Instruction* getInstructionFromValue(int);
    Instruction* getInstructionFromMnemonic(QString);

    AddressingMode::AddressingModeCode getDefaultAddressingModeCode();
    int getAddressingModeBitCode(AddressingMode::AddressingModeCode addressingModeCode);

    int getInstructionCount();
    int getAccessCount();
    void clearCounters();

    void clear();

    void generateDescriptions();
    QString getDescription(QString assemblyFormat);

protected:

    QString identifier;
    QVector<Register*> registers;
    Register* PC;
    QVector<Byte*> memory;
    QVector<Byte*> assemblerMemory;
    QVector<bool> reserved;
    QVector<int> correspondingLine, correspondingAddress; // Each address may be associated with a line of code
    QVector<bool> changed;
    QVector<Flag*> flags;
    QVector<Instruction*> instructions;
    QVector<AddressingMode*> addressingModes;
    QHash<QString, int> labelPCMap;
    QHash<QString, QString> descriptions;
    bool buildSuccessful;
    bool running;
    int breakpoint;
    int instructionCount;
    int accessCount;

signals:
    void buildErrorDetected(QString);
public slots:

};


#endif // MACHINE_H
