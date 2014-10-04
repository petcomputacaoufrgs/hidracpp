#include "ramsesmachine.h"

RamsesMachine::RamsesMachine()
{
    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    registers = QVector<Register*>(4);

    for (int i=0; i<registers.size(); i++)
        registers[i] = new Register(8);

    RA = registers[0];
    RB = registers[1];
    RX = registers[2];
    PC = registers[3];



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

    flags = QVector<Bit*>(3);

    for (int i = 0; i < flags.size(); i++)
        flags[i] = new Bit();

    N = flags[0];
    Z = flags[1];
    C = flags[2];

    Z->setValue(true);



    //////////////////////////////////////////////////
    // Initialize instructions
    //////////////////////////////////////////////////

    instructions = QVector<Instruction*>(16);
    instructions[0]  = new Instruction("nop",   0, 0, 1);
    instructions[1]  = new Instruction("str",  16, 2, 2);
    instructions[2]  = new Instruction("ldr",  32, 2, 2);
    instructions[3]  = new Instruction("add",  48, 2, 2);
    instructions[4]  = new Instruction( "or",  64, 2, 2);
    instructions[5]  = new Instruction("and",  80, 2, 2);
    instructions[6]  = new Instruction("not",  96, 1, 1);
    instructions[7]  = new Instruction("sub", 112, 2, 2);
    instructions[8]  = new Instruction("jmp", 128, 1, 2);
    instructions[9]  = new Instruction( "jn", 144, 1, 2);
    instructions[10] = new Instruction( "jz", 160, 1, 2);
    instructions[11] = new Instruction( "jc", 176, 1, 2);
    instructions[12] = new Instruction("jsr", 192, 1, 2);
    instructions[13] = new Instruction("neg", 208, 1, 1);
    instructions[14] = new Instruction("shr", 224, 1, 1);
    instructions[15] = new Instruction("hlt", 240, 0, 1);
}

void RamsesMachine::printStatusDebug()
{
    std::cout << std::endl;
    std::cout << "RA: " << (int)RA->getValue() << std::endl;;
    std::cout << "RB: " << (int)RB->getValue() << std::endl;;
    std::cout << "RX: " << (int)RX->getValue() << std::endl;;
}

void RamsesMachine::load(QString filename)
{
    bool err = false;
    QFile memFile(filename);
    memFile.open(QFile::ReadOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);
    unsigned char buffer;
    unsigned char IDBuffer[4];
    unsigned char olderMachineIdentifier[4] = {3, 'N', 'D', 'R'};    //Ramses is code-compatible with Neander
    unsigned char machineIdentifier[4] = {3, 'R', 'M', 'S'};    //machine identifier
    stream >> IDBuffer[0];
    stream >> IDBuffer[1];
    stream >> IDBuffer[2];
    stream >> IDBuffer[3];
    int i;
    for(i = 0; i < 4; i++) {
        if(IDBuffer[i] != machineIdentifier[i]) {
            err = true;
            break;
        }
    }
    for(i = 0; i < 4; i++) {
        if(IDBuffer[i] != olderMachineIdentifier[i]) {
            err = true;
            break;
        }
    }
    i = 0;
    if(!err) {
        while(!stream.atEnd()) {
            stream >> buffer;
            memory[i++]->setValue((unsigned char)buffer);
            stream >> buffer;
        }
    }
    memFile.close();
}

