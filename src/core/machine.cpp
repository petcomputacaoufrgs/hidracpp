/********************************************************************************
 *
 * Copyright (C) 2014-2021 PET Computação UFRGS
 *
 * Este arquivo é parte do programa Hidra.
 *
 * Hidra é um software livre; você pode redistribuí-lo e/ou modificá-lo
 * dentro dos termos da Licença Pública Geral GNU como publicada pela
 * Fundação do Software Livre (FSF); na versão 3 da Licença, ou
 * (de opção sua) qualquer versão posterior.
 *
 *******************************************************************************/

#include "machine.h"

#define DEBUG_INT(value) qDebug(QString::number(value).toStdString().c_str());
#define DEBUG_STRING(value) qDebug(value.toStdString().c_str());

Machine::Machine(QObject *parent) :
    QObject(parent)
{
    PC = nullptr;
    littleEndian = false;
 
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

    fetchInstruction(); // Fetches values from memory
    decodeInstruction (); // Fetches addressing mode, register, immediate address and any other relevant data
    executeInstruction(); // Uses the values above to execute an instruction

    if (getPCValue() == getBreakpoint())
        setRunning(false);
}

void Machine::fetchInstruction()
{
    // Read first byte
    fetchedValue = memoryReadNext();
    currentInstruction = getInstructionFromValue(fetchedValue);
}

void Machine::decodeInstruction()
{
    decodedAdressingModeCode1 = extractAddressingModeCode(fetchedValue);
    decodedRegisterName1 = extractRegisterName(fetchedValue);

    if (currentInstruction && currentInstruction->getNumBytes() > 1)
    {
        decodedImmediateAddress = getPCValue(); // Address that contains first argument byte
        incrementPCValue(currentInstruction->getNumBytes() - 1); // Skip argument bytes
    }
}

