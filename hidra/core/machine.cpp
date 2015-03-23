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

    // Constants
    QString ALLOCATE_SYMBOL = "&";
    QString CHAR_SYMBOL = "#";
    QChar   AMPERSAND = '\'';

    // RegExes
    QRegExp matchBrackets("\\[(\\d*)\\]"); // Digits between brackets
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
    int charCount = 0; // Char count of ASCII string

    bool insideString = false;

    for (QChar &c : arguments)
    {
        // NUMBER MODE:
        if (insideString == false)
        {
            if (c == AMPERSAND)
            {
                // Add finished number argument list
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
            if (c == AMPERSAND && charCount > 0)
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
        throw invalidValue;

    return finalArgumentList;



    /*
    //////////////////////////////////////////////////
    // ORIGINAL CODE
    //////////////////////////////////////////////////

    QStringList argumentsByComma;
    QStringList finalArgumentsList;

    //////////////////////////////////////////////////
    // FIRST PASS: Divide by comma joining strings
    //////////////////////////////////////////////////



    {
        QStringList splitedArguments = arguments.split(",");
        int argumentCounter = 0;
        int partsQuantity = splitedArguments.size();

        QString argument;
        bool isSingleQuote = false;

        while (argumentCounter < partsQuantity)
        {
            argument = splitedArguments.at(argumentCounter);

            // Remove whitespaces from the beginning
            int position = 0;
            while (position < argument.size() && QRegExp("\\s").exactMatch(argument.at(position)))
                position++;

            // If there's only whitespaces, something is wrong
            if (position == argument.size())
                throw 0;

            // If it starts with a single quote it's a string or a char
            if (argument.at(position) == '\'')
            {
                QStringList argumentParts;
                do
                {
                    // Remove whitespaces from the end
                    int final = argument.size() - 1;
                    while (final >= 0 && QRegExp("\\s").exactMatch(argument.at(final)))
                        final--;

                    // If there's a sigle quote in the end and it's not the same as the one at the beginning then probably it's a correct string
                    if (final != position && argument.at(final) == '\'')
                    {
                        argumentParts.append(argument.mid(position+1, final-(position+1)));
                        isSingleQuote = false;
                    }
                    // Else if final reaches position then there's only whitespaces after the single quote, or if there isn't a single quote in the end then probably there was a comma in the string
                    else
                    {
                        argumentParts.append(argument.mid(position+1));
                        argumentCounter++;
                        if (argumentCounter >= partsQuantity)
                            throw 0;

                        argument = splitedArguments.at(argumentCounter);
                        position = -1;
                        isSingleQuote = true;
                    }
                } while (isSingleQuote);

                // # means it's a string
                argumentsByComma.append("#" + argumentParts.join(","));
            }
            // Else if it starts with underline or a letter it's a label or hexadecimal number
            else if (argument.at(position) == '_' || argument.at(position).isLetter())
            {
                // Remove whitespaces from the end
                int final = argument.size() - 1;
                while (final >= 0 && QRegExp("\\s").exactMatch(argument.at(final)))
                    final--;

                // $ means it's a label or hexadecimal number
                argumentsByComma.append("$" + argument.mid(position, (final+1)-position).toLower());
            }
            // Else if it starts with a bracket it's the number os bytes/word to be alocated
            else if (argument.at(position) == '[')
            {
                // Remove whitespaces from the end
                int final = argument.size() - 1;
                while (final >= 0 && QRegExp("\\s").exactMatch(argument.at(final)))
                    final--;

                if (argument.at(final) == ']')
                    // & means it's a number of bytes/words to be alocated
                    argumentsByComma.append("&" + argument.mid(position+1, final-(position+1)));
                else
                    throw 0;
            }
            else
            {
                // Remove whitespaces from the end
                int final = argument.size() - 1;
                while (final >= 0 && QRegExp("\\s").exactMatch(argument.at(final)))
                    final--;

                argumentsByComma.append(argument.mid(position, (final+1)-position));
            }

            argumentCounter++;
        }
    }
    //////////////////////////////////////////////////
    // SECOND PASS: Divide string into chars and validate data
    //////////////////////////////////////////////////

    for (QString &argument : argumentsByComma)
    {
        int position = 0;
        // If it's a string divide into chars
        if (argument.at(position) == '#')
        {
            position++;
            // If it's a single quote check if there's another char after the quote
            if (argument.at(position) == '\'')
            {
                if (position+1 < argument.size())
                    throw 0;

                finalArgumentsList.append("#'");
            }
            else
            {
                // If it's any char insert all into the final arguments list checking if there's a single quote inside the string
                for (; position < argument.size(); position++)
                {
                    if (argument.at(position) == '\'')
                        throw 0;

                    finalArgumentsList.append("#" + QString(argument.at(position)));
                }
            }
        }
        else if (argument.at(position) == '$')
        {
            QRegExp validLabel("[a-z_][a-z0-9_]*");
            position++;
            if (!validLabel.exactMatch(argument.mid(position)))
                throw 0;

            finalArgumentsList.append(argument.mid(position));
        }
        else if (argument.at(position) == '&')
        {
            QRegExp validNumber("\\d+");
            position++;
            if (!validNumber.exactMatch(argument.mid(position)))
                throw 0;

            finalArgumentsList.append(argument);
        }
        else
        {
            QRegExp validNumber("-?\\d+");
            if (!validNumber.exactMatch(argument))
                throw 0;

            finalArgumentsList.append(argument);
        }
    }

    */
}

int Machine::convertToUnsigned(int value, int numberOfBytes)
{
    if (value >= 0)
        return value;
    else if (numberOfBytes == 1)
    {
        return 256+value;
    }
    else
    {
        return 65536+value;
    }
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

    case memoryOverflow:
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

Machine::ErrorCode Machine::obeyDirective(QString mnemonic, QString arguments, bool reserveOnly, QHash<QString, int> &labelPCMap)
{
    if (mnemonic == "org")
    {
        QStringList argumentList = arguments.split(" ", QString::SkipEmptyParts);
        int numberOfArguments = argumentList.size();

        if (numberOfArguments != 1)
            return wrongNumberOfArguments;

        if (!isValidAddress(argumentList.first()))
            return invalidAddress;

        PC->setValue(argumentList.first().toInt(0));
    }
    else if (QRegExp("da?[bw]").exactMatch(mnemonic))
    {
        QStringList argumentList;
        try
        {
            argumentList = splitArguments(arguments);
        }
        catch (ErrorCode errorCode)
        {
            return errorCode;
        }

        int numberOfArguments = argumentList.size();
        int bytesPerArgument = QRegExp("da?b").exactMatch(mnemonic) ? 1 : 2;

        if (QRegExp("d[bw]").exactMatch(mnemonic) && numberOfArguments != 1)
            return wrongNumberOfArguments;

        if (reserveOnly)
        {
            // If it's the number of bytes/words to reserve then verify the mnemonic and the number of arguments
            if (argumentList.first().at(0) == '&')
            {
                if (numberOfArguments != 1)
                    return wrongNumberOfArguments;
                else if (!QRegExp("da[bw]").exactMatch(mnemonic))
                    return invalidValue;
                else
                    return reserveAssemblerMemory(argumentList.first().mid(1).toInt() * bytesPerArgument);
            }
            else
                return reserveAssemblerMemory(numberOfArguments * bytesPerArgument); // Increments PC
        }
        else
        {
            for (QString argument : argumentList)
            {
                // If it starts with a sharp it's a char
                if (argument.at(0) == '#')
                {
                    if (QRegExp("da?w").exactMatch(mnemonic))
                         PC->incrementValue();

                    assemblerMemory[PC->getValue()]->setValue(argument.at(1).toLatin1());
                    PC->incrementValue();
                }
                // Else if it starts with a letter or underline it's a label or a hexadecimal number
                else if (argument.at(0) == '_' || argument.at(0).isLetter())
                {
                    argument = argument.toLower(); // Convert to lowercase

                    int value;
                    if (QRegExp("d[bw]").exactMatch(mnemonic) && labelPCMap.contains(argument))
                    {
                        if (QRegExp("dw").exactMatch(mnemonic))
                            PC->incrementValue();

                        value = labelPCMap.value(argument);
                    }
                    else
                    {
                        if (argument.at(0) == 'h')
                        {
                            bool ok;
                            value = argument.mid(1).toInt(&ok, 16);
                            if (!ok || !isValidNBytesValue(QString::number(value), bytesPerArgument))
                                return invalidValue;

                            if (value > 255)
                            {
                                int mostSignificantBytes = value >> 8;
                                value = value - (mostSignificantBytes << 8);
                                assemblerMemory[PC->getValue()]->setValue(mostSignificantBytes);
                                PC->incrementValue();
                            }
                            else if (bytesPerArgument == 2)
                            {
                                assemblerMemory[PC->getValue()]->setValue(value);
                                PC->incrementValue();
                                value = 0;
                            }
                        }
                        else
                            return invalidValue;
                    }
                    assemblerMemory[PC->getValue()]->setValue(value);
                    PC->incrementValue();
                }
                // If it starts with a & it's the number os bytes/words to be alocated
                else if (argument.at(0) == '&')
                {
                    for (int i = 0; i < argument.mid(1).toInt()*bytesPerArgument; i++)
                        PC->incrementValue();
                }
                // Else it's a number
                else
                {
                    int value;
                    if (!isValidNBytesValue(argument, bytesPerArgument))
                        return invalidValue;

                    value = argument.toInt();
                    value = convertToUnsigned(value, bytesPerArgument);

                    if (bytesPerArgument == 2)
                    {
                        int mostSignificantBytes = value >> 8;
                        value = value - (mostSignificantBytes << 8);
                        assemblerMemory[PC->getValue()]->setValue(mostSignificantBytes);
                        PC->incrementValue();
                    }
                    assemblerMemory[PC->getValue()]->setValue(value);
                    PC->incrementValue();
                }
            }
        }
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
        sourceLines[lineNumber] = sourceLines[lineNumber].section(";", 0, 0).trimmed(); //elimina os comentarios do codigo
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
        //////////////////////////////////////////////////
        // Read labels
        //////////////////////////////////////////////////

        if (sourceLines[lineNumber].contains(":")) // If getLabel found a label
        {
            QString labelName = sourceLines[lineNumber].section(":", 0, 0);

            // Check if label name is valid:
            if (!validLabel.exactMatch(labelName.toLower()))
            {
                emitError(lineNumber, invalidLabel);
                return;
            }

            // Check for duplicated labels:
            if (labelPCMap.contains(labelName.toLower()))
            {
                emitError(lineNumber, duplicatedLabel);
                return;
            }

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
                errorCode = reserveAssemblerMemory(instruction->getSize());
            }
            else // Directive
            {
                QString arguments = sourceLines[lineNumber].section(whitespaces, 1); // Everything after mnemonic
                errorCode = obeyDirective(mnemonic, arguments, true, labelPCMap);
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
            QString mnemonic  = sourceLines[lineNumber].section(whitespaces, 0, 0).toLower();
            QString arguments = sourceLines[lineNumber].section(whitespaces, 1); // Everything after mnemonic

            const Instruction *instruction = getInstructionFromMnemonic(mnemonic);
            if (instruction != NULL)
            {
                correspondingLine[PC->getValue()] = lineNumber;
                errorCode = mountInstruction(mnemonic, arguments.toLower(), labelPCMap);
            }
            else // Directive
            {
                correspondingLine[PC->getValue()] = lineNumber;
                errorCode = obeyDirective(mnemonic, arguments, false, labelPCMap);
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
