#include "machine.h"

#define DEBUG_INT(value) qDebug(QString::number(value).toStdString().c_str());
#define DEBUG_STRING(value) qDebug(value.toStdString().c_str());

Machine::Machine(QObject *parent) :
    QObject(parent)
{
}

Machine::~Machine()
{
    qDeleteAll(memory);
    qDeleteAll(assemblerMemory);
    qDeleteAll(registers);
    qDeleteAll(flags);
    qDeleteAll(instructions);
}



//////////////////////////////////////////////////
// Import/Export memory
//////////////////////////////////////////////////

// Returns true if successful
FileErrorCode::FileErrorCode Machine::importMemory(QString filename)
{
    char byte;
    QFile memFile(filename); // Implicitly closed

    // Open file
    memFile.open(QFile::ReadOnly);

    if (memFile.size() != 1 + identifier.length() + memory.size() * 2)
        return FileErrorCode::incorrectSize;

    // Read identifier length
    memFile.getChar(&byte);
    if (byte != identifier.length())
        return FileErrorCode::invalidIdentifier; // Incorrect identifier length

    // Read identifier
    for (int i = 0; i < identifier.length(); i++)
    {
        memFile.getChar(&byte);

        if (byte != identifier[i].toLatin1())
            return FileErrorCode::invalidIdentifier; // Wrong character
    }

    // Read memory
    for (int index = 0; index < memory.length(); index++)
    {
        memFile.getChar(&byte);
        memory[index]->setValue(byte);
        memFile.getChar(&byte); // Skip byte
    }

    // Return error status
    if (memFile.error() != QFileDevice::NoError)
        return FileErrorCode::inputOutput;
    else
        return FileErrorCode::noError;
}

// Returns true if successful
FileErrorCode::FileErrorCode Machine::exportMemory(QString filename)
{
    QFile memFile(filename); // Implicitly closed

    // Open file
    memFile.open(QFile::WriteOnly);

    // Write identifier length
    memFile.putChar((unsigned char)identifier.length());

    // Write identifier
    for (int i = 0; i < identifier.length(); i++)
    {
        memFile.putChar(identifier.at(i).toLatin1());
    }

    // Write memory bytes
    foreach (Byte *byte, memory)
    {
        memFile.putChar(byte->getValue());
        memFile.putChar(0);
    }

    // Return error status
    if (memFile.error() != QFileDevice::NoError)
        return FileErrorCode::inputOutput;
    else
        return FileErrorCode::noError;
}



//////////////////////////////////////////////////
// Step
//////////////////////////////////////////////////

void Machine::step()
{
    int byteArray[4], registerId, operandAddress;
    AddressingMode addressingMode;
    Instruction *instruction = nullptr;

    fetchInstruction(byteArray, instruction);
    decodeInstruction(byteArray, instruction, addressingMode, registerId, operandAddress);
    executeInstruction(instruction, registerId, operandAddress);
}

void Machine::fetchInstruction(int byteArray[], Instruction *&instruction)
{
    // Read first byte
    byteArray[0] = memoryReadNext();
    instruction = getInstructionFromValue(byteArray[0]);
}

void Machine::decodeInstruction(int byteArray[], Instruction *&instruction, AddressingMode &addressingMode, int &registerId, int &operandAddress)
{
    addressingMode = extractAddressingMode(byteArray);
    registerId = extractRegisterId(byteArray);

    if (instruction->getNumBytes() == 2)
    {
        int immediateAddress = getPCValue();
        operandAddress = memoryGetOperandAddress(immediateAddress, addressingMode);
        incrementPCValue();
    }
}