void Machine::executeInstruction()
{
    int value1, value2, result;
    int immediateAddress = decodedImmediateAddress;
    Instruction::InstructionCode instructionCode;
    instructionCode = (currentInstruction) ? currentInstruction->getInstructionCode() : Instruction::NOP;
    bool isImmediate = (decodedAdressingModeCode1 == AddressingMode::IMMEDIATE); // Used to invalidate immediate jumps

    QString registerName = decodedRegisterName1;
    AddressingMode::AddressingModeCode addressingModeCode = decodedAdressingModeCode1;

    switch (instructionCode)
    {

    //////////////////////////////////////////////////
    // Load/Store
    //////////////////////////////////////////////////

    case Instruction::LDR:
        result = GetCurrentOperandValue();
        setRegisterValue(registerName, result);
        updateFlags(result);
        break;

    case Instruction::STR:
        result = getRegisterValue(registerName);
        memoryWrite(GetCurrentOperandAddress(), result);
        break;



    //////////////////////////////////////////////////
    // Arithmetic and logic
    //////////////////////////////////////////////////

    case Instruction::ADD:
        value1 = getRegisterValue(registerName);
        value2 = GetCurrentOperandValue();
        result = (value1 + value2) & 0xFF;

        setRegisterValue(registerName, result);
        setCarry(value1 + value2 > 0xFF);
        setOverflow(toSigned(value1) + toSigned(value2) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction::OR:
        value1 = getRegisterValue(registerName);
        value2 = GetCurrentOperandValue();
        result = (value1 | value2);

        setRegisterValue(registerName, result);
        updateFlags(result);
        break;

    case Instruction::AND:
        value1 = getRegisterValue(registerName);
        value2 = GetCurrentOperandValue();
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
        value2 = GetCurrentOperandValue();
        result = (value1 - value2) & 0xFF;

        setRegisterValue(registerName, result);
        setBorrowOrCarry(value1 < value2);
        setOverflow(toSigned(value1) - toSigned(value2) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction::NEG:
        value1 = getRegisterValue(registerName);
        result = (-value1) & 0xFF;

        setRegisterValue(registerName, result);
        updateFlags(result);
        break;

    case Instruction::SHR:
        value1 = getRegisterValue(registerName);
        result = (value1 >> 1) & 0xFF; // Logical shift (unsigned)

        setRegisterValue(registerName, result);
        setCarry(value1 & 0x01);
        updateFlags(result);
        break;

    case Instruction::SHL:
        value1 = getRegisterValue(registerName);
        result = (value1 << 1) & 0xFF;

        setRegisterValue(registerName, result);
        setCarry((value1 & 0x80) ? 1 : 0);
        updateFlags(result);
        break;

    case Instruction::ROR:
        value1 = getRegisterValue(registerName);
        result = ((value1 >> 1) | (getFlagValue("C") == true ? 0x80 : 0x00)) & 0xFF;

        setRegisterValue(registerName, result);
        setCarry(value1 & 0x01);
        updateFlags(result);
        break;

    case Instruction::ROL:
        value1 = getRegisterValue(registerName);
        result = ((value1 << 1) | (getFlagValue("C") == true ? 0x01 : 0x00)) & 0xFF;

        setRegisterValue(registerName, result);
        setCarry((value1 & 0x80) ? 1 : 0);
        updateFlags(result);
        break;

    case Instruction::INC:
        setRegisterValue(registerName, getRegisterValue(registerName) + 1);
        break;

    case Instruction::DEC:
        setRegisterValue(registerName, getRegisterValue(registerName) - 1);
        break;



    //////////////////////////////////////////////////
    // Jumps
    //////////////////////////////////////////////////

    case Instruction::JMP:
        if (!isImmediate) // Invalidate immediate jumps
            setPCValue(GetCurrentJumpAddress());
        break;

    case Instruction::JN:
        if (getFlagValue("N") == true && !isImmediate)
            setPCValue(GetCurrentJumpAddress());
        break;

    case Instruction::JP:
        if (getFlagValue("N") == false && !isImmediate)
            setPCValue(GetCurrentJumpAddress());
        break;

    case Instruction::JV:
        if (getFlagValue("V") == true && !isImmediate)
            setPCValue(GetCurrentJumpAddress());
        break;

    case Instruction::JNV:
        if (getFlagValue("V") == false && !isImmediate)
            setPCValue(GetCurrentJumpAddress());
        break;

    case Instruction::JZ:
        if (getFlagValue("Z") == true && !isImmediate)
            setPCValue(GetCurrentJumpAddress());
        break;

    case Instruction::JNZ:
        if (getFlagValue("Z") == false && !isImmediate)
            setPCValue(GetCurrentJumpAddress());
        break;

    case Instruction::JC:
        if (getFlagValue("C") == true && !isImmediate)
            setPCValue(GetCurrentJumpAddress());
        break;

    case Instruction::JNC:
        if (getFlagValue("C") == false && !isImmediate)
            setPCValue(GetCurrentJumpAddress());
        break;

    case Instruction::JB:
        if (getFlagValue("B") == true && !isImmediate)
            setPCValue(GetCurrentJumpAddress());
        break;

    case Instruction::JNB:
        if (getFlagValue("B") == false && !isImmediate)
            setPCValue(GetCurrentJumpAddress());
        break;

    case Instruction::JSR:
        if (!isImmediate)
        {
            int jumpAddress = GetCurrentJumpAddress();
            memoryWrite(jumpAddress, getPCValue());
            setPCValue(jumpAddress+1);
        }
        break;

    case Instruction::REG_IF:
        if (getRegisterValue(registerName) == 0)
            setPCValue(getMemoryValue(immediateAddress));
        else
            setPCValue(getMemoryValue(immediateAddress + 1));
        break;



    //////////////////////////////////////////////////
    // Halt
    //////////////////////////////////////////////////

    case Instruction::HLT:
        setRunning(false);
        break;

    default: // NOP etc.
        break;
    }

    instructionCount++;
}

AddressingMode::AddressingModeCode Machine::extractAddressingModeCode(int fetchedValue)
{
    foreach (AddressingMode *addressingMode, addressingModes)
    {
        QRegExp matchAddressingMode(addressingMode->getBitPattern());

        if (matchAddressingMode.exactMatch(Conversion::valueToString(fetchedValue)))
            return addressingMode->getAddressingModeCode();
    }

    throw QString("Addressing mode not found.");
}

QString Machine::extractRegisterName(int fetchedValue)
{
    foreach (Register *reg, registers)
    {
        if (reg->matchByte(fetchedValue))
            return reg->getName();
    }

    return ""; // Undefined register
}

void Machine::setOverflow(bool state)
{
    foreach (Flag *flag, flags)
    {
        if (flag->getFlagCode() == Flag::OVERFLOW_FLAG)
            flag->setValue(state);
    }
}

void Machine::setCarry(bool state)
{
    setFlagValue(Flag::CARRY, state);
}

void Machine::setBorrowOrCarry(bool borrowState)
{
    if (hasFlag(Flag::BORROW))
        setFlagValue(Flag::BORROW, borrowState);
    else
        setFlagValue(Flag::CARRY, !(borrowState));
}

void Machine::updateFlags(int value)
{
    setFlagValue("N", toSigned(value) < 0);
    setFlagValue("Z", value == 0);
}

int Machine::address(int value)
{
    return (value & (memory.size() - 1)); // Bit-and, removes excess bits
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
    running = false;
    buildSuccessful = false;
    firstErrorLine = -1;



    //////////////////////////////////////////////////
    // Regular expressions
    //////////////////////////////////////////////////

    // Regular expression to capture comments
    static QString CHARS_EXCEPT_QUOTE_SEMICOLON = "[^';]*";
    static QString STRING = "(?:'[^']*')";
    static QString COMMENT = "(;.*)$";

    static QString commentsPattern = "^(?:" + CHARS_EXCEPT_QUOTE_SEMICOLON + STRING + "?)*" + COMMENT;
    static QRegExp matchComments(commentsPattern);

    static QRegExp validLabel("[a-z_][a-z0-9_]*"); // Validates label names (must start with a letter/underline, may have numbers)
    static QRegExp whitespace("\\s+");



    //////////////////////////////////////////////////
    // Simplify source code
    //////////////////////////////////////////////////

    QStringList sourceLines = sourceCode.split("\n"); // Split source code to individual lines

    // Strip comments and extra spaces
    for (int lineNumber = 0; lineNumber < sourceLines.size(); lineNumber++)
    {
        // Convert literal quotes to special symbol
        sourceLines[lineNumber].replace("''''", "'" + QUOTE_SYMBOL); // '''' -> 'QUOTE_SYMBOL
        sourceLines[lineNumber].replace("'''", QUOTE_SYMBOL); // ''' -> QUOTE_SYMBOL

        // Remove comments
        if (matchComments.exactMatch(sourceLines[lineNumber]))
            sourceLines[lineNumber].replace(matchComments.cap(1), "");

        // Trim whitespace
        sourceLines[lineNumber] = sourceLines[lineNumber].trimmed();
    }



    //////////////////////////////////////////////////
    // FIRST PASS: Read labels, reserve memory
    //////////////////////////////////////////////////

    clearAssemblerData();
    PC->setValue(0);

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
                    throw duplicateLabel;

                labelPCMap.insert(labelName.toLower(), PC->getValue()); // Add to map
                addressCorrespondingLabel[PC->getValue()] = labelName;
                sourceLines[lineNumber] = sourceLines[lineNumber].replace(labelName + ":", "").trimmed(); // Remove label from sourceLines
            }

            //////////////////////////////////////////////////
            // Reserve memory for instructions/directives
            //////////////////////////////////////////////////

            if (!sourceLines[lineNumber].isEmpty())
            {
                QString mnemonic = sourceLines[lineNumber].section(whitespace, 0, 0).toLower();

                const Instruction *instruction = getInstructionFromMnemonic(mnemonic);
                if (instruction != NULL)
                {
                    int numBytes = instruction->getNumBytes();

                    if (numBytes == 0) // If instruction has variable number of bytes
                    {
                        QString addressArgument = sourceLines[lineNumber].section(whitespace, -1); // Last argument
                        numBytes = calculateBytesToReserve(addressArgument);
                    }

                    reserveAssemblerMemory(numBytes, lineNumber);
                }
                else // Directive
                {
                    QString arguments = sourceLines[lineNumber].section(whitespace, 1); // Everything after mnemonic
                    obeyDirective(mnemonic, arguments, true, lineNumber);
                }
            }
        }
        catch (ErrorCode errorCode)
        {
            if (firstErrorLine == -1)
                firstErrorLine = lineNumber;
            emitError(lineNumber, errorCode);
        }
    }

    if (firstErrorLine >= 0)
        return; // Error(s) found, abort compilation



    //////////////////////////////////////////////////
    // SECOND PASS: Build instructions/defines
    //////////////////////////////////////////////////

    sourceLineCorrespondingAddress.fill(-1, sourceLines.size());
    PC->setValue(0);

    for (int lineNumber = 0; lineNumber < sourceLines.size(); lineNumber++)
    {
        try
        {
            sourceLineCorrespondingAddress[lineNumber] = PC->getValue();

            if (!sourceLines[lineNumber].isEmpty())
            {
                QString mnemonic  = sourceLines[lineNumber].section(whitespace, 0, 0).toLower();
                QString arguments = sourceLines[lineNumber].section(whitespace, 1); // Everything after mnemonic

                const Instruction *instruction = getInstructionFromMnemonic(mnemonic);
                if (instruction != NULL)
                {
                    buildInstruction(mnemonic, arguments);
                }
                else // Directive
                {
                    obeyDirective(mnemonic, arguments, false, lineNumber);
                }
            }
        }
        catch (ErrorCode errorCode)
        {
            if (firstErrorLine == -1)
                firstErrorLine = lineNumber;
            emitError(lineNumber, errorCode);
        }
    }

    if (firstErrorLine >= 0)
        return; // Error(s) found, abort compilation



    buildSuccessful = true;

    copyAssemblerMemoryToMemory();
    clearAfterBuild();
}

