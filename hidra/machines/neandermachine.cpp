#include "neandermachine.h"

NeanderMachine::NeanderMachine()
{
    identifier = "NDR";

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

    correspondingLine = QVector<int>(MEM_SIZE, -1); // Each PC value may be associated with a line of code

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

    instructions.append(new Instruction(1, "0000....", Instruction::NOP, "nop"));
    instructions.append(new Instruction(2, "0001....", Instruction::STR, "sta a"));
    instructions.append(new Instruction(2, "0010....", Instruction::LDR, "lda a"));
    instructions.append(new Instruction(2, "0011....", Instruction::ADD, "add a"));
    instructions.append(new Instruction(2, "0100....", Instruction::OR,  "or a"));
    instructions.append(new Instruction(2, "0101....", Instruction::AND, "and a"));
    instructions.append(new Instruction(1, "0110....", Instruction::NOT, "not"));
    instructions.append(new Instruction(2, "1000....", Instruction::JMP, "jmp a"));
    instructions.append(new Instruction(2, "1001....", Instruction::JN,  "jn a"));
    instructions.append(new Instruction(2, "1010....", Instruction::JZ,  "jz a"));
    instructions.append(new Instruction(1, "1111....", Instruction::HLT, "hlt"));

//    instructions = QVector<Instruction*>(11);

//    instructions[0]  = new Instruction("nop",   0, 0, 1);
//    instructions[1]  = new Instruction("sta",  16, 1, 2);
//    instructions[2]  = new Instruction("lda",  32, 1, 2);
//    instructions[3]  = new Instruction("add",  48, 1, 2);
//    instructions[4]  = new Instruction( "or",  64, 1, 2);
//    instructions[5]  = new Instruction("and",  80, 1, 2);
//    instructions[6]  = new Instruction("not",  96, 0, 1);
//    instructions[7]  = new Instruction("jmp", 128, 1, 2);
//    instructions[8]  = new Instruction( "jn", 144, 1, 2);
//    instructions[9]  = new Instruction( "jz", 160, 1, 2);
//    instructions[10] = new Instruction("hlt", 240, 0, 1);

    clearCounters();
    running = false;
}

//void NeanderMachine::step()
//{
//    int jumpAddress;
//    Byte *operand = NULL;

//    // Read first byte
//    const Instruction* currentInstruction = getInstructionFromValue(memory[PC->getValue()]->getValue());
//    accessCount++;
//    instructionCount++;

//    if (currentInstruction->getNumBytes() == 2)
//    {
//        // Read second byte, which contains either the operand's address (ALU, load and store) or the jump's destination address:
//        int currentByteValue;

//        PC->incrementValue(); // Go to next byte
//        currentByteValue = memory[PC->getValue()]->getValue(); // Read byte
//        operand = memory[currentByteValue]; // Pointer to operand
//        jumpAddress = currentByteValue; // Address to jump to

//        accessCount++;
//    }

//    PC->incrementValue(); // Prepare for the next step
//    if (PC->getValue() == breakpoint)
//        this->running = false;

//    switch (currentInstruction->getValue())
//    {
//        case 0x00: // NOP
//            break;

//        case 0x10: // STA
//            operand->setValue(AC->getValue());
//            accessCount++;
//            break;

//        case 0x20: // LDA
//            AC->setValue(operand->getValue());
//            accessCount++;
//            break;

//        case 0x30: // ADD
//            AC->setValue((AC->getValue() + operand->getValue()) & MAX_VALUE);
//            accessCount++;
//            break;

//        case 0x40: // OR
//            AC->setValue(AC->getValue() | operand->getValue());
//            accessCount++;
//            break;

//        case 0x50: // AND
//            AC->setValue(AC->getValue() & operand->getValue());
//            accessCount++;
//            break;

//        case 0x60: // NOT
//            AC->setValue(AC->getValue() ^ 0xFF);
//            break;

//        case 0x80: // JMP
//            PC->setValue(jumpAddress);
//            break;

//        case 0x90: // JN
//            if (N->getValue())
//                PC->setValue(jumpAddress);
//            break;

//        case 0xA0: // JZ
//            if (Z->getValue())
//                PC->setValue(jumpAddress);
//            break;

//        case 0xF0: // HLT
//            this->running = false;
//            break;

//        default:
//            break;
//    }

//    // Update flags:
//    N->setValue(AC->getValue() > MAX_SIGNED_VALUE);
//    Z->setValue(AC->getValue() == 0);
//}

//void NeanderMachine::run() {
//    this->running = true;
//    while (this->running) {
//        this->step();
//    }
//}

void NeanderMachine::buildInstruction(QString mnemonic, QString arguments, QHash<QString, int> &labelPCMap)
{
    Instruction *instruction = getInstructionFromMnemonic(mnemonic);
    QStringList argumentList = arguments.split(" ", QString::SkipEmptyParts);
    int numberOfArguments = instruction->getArguments().size();

    // Check if correct number of arguments:
    if (argumentList.size() != numberOfArguments)
        throw wrongNumberOfArguments;

    // Write instruction:
    assemblerMemory[getPCValue()]->setValue(instruction->getByteValue());
    PC->incrementValue();

    if (numberOfArguments == 1)
    {
        // Convert possible label to number:
        if (labelPCMap.contains(argumentList[0]))
            argumentList[0] = QString::number(labelPCMap.value(argumentList[0]));

        // Check if valid argument:
        if (!isValidAddress(argumentList[0]))
            throw invalidAddress;

        // Write argument:
        assemblerMemory[PC->getValue()]->setValue(argumentList[0].toInt(NULL, 0));
        PC->incrementValue();
    }
}

