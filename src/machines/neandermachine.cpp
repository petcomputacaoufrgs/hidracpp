#include "neandermachine.h"

NeanderMachine::NeanderMachine()
{
    identifier = "NDR";

    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    registers.append(new Register("AC", "........", 8));
    registers.append(new Register("PC", "", 8, false));

    PC = registers.last();



    //////////////////////////////////////////////////
    // Initialize memory
    //////////////////////////////////////////////////

    setMemorySize(256);



    //////////////////////////////////////////////////
    // Initialize flags
    //////////////////////////////////////////////////

    flags.append(new Flag(Flag::NEGATIVE, "N"));
    flags.append(new Flag(Flag::ZERO, "Z", true));



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



    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////

    addressingModes.append(new AddressingMode("........", AddressingMode::DIRECT, AddressingMode::NO_PATTERN));
}
