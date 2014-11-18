#include "neandermachine.h"

NeanderMachine::NeanderMachine()
{
    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    registers = QVector<Register*>(2);

    AC = registers[0] = new Register("AC", 8);
    PC = registers[1] = new Register("PC", 8);



    //////////////////////////////////////////////////
    // Initialize memory
    //////////////////////////////////////////////////

    memory = QVector<Byte*>(MEM_SIZE);
    assemblerMemory = QVector<Byte*>(MEM_SIZE);
    reserved = QVector<bool>(MEM_SIZE);

    correspondingLine = QVector<int>(MEM_SIZE); // Each PC value can have its corresponding line of code

    for (int i=0; i<memory.size(); i++)
        memory[i] = new Byte();

    for (int i=0; i<assemblerMemory.size(); i++)
        assemblerMemory[i] = new Byte();

    setBreakpoint(0); // Reset breakpoint



    //////////////////////////////////////////////////
    // Initialize flags
    //////////////////////////////////////////////////

    flags = QVector<Flag*>(2);

    N = flags[0] = new Flag("N");
    Z = flags[1] = new Flag("Z", true);



    //////////////////////////////////////////////////
    // Initialize instructions
    //////////////////////////////////////////////////

    instructions = QVector<Instruction*>(11);

    instructions[0]  = new Instruction("nop",   0, 0, 1);
    instructions[1]  = new Instruction("sta",  16, 1, 2);
    instructions[2]  = new Instruction("lda",  32, 1, 2);
    instructions[3]  = new Instruction("add",  48, 1, 2);
    instructions[4]  = new Instruction( "or",  64, 1, 2);
    instructions[5]  = new Instruction("and",  80, 1, 2);
    instructions[6]  = new Instruction("not",  96, 0, 1);
    instructions[7]  = new Instruction("jmp", 128, 1, 2);
    instructions[8]  = new Instruction( "jn", 144, 1, 2);
    instructions[9]  = new Instruction( "jz", 160, 1, 2);
    instructions[10] = new Instruction("hlt", 240, 0, 1);
}

void NeanderMachine::printStatusDebug()
{
    std::cout << "PC: " << PC->getValue() << std::endl;
    std::cout << "AC: " << AC->getValue() << std::endl;
    std::cout << "N: "  << (N->getValue()? "1" : "0") << "\t";
    std::cout << "Z: "  << (Z->getValue()? "1" : "0") << std::endl;
}

void NeanderMachine::load(QString filename)
{
    bool err = false;
    QFile memFile(filename);
    memFile.open(QFile::ReadOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);
    unsigned char buffer;
    unsigned char machineIdentifier[4] = {3, 'N', 'D', 'R'};    //machine identifier
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
            stream >> buffer;
        }
    }
    memFile.close();
}

void NeanderMachine::save(QString filename){
    QFile memFile(filename);
    memFile.open(QFile::WriteOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << (unsigned char)3 << (unsigned char)'N' << (unsigned char)'D' << (unsigned char)'R'; //prefixo identificador da maquina (basicamente o que muda em cada maquina

    foreach (Byte *byte, memory) {
        stream << byte->getValue() << (unsigned char)0;
    }
    memFile.close();
}

void NeanderMachine::step()
{
    const Instruction* currentInstruction = getInstructionFromValue(memory[PC->getValue()]->getValue());
    int jumpAddress;
    Byte *operand = NULL;

    if (currentInstruction->getSize() == 2)
    {
        // Read second byte, which contains either the operand's address (ALU, load and store) or the jump's destination address:
        int currentByteValue;

        PC->incrementValue(); // Go to next byte
        currentByteValue = memory[PC->getValue()]->getValue(); // Read byte
        operand = memory[currentByteValue]; // Pointer to operand
        jumpAddress = currentByteValue; // Address to jump to
    }

    PC->incrementValue(); // Prepare for the next step
    if (PC->getValue() == breakpoint)
        this->running = false;

    switch (currentInstruction->getValue())
    {
        case 0x00: // NOP
            break;

        case 0x10: // STA
            operand->setValue(AC->getValue());
            break;

        case 0x20: // LDA
            AC->setValue(operand->getValue());
            break;

        case 0x30: // ADD
            AC->setValue((AC->getValue() + operand->getValue()) & MAX_VALUE);
            break;

        case 0x40: // OR
            AC->setValue(AC->getValue() | operand->getValue());
            break;

        case 0x50: // AND
            AC->setValue(AC->getValue() & operand->getValue());
            break;

        case 0x60: // NOT
            AC->setValue(AC->getValue() ^ 0xFF);
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

        case 0xF0: // HLT
            this->running = false;
            break;

        default:
            break;
    }

    // Update flags:
    N->setValue(AC->getValue() > MAX_SIGNED_VALUE);
    Z->setValue(AC->getValue() == 0);
}

//void NeanderMachine::run() {
//    this->running = true;
//    while (this->running) {
//        this->step();
//    }
//}

// Returns 0 if no error, otherwise returns error code
Machine::ErrorCode NeanderMachine::mountInstruction(QString mnemonic, QString arguments, QHash<QString, int> &labelPCMap)
{
    Instruction *instruction = getInstructionFromMnemonic(mnemonic);
    QStringList argumentList = arguments.split(" ", QString::SkipEmptyParts);
    int numberOfArguments = instruction->getNumberOfArguments();

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
        assemblerMemory[PC->getValue()]->setValue(argumentList[0].toInt(NULL, 0));
        PC->incrementValue();
    }

    return noError;
}

Instruction* NeanderMachine::getInstructionFromValue(int value)
{
    QVector<Instruction*>::iterator i;
    value &= 0xf0; // Filter "don't care" bits

    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getValue() == value) {
            return (*i);
        }
    }
    return NULL;
}

Instruction* NeanderMachine::getInstructionFromMnemonic(QString desiredInstruction)
{
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getMnemonic() == desiredInstruction) {
            return (*i);
        }
    }
    return NULL;
}
