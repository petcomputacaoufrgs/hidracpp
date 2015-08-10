#include "machine.h"

#define DEBUG_INT(value) qDebug(QString::number(value).toStdString().c_str());
#define DEBUG_STRING(value) qDebug(value.toStdString().c_str());

Machine::Machine(QObject *parent) :
    QObject(parent)
{
    clearCounters();
    setBreakpoint(-1);
    setRunning(false);
}

Machine::~Machine()
{
    qDeleteAll(memory);
    qDeleteAll(assemblerMemory);
    qDeleteAll(registers);
    qDeleteAll(flags);
    qDeleteAll(instructions);
    qDeleteAll(addressingModes);
}



//////////////////////////////////////////////////
// Step
//////////////////////////////////////////////////

void Machine::step()
{
    int byteArray[4], operandAddress;
    QString registerName;
    AddressingMode::AddressingModeCode addressingModeCode;
    Instruction *instruction = nullptr;

    fetchInstruction(byteArray, instruction); // Outputs bytes read and corresponding instruction
    decodeInstruction(byteArray, instruction, addressingModeCode, registerName, operandAddress); // Outputs addressing mode, register and operand address
    executeInstruction(instruction, addressingModeCode, registerName, operandAddress);

    if (getPCValue() == getBreakpoint())
        setRunning(false);
}

void Machine::fetchInstruction(int byteArray[], Instruction *&instruction)
{
    // Read first byte
    byteArray[0] = memoryReadNext();
    instruction = getInstructionFromValue(byteArray[0]);
}

void Machine::decodeInstruction(int byteArray[], Instruction *&instruction, AddressingMode::AddressingModeCode &addressingModeCode, QString &registerName, int &operandAddress)
{
    addressingModeCode = extractAddressingModeCode(byteArray);
    registerName = extractRegisterName(byteArray);

    if (instruction && instruction->getNumBytes() == 2)
    {
        int immediateAddress = getPCValue();
        operandAddress = memoryGetOperandAddress(immediateAddress, addressingModeCode);
        incrementPCValue();
    }
}

