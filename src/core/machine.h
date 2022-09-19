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
        invalidAddress
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
        duplicateLabel,
        memoryOverlap,
        notImplemented,
        undefinedError,
    };

    // Constants
    QString ALLOCATE_SYMBOL = "%";
    QString QUOTE_SYMBOL = "¢";

    explicit Machine(QObject *parent = 0);
    ~Machine();



    //////////////////////////////////////////////////
    // Step
    //////////////////////////////////////////////////

    ///Do a step of the simulation
    void step();
    ///Get next instruction
    void fetchInstruction(int &fetchedValue, Instruction *&instruction);
    ///Decode the instruction
    virtual void decodeInstruction(int fetchedValue, Instruction *&instruction, AddressingMode::AddressingModeCode &addressingMode, QString &registerId, int &immediateAddress);
    ///Execute the instruction
    virtual void executeInstruction(Instruction *&instruction, AddressingMode::AddressingModeCode addressingModeCode, QString registerName, int immediateAddress);

    ///Get the adressing mode code from a memory value
    AddressingMode::AddressingModeCode extractAddressingModeCode(int fetchedValue);
    ///Get the register's name
    QString extractRegisterName(int fetchedValue);

    //Flag setting
    void setOverflow(bool state);
    void setCarry(bool state);
    void setBorrowOrCarry(bool borrowState); // Some machines use carry as not borrow
    void updateFlags(int value); // Updates N and Z

    /// Returns a valid address based on a value, removing excess bits (overflow)
    int address(int value);
    int toSigned(int unsignedByte);



    //////////////////////////////////////////////////
    // Assembler
    //////////////////////////////////////////////////

    // Assembly
    void assemble(QString sourceCode);
    void obeyDirective(QString mnemonic, QString arguments, bool reserveOnly, int sourceLine);
    void buildInstruction(QString mnemonic, QString arguments);
    void emitError(int lineNumber, Machine::ErrorCode errorCode);

    // Assembler memory
    void clearAssemblerData();
    void setAssemblerMemoryNext(int value); // Increments PC
    void copyAssemblerMemoryToMemory();
    void reserveAssemblerMemory(int sizeToReserve, int associatedSourceLine);
    virtual int calculateBytesToReserve(QString addressArgument);

    // Assembler checks
    bool isValidValue(QString valueString, int min, int max);
    bool isValidNBytesValue(QString valueString, int numberOfBytes);
    bool isValidByteValue(QString valueString);
    bool isValidAddress(QString addressString);
    bool isValidOrg(QString offsetString);

    // Auxiliary methods
    QStringList splitDirectiveArguments(QString arguments);
    QStringList splitInstructionArguments(QString const& arguments, Instruction const& instruction);
    void extractArgumentAddressingModeCode(QString &argument, AddressingMode::AddressingModeCode &addressingModeCode);
    int convertToUnsigned(int value, int numberOfBytes);
    int argumentToValue(QString argument, bool isImmediate, int immediateNumBytes = 1);
    int stringToInt(QString valueString);



    //////////////////////////////////////////////////
    // Memory read/write with access count
    //////////////////////////////////////////////////

    int  memoryRead(int address); // Increments accessCount
    void memoryWrite(int address, int value); // Increments accessCount
    int  memoryReadNext(); // Returns value pointed to by PC, then increments PC; Increments accessCount

    virtual int memoryGetOperandAddress(int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode); // increments accessCount
    int memoryGetOperandValue(int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode); // increments accessCount
    int memoryGetJumpAddress( int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode); // increments accessCount



    //////////////////////////////////////////////////
    // Import/Export memory
    //////////////////////////////////////////////////

    ///Set up the machine's memory from a .mem file
    FileErrorCode::FileErrorCode importMemory(QString filename, int start, int end, int dest);
    ///Save the machine's memory in a .mem file
    FileErrorCode::FileErrorCode exportMemory(QString filename);



    //////////////////////////////////////////////////
    // Instruction strings
    //////////////////////////////////////////////////

    ///Given the current position of the program counter, update the interpretation of the bytes
    void updateInstructionStrings();
    QString generateInstructionString(int address, int &argumentsSize); // TODO: Fix Pericles
    virtual QString generateArgumentsString(int address, Instruction *instruction, AddressingMode::AddressingModeCode addressingModeCode, int &argumentsSize);



    //////////////////////////////////////////////////
    // Getters/setters, clear
    //////////////////////////////////////////////////

    bool isRunning() const;
    void setRunning(bool running);

    bool getBuildSuccessful();
    int  getFirstErrorLine();

    int  getBreakpoint() const;
    void setBreakpoint(int value);

    virtual void getNextOperandAddress(int &intermediateAddress, int &intermediateAddress2, int &finalOperandAddress);

    int  getMemorySize() const;
    void setMemorySize(int size);
    int  getMemoryValue(int address) const;
    void setMemoryValue(int address, int value);
    bool hasByteChanged(int address); // Since last look-up
    void clearMemory();

    QString getInstructionString(int address);
    void setInstructionString(int address, QString value);
    void clearInstructionStrings();

    int  getNumberOfFlags() const;
    QString getFlagName(int id) const;
    int  getFlagValue(int id) const;
    void setFlagValue(int id, int value);
    bool hasFlag(Flag::FlagCode flagCode) const;
    int  getFlagValue(QString flagName) const;
    void setFlagValue(QString flagName, int value);
    void setFlagValue(Flag::FlagCode flagCode, int value);
    void clearFlags();

    int  getNumberOfRegisters() const;
    int  getRegisterBitCode(QString registerName) const; // -1 if no code
    QString getRegisterName(int id) const;
    bool hasRegister(QString registerName) const;
    int  getRegisterValue(int id, bool signedData = false) const;
    int  getRegisterValue(QString registerName) const;
    void setRegisterValue(int id, int value);
    void setRegisterValue(QString registerName, int value);
    bool isRegisterData(int id);
    void clearRegisters();

    int  getPCValue() const;
    void setPCValue(int value);
    void incrementPCValue(int units = 1);

    int getPCCorrespondingSourceLine();
    int getSourceLineCorrespondingAddress(int line);
    int getAddressCorrespondingSourceLine(int address);
    QString getAddressCorrespondingLabel(int address);

    QVector<Instruction *> getInstructions() const;
    Instruction* getInstructionFromValue(int value);
    Instruction* getInstructionFromMnemonic(QString mnemonic);

    QVector<AddressingMode *> getAddressingModes() const;
    AddressingMode::AddressingModeCode getDefaultAddressingModeCode();
    int getAddressingModeBitCode(AddressingMode::AddressingModeCode addressingModeCode);
    QString getAddressingModePattern(AddressingMode::AddressingModeCode addressingModeCode);

    int  getInstructionCount();
    int  getAccessCount();
    void clearCounters();

    virtual void clear();
    virtual void clearAfterBuild();

    virtual void generateDescriptions();
    QString getDescription(QString assemblyFormat);
    void getAddressingModeDescription(AddressingMode::AddressingModeCode addressingModeCode, QString &acronym, QString &name, QString &format, QString &description);

