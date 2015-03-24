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

// Reserve 'sizeToReserve' bytes starting from PC. Throws exception on overlap.
void Machine::reserveAssemblerMemory(int sizeToReserve)
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
            throw memoryOverlap; // Memory already reserved
    }
}

bool Machine::isValidValue(QString valueString, int min, int max)
{
    bool ok;
    int value = valueString.toInt(&ok, 0);

    // Returns true if conversion ok and value between min and max
    return (ok && value >= min && value <= max);
}

// Checks if string is a valid number for the machine
bool Machine::isValidNBytesValue(QString valueString, int n)
{
    if (n == 1)
        return isValidValue(valueString, -128, 255);
    else
        return isValidValue(valueString, -32768, 65535);
}

/*bool Machine::isValidRegisterValue(QString valueString)
{

}*/

bool Machine::isValidAddress(QString addressString)
{
    return isValidValue(addressString, 0, memory.size()-1);
}

QStringList Machine::splitArguments(QString arguments)
{
    QStringList finalArgumentList;

    // Regular expressions
    QRegExp matchBrackets("\\[(\\d+)\\]"); // Digits between brackets
    QRegExp matchSeparator("\\s|,"); // Whitespace or comma

    arguments = arguments.trimmed(); // Trim whitespace



    //////////////////////////////////////////////////
    // Byte/Word allocation
    //////////////////////////////////////////////////

    if (matchBrackets.exactMatch(arguments))
    {
        finalArgumentList.append(ALLOCATE_SYMBOL + matchBrackets.cap(1));
        return finalArgumentList;
    }



    //////////////////////////////////////////////////
    // Process string char by char
    //////////////////////////////////////////////////

    QString numberString; // Incrementally add characters to this string until a separator/ampersand is found
    int charCount = 0; // Char count of ASCII string (treats quote as literal char if count equals zero)

    bool insideString = false;

    for (QChar &c : arguments)
    {
        // NUMBER MODE:
        if (insideString == false)
        {
            if (c == SINGLE_QUOTE)
            {
                // Add finished number to argument list
                if (numberString != "")
                {
                    finalArgumentList.append(numberString);
                    numberString = "";
                }

                // Begin string
                insideString = true;
            }
            else if (matchSeparator.exactMatch(c))
            {
                // Add finished number to argument list
                if (numberString != "")
                {
                    finalArgumentList.append(numberString);
                    numberString = "";
                }
            }
            else
            {
                // Add character to number string
                numberString += c;
            }
        }

        // ASCII STRING MODE:
        else // (insideString == true)
        {
            if (c == SINGLE_QUOTE && charCount != 0)
            {
                // Finish string and enter number mode
                insideString = false;
                charCount = 0;
            }
            else
            {
                // Add character to argument list
                finalArgumentList.append(CHAR_SYMBOL + c);
                charCount += 1;
            }
        }
    }

    // Add remaining number
    if (numberString != "")
    {
        finalArgumentList.append(numberString);
        numberString = "";
    }

    if (finalArgumentList.count() == 0)
        throw wrongNumberOfArguments;
    else if (insideString)
        throw invalidString;

    return finalArgumentList;
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

    case invalidString:
        errorString += "String inválido.";
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

    case memoryOverlap:
        errorString += "Sobreposição de memória.";
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

void Machine::obeyDirective(QString mnemonic, QString arguments, bool reserveOnly, QHash<QString, int> &labelPCMap)
{
    if (mnemonic == "org")
    {
        QStringList argumentList = arguments.split(" ", QString::SkipEmptyParts);
        int numberOfArguments = argumentList.size();

        if (numberOfArguments != 1)
            throw wrongNumberOfArguments;
        if (!isValidAddress(argumentList.first()))
            throw invalidAddress;

        PC->setValue(argumentList.first().toInt(0));
    }
    else if (QRegExp("da?[bw]").exactMatch(mnemonic))
    {
        QStringList argumentList;
        argumentList = splitArguments(arguments);

        int numberOfArguments = argumentList.size();
        int bytesPerArgument = QRegExp("da?b").exactMatch(mnemonic) ? 1 : 2;

        if ((mnemonic == "db" || mnemonic == "dw") && numberOfArguments > 1)
            throw wrongNumberOfArguments;

        if ((mnemonic == "db" || mnemonic == "dw") && numberOfArguments == 0)
            argumentList.append("0"); // Default to argument 0 in case of DB and DW

        // Memory allocation
        if (argumentList.first().at(0) == ALLOCATE_SYMBOL)
        {
            if (mnemonic == "dab" || mnemonic == "daw")
            {
                throw invalidArgument;
            }
            else if (reserveOnly)
            {
                reserveAssemblerMemory(argumentList.first().mid(1).toInt() * bytesPerArgument);
            }
            else // Skip bytes
            {
                for (int i = 0; i < argumentList.first().mid(1).toInt()*bytesPerArgument; i++)
                    PC->incrementValue();
            }
        }

        if (reserveOnly)
        {
            reserveAssemblerMemory(numberOfArguments * bytesPerArgument); // Increments PC
        }
        else
        {
            // Process each argument
            for (QString argument : argumentList)
            {
                int value = 0;

                // Process character
                if (argument.at(0) == CHAR_SYMBOL)
                {
                    value = argument.at(1).toLatin1();
                }

                // Process label
                else if (labelPCMap.contains(argument.toLower()))
                {
                    if (mnemonic == "dab" || mnemonic == "daw")
                        throw invalidArgument;

                    value = labelPCMap.value(argument.toLower());
                }

                // Process hexadecimal number
                else if (argument.at(0).toLower() == 'h')
                {
                    // Convert hexadecimal string to int
                    bool ok;
                    value = argument.mid(1).toInt(&ok, 16);

                    // Check if invalid
                    if (!ok)
                        throw invalidArgument;
                    else if (!isValidNBytesValue(QString::number(value), bytesPerArgument) || value < 0)
                        throw invalidValue;
                }

                // Process decimal number
                else
                {
                    // Convert decimal string to int
                    bool ok;
                    value = argument.toInt(&ok, 10);

                    // Check if invalid
                    if (!ok)
                        throw invalidArgument;
                    else if (!isValidNBytesValue(argument, bytesPerArgument))
                        throw invalidValue;

                    //value = convertToUnsigned(argument.toInt(), bytesPerArgument);
                }



                // Write final value
                if (bytesPerArgument == 2)
                {
                    assemblerMemory[PC->getValue()]->setValue((value & 0xFF00) >> 8); // Big endian
                    PC->incrementValue();
                }

                assemblerMemory[PC->getValue()]->setValue(value & 0x00FF);
                PC->incrementValue();
            }
        }
    }
    else
    {
        throw invalidInstruction;
    }
}

void Machine::assemble(QString sourceCode)
{
    int errorCount = 0;

    running = false;
    buildSuccessful = false;

    //////////////////////////////////////////////////
    // Simplify source code
    //////////////////////////////////////////////////

    QStringList sourceLines = sourceCode.split("\n"); // Split source code to individual lines

    // Strip comments and extra spaces
    for (int lineNumber = 0; lineNumber < sourceLines.size(); lineNumber++)
    {
        sourceLines[lineNumber] = sourceLines[lineNumber].section(";", 0, 0).trimmed();
    }



    //////////////////////////////////////////////////
    // FIRST PASS: Read labels, reserve memory
    //////////////////////////////////////////////////

    QHash<QString, int> labelPCMap;
    clearAssemblerMemory();
    PC->setValue(0);

    QRegExp validLabel("[a-z_][a-z0-9_]*"); // Validates label names (must start with a letter/underline, may have numbers)
    QRegExp whitespaces("\\s+");

    for (int lineNumber = 0; lineNumber < sourceLines.size(); lineNumber++)
    {
        try
        {
            //////////////////////////////////////////////////
            // Read labels
            //////////////////////////////////////////////////

            if (sourceLines[lineNumber].contains(":")) // If getLabel found a label
            {
                QString labelName = sourceLines[lineNumber].section(":", 0, 0);

                // Check for invalid or duplicated label
                if (!validLabel.exactMatch(labelName.toLower()))
                    throw invalidLabel;
                if (labelPCMap.contains(labelName.toLower()))
                    throw duplicatedLabel;

                labelPCMap.insert(labelName.toLower(), PC->getValue()); // Add to map
                sourceLines[lineNumber] = sourceLines[lineNumber].replace(labelName + ":", "").trimmed(); // Remove label from sourceLines
            }



            //////////////////////////////////////////////////
            // Reserve memory for instructions/directives
            //////////////////////////////////////////////////

            if (!sourceLines[lineNumber].isEmpty())
            {
                QString mnemonic = sourceLines[lineNumber].section(whitespaces, 0, 0).toLower();

                const Instruction *instruction = getInstructionFromMnemonic(mnemonic);
                if (instruction != NULL)
                {
                    reserveAssemblerMemory(instruction->getSize());
                }
                else // Directive
                {
                    QString arguments = sourceLines[lineNumber].section(whitespaces, 1); // Everything after mnemonic
                    obeyDirective(mnemonic, arguments, true, labelPCMap);
                }
            }
        }
        catch (ErrorCode errorCode)
        {
            emitError(lineNumber, errorCode);
            errorCount += 1;
        }
    }

    if (errorCount > 0)
        return; // Abort compilation



    //////////////////////////////////////////////////
    // SECOND PASS: Mount instructions/defines
    //////////////////////////////////////////////////

    correspondingAddress = QVector<int>(sourceLines.size());
    PC->setValue(0);

    for (int lineNumber = 0; lineNumber < sourceLines.size(); lineNumber++)
    {
        try
        {
            correspondingAddress[lineNumber] = PC->getValue();

            if (!sourceLines[lineNumber].isEmpty())
            {
                QString mnemonic  = sourceLines[lineNumber].section(whitespaces, 0, 0).toLower();
                QString arguments = sourceLines[lineNumber].section(whitespaces, 1); // Everything after mnemonic

                const Instruction *instruction = getInstructionFromMnemonic(mnemonic);
                if (instruction != NULL)
                {
                    correspondingLine[PC->getValue()] = lineNumber;
                    mountInstruction(mnemonic, arguments.toLower(), labelPCMap);
                }
                else // Directive
                {
                    correspondingLine[PC->getValue()] = lineNumber;
                    obeyDirective(mnemonic, arguments, false, labelPCMap);
                }
            }
        }
        catch (ErrorCode errorCode)
        {
            emitError(lineNumber, errorCode);
            errorCount += 1;
        }
    }

    if (errorCount > 0)
        return; // Abort compilation



    buildSuccessful = true;
    copyAssemblerMemoryToMemory();
    clearRegisters();
    clearFlags();
    clearCounters();
}
