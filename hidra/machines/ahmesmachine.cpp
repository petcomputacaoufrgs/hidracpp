#include "ahmesmachine.h"

AhmesMachine::AhmesMachine()
{
    identifier = "AHD";

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

    flags = QVector<Flag*>(5);

    N = flags[0] = new Flag("N");
    Z = flags[1] = new Flag("Z", true);
    V = flags[2] = new Flag("V");
    C = flags[3] = new Flag("C");
    B = flags[4] = new Flag("B");



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
    instructions.append(new Instruction(2, "0111....", Instruction::SUB, "sub a"));
    instructions.append(new Instruction(2, "1000....", Instruction::JMP, "jmp a"));
    instructions.append(new Instruction(2, "100100..", Instruction::JN,  "jn a"));
    instructions.append(new Instruction(2, "100101..", Instruction::JP,  "jp a"));
    instructions.append(new Instruction(2, "100110..", Instruction::JV,  "jv a"));
    instructions.append(new Instruction(2, "100111..", Instruction::JNV, "jnv a"));
    instructions.append(new Instruction(2, "101000..", Instruction::JZ,  "jz a"));
    instructions.append(new Instruction(2, "101001..", Instruction::JNZ, "jnz a"));
    instructions.append(new Instruction(2, "101100..", Instruction::JC,  "jc a"));
    instructions.append(new Instruction(2, "101101..", Instruction::JNC, "jnc a"));
    instructions.append(new Instruction(2, "101110..", Instruction::JB,  "jb a"));
    instructions.append(new Instruction(2, "101111..", Instruction::JNB, "jnb a"));
    instructions.append(new Instruction(1, "1110..00", Instruction::SHR, "shr"));
    instructions.append(new Instruction(1, "1110..01", Instruction::SHL, "shl"));
    instructions.append(new Instruction(1, "1110..10", Instruction::ROR, "ror"));
    instructions.append(new Instruction(1, "1110..11", Instruction::ROL, "rol"));
    instructions.append(new Instruction(1, "1111....", Instruction::HLT, "hlt"));

    clearCounters();
    running = false;
}

// EXACT DUPLICATE OF NEANDER'S METHOD:
void AhmesMachine::buildInstruction(QString mnemonic, QString arguments, QHash<QString, int> &labelPCMap)
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

void AhmesMachine::setCarry(bool state)
{
    setFlagValue("C", state);
}

void AhmesMachine::setBorrowOrCarry(bool state)
{
    setFlagValue("B", state);
}

void AhmesMachine::setOverflow(bool state)
{
    setFlagValue("V", state);
}