protected:
    ///Machine identifier string for file opening
    QString identifier;
    ///The machine's registers
    QVector<Register*> registers;
    ///Program counter
    Register *PC;
    ///Simulated memory
    QVector<Byte*> memory;
    ///Simulated memory configuration created by the assembler
    QVector<Byte*> assemblerMemory;
    ///Interpretation of each memory adress as an instruction based on PC position
    QVector<QString> instructionStrings;
    ///Reserved memory
    QVector<bool> reserved;
    // Each address may be associated with a line of code
    QVector<int> addressCorrespondingSourceLine, sourceLineCorrespondingAddress;
    ///Each adress' corresponding label
    QVector<QString> addressCorrespondingLabel;
    ///True values indicate the memory adress associated to this position has been changed
    QVector<bool> changed;
    ///The machine's flags
    QVector<Flag*> flags;
    ///The machine's instructions
    QVector<Instruction*> instructions;
    ///The machine's adressing modes
    QVector<AddressingMode*> addressingModes;
    ///Map of labels to adresses
    QHash<QString, int> labelPCMap;
    ///Instruction descriptions
    QHash<QString, QString> descriptions;


    bool buildSuccessful;
    bool running;
    bool littleEndian;
    int firstErrorLine;

    ///Breakpoint position
    int breakpoint;
    ///Amount of instructions executed during simulation
    int instructionCount;
    ///Number of memory acesses done during simulation
    int accessCount;
    ///Used to "cut down" memory adresses to the machine's maximum address
    int memoryMask;

signals:
    void buildErrorDetected(QString);
};


#endif // MACHINE_H
