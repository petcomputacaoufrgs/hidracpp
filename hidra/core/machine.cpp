#include "machine.h"

Machine::Machine(QObject *parent) :
    QObject(parent)
{
}

int Machine::getBreakpoint() const
{
    return breakpoint;
}

void Machine::setBreakpoint(int value)
{
    if (value > memory.size() || value < 0)
        breakpoint = 0;
    else
        breakpoint = value;
}

int Machine::getMemorySize() const
{
    return memory.size();
}

int Machine::getMemoryValue(int address) const
{
    return memory.at(address)->getValue();
}

void Machine::setMemoryValue(int address, int value)
{
    memory.at(address)->setValue(value);
}

void Machine::clearMemory()
{
    for (int i=0; i<memory.size(); i++)
        memory[i]->setValue(0);
}

int Machine::getNumberOfRegisters() const
{
    return registers.count();
}

QString Machine::getRegisterName(int id) const
{
    return registers[id]->getName();
}

int Machine::getRegisterValue(int id) const
{
    return registers[id]->getValue();
}

void Machine::setRegisterValue(int id, int value)
{
    registers[id]->setValue(value);
}

void Machine::clearRegisters()
{
    for (int i=0; i<registers.size(); i++)
        registers[i]->setValue(0);
}

int Machine::getPCValue() const
{
    return PC->getValue();
}

void Machine::setPCValue(int value)
{
    PC->setValue(value);
}

int Machine::getPCCorrespondingLine()
{
    if (!correspondingLine.isEmpty())
        return correspondingLine[PC->getValue()];
    else
        return -1;
}

int Machine::getLineCorrespondingAddress(int line)
{
    if (line >= 0 && line < correspondingAddress.size())
        return correspondingAddress[line];
    else
        return 0;
}

QVector<Instruction *> Machine::getInstructions() const
{
    return instructions;
}

void Machine::setInstructions(const QVector<Instruction *> &value)
{
    instructions = value;
}

int Machine::getInstructionCount()
{
    return instructionCount;
}

int Machine::getAccessCount()
{
    return accessCount;
}

void Machine::clearCounters()
{
    instructionCount = 0;
    accessCount = 0;
}

int Machine::getNumberOfFlags() const
{
    return flags.count();
}

QString Machine::getFlagName(int id) const
{
    return flags[id]->getName();
}

int Machine::getFlagValue(int id) const
{
    return flags[id]->getValue();
}

void Machine::setFlagValue(int id, int value)
{
    flags[id]->setValue(value);
}

void Machine::clearFlags()
{
    for (int i=0; i<flags.size(); i++)
        flags[i]->resetValue();
}

void Machine::clearAssemblerMemory()
{
    for (int i=0; i<assemblerMemory.size(); i++)
    {
        assemblerMemory[i]->setValue(0);
        reserved[i] = false;
        correspondingLine[i] = -1;
    }
}

// Copies assemblerMemory to machine's memory
void Machine::copyAssemblerMemoryToMemory()
{
    for (int i=0; i<memory.size(); i++)
        memory[i]->setValue(assemblerMemory[i]->getValue());
}

// Reserve 'sizeToReserve' bytes starting from PC. Returns ErrorCode 0 if successful
Machine::ErrorCode Machine::reserveAssemblerMemory(int sizeToReserve)
{
    while (sizeToReserve > 0)
    {
        if (!reserved[PC->getValue()])
        {
            reserved[PC->getValue()] = true;
            PC->incrementValue();
            sizeToReserve--;
        }
        else
            return memoryOverflow; // Already reserved
    }

    return noError;
}

bool Machine::isValidValue(QString valueString, int min, int max)
{
    bool ok;
    int value = valueString.toInt(&ok, 0);

    // Returns true if conversion ok and value between min and max
    return (ok && value >= min && value <= max);
}

// Checks if string is a valid number for the machine
bool Machine::isValidByteValue(QString valueString)
{
    return isValidValue(valueString, 0, 255);
}

/*bool Machine::isValidRegisterValue(QString valueString)
{

}*/

bool Machine::isValidAddress(QString addressString)
{
    return isValidValue(addressString, 0, memory.size()-1);
}

void Machine::setRunning(bool running)
{
    this->running = running;
}

bool Machine::isRunning() const
{
    return this->running;
}

void Machine::emitError(int lineNumber, Machine::ErrorCode errorCode)
{
    QString errorString;
    errorString += "Linha " + QString::number(lineNumber+1) + ": ";
    switch(errorCode)
    {
    case wrongNumberOfArguments:
        errorString += "Número de argumentos inválido.";
        break;

    case invalidInstruction:
        errorString += "Mnemônico inválido.";
        break;

    case invalidAddress:
        errorString += "Endereço inválido.";
        break;

    case invalidValue:
        errorString += "Valor inválido.";
        break;

    case invalidLabel:
        errorString += "Label inválido.";
        break;

    case invalidArgument:
        errorString += "Argumento inválido.";
        break;

    case duplicatedLabel:
        errorString += "Label já definido.";
        break;

    case notImplemented:
        errorString += "Funcionalidade não implementada.";
        break;

    default:
        errorString += "Erro indefinido.";
        break;
    }

    emit buildErrorDetected(errorString);
}