void RamsesMachine::save(QString filename)
{
    QFile memFile(filename);
    memFile.open(QFile::WriteOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << (unsigned char)3 << (unsigned char)'R' << (unsigned char)'M' << (unsigned char)'S'; //prefixo identificador da maquina (basicamente o que muda em cada maquina

    foreach (Byte *byte, memory) {
        stream << byte->getValue() << (unsigned char)0;
    }
    memFile.close();
}

void RamsesMachine::step()
{
    const Instruction *currentInstruction = getInstructionFromValue(memory[PC->getValue()]->getValue());
    Byte *operand = NULL; // Final operand byte
    int jumpAddress;

    int addressingMode = memory[PC->getValue()]->getValue() & 0b00000011; // Extract last two bits
    int reg = (memory[PC->getValue()]->getValue() & 0b00001100) >> 2; // Bits 2 and 3 indicate register (A, B or X)

    bool updateFlags = false;

    if (currentInstruction->getSize() == 2)
    {
        int operandAddress, currentByteValue;

        PC->incrementValue(); // Go to next byte
        currentByteValue = memory[PC->getValue()]->getValue();

        switch (addressingMode)
        {
            case 0x00: // Direct addressing mode
                operandAddress = currentByteValue;
                operand = memory[operandAddress];
                jumpAddress = operandAddress;
                break;

            case 0x01: // Indirect addressing mode
                operandAddress = memory[currentByteValue]->getValue();
                operand = memory[operandAddress];
                jumpAddress = operandAddress;
                break;

            case 0x02: // Immediate addressing mode
                operand = memory[PC->getValue()];
                jumpAddress = (PC->getValue()+1) % MEM_SIZE; // Invalid, jump to next byte (ignore jump)
                break;

            case 0x03: // Indexed addressing mode
                operandAddress = (currentByteValue + RX->getValue()) % MEM_SIZE;
                operand = memory[operandAddress];
                jumpAddress = operandAddress;
                break;
        }
    }

    PC->incrementValue(); // Prepare for the next step
    if (PC->getValue() == breakpoint)
        this->running = false;

    switch (currentInstruction->getValue())
    {
        case 0x00: // NOP
            break;

        case 0x10: // STR
            operand->setValue(registers[reg]->getValue());
            break;

        case 0x20: // LDR
            registers[reg]->setValue(operand->getValue());
            C->setValue(false);
            updateFlags = true;
            break;

        case 0x30: // ADD
            registers[reg]->setValue((operand->getValue() + registers[reg]->getValue()) & MAX_VALUE);
            C->setValue((registers[reg]->getValue() + operand->getValue()) > MAX_VALUE); // Carry flag (unsigned)
            updateFlags = true;
            break;

        case 0x40: // OR
            registers[reg]->setValue(registers[reg]->getValue() | operand->getValue());
            C->setValue(false);
            updateFlags = true;
            break;

        case 0x50: // AND
            registers[reg]->setValue(registers[reg]->getValue() & operand->getValue());
            C->setValue(false);
            updateFlags = true;
            break;

        case 0x60: // NOT
            registers[reg]->setValue(~registers[reg]->getValue());
            C->setValue(false);
            updateFlags = true;
            break;

        case 0x70: // SUB
            registers[reg]->setValue((registers[reg]->getValue() - operand->getValue()) & MAX_VALUE);
            C->setValue((registers[reg]->getValue() - operand->getValue()) < 0); // Carry flag (unsigned)
            updateFlags = true;
            break;

        case 0x80: // JMP
            PC->setValue(jumpAddress);
            break;

        case 0x90: // JN
            if (N->getValue())
                PC->setValue(jumpAddress);
            break;

        case 0xA0: // JZ
            if (Z->getValue())
                PC->setValue(jumpAddress);
            break;

        case 0xB0: // JC
            if (C->getValue())
                PC->setValue(jumpAddress);
            break;

        case 0xC0: // JSR
            memory[operand->getValue()]->setValue(PC->getValue());
            PC->setValue(jumpAddress + 1);
            break;

        case 0xD0: // NEG
            registers[reg]->setValue(~registers[reg]->getValue() + 1);
            updateFlags = true;
            break;

        case 0xE0: // SHR
            C->setValue((registers[reg]->getValue() & 0x01) == 1);
            registers[reg]->setValue(registers[reg]->getValue() >> 1);
            updateFlags = true;
            break;

        case 0xF0: // HLT
            this->running = false;
            break;
    }

    // Update flags:
    if (updateFlags)
    {
        N->setValue(registers[reg]->getValue() > MAX_SIGNED_VALUE);
        Z->setValue(registers[reg]->getValue() == 0);
    }
}

//void RamsesMachine::run()
//{
//    this->running = true;
//    while (this->running) {
//        this->step();
//    }
//}

Machine::ErrorCode RamsesMachine::mountInstruction(QString mnemonic, QString arguments, QHash<QString, int> &labelPCMap)
{
    Instruction *instruction = getInstructionFromMnemonic(mnemonic);
    QStringList argumentList = arguments.split(" ", QString::SkipEmptyParts);
    int numberOfArguments = instruction->getNumberOfArguments();

    int registerID = 0;
    int addressingMode = 0;

    // Check if correct number of arguments:
    if (argumentList.size() != numberOfArguments)
        return wrongNumberOfArguments;

    // If first argument is a register (every instruction that has arguments, except jumps):
    if (numberOfArguments > 0 && !(mnemonic.startsWith("j")))
    {
        if (argumentList.first() == "a")
            registerID = 0;
        else if (argumentList.first() == "b")
            registerID = 1;
        else if (argumentList.first() == "x")
            registerID = 2;
        else
            return invalidArgument;
    }

    // If last argument is an address (every two-byte instruction):
    if (instruction->getSize() == 2)
    {
        // Extract and remove addressing mode:
        if (argumentList.last().endsWith(",i"))
        {
            argumentList.last() = argumentList.last().replace(",i", "");
            addressingMode = 1; // Direct
        }
        else if (argumentList.last().startsWith("#"))
        {
            argumentList.last() = argumentList.last().replace("#", "");
            addressingMode = 2; // Indirect
        }
        else if (argumentList.last().endsWith(",x"))
        {
            argumentList.last() = argumentList.last().replace(",x", "");
            addressingMode = 3; // Indexed
        }
    }

    // Write first byte (instruction with register and addressing mode):
    assemblerMemory[PC->getValue()]->setValue(instruction->getValue() + (registerID << 2) + addressingMode);
    PC->incrementValue();

    // If instruction has two bytes, write second byte:
    if (instruction->getSize() > 1)
    {
        // Convert possible label to number:
        if (labelPCMap.contains(argumentList.last()))
            argumentList.last() = QString::number(labelPCMap.value(argumentList.last()));

        // Check if valid address:
        if (!isValidAddress(argumentList.last()))
            return invalidArgument;

        // Write address argument:
        assemblerMemory[PC->getValue()]->setValue(argumentList.last().toInt(NULL, 0));
        PC->incrementValue();
    }

    return noError;
}

Instruction* RamsesMachine::getInstructionFromValue(int value)
{
    QVector<Instruction*>::iterator i;
    value &= 0b11110000; // Filter bits

    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getValue() == value) {
            return (*i);
        }
    }
    return NULL;
}

Instruction* RamsesMachine::getInstructionFromMnemonic(QString desiredInstruction)
{
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getMnemonic() == desiredInstruction) {
            return (*i);
        }
    }
    return NULL;
}
