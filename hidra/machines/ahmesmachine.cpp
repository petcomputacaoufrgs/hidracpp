#include "ahmesmachine.h"

AhmesMachine::AhmesMachine()
{
    identifier = "AHD";

    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    registers.append(new Register("AC", "........", 8));
    registers.append(new Register("PC", "", 8));

    PC = registers.last();



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

    flags.append(new Flag("N"));
    flags.append(new Flag("Z", true));
    flags.append(new Flag("V"));
    flags.append(new Flag("C"));
    flags.append(new Flag("B"));



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



    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////

    addressingModes.append(DIRECT);

    clearCounters();
    running = false;
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