Machine::ErrorCode Machine::obeyDirective(QString mnemonic, QString arguments, bool reserveOnly)
{
    QStringList argumentList = arguments.split(" ", QString::SkipEmptyParts);
    int numberOfArguments = argumentList.size();

    if (mnemonic == "org")
    {
        if (numberOfArguments != 1)
            return wrongNumberOfArguments;

        if (!isValidAddress(argumentList.first()))
            return invalidAddress;

        PC->setValue(argumentList.first().toInt(0));
    }
    else if (mnemonic == "db")
    {
        if (numberOfArguments != 1)
            return wrongNumberOfArguments;

        if (!isValidByteValue(argumentList.first()))
            return invalidValue;

        if (reserveOnly)
        {
            return reserveAssemblerMemory(1); // Increments PC
        }
        else
        {
            assemblerMemory[PC->getValue()]->setValue(argumentList.first().toInt(0));
            PC->incrementValue();
        }
    }
    // TO-DO: dw, dab, daw (check endianness)
    else if (mnemonic == "dw" || mnemonic == "dab" || mnemonic == "daw")
    {
        return notImplemented;
    }
    else
    {
        return invalidInstruction;
    }

    return noError;
}

void Machine::assemble(QString sourceCode)
{
    Machine::ErrorCode errorCode;

    running = false;
    buildSuccessful = false;

    //////////////////////////////////////////////////
    // Simplify source code
    //////////////////////////////////////////////////

    QStringList sourceLines = sourceCode.split("\n"); //separa o arquivo fonte por linhas de codigo

    // Strip comments and extra spaces:
    for (int lineNumber = 0; lineNumber < sourceLines.size(); lineNumber++)
    {
        sourceLines[lineNumber] = sourceLines[lineNumber].section(";", 0, 0).simplified().toLower(); //elimina os comentarios do codigo
    }



    //////////////////////////////////////////////////
    // FIRST PASS: Read labels, reserve memory
    //////////////////////////////////////////////////

    QHash<QString, int> labelPCMap;
    clearAssemblerMemory();
    PC->setValue(0);

    QRegExp validLabel("[a-z_][a-z0-9_]*"); // Validates label names (must start with a letter/underline, may have numbers)

    for (int lineNumber = 0; lineNumber < sourceLines.size(); lineNumber++)
    {
        //////////////////////////////////////////////////
        // Read labels
        //////////////////////////////////////////////////

        if (sourceLines[lineNumber].contains(":")) // If getLabel found a label
        {
            QString labelName = sourceLines[lineNumber].section(":", 0, 0);

            // Check if label name is valid:
            if (!validLabel.exactMatch(labelName))
            {
                emitError(lineNumber, invalidLabel);
                return;
            }

            // Check for duplicated labels:
            if (labelPCMap.contains(labelName))
            {
                emitError(lineNumber, duplicatedLabel);
                return;
            }

            labelPCMap.insert(labelName, PC->getValue()); // Add to map
            sourceLines[lineNumber] = sourceLines[lineNumber].replace(labelName + ":", "").simplified(); // Remove label from sourceLines
        }



        //////////////////////////////////////////////////
        // Reserve memory for instructions/directives
        //////////////////////////////////////////////////

        if (!sourceLines[lineNumber].isEmpty())
        {
            QString mnemonic = sourceLines[lineNumber].section(" ", 0, 0);

            const Instruction *instruction = getInstructionFromMnemonic(mnemonic);
            if (instruction != NULL)
            {
                errorCode = reserveAssemblerMemory(instruction->getSize());
            }
            else // Directive
            {
                QString arguments = sourceLines[lineNumber].section(" ", 1); // Everything after mnemonic
                errorCode = obeyDirective(mnemonic, arguments, true);
            }

            if (errorCode)
            {
                emitError(lineNumber, errorCode);
                return;
            }
        }
    }



    //////////////////////////////////////////////////
    // SECOND PASS: Mount instructions/defines
    //////////////////////////////////////////////////

    correspondingAddress = QVector<int>(sourceLines.size());
    PC->setValue(0);

    for (int lineNumber = 0; lineNumber < sourceLines.size(); lineNumber++)
    {
        correspondingAddress[lineNumber] = PC->getValue();

        if (!sourceLines[lineNumber].isEmpty())
        {
            QString mnemonic  = sourceLines[lineNumber].section(" ", 0, 0);
            QString arguments = sourceLines[lineNumber].section(" ", 1); // Everything after mnemonic

            const Instruction *instruction = getInstructionFromMnemonic(mnemonic);
            if (instruction != NULL)
            {
                correspondingLine[PC->getValue()] = lineNumber;
                errorCode = mountInstruction(mnemonic, arguments, labelPCMap);
            }
            else // Directive
            {
                correspondingLine[PC->getValue()] = lineNumber;
                errorCode = obeyDirective(mnemonic, arguments, false);
            }

            if (errorCode)
            {
                emitError(lineNumber, errorCode);
                return;
            }
        }
    }

    buildSuccessful = true;
    copyAssemblerMemoryToMemory();
    clearRegisters();
    clearFlags();
    clearCounters();
}
