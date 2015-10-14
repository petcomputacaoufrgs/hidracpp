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
    QString QUOTE_SYMBOL = "¢";

    explicit Machine(QObject *parent = 0);
    ~Machine();



    //////////////////////////////////////////////////
    // Step
    //////////////////////////////////////////////////

    void step();
    void fetchInstruction(int &fetchedValue, Instruction *&instruction);

    virtual void decodeInstruction(int fetchedValue, Instruction *&instruction, AddressingMode::AddressingModeCode &addressingMode, QString &registerId, int &immediateAddress);
    virtual void executeInstruction(Instruction *&instruction, AddressingMode::AddressingModeCode addressingModeCode, QString registerName, int immediateAddress);

    AddressingMode::AddressingModeCode extractAddressingModeCode(int fetchedValue);
    QString extractRegisterName(int fetchedValue);

    void setOverflow(bool state);
    void setCarry(bool state);
    void setBorrowOrCarry(bool borrowState); // Some machines use carry as not borrow
    void updateFlags(int value); // Updates N and Z

    int address(int value); // Returns a valid address based on a value, removing excess bits (overflow)
    int toSigned(int unsignedByte);



    //////////////////////////////////////////////////
    // Assembler
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
    virtual int  reserveAssemblerMemory(const Instruction *instruction, QString arguments);
    virtual bool customAddressWrite(QString argument, bool isImmediate);

    // Assembler checks
    bool isValidValue(QString valueString, int min, int max);
    bool isValidNBytesValue(QString valueString, int numberOfBytes);
    bool isValidByteValue(QString valueString);
    bool isValidAddress(QString addressString);
    bool isValidOrg(QString offsetString);

    // Auxiliary methods
    QStringList splitArguments(QString arguments);
    void extractArgumentAddressingModeCode(QString &argument, AddressingMode::AddressingModeCode &addressingModeCode);
    int convertToUnsigned(int value, int numberOfBytes);
    int argumentToValue(QString argument, bool isImmediate);
    int stringToInt(QString valueString);



    //////////////////////////////////////////////////
    // Memory read/write with access count
    //////////////////////////////////////////////////

    int memoryRead(int address); // Increments accessCount
    void memoryWrite(int address, int value); // Increments accessCount
    int memoryReadNext(); // Returns value pointed to by PC, then increments PC; Increments accessCount

    virtual int memoryGetOperandAddress(int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode); // increments accessCount
    int memoryGetOperandValue(int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode); // increments accessCount
    int memoryGetJumpAddress( int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode); // increments accessCount



    //////////////////////////////////////////////////
    // Import/Export memory
    //////////////////////////////////////////////////

    FileErrorCode::FileErrorCode importMemory(QString filename);
    FileErrorCode::FileErrorCode exportMemory(QString filename);



    //////////////////////////////////////////////////
    // Getters/setters, clear
    //////////////////////////////////////////////////

    bool isRunning() const;
    void setRunning(bool running);

    bool getBuildSuccessful();
    int getFirstErrorLine();

    int getBreakpoint() const;
    void setBreakpoint(int value);

    virtual void getNextOperandAddress(int &intermediateAddress, int &finalOperandAddress);

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
    int getLineCorrespondingAddress(int line);
    int getAddressCorrespondingLine(int address);
    QString getAddressCorrespondingLabel(int address);

    QVector<Instruction *> getInstructions() const;
    Instruction* getInstructionFromValue(int value);
    Instruction* getInstructionFromMnemonic(QString mnemonic);

    AddressingMode::AddressingModeCode getDefaultAddressingModeCode();
    int getAddressingModeBitCode(AddressingMode::AddressingModeCode addressingModeCode);

    int getInstructionCount();
    int getAccessCount();
    void clearCounters();

    virtual void clear();

    virtual void generateDescriptions();
    QString getDescription(QString assemblyFormat);

protected:

    QString identifier;
    QVector<Register*> registers;
    Register *PC;
    QVector<Byte*> memory;
    QVector<Byte*> assemblerMemory;
    QVector<bool> reserved;
    QVector<int> addressCorrespondingLine, lineCorrespondingAddress; // Each address may be associated with a line of code
    QVector<QString> addressCorrespondingLabel;
    QVector<bool> changed;
    QVector<Flag*> flags;
    QVector<Instruction*> instructions;
    QVector<AddressingMode*> addressingModes;
    QHash<QString, int> labelPCMap;
    QHash<QString, QString> descriptions;
    int instructionPC; // Used by PC addressing mode
    bool buildSuccessful;
    bool running;
    int firstErrorLine;
    int breakpoint;
    int instructionCount;
    int accessCount;
    int memoryMask;

signals:
    void buildErrorDetected(QString);
public slots:

};


#endif // MACHINE_H