// Mnemonic must be lowercase
void Machine::obeyDirective(QString mnemonic, QString arguments, bool reserveOnly, int sourceLine)
{
    static QRegExp whitespace("\\s+");

    if (mnemonic == "org")
    {
        QStringList argumentList = arguments.split(whitespace, QString::SkipEmptyParts);
        int numberOfArguments = argumentList.size();

        if (numberOfArguments != 1)
            throw wrongNumberOfArguments;
        if (!isValidOrg(argumentList.first()))
            throw invalidAddress;

        PC->setValue(stringToInt(argumentList.first()));
    }
    else if (QRegExp("db|dw|dab|daw").exactMatch(mnemonic))
    {
        QStringList argumentList;
        argumentList = splitDirectiveArguments(arguments);

        int numberOfArguments = argumentList.size();
        int bytesPerArgument = (mnemonic == "db" || mnemonic == "dab") ? 1 : 2;
        bool isArray = (mnemonic == "dab" || mnemonic == "daw") ? true : false;

        if (bytesPerArgument == 1 && numberOfArguments == 0)
        {
            argumentList.append("0"); // Default to argument 0 in case of DB and DW
            numberOfArguments = 1;
        }

        if (!isArray && numberOfArguments > 1) // Too many arguments
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
                reserveAssemblerMemory(argumentList.first().mid(1).toInt() * bytesPerArgument, sourceLine);
            }
            else // Skip bytes
            {
                incrementPCValue(argumentList.first().mid(1).toInt()*bytesPerArgument);
            }
        }
        else if (reserveOnly)
        {
            reserveAssemblerMemory(numberOfArguments * bytesPerArgument, sourceLine); // Increments PC
        }
        else
        {
            // Process each argument
            for (QString argument : argumentList)
            {
                // TODO: Should DAB/DAW disallow labels as in Daedalus?
                int value = argumentToValue(argument, true, bytesPerArgument);

                // Write value
                if (bytesPerArgument == 2 && littleEndian)
                {
                    setAssemblerMemoryNext( value       & 0xFF); // Least significant byte first
                    setAssemblerMemoryNext((value >> 8) & 0xFF);
                }
                else if (bytesPerArgument == 2) // Big endian
                {
                    setAssemblerMemoryNext((value >> 8) & 0xFF); // Most significant byte first
                    setAssemblerMemoryNext( value       & 0xFF);
                }
                else
                {
                    setAssemblerMemoryNext(value & 0xFF);
                }
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
    QStringList argumentList = splitInstructionArguments(arguments, *instruction);

    AddressingMode::AddressingModeCode addressingModeCode = AddressingMode::DIRECT; // Default mode
    QStringList instructionArguments = instruction->getArguments();
    bool isImmediate = false;

    int registerBitCode = 0b00000000;
    int addressingModeBitCode = 0b00000000;

    // If argumentList contains a register:
    if (instructionArguments.contains("r"))
    {
        registerBitCode = getRegisterBitCode(argumentList.first());

        if (registerBitCode == Register::NO_BIT_CODE)
            throw invalidArgument; // Register not found (or invisible)
    }

    // If argumentList contains an address/value:
    if (instructionArguments.contains("a"))
    {
        extractArgumentAddressingModeCode(argumentList.last(), addressingModeCode); // Removes addressing mode from argument
        addressingModeBitCode = getAddressingModeBitCode(addressingModeCode);
        isImmediate = (addressingModeCode == AddressingMode::IMMEDIATE);
    }

    // Write first byte (instruction with register and addressing mode):
    setAssemblerMemoryNext(instruction->getByteValue() | registerBitCode | addressingModeBitCode);

    // Write second byte (if 1-byte address/immediate value):
    if (instruction->getNumBytes() == 2 || isImmediate)
    {
        setAssemblerMemoryNext(argumentToValue(argumentList.last(), isImmediate)); // Converts labels, chars, etc.
    }
    // Write second and third bytes (if 2-byte addresses):
    else if (instructionArguments.contains("a"))
    {
        int address = argumentToValue(argumentList.last(), isImmediate);

        setAssemblerMemoryNext( address       & 0xFF); // Least significant byte (little-endian)
        setAssemblerMemoryNext((address >> 8) & 0xFF); // Most significant byte
    }
    // If instruction has two addresses (REG_IF), write both addresses:
    else if (instructionArguments.contains("a0") && instructionArguments.contains("a1"))
    {
        setAssemblerMemoryNext(argumentToValue(argumentList.at(instructionArguments.indexOf("a0")), false));
        setAssemblerMemoryNext(argumentToValue(argumentList.at(instructionArguments.indexOf("a1")), false));
    }
}

QStringList Machine::splitInstructionArguments(QString const& arguments, Instruction const& instruction)
{
    // Regex to split.
    static QRegExp separator("\\s*(\\s|,)\\s*");
    // Regex to detect if two arguments in final position are actually two arguments or an address.
    static QRegExp whitespace("\\s");

    // To be returned.
    QStringList argumentList;
    // Specification of the instruction's arguments.
    QStringList instructionArgs = instruction.getArguments();
    // To keep track of the last separator, so we can combine two final arguments in one address,
    // if the instruction can take addressing mode, and we exceed the number of arguments.
    QString lastSeparator;
    // To keep track if we actually have a last separator.
    bool hasLastSep = false;
    // To keep track on where we are in the arguments' string.
    int index = 0;

    // The split loop.
    while (index < arguments.size() && argumentList.length() <= instruction.getNumberOfArguments())
    {
        QString argument;
        // Find where there is a separator.
        int newIndex = arguments.indexOf(separator, index);
        if (newIndex >= 0)
        {
            // Then there is a separator.
            hasLastSep = true;
            // Save this separator.
            lastSeparator = arguments.mid(newIndex, separator.matchedLength());
            // Take the argument part.
            argument = arguments.mid(index, newIndex - index);;
            // Update index beyond the separator.s
            index = newIndex + separator.matchedLength();
        }
        else
        {
            // The last argument.
            argument = arguments.right(arguments.size() - index);
            index = arguments.size();
        }
        argumentList.push_back(argument);
    }

    // Detecting if found < expected or found > expected + 1.
    if (index < arguments.size() || argumentList.length() < instruction.getNumberOfArguments())
        throw wrongNumberOfArguments;

    if (argumentList.length() == instruction.getNumberOfArguments() + 1)
    {
        // Found == expected + 1 only allowed if addressing mode is allowed, and we have a last
        // separator. Also, it must not contain whitespace.
        if (!instructionArgs.contains("a") || !hasLastSep || lastSeparator.contains(whitespace))
            throw wrongNumberOfArguments;
        // Joining two last arguments with separator, probably an addressing mode.
        QString last = argumentList.takeLast();
        argumentList.back() += lastSeparator;
        argumentList.back() += last;
    }

    return argumentList;
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
    errorMessages[duplicateLabel] = "Label já definido.";
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
        addressCorrespondingSourceLine[i] = -1;
        addressCorrespondingLabel[i] = "";
    }

    sourceLineCorrespondingAddress.clear();
    labelPCMap.clear();
}