void Machine::executeInstruction(Instruction *&instruction, AddressingMode::AddressingModeCode addressingModeCode, QString registerName, int operandAddress)
{
    int value1, value2, result;
    Instruction::InstructionCode instructionCode;
    instructionCode = (instruction) ? instruction->getInstructionCode() : Instruction::NOP;
    bool isImmediate = (addressingModeCode == AddressingMode::IMMEDIATE); // Invalidate immediate jumps

    switch (instructionCode)
    {
    case Instruction::NOP:
        break;

    case Instruction::LDR:
        result = memoryRead(operandAddress);
        setRegisterValue(registerName, result);
        updateFlags(result);
        break;

    case Instruction::STR:
        result = getRegisterValue(registerName);
        memoryWrite(operandAddress, result);
        break;

    case Instruction::ADD:
        value1 = getRegisterValue(registerName);
        value2 = memoryRead(operandAddress);
        result = (value1 + value2) & 0xFF;

        setRegisterValue(registerName, result);
        setCarry(value1 + value2 > 0xFF);
        setOverflow(toSigned(value1) + toSigned(value2) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction::OR:
        value1 = getRegisterValue(registerName);
        value2 = memoryRead(operandAddress);
        result = (value1 | value2);

        setRegisterValue(registerName, result);
        updateFlags(result);
        break;

    case Instruction::AND:
        value1 = getRegisterValue(registerName);
        value2 = memoryRead(operandAddress);
        result = (value1 & value2);

        setRegisterValue(registerName, result);
        updateFlags(result);
        break;

    case Instruction::NOT:
        value1 = getRegisterValue(registerName);
        result = ~value1 & 0xFF;

        setRegisterValue(registerName, result);
        updateFlags(result);
        break;

    case Instruction::SUB:
        value1 = getRegisterValue(registerName);
        value2 = memoryRead(operandAddress);
        result = (value1 - value2) & 0xFF;

        setRegisterValue(registerName, result);
        setBorrowOrCarry(value1 < value2);
        setOverflow(toSigned(value1) - toSigned(value2) != toSigned(result));
        break;

    case Instruction::JMP:
        if (!isImmediate) // Invalidate immediate jumps
            setPCValue(operandAddress);
        break;

    case Instruction::JN:
        if (getFlagValue("N") == true && !isImmediate)
            setPCValue(operandAddress);
        break;

    case Instruction::JP:
        if (getFlagValue("N") == false && !isImmediate)
            setPCValue(operandAddress);
        break;

    case Instruction::JV:
        if (getFlagValue("V") == true && !isImmediate)
            setPCValue(operandAddress);
        break;

    case Instruction::JNV:
        if (getFlagValue("V") == false && !isImmediate)
            setPCValue(operandAddress);
        break;

    case Instruction::JZ:
        if (getFlagValue("Z") == true && !isImmediate)
            setPCValue(operandAddress);
        break;

    case Instruction::JNZ:
        if (getFlagValue("Z") == false && !isImmediate)
            setPCValue(operandAddress);
        break;

    case Instruction::JC:
        if (getFlagValue("C") == true && !isImmediate)
            setPCValue(operandAddress);
        break;

    case Instruction::JNC:
        if (getFlagValue("C") == false && !isImmediate)
            setPCValue(operandAddress);
        break;

    case Instruction::JB:
        if (getFlagValue("B") == true && !isImmediate)
            setPCValue(operandAddress);
        break;

    case Instruction::JNB:
        if (getFlagValue("B") == false && !isImmediate)
            setPCValue(operandAddress);
        break;

    case Instruction::JSR:
        if (!isImmediate)
        {
            memoryWrite(operandAddress, getPCValue());
            setPCValue(operandAddress+1);
        }
        break;

    case Instruction::NEG:
        value1 = getRegisterValue(registerName);
        result = (-value1) & 0xFF;

        setRegisterValue(registerName, result);
        updateFlags(result);
        break;

    case Instruction::SHR:
        value1 = getRegisterValue(registerName);
        result = (value1 >> 1) & 0xFF;

        setRegisterValue(registerName, result);
        setCarry(value1 & 0x01);
        break;

    case Instruction::SHL:
        value1 = getRegisterValue(registerName);
        result = (value1 << 1) & 0xFF;

        setRegisterValue(registerName, result);
        setCarry((value1 & 0x80) ? 1 : 0);
        break;

    case Instruction::ROR:
        value1 = getRegisterValue(registerName);
        result = ((value1 >> 1) | (getFlagValue("C") == true ? 0x80 : 0x00)) & 0xFF;

        setRegisterValue(registerName, result);
        setCarry(value1 & 0x01);
        break;

    case Instruction::ROL:
        value1 = getRegisterValue(registerName);
        result = ((value1 << 1) | (getFlagValue("C") == true ? 0x01 : 0x00)) & 0xFF;

        setRegisterValue(registerName, result);
        setCarry((value1 & 0x80) ? 1 : 0);
        break;

    case Instruction::HLT:
        setRunning(false);
        break;

    default:
        break;
    }

    instructionCount++;
}

AddressingMode::AddressingModeCode Machine::extractAddressingModeCode(int byteArray[])
{
    foreach (AddressingMode *addressingMode, addressingModes)
    {
        QRegExp matchAddressingMode(addressingMode->getBitPattern());

        if (matchAddressingMode.exactMatch(Byte(byteArray[0]).toString()))
            return addressingMode->getAddressingModeCode();
    }

    throw QString("Addressing mode not found.");
}

QString Machine::extractRegisterName(int byteArray[])
{
    foreach (Register *reg, registers)
    {
        if (reg->matchByte(byteArray[0]))
            return reg->getName();
    }

    return ""; // Undefined register
}

void Machine::setOverflow(bool state)
{
    foreach (Flag *flag, flags)
    {
        if (flag->getFlagCode() == Flag::OVERFLOW)
            flag->setValue(state);
    }
}

void Machine::setCarry(bool state)
{
    setFlagValue(Flag::CARRY, state);
    setFlagValue(Flag::CARRY_NOT_BORROW, state);
}

void Machine::setBorrowOrCarry(bool borrowState)
{
    setFlagValue(Flag::BORROW, borrowState);
    setFlagValue(Flag::CARRY_NOT_BORROW, !(borrowState));
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
// Assembler
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

    clearAssemblerData();
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
                addressCorrespondingLabel[PC->getValue()] = labelName;
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
                    obeyDirective(mnemonic, arguments, true);
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
    // SECOND PASS: Build instructions/defines
    //////////////////////////////////////////////////

    lineCorrespondingAddress.fill(-1, sourceLines.size());
    PC->setValue(0);

    for (int lineNumber = 0; lineNumber < sourceLines.size(); lineNumber++)
    {
        try
        {
            lineCorrespondingAddress[lineNumber] = PC->getValue();

            if (!sourceLines[lineNumber].isEmpty())
            {
                QString mnemonic  = sourceLines[lineNumber].section(whitespaces, 0, 0).toLower();
                QString arguments = sourceLines[lineNumber].section(whitespaces, 1); // Everything after mnemonic

                const Instruction *instruction = getInstructionFromMnemonic(mnemonic);
                if (instruction != NULL)
                {
                    // TODO: associateLine instead on FIRST PASS
                    addressCorrespondingLine[PC->getValue()] = lineNumber;
                    if (instruction->getNumBytes() == 2)
                        addressCorrespondingLine[(PC->getValue() + 1) & 0xFF] = lineNumber;

                    buildInstruction(mnemonic, arguments.toLower());
                }
                else // Directive
                {
                    // TODO: addressCorrespondingLine for arrays (DAB, DAW)
                    if (mnemonic != "org")
                        addressCorrespondingLine[PC->getValue()] = lineNumber;
                    obeyDirective(mnemonic, arguments, false);
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

void Machine::obeyDirective(QString mnemonic, QString arguments, bool reserveOnly)
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
    else if (QRegExp("db|dw|dab|daw").exactMatch(mnemonic))
    {
        QStringList argumentList;
        argumentList = splitArguments(arguments);

        int numberOfArguments = argumentList.size();
        int bytesPerArgument = (mnemonic == "db" || mnemonic == "dab") ? 1 : 2;
        bool isArray = (mnemonic == "dab" || mnemonic == "daw") ? true : false;

        if (bytesPerArgument == 1 && numberOfArguments == 0)
        {
            argumentList.append("0"); // Default to argument 0 in case of DB and DW
            numberOfArguments = 1;
        }

        if (isArray && numberOfArguments > 1) // Too many arguments
            throw wrongNumberOfArguments;

        if (isArray && numberOfArguments < 1) // No arguments
            throw wrongNumberOfArguments;

        // Memory allocation
        if (argumentList.first().at(0) == ALLOCATE_SYMBOL)
        {
            if (mnemonic != "dab" && mnemonic != "daw")
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
        else if (reserveOnly)
        {
            reserveAssemblerMemory(numberOfArguments * bytesPerArgument); // Increments PC
        }
        else
        {
            // Process each argument
            for (QString argument : argumentList)
            {
                int value = 0;
                bool ok;

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

void Machine::buildInstruction(QString mnemonic, QString arguments)
{
    Instruction *instruction = getInstructionFromMnemonic(mnemonic);
    QStringList argumentList = arguments.split(" ", QString::SkipEmptyParts);
    int numberOfArguments = instruction->getArguments().size();

    int registerBitCode = 0b00000000;
    int addressingModeBitCode = 0b00000000;

    // Check if correct number of arguments:
    if (argumentList.size() != numberOfArguments)
        throw wrongNumberOfArguments;

    // If first argument is a register:
    if (numberOfArguments > 0 && instruction->getArguments().first() == "r")
    {
        registerBitCode = getRegisterBitCode(argumentList.first());

        if (registerBitCode == Register::NO_BIT_CODE)
            throw invalidArgument; // Hidden register/invalid register name
    }

    // If last argument is an address:
    if (numberOfArguments > 0 && instruction->getArguments().last() == "a")
    {
        AddressingMode::AddressingModeCode addressingModeCode;
        extractArgumentAddressingModeCode(argumentList.last(), addressingModeCode); // Removes addressing mode from argument
        addressingModeBitCode = getAddressingModeBitCode(addressingModeCode);
    }

    // Write first byte (instruction with register and addressing mode):
    assemblerMemory[PC->getValue()]->setValue(instruction->getByteValue() | registerBitCode | addressingModeBitCode);
    PC->incrementValue();

    // If instruction has two bytes, write second byte:
    if (instruction->getNumBytes() > 1)
    {
        // Convert possible label to number:
        if (labelPCMap.contains(argumentList.last()))
            argumentList.last() = QString::number(labelPCMap.value(argumentList.last()));

        // Check if valid address/value:
        if (!isValidAddress(argumentList.last()))
            throw invalidValue;

        // Write address argument:
        assemblerMemory[PC->getValue()]->setValue(argumentList.last().toInt(NULL, 0));
        PC->incrementValue();
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


void Machine::clearAssemblerData()
{
    for (int i=0; i<assemblerMemory.size(); i++)
    {
        assemblerMemory[i]->setValue(0);
        reserved[i] = false;
        addressCorrespondingLine[i] = -1;
        addressCorrespondingLabel[i] = "";
    }

    lineCorrespondingAddress.clear();
    labelPCMap.clear();
}

// Copies assemblerMemory to machine's memory
void Machine::copyAssemblerMemoryToMemory()
{
    for (int i=0; i<memory.size(); i++)
    {
        // Copy only different values to avoid marking bytes as changed
        if (getMemoryValue(i) != assemblerMemory[i]->getValue())
            setMemoryValue(i, assemblerMemory[i]->getValue());
    }
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

void Machine::extractArgumentAddressingModeCode(QString &argument, AddressingMode::AddressingModeCode &addressingModeCode)
{
    addressingModeCode = getDefaultAddressingModeCode();

    foreach (AddressingMode *addressingMode, addressingModes)
    {
        QRegExp matchAddressingMode(addressingMode->getAssemblyPattern());

        if (matchAddressingMode.exactMatch(argument))
        {
            argument = matchAddressingMode.cap(1); // Remove addressing mode
            addressingModeCode = addressingMode->getAddressingModeCode();
            return;
        }
    }
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
    incrementPCValue();
    return value;
}

int Machine::memoryGetOperandAddress(int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode)
{
    switch (addressingModeCode)
    {
        case AddressingMode::DIRECT:
            return memoryRead(immediateAddress);

        case AddressingMode::INDIRECT:
            return memoryRead(memoryRead(immediateAddress));

        case AddressingMode::IMMEDIATE: // Immediate addressing mode
            return immediateAddress;

        case AddressingMode::INDEXED_BY_X: // Indexed addressing mode
            return (memoryRead(immediateAddress) + getRegisterValue("X")) % getMemorySize();

        case AddressingMode::INDEXED_BY_PC:
            return (memoryRead(immediateAddress) + getRegisterValue("PC")) % getMemorySize();
    }

    return 0;
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
    for (int address = 0; address < memory.size(); address++)
    {
        memFile.getChar(&byte);
        setMemoryValue(address, byte);
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

bool Machine::getBuildSuccessful()
{
    return buildSuccessful;
}

int Machine::getBreakpoint() const
{
    return breakpoint;
}

void Machine::setBreakpoint(int value)
{
    if (value >= memory.size() || value < 0)
        breakpoint = -1;
    else
        breakpoint = value;
}

int Machine::getMemorySize() const
{
    return memory.size();
}

void Machine::setMemorySize(int size)
{
    memory.fill(nullptr, size);
    assemblerMemory.fill(nullptr, size);
    reserved.fill(false, size);
    changed.fill(true, size);
    addressCorrespondingLine.fill(-1, size);
    addressCorrespondingLabel.fill("", size);

    for (int i=0; i<memory.size(); i++)
    {
        memory[i] = new Byte();
        assemblerMemory[i] = new Byte();
    }
}

int Machine::getMemoryValue(int address) const
{
    return memory[address]->getValue();
}

void Machine::setMemoryValue(int address, int value)
{
    memory[address]->setValue(value);
    changed[address] = true;
}

bool Machine::hasByteChanged(int address)
{
    if (changed[address])
    {
        changed[address] = false;
        return true;
    }
    else
    {
        return false;
    }
}

void Machine::clearMemory()
{
    for (int i=0; i<memory.size(); i++)
        setMemoryValue(i, 0);
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

void Machine::setFlagValue(Flag::FlagCode flagCode, int value)
{
    foreach (Flag *flag, flags)
    {
        if (flag->getFlagCode() == flagCode)
            flag->setValue(value);
    }
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

int Machine::getRegisterBitCode(QString registerName) const
{
    foreach (Register *reg, registers)
    {
        if (reg->getName().compare(registerName, Qt::CaseInsensitive) == 0)
        {
            return reg->getBitCode();
        }
    }

    return Register::NO_BIT_CODE; // Register not found
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
    if (registerName == "") // Undefined register
        return 0;

    foreach (Register *reg, registers)
    {
        if (reg->getName().compare(registerName, Qt::CaseInsensitive) == 0)
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
    if (registerName == "") // Undefined register
        return;

    foreach (Register *reg, registers)
    {
        if (reg->getName().compare(registerName, Qt::CaseInsensitive) == 0)
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
    return addressCorrespondingLine.value(PC->getValue(), -1);
}

int Machine::getAddressCorrespondingLine(int address)
{
    return (buildSuccessful) ? addressCorrespondingLine.value(address, -1) : -1;
}

int Machine::getLineCorrespondingAddress(int line)
{
    return (buildSuccessful) ? lineCorrespondingAddress.value(line, -1) : -1;
}

QString Machine::getAddressCorrespondingLabel(int address)
{
    return (buildSuccessful) ? addressCorrespondingLabel.value(address) : "";
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

    return nullptr;
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

AddressingMode::AddressingModeCode Machine::getDefaultAddressingModeCode()
{
    foreach (AddressingMode *addressingMode, addressingModes)
    {
        if (addressingMode->getAssemblyPattern() == AddressingMode::NO_PATTERN)
            return addressingMode->getAddressingModeCode();
    }

    throw QString("Error defining default addressing mode.");
}

int Machine::getAddressingModeBitCode(AddressingMode::AddressingModeCode addressingModeCode)
{
    foreach (AddressingMode *addressingMode, addressingModes)
    {
        if (addressingMode->getAddressingModeCode() == addressingModeCode)
            return Byte(addressingMode->getBitPattern()).getValue();
    }

    throw QString("Invalid addressing mode code.");
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

void Machine::clear()
{
    clearMemory();
    clearRegisters();
    clearFlags();
    clearCounters();
    clearAssemblerData();

    setPCValue(0);
    setBreakpoint(-1);
    setRunning(false);
}

void Machine::generateDescriptions()
{
    // Neander
    descriptions["nop"]   = "Nenhuma operação.";
    descriptions["sta a"] = "Armazena o valor do acumulador no endereço 'a'.";
    descriptions["lda a"] = "Carrega o valor no endereço 'a' para o acumulador.";
    descriptions["add a"] = "Adiciona o valor no endereço 'a' ao acumulador.";
    descriptions["or a"]  = "Realiza um 'ou' lógico entre o valor no endereço 'a' e o valor do acumulador.";
    descriptions["and a"] = "Realiza um 'e' lógico entre o valor no endereço 'a' e o valor do acumulador.";
    descriptions["not"]   = "Inverte (complementa) o valor dos bits do acumulador.";
    descriptions["jmp a"] = "Desvia a execução para o endereço 'a' (desvio incondicional).";
    descriptions["jn a"]  = "Se a flag N estiver ativada (acumulador negativo), desvia a execução para o endereço 'a'.";
    descriptions["jz a"]  = "Se a flag Z estiver ativada (acumulador zerado), desvia a execução para o endereço 'a'.";
    descriptions["hlt"]   = "Termina a execução.";

    // Ahmes
    descriptions["sub a"] = "Subtrai o valor no endereço 'a' do acumulador.";
    descriptions["jp a"]  = "Se as flags N e Z estiverem desativadas (acumulador positivo), desvia a execução para o endereço 'a'.";
    descriptions["jv a"]  = "Se a flag V estiver ativada (overflow), desvia a execução para o endereço 'a'.";
    descriptions["jnv a"] = "Se a flag V estiver desativada (not overflow), desvia a execução para o endereço 'a'.";
    descriptions["jnz a"] = "Se a flag Z estiver desativada (acumulador diferente de zero), desvia a execução para o endereço 'a'.";
    descriptions["jc a"]  = "Se a flag C estiver ativada (carry), desvia a execução para o endereço 'a'.";
    descriptions["jnc a"] = "Se a flag C estiver desativada (not carry), desvia a execução para o endereço 'a'.";
    descriptions["jb a"]  = "Se a flag B estiver ativada (borrow), desvia a execução para o endereço 'a'.";
    descriptions["jnb a"] = "Se a flag B estiver desativada (not borrow), desvia a execução para o endereço 'a'.";
    descriptions["shr"]   = "Reliza shift lógico dos bits do acumulador para a direita, passando o estado do bit menos significativo para a flag C (carry) e preenchendo o bit mais significativo com 0.";
    descriptions["shl"]   = "Reliza shift lógico dos bits do acumulador para a esquerda, passando o estado do bit mais significativo para a flag C (carry) e preenchendo o bit menos significativo com 0.";
    descriptions["ror"]   = "Realiza rotação para a esquerda dos bits do acumulador, incluindo a flag C (carry) como um bit.";
    descriptions["rol"]   = "Realiza rotação para a direita dos bits do acumulador, incluindo a flag C (carry) como um bit.";

    // Ramses
    descriptions["str r a"] = "Armazena o valor do registrador 'r' no endereço 'a'.";
    descriptions["ldr r a"] = "Carrega o valor no endereço 'a' para o registrador 'r'.";
    descriptions["add r a"] = "Adiciona o valor no endereço 'a' ao registrador 'r'.";
    descriptions["or r a"]  = "Realiza um 'ou' lógico entre o valor no endereço 'a' e o valor do registrador 'r'.";
    descriptions["and r a"] = "Realiza um 'e' lógico entre o valor no endereço 'a' e o valor do registrador 'r'.";
    descriptions["not r"]   = "Inverte (complementa) o valor dos bits do registrador 'r'.";
    descriptions["sub r a"] = "Subtrai o valor no endereço 'a' do registrador 'r'.";
    descriptions["jsr a"]   = "Desvia para subrotina, armazenando o valor atual de PC em 'a' e desviando a execução para o endereço 'a' + 1.";
    descriptions["neg r"]   = "Troca o sinal do valor em complemento de 2 do registrador 'r' de positivo para negativo e vice-versa.";
    descriptions["shr r"]   = "Reliza shift lógico dos bits do registrador 'r' para a direita, passando o estado do bit menos significativo para a flag C (carry) e preenchendo o bit mais significativo com 0.";
}

QString Machine::getDescription(QString assemblyFormat)
{
    // Initialize descriptions
    if (descriptions.isEmpty())
        generateDescriptions();

    return descriptions.value(assemblyFormat, "");
}
