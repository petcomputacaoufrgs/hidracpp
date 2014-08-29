#include "ahmesmachine.h"

AhmesMachine::AhmesMachine()
{
    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    registers = QVector<Register*>(2);

    for (int i=0; i<registers.size(); i++)
        registers[i] = new Register(8);

    PC = registers[1];
    AC = registers[0];



    //////////////////////////////////////////////////
    // Initialize memory
    //////////////////////////////////////////////////

    memory = QVector<Byte*>(MEM_SIZE);
    assemblerMemory = QVector<Byte*>(MEM_SIZE);
    reserved = QVector<bool>(MEM_SIZE);

    correspondingLine = QVector<int>(MEM_SIZE); // Every PC value can have a corresponding line of code

    for (int i=0; i<memory.size(); i++)
        memory[i] = new Byte();

    for (int i=0; i<assemblerMemory.size(); i++)
        assemblerMemory[i] = new Byte();

    setBreakpoint(0); // Reset breakpoint



    //////////////////////////////////////////////////
    // Initialize flags
    //////////////////////////////////////////////////

    flags = QVector<Bit*>(5);

    for (int i = 0; i < flags.size(); i++)
        flags[i] = new Bit();

    N = flags[0];
    Z = flags[1];
    V = flags[2];
    C = flags[3];
    B = flags[4];

    Z->setValue(true);



    //////////////////////////////////////////////////
    // Initialize instructions
    //////////////////////////////////////////////////

    instructions = QVector<Instruction*>(24);
    instructions[0]  = new Instruction("nop",   0, 0, 1);
    instructions[1]  = new Instruction("sta",  16, 1, 2);
    instructions[2]  = new Instruction("lda",  32, 1, 2);
    instructions[3]  = new Instruction("add",  48, 1, 2);
    instructions[4]  = new Instruction( "or",  64, 1, 2);
    instructions[5]  = new Instruction("and",  80, 1, 2);
    instructions[6]  = new Instruction("not",  96, 0, 1);
    instructions[7]  = new Instruction("sub", 112, 1, 2);
    instructions[8]  = new Instruction("jmp", 128, 1, 2);
    instructions[9]  = new Instruction( "jn", 144, 1, 2);
    instructions[10] = new Instruction( "jp", 148, 1, 2);
    instructions[11] = new Instruction( "jv", 152, 1, 2);
    instructions[12] = new Instruction("jnv", 156, 1, 2);
    instructions[13] = new Instruction( "jz", 160, 1, 2);
    instructions[14] = new Instruction("jnz", 164, 1, 2);
    instructions[15] = new Instruction( "jc", 176, 1, 2);
    instructions[16] = new Instruction("jnc", 180, 1, 2);
    instructions[17] = new Instruction( "jb", 184, 1, 2);
    instructions[18] = new Instruction("jnb", 188, 1, 2);
    instructions[19] = new Instruction("shr", 225, 0, 1);
    instructions[20] = new Instruction("shl", 225, 0, 1);
    instructions[21] = new Instruction("ror", 226, 0, 1);
    instructions[22] = new Instruction("rol", 227, 0, 1);
    instructions[23] = new Instruction("hlt", 240, 0, 1);
}

void AhmesMachine::printStatusDebug()
{
    std::cout << "PC: " << PC->getValue() << std::endl;
    std::cout << "AC: " << AC->getValue() << std::endl;
}

void AhmesMachine::load(QString filename)
{
    bool err = false;
    QFile memFile(filename);
    memFile.open(QFile::ReadOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);
    unsigned char buffer;
    unsigned char machineIdentifier[4] = {3, 'A', 'H', 'D'};    //machine identifier
    int i;
    for(i = 0; i < 4; i++) {
        stream >> buffer;
        if(buffer != machineIdentifier[i]) {
            err = true;
            break;
        }
    }
    i = 0;
    if(!err) {
        while(!stream.atEnd()) {
            stream >> buffer;
            memory[i++]->setValue((unsigned char)buffer);
            stream >> buffer;   //ignore the byte 0
        }
    }
    memFile.close();
}