void Machine::setAssemblerMemoryNext(int value)
{
    assemblerMemory[PC->getValue()]->setValue(value);
    incrementPCValue();
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

// Reserve 'sizeToReserve' bytes starting from PC, associate addresses with a source line. Throws exception on overlap.
void Machine::reserveAssemblerMemory(int sizeToReserve, int associatedSourceLine)
{
    while (sizeToReserve > 0)
    {
        if (!reserved[PC->getValue()])
        {
            reserved[PC->getValue()] = true;
            addressCorrespondingSourceLine[PC->getValue()] = associatedSourceLine;
            incrementPCValue();
            sizeToReserve--;
        }
        else
            throw memoryOverlap; // Memory already reserved
    }
}

// Method for machines that require the addressing mode to reserve memory.
int Machine::calculateBytesToReserve(QString)
{
    return 0;
}

bool Machine::isValidValue(QString valueString, int min, int max)
{
    bool ok;
    int value;

    if (valueString.left(1).toLower() == "h")
        value = valueString.mid(1).toInt(&ok, 16); // Remove "h"
    else
        value = valueString.toInt(&ok, 10);

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

bool Machine::isValidByteValue(QString valueString)
{
    return isValidValue(valueString, -128, 255);
}

bool Machine::isValidAddress(QString addressString) // Allows negative values for offsets
{
    return isValidValue(addressString, -memory.size(), memory.size()-1);
}

bool Machine::isValidOrg(QString offsetString)
{
    return isValidValue(offsetString, 0, memory.size()-1);
}

QStringList Machine::splitDirectiveArguments(QString arguments)
{
    QStringList finalArgumentList;

    // Regular expressions
    static QRegExp matchBrackets("\\[(\\d+)\\]"); // Digits between brackets

    static QString VALUE = "([^'\\s,]+)";
    static QString STRING = "('[^']+')";
    static QString SEPARATOR = "([,\\s]*|$)";

    static QString matchArgumentString = "(" + VALUE + "|" + STRING + ")" + SEPARATOR;
    static QRegExp matchArgument(matchArgumentString);

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
    // Process string
    //////////////////////////////////////////////////

    int index = arguments.indexOf(matchArgument); // Match first argument
    int totalMatchedLength = 0;

    while (index >= 0) // While there are arguments
    {
        QString argument = matchArgument.cap(1);

        // Ascii string
        if (argument.contains("'"))
        {
            argument.replace("'", "");
            foreach (QChar c, argument)
            {
                finalArgumentList.append('\'' + c + '\''); // Char between single quotes
            }
        }
        else // Value
        {
            finalArgumentList.append(argument);
        }

        totalMatchedLength += matchArgument.matchedLength();

        index = arguments.indexOf(matchArgument, index + matchArgument.matchedLength());
    }

    if (totalMatchedLength != arguments.length()) // If not fully matched, an error occurred
        throw invalidString;

    return finalArgumentList;
}

void Machine::extractArgumentAddressingModeCode(QString &argument, AddressingMode::AddressingModeCode &addressingModeCode)
{
    addressingModeCode = getDefaultAddressingModeCode();

    foreach (AddressingMode *addressingMode, addressingModes)
    {
        QRegExp matchAddressingMode = addressingMode->getAssemblyRegExp();
        matchAddressingMode.setCaseSensitivity(Qt::CaseInsensitive);

        if (matchAddressingMode.exactMatch(argument))
        {
            argument = matchAddressingMode.cap(1); // Remove addressing mode
            addressingModeCode = addressingMode->getAddressingModeCode();
            return;
        }
    }
}

int Machine::argumentToValue(QString argument, bool isImmediate, int immediateNumBytes)
{
    static QRegExp matchChar("'.'");
    static QRegExp labelOffset("(.+)(\\+|\\-)(.+)"); // (label) (+|-) (offset)

    // Convert label with +/- offset to number
    if (labelOffset.exactMatch(argument.toLower()))
    {
        int sign = (labelOffset.cap(2) == "+") ? +1 : -1;

        if (!labelPCMap.contains(labelOffset.cap(1))) // Validate label
            throw invalidLabel;
        if (!isValidAddress(labelOffset.cap(3))) // Validate offset
            throw invalidArgument;

        // Argument = Label + Offset
        argument = QString::number(labelPCMap.value(labelOffset.cap(1)) + sign*stringToInt(labelOffset.cap(3)));
    }

    // Convert label to number string
    if (labelPCMap.contains(argument.toLower()))
        argument = QString::number(labelPCMap.value(argument.toLower()));

    if (isImmediate)
    {
        if (argument.contains(QUOTE_SYMBOL)) // Immediate quote
            return (int)'\'';
        else if (matchChar.exactMatch(argument)) // Immediate char
            return (int)argument.at(1).toLatin1();
        else if (isValidNBytesValue(argument, immediateNumBytes)) // Immediate hex/dec value
            return stringToInt(argument);
        else
            throw invalidValue;
    }
    else
    {
        if (isValidAddress(argument)) // Address
            return stringToInt(argument);
        else
            throw invalidAddress;
    }
}

int Machine::stringToInt(QString valueString)
{
    if (valueString.left(1).toLower() == "h") // Remove H
        return valueString.mid(1).toInt(NULL, 16);
    else
        return valueString.toInt(NULL, 10);
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

int Machine::GetCurrentOperandAddress()
{

    int immediateAddress = decodedImmediateAddress; 
    AddressingMode::AddressingModeCode addressingModeCode = decodedAdressingModeCode1;

    switch (addressingModeCode)
    {
        case AddressingMode::DIRECT:
            return memoryRead(immediateAddress);

        case AddressingMode::INDIRECT:
            return memoryRead(memoryRead(immediateAddress));

        case AddressingMode::IMMEDIATE:
            return immediateAddress;

        case AddressingMode::INDEXED_BY_X:
            return address(memoryRead(immediateAddress) + getRegisterValue("X"));

        case AddressingMode::INDEXED_BY_PC:
            return address(memoryRead(immediateAddress) + getRegisterValue("PC"));

        default:
            return 0;
    }
}

int Machine::GetCurrentOperandValue()
{
    return memoryRead(GetCurrentOperandAddress()); // Return 1-byte value
}

int Machine::GetCurrentJumpAddress()
{
    return GetCurrentOperandAddress();
}



//////////////////////////////////////////////////
// Import/Export memory
//////////////////////////////////////////////////

// Returns true if successful
FileErrorCode::FileErrorCode Machine::importMemory(QString filename, int start, int end, int dest)
{
    if(start < 0 || end > memory.size()){
        return FileErrorCode::invalidAddress;
    }

    char byte;
    QFile memFile(filename); // Implicitly closed

    // Machines that fetch bytes should skip every second byte
    int byte_step_size = fetchByteSize > 1 ? 1 : 2;

    // Open file
    memFile.open(QFile::ReadOnly);

    // The file must contain the identifier's length, the machine's identifier and twice the amount of memory
    if (memFile.size() != 1 + identifier.length() + memory.size() * byte_step_size)
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
    memFile.seek(1 + identifier.length() + (byte_step_size * start)); //Set loaded file read starting point
    int read_size = qMin<int>(end - start, getMemorySize() - dest); //Put the maximum amount of bytes read possible

    for (int address = dest; address < (dest + read_size); address++)
    {
        memFile.getChar(&byte); 
        setMemoryValue(address, byte);
        
        if(byte_step_size > 1){ //Skip byte
            memFile.getChar(&byte);
        }
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
// Instruction strings
//////////////////////////////////////////////////

void Machine::updateInstructionStrings()
{
    int address = 0, pendingArgumentBytes = 0;
    bool pcReached = false;

    while (address < getMemorySize())
    {
        // Realign interpretation to PC when PC is reached
        if (!pcReached && address >= getPCValue())
        {
            address = getPCValue();
            pendingArgumentBytes = 0;
            pcReached = true;
        }

        if (pendingArgumentBytes == 0) // Used to skip argument lines
        {
            setInstructionString(address, generateInstructionString(address, pendingArgumentBytes));
        }
        else // Skip instruction's arguments (leave empty strings)
        {
            setInstructionString(address, "");
            pendingArgumentBytes -= 1;
        }

        address += 1;
    }
}

QString Machine::generateInstructionString(int address, int &argumentsSize)
{
    QString memoryString;
    argumentsSize = 0;

    // Fetch and decode instruction
    int fetchedValue = getMemoryValue(address);
    Instruction *instruction = getInstructionFromValue(getMemoryValue(address));
    AddressingMode::AddressingModeCode addressingModeCode = extractAddressingModeCode(fetchedValue);
    QString registerName = extractRegisterName(fetchedValue);

    if (instruction == nullptr || instruction->getInstructionCode() == Instruction::NOP || instruction->getInstructionCode() == Instruction::VOLTA_NOP)
        return "";

    // Instruction name
    memoryString = instruction->getMnemonic().toUpper();

    // Register name
    if (instruction->getArguments().contains("r"))
        memoryString += " " + ((registerName != "") ? registerName : "?");

    // Argument value (with addressing mode)
    // Size can be 0 (variable number of bytes)
    if (instruction->getNumBytes() != 1)
    {
        QString argumentString = generateArgumentsString(address, instruction, addressingModeCode, argumentsSize);
        if (argumentString.length() > 0) {
            memoryString += " " + argumentString;
        }
    }

    return memoryString;
}

QString Machine::generateArgumentsString(int address, Instruction *instruction, AddressingMode::AddressingModeCode addressingModeCode, int &argumentsSize)
{
    QString argument;
    QString addressingModePattern = getAddressingModePattern(addressingModeCode);

    if (addressingModePattern != AddressingMode::NO_PATTERN)
        argument = addressingModePattern.replace("(.*)", "").toUpper(); // Surround argument string with the corresponding addressing mode syntax

    argumentsSize = instruction->getNumBytes() - 1;
    return argument;
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

int Machine::getFirstErrorLine()
{
    return firstErrorLine;
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

// Used to highlight the next operand
void Machine::getNextOperandAddress(int &intermediateAddress, int &intermediateAddress2, int &finalOperandAddress)
{
    int fetchedValue = getMemoryValue(PC->getValue());
    Instruction *instruction = getInstructionFromValue(fetchedValue);
    AddressingMode::AddressingModeCode addressingModeCode = extractAddressingModeCode(fetchedValue);
    int immediateAddress;

    intermediateAddress  = -1;
    intermediateAddress2 = -1;
    finalOperandAddress  = -1;

    if (!instruction || instruction->getNumBytes() != 2)
        return;

    immediateAddress = address(PC->getValue() + 1);

    switch (addressingModeCode)
    {
        case AddressingMode::DIRECT:
            finalOperandAddress = getMemoryValue(immediateAddress);
            break;

        case AddressingMode::INDIRECT:
            intermediateAddress = getMemoryValue(immediateAddress);
            finalOperandAddress = getMemoryValue(intermediateAddress);
            break;

        case AddressingMode::IMMEDIATE: // Immediate addressing mode
            finalOperandAddress = immediateAddress;
            break;

        case AddressingMode::INDEXED_BY_X: // Indexed addressing mode
            finalOperandAddress = address(getMemoryValue(immediateAddress) + getRegisterValue("X"));
            break;

        case AddressingMode::INDEXED_BY_PC:
            finalOperandAddress = address(getMemoryValue(immediateAddress) + getRegisterValue("PC"));
            break;
    }
}

int Machine::getMemorySize() const
{
    return memory.size();
}

static bool isPowerOfTwo(unsigned int value)
{
    while (((value % 2) == 0) && value > 1) // While value is even and greater than one
        value /= 2;

    return (value == 1);
}

void Machine::setMemorySize(int size)
{
    memory.fill(nullptr, size);
    assemblerMemory.fill(nullptr, size);
    instructionStrings.fill("", size);
    reserved.fill(false, size);
    changed.fill(true, size);
    addressCorrespondingSourceLine.fill(-1, size);
    addressCorrespondingLabel.fill("", size);

    for (int i=0; i<memory.size(); i++)
    {
        memory[i] = new Byte();
        assemblerMemory[i] = new Byte();
    }

    Q_ASSERT(isPowerOfTwo(size)); // Size must be a power of two for the mask to work
    memoryMask = (size - 1);
}

int Machine::getMemoryValue(int address) const
{
    return memory[address & memoryMask]->getValue();
}

void Machine::setMemoryValue(int address, int value)
{
    memory[address & memoryMask]->setValue(value);
    changed[address & memoryMask] = true;
}

bool Machine::hasByteChanged(int address)
{
    if (changed[address & memoryMask])
    {
        changed[address & memoryMask] = false;
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

QString Machine::getInstructionString(int address)
{
    return instructionStrings[address & memoryMask];
}

void Machine::setInstructionString(int address, QString value)
{
    instructionStrings[address & memoryMask] = value;
}

void Machine::clearInstructionStrings()
{
    instructionStrings.fill("", getMemorySize());
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

bool Machine::hasFlag(Flag::FlagCode flagCode) const
{
    foreach (Flag *flag, flags)
    {
        if (flag->getFlagCode() == flagCode)
            return true;
    }

    return false;
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
            return reg->getBitCode();
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

bool Machine::hasRegister(QString registerName) const
{
    foreach (Register *reg, registers)
    {
        if (reg->getName().compare(registerName, Qt::CaseInsensitive) == 0)
            return true;
    }

    return false;
}

int Machine::getRegisterValue(int id, bool signedData) const
{
    if (signedData && registers[id]->isData())
        return registers[id]->getSignedValue();
    else
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

bool Machine::isRegisterData(int id)
{
    return registers[id]->isData();
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

void Machine::incrementPCValue(int units)
{
    PC->setValue(PC->getValue() + units);
}

int Machine::getPCCorrespondingSourceLine()
{
    return addressCorrespondingSourceLine.value(PC->getValue(), -1);
}

int Machine::getAddressCorrespondingSourceLine(int address)
{
    return (buildSuccessful) ? addressCorrespondingSourceLine.value(address, -1) : -1;
}

int Machine::getSourceLineCorrespondingAddress(int line)
{
    return (buildSuccessful) ? sourceLineCorrespondingAddress.value(line, -1) : -1;
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

QVector<AddressingMode *> Machine::getAddressingModes() const
{
    return addressingModes;
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
            return Conversion::stringToValue(addressingMode->getBitPattern());
    }

    throw QString("Invalid addressing mode code.");
}

QString Machine::getAddressingModePattern(AddressingMode::AddressingModeCode addressingModeCode)
{
    foreach (AddressingMode *addressingMode, addressingModes)
    {
        if (addressingMode->getAddressingModeCode() == addressingModeCode)
            return addressingMode->getAssemblyPattern();
    }

    return QString("");
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
    clearInstructionStrings();
    clearAssemblerData();

    setBreakpoint(-1);
    setRunning(false);
}

void Machine::clearAfterBuild()
{
    clearRegisters();
    clearFlags();
    clearCounters();
    clearInstructionStrings();

    setRunning(false);
}

void Machine::generateDescriptions()
{
    // Neander
    descriptions["nop"]   = "Nenhuma operação.";
    descriptions["sta a"] = "Armazena o valor do acumulador no endereço 'a'.";
    descriptions["lda a"] = "Carrega o valor no endereço 'a' para o acumulador.";
    descriptions["add a"] = "Adiciona o valor no endereço 'a' ao acumulador.";
    descriptions["or a"]  = "Realiza um 'ou' lógico entre cada bit de 'a' e o bit correspondente no acumulador.";
    descriptions["and a"] = "Realiza um 'e' lógico entre cada bit de 'a' e o bit correspondente no acumulador.";
    descriptions["not"]   = "Inverte (complementa) o valor dos bits do acumulador.";
    descriptions["jmp a"] = "Desvia a execução para o endereço 'a' (desvio incondicional).";
    descriptions["jn a"]  = "Se a flag N estiver ativada (acumulador negativo), desvia a execução para o endereço 'a'.";
    descriptions["jz a"]  = "Se a flag Z estiver ativada (acumulador zerado), desvia a execução para o endereço 'a'.";
    descriptions["hlt"]   = "Termina a execução.";

    // Ahmes
    descriptions["sub a"] = "Subtrai o valor no endereço 'a' do acumulador.";
    descriptions["jp a"]  = "Se a flag N estiver desativada (acumulador positivo ou zero), desvia a execução para o endereço 'a'.";
    descriptions["jv a"]  = "Se a flag V estiver ativada (overflow), desvia a execução para o endereço 'a'.";
    descriptions["jnv a"] = "Se a flag V estiver desativada (not overflow), desvia a execução para o endereço 'a'.";
    descriptions["jnz a"] = "Se a flag Z estiver desativada (acumulador diferente de zero), desvia a execução para o endereço 'a'.";
    descriptions["jc a"]  = "Se a flag C estiver ativada (carry), desvia a execução para o endereço 'a'.";
    descriptions["jnc a"] = "Se a flag C estiver desativada (not carry), desvia a execução para o endereço 'a'.";

    if (hasFlag(Flag::BORROW))
        descriptions["jb a"]  = "Se a flag B estiver ativada (borrow), desvia a execução para o endereço 'a'.";
    else
        descriptions["jb a"]  = "Se a flag C estiver desativada (borrow), desvia a execução para o endereço 'a'.";

    descriptions["jnb a"] = "Se a flag B estiver desativada (not borrow), desvia a execução para o endereço 'a'.";
    descriptions["shr"]   = "Realiza shift lógico dos bits do acumulador para a direita, passando o estado do bit menos significativo para a flag C (carry) e preenchendo o bit mais significativo com 0.";
    descriptions["shl"]   = "Realiza shift lógico dos bits do acumulador para a esquerda, passando o estado do bit mais significativo para a flag C (carry) e preenchendo o bit menos significativo com 0.";
    descriptions["ror"]   = "Realiza rotação para a esquerda dos bits do acumulador, incluindo a flag C (carry) como um bit.";
    descriptions["rol"]   = "Realiza rotação para a direita dos bits do acumulador, incluindo a flag C (carry) como um bit.";

    // Ramses
    descriptions["str r a"] = "Armazena o valor do registrador 'r' no endereço 'a'.";
    descriptions["ldr r a"] = "Carrega o valor no endereço 'a' para o registrador 'r'.";
    descriptions["add r a"] = "Adiciona o valor no endereço 'a' ao registrador 'r'.";
    descriptions["or r a"]  = "Realiza um 'ou' lógico entre cada bit de 'a' e o bit correspondente no registrador 'r'.";
    descriptions["and r a"] = "Realiza um 'e' lógico entre cada bit de 'a' e o bit correspondente no registrador 'r'.";
    descriptions["not r"]   = "Inverte (complementa) o valor dos bits do registrador 'r'.";
    descriptions["sub r a"] = "Subtrai o valor no endereço 'a' do registrador 'r'.";
    descriptions["jsr a"]   = "Desvia para subrotina, armazenando o valor atual de PC em 'a' e desviando a execução para o endereço 'a' + 1.";
    descriptions["neg r"]   = "Troca o sinal do valor em complemento de 2 do registrador 'r' entre positivo e negativo.";
    descriptions["shr r"]   = "Realiza shift lógico dos bits do registrador 'r' para a direita, passando o estado do bit menos significativo para a flag C (carry) e preenchendo o bit mais significativo com 0.";

    // Pitagoras
    descriptions["jd a"] = "Se a flag Z estiver desativada (acumulador diferente de zero), desvia a execução para o endereço 'a'.";

    // REG
    descriptions["inc r"] = "Incrementa o registrador 'r' em uma unidade.";
    descriptions["dec r"] = "Decrementa o registrador 'r' de uma unidade.";
    descriptions["if r a0 a1"] = "Se o registrador 'r' for igual a zero (if zero), desvia a execução para o endereço 'a0'. Se for diferente de zero, desvia para 'a1'.";
}

QString Machine::getDescription(QString assemblyFormat)
{
    // Initialize descriptions
    if (descriptions.isEmpty())
        generateDescriptions();

    return descriptions.value(assemblyFormat, "");
}

void Machine::getAddressingModeDescription(AddressingMode::AddressingModeCode addressingModeCode, QString &acronym, QString &name, QString &format, QString &description)
{
    switch (addressingModeCode)
    {
        case AddressingMode::DIRECT:
            acronym     = "DIR";
            name        = "Direto";
            format      = "Formato: a";
            description = "Valor de 'a' representa o endereço do operando, ou endereço de desvio em operações de jump.";
            break;

        case AddressingMode::INDIRECT:
            acronym     = "IND";
            name        = "Indireto";
            format      = "Formato: a,I (sufixo ,I)";
            description = "Valor de 'a' representa o endereço que contém o endereço direto.";
            break;

        case AddressingMode::IMMEDIATE:
            acronym     = "IMD";
            name        = "Imediato";
            format      = "Formato: #a (prefixo #)";
            description = "Valor de 'a' representa não um endereço, mas um valor imediato a ser carregado ou utilizado em operações aritméticas/lógicas.";
            break;

        case AddressingMode::INDEXED_BY_X:
            acronym     = "IDX";
            name        = "Indexado por X";
            format      = "Formato: a,X (sufixo ,X)";
            description = "Endereçamento direto com deslocamento (offset). A soma dos valores de ‘a’ e do registrador X representa o endereço direto.";
            break;

        case AddressingMode::INDEXED_BY_PC:
            acronym     = "IPC";
            name        = "Indexado por PC";
            format      = "Formato: a,PC (sufixo ,PC)";
            description = "Endereçamento direto com deslocamento (offset). A soma dos valores de ‘a’ e do registrador PC representa o endereço direto.";
            break;
    }
}