void Machine::executeInstruction(Instruction *&instruction, int registerId, int operandAddress)
{
    int value1, value2, result;

    switch (instruction->getInstructionCode())
    {
    case Instruction::NOP:
        break;

    case Instruction::ADD:
        value1 = getRegisterValue(registerId);
        value2 = memoryRead(operandAddress);
        result = (value1 + value2) & 0xFF;

        setRegisterValue(registerId, result);
        setCarry(value1 + value2 > 0xFF);
        setOverflow(toSigned(value1) + toSigned(value2) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction::JN:
        if (getFlagValue("N") == true)
            setPCValue(operandAddress);
        break;

    default:
        break;
    }

    instructionCount++;
}

Machine::AddressingMode Machine::extractAddressingMode(int[])
{
    return AddressingMode::DIRECT;
}

int Machine::extractRegisterId(int[])
{
    return 0;
}

void Machine::setOverflow(bool)
{
    return;
}

void Machine::setCarry(bool)
{
    return;
}

void Machine::setBorrowOrCarry(bool)
{
    return;
}

void Machine::updateFlags(int value)
{
    setFlagValue("N", toSigned(value) < 0);
    setFlagValue("Z", value == 0);
}

int Machine::toSigned(int unsignedByte)
{
    if (unsignedByte <= 127) // Max signed byte
        return unsignedByte;
    else
        return unsignedByte - 256;
}



//////////////////////////////////////////////////
// Memory read/write with access count
//////////////////////////////////////////////////

int Machine::memoryRead(int address)
{
    accessCount++;
    return getMemoryValue(address);
}

void Machine::memoryWrite(int address, int value)
{
    accessCount++;
    setMemoryValue(address, value);
}

int Machine::memoryReadNext()
{
    int value = memoryRead(getPCValue());
    setPCValue(getPCValue() + 1);
    accessCount++;
    return value;
}

int Machine::memoryGetOperandAddress(int immediateAddress, AddressingMode addressingMode)
{
    switch (addressingMode)
    {
        case AddressingMode::DIRECT:
            return memoryRead(immediateAddress);

        case AddressingMode::INDIRECT:
            return memoryRead(memoryRead(immediateAddress));

        case AddressingMode::IMMEDIATE: // Immediate addressing mode
            return immediateAddress;

        case AddressingMode::INDEXED: // Indexed addressing mode
            return memoryRead((memoryRead(immediateAddress) + getRegisterValue("X")) % getMemorySize());
    }

    return 0;
}



//////////////////////////////////////////////////
// Assembly
//////////////////////////////////////////////////

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
                    reserveAssemblerMemory(instruction->getNumBytes());
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
                    // TODO: associateLine instead on FIRST PASS
                    correspondingLine[PC->getValue()] = lineNumber;
                    if (instruction->getNumBytes() == 2)
                        correspondingLine[(PC->getValue() + 1) % 256] = lineNumber;

                    mountInstruction(mnemonic, arguments.toLower(), labelPCMap);
                }
                else // Directive
                {
                    // TODO: correspondingLine for arrays (DAB, DAW)
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

        if ((mnemonic == "db" || mnemonic == "dw") && numberOfArguments == 0)
            argumentList.append("0"); // Default to argument 0 in case of DB and DW

        if ((mnemonic == "db" || mnemonic == "dw") && numberOfArguments > 1) // Too much arguments
            throw wrongNumberOfArguments;

        if ((mnemonic == "dab" || mnemonic == "daw") && numberOfArguments < 1) // No arguments
            throw wrongNumberOfArguments;

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

void Machine::emitError(int lineNumber, Machine::ErrorCode errorCode)
{
    QString errorString;
    errorString += "Linha " + QString::number(lineNumber+1) + ": ";

    QMap<Machine::ErrorCode, QString> errorMessages;
    errorMessages[wrongNumberOfArguments] = "Número de argumentos inválido.";
    errorMessages[invalidInstruction] = "Mnemônico inválido.";
    errorMessages[invalidAddress] = "Endereço inválido.";
    errorMessages[invalidValue] = "Valor inválido.";
    errorMessages[invalidString] = "String inválido.";
    errorMessages[invalidLabel] = "Label inválido.";
    errorMessages[invalidArgument] = "Argumento inválido.";
    errorMessages[duplicatedLabel] = "Label já definido.";
    errorMessages[memoryOverlap] = "Sobreposição de memória.";
    errorMessages[notImplemented] = "Funcionalidade não implementada.";
    errorMessages[undefinedError] = "Erro indefinido.";

    if (errorMessages.contains(errorCode))
        errorString += errorMessages[errorCode];
    else
        errorString += errorMessages[undefinedError];

    emit buildErrorDetected(errorString);
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

    if (insideString)
        throw invalidString;

    return finalArgumentList;
}




//////////////////////////////////////////////////
// Getters/setters, clear
//////////////////////////////////////////////////

bool Machine::isRunning() const
{
    return this->running;
}

void Machine::setRunning(bool running)
{
    this->running = running;
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

int Machine::getFlagValue(QString flagName) const
{
    foreach (Flag *flag, flags)
    {
        if (flag->getName() == flagName)
            return flag->getValue();
    }

    throw QString("Invalid flag name: ") + flagName;
}

void Machine::setFlagValue(int id, int value)
{
    flags[id]->setValue(value);
}

void Machine::setFlagValue(QString flagName, int value)
{
    foreach (Flag *flag, flags)
    {
        if (flag->getName() == flagName)
        {
            flag->setValue(value);
            return;
        }
    }

    throw QString("Invalid flag name: ") + flagName;
}

void Machine::clearFlags()
{
    for (int i=0; i<flags.size(); i++)
        flags[i]->resetValue();
}

int Machine::getNumberOfRegisters() const
{
    return registers.count();
}

QString Machine::getRegisterName(int id) const
{
    QString name = registers[id]->getName();

    if (name != "")
        return name;
    else
        return "R" + QString::number(id); // Default to R0..R63
}

int Machine::getRegisterValue(int id) const
{
    return registers[id]->getValue();
}

int Machine::getRegisterValue(QString registerName) const
{
    foreach (Register *reg, registers)
    {
        if (reg->getName() == registerName)
            return reg->getValue();
    }

    throw QString("Invalid register name: ") + registerName;
}

void Machine::setRegisterValue(int id, int value)
{
    registers[id]->setValue(value);
}

void Machine::setRegisterValue(QString registerName, int value)
{
    foreach (Register *reg, registers)
    {
        if (reg->getName() == registerName)
        {
            reg->setValue(value);
            return;
        }
    }

    throw QString("Invalid register name: ") + registerName;
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

void Machine::incrementPCValue()
{
    PC->setValue(PC->getValue() + 1);
}

int Machine::getPCCorrespondingLine()
{
    if (!correspondingLine.isEmpty())
        return correspondingLine[PC->getValue()];
    else
        return -1;
}

int Machine::getAddressCorrespondingLine(int address)
{
    if (!correspondingLine.isEmpty() && address >= 0 && address < correspondingLine.size())
        return correspondingLine[address];
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

Instruction* Machine::getInstructionFromValue(int value)
{
    foreach (Instruction *instruction, instructions)
    {
        if (instruction->matchByte(value))
            return instruction;
    }

    throw QString("Invalid instruction value: %1").arg(value);
}

Instruction* Machine::getInstructionFromMnemonic(QString mnemonic)
{
    foreach (Instruction *instruction, instructions)
    {
        if (instruction->getMnemonic() == mnemonic)
            return instruction;
    }

    return nullptr;
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