void AhmesMachine::save(QString filename)
{
    QFile memFile(filename);
    memFile.open(QFile::WriteOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << (unsigned char)3 << (unsigned char)'A' << (unsigned char)'H' << (unsigned char)'D'; //prefixo identificador da maquina (basicamente o que muda em cada maquina

    foreach (Byte *byte, memory) {
        stream << byte->getValue() << (unsigned char)0;
    }
    memFile.close();
}

void AhmesMachine::step()
{
    const Instruction* currentInstruction = getInstructionFromValue(memory[PC->getValue()]->getValue());
    Byte *operand;

    int previousAC = AC->getValue();
    if(currentInstruction->getSize() == 2)
    {
        PC->incrementValue(); // Go to next byte
        operand = memory[PC->getValue()];
    }

    PC->incrementValue(); // Prepare for the next step
    if(PC->getValue() == breakpoint)
        this->running = false;

    switch (currentInstruction->getValue())
    {
        case 0x00: // NOP
            break;

        case 0x10: // STA
            memory[operand->getValue()]->setValue(AC->getValue());
            break;

        case 0x20: // LDA
            AC->setValue(memory[operand->getValue()]->getValue());
            break;

        case 0x30: // ADD
            AC->setValue((AC->getValue() + memory[operand->getValue()]->getValue()) & 0xFF);

            // Unsigned carry flag (true when result is less than initial value):
            C->setValue(AC->getValue() < previousAC);
            // Signed overflow flag (true if unexpected signed result):
            V->setValue(getSignedInt(previousAC) + getSignedInt(memory[operand->getValue()]->getValue()) != getSignedInt(AC->getValue()));

            break;

        case 0x40: // OR
            AC->setValue(AC->getValue() | memory[operand->getValue()]->getValue());
            break;

        case 0x50: // AND
            AC->setValue(AC->getValue() & memory[operand->getValue()]->getValue());
            break;

        case 0x60: // NOT
            AC->setValue(AC->getValue() ^ 0xFF);
            break;

        case 0x70: // SUB
            AC->setValue((AC->getValue() - memory[operand->getValue()]->getValue()) & 0xFF);

            // Unsigned carry flag (true when result is less than initial value):
            B->setValue(previousAC < operand->getValue());
            // Signed overflow flag (true if unexpected signed result):
            V->setValue(getSignedInt(previousAC) - getSignedInt(memory[operand->getValue()]->getValue()) != getSignedInt(AC->getValue()));

            break;

        case 0x80: // JMP
            PC->setValue(operand->getValue());
            break;

        case 0x90: // JN
            if (N->getValue())
                PC->setValue(operand->getValue());
            break;

        case 0x94: // JP
            if (!N->getValue())
                PC->setValue(operand->getValue());
            break;

        case 0x98: // JV
            if (V->getValue())
                PC->setValue(operand->getValue());
            break;

        case 0x9C: // JNV
            if (!V->getValue())
                PC->setValue(operand->getValue());
            break;

        case 0xA0: // JZ
            if (Z->getValue())
                PC->setValue(operand->getValue());
            break;

        case 0xA4: // JNZ
            if (!Z->getValue())
                PC->setValue(operand->getValue());
            break;

        case 0xB0: // JC
            if (C->getValue())
                PC->setValue(operand->getValue());
            break;

        case 0xB4: // JNC
            if (!C->getValue())
                PC->setValue(operand->getValue());
            break;

        case 0xB8: // JB
            if (B->getValue())
                PC->setValue(operand->getValue());
            break;

        case 0xBC: // JNB
            if (!B->getValue())
                PC->setValue(operand->getValue());
            break;

        case 0xE0: // SHR
            AC->setValue(AC->getValue() >> 1);
            C->setValue((previousAC & 0x01) != 0);
            break;

        case 0xE1: // SHL
            AC->setValue(AC->getValue() << 1);
            C->setValue((AC->getValue() & 0x80) != 0);
            break;

        case 0xE2: // ROR
            AC->setValue((AC->getValue() >> 1) & 0xFF);
            C->setValue((previousAC & 0x01) != 0);
            break;

        case 0xE3: // ROL
            AC->setValue((AC->getValue() << 1) & 0xFF);
            if (C->getValue())
                AC->setValue(AC->getValue() | 0x01);
            C->setValue((previousAC & 0x80) != 0);
            break;

        case 0xF0: // HLT
            this->running = false;
            break;

        default:
            break;
    }

    // Update flags:
    Z->setValue(AC->getValue() == 0);
    N->setValue(AC->getValue() > MAX_SIGNED_VALUE);
}

void AhmesMachine::run() {
    this->running = true;
    while (this->running && this->PC->getValue() <= 255) {
        this->step();
    }
}

int AhmesMachine::getSignedInt(int eightBitValue)
{
    if (eightBitValue <= MAX_SIGNED_VALUE)
        return eightBitValue;
    else
        return eightBitValue - (MAX_VALUE + 1);
}

// EXACT DUPLICATE OF NEANDER'S METHOD:
// Returns 0 if no error, otherwise returns error code
Machine::ErrorCode AhmesMachine::mountInstruction(QString mnemonic, QString arguments, QHash<QString, int> &labelPCMap)
{
    Instruction *instruction = getInstructionFromMnemonic(mnemonic);
    QStringList argumentList = arguments.split(" ", QString::SkipEmptyParts);
    int numberOfArguments = instruction->getNumberOfArguments();
    bool ok;

    // Check if correct number of arguments:
    if (argumentList.size() != numberOfArguments)
        return wrongNumberOfArguments;

    // Write instruction:
    assemblerMemory[PC->getValue()]->setValue(instruction->getValue());
    PC->incrementValue();

    if (numberOfArguments == 1)
    {
        // Convert possible label to number:
        if (labelPCMap.contains(argumentList[0]))
            argumentList[0] = QString::number(labelPCMap.value(argumentList[0]));

        // Check if valid argument:
        if (!isValidAddress(argumentList[0]))
            return invalidAddress;

        // Write argument:
        assemblerMemory[PC->getValue()]->setValue(argumentList[0].toInt(&ok, 0));
        PC->incrementValue();
    }

    return noError;
}

Instruction* AhmesMachine::getInstructionFromValue(int value)
{
    QVector<Instruction*>::iterator i;

    // Ignore "don't care" bits:
    if (value < 0x90 || value >= 0xF0)
        value &= 0b11110000;
    else if (value < 0xC0)
        value &= 0b11111100; // Special jumps
    else
        value &= 0b11110011; // Shift and rotate

    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getValue() == value) {
            return (*i);
        }
    }
    return NULL;
}

Instruction* AhmesMachine::getInstructionFromMnemonic(QString desiredInstruction)
{
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getMnemonic() == desiredInstruction) {
            return (*i);
        }
    }
    return NULL;
}
