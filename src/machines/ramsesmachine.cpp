#include "ramsesmachine.h"

RamsesMachine::RamsesMachine()
{
    identifier = "RMS";

    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    registers.append(new Register( "A", "....00..", 8));
    registers.append(new Register( "B", "....01..", 8));
    registers.append(new Register( "X", "....10..", 8));
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
    flags.append(new Flag(Flag::CARRY_NOT_BORROW, "C"));



    //////////////////////////////////////////////////
    // Initialize instructions
    //////////////////////////////////////////////////

    instructions.append(new Instruction(1, "0000....", Instruction::NOP, "nop"));
    instructions.append(new Instruction(2, "0001....", Instruction::STR, "str r a"));
    instructions.append(new Instruction(2, "0010....", Instruction::LDR, "ldr r a"));
    instructions.append(new Instruction(2, "0011....", Instruction::ADD, "add r a"));
    instructions.append(new Instruction(2, "0100....", Instruction::OR,  "or r a"));
    instructions.append(new Instruction(2, "0101....", Instruction::AND, "and r a"));
    instructions.append(new Instruction(1, "0110....", Instruction::NOT, "not r"));
    instructions.append(new Instruction(2, "0111....", Instruction::SUB, "sub r a"));
    instructions.append(new Instruction(2, "1000....", Instruction::JMP, "jmp a"));
    instructions.append(new Instruction(2, "1001....", Instruction::JN,  "jn a"));
    instructions.append(new Instruction(2, "1010....", Instruction::JZ,  "jz a"));
    instructions.append(new Instruction(2, "1011....", Instruction::JC,  "jc a"));
    instructions.append(new Instruction(2, "1100....", Instruction::JSR, "jsr a"));
    instructions.append(new Instruction(1, "1101....", Instruction::NEG, "neg r"));
    instructions.append(new Instruction(1, "1110....", Instruction::SHR, "shr r"));
    instructions.append(new Instruction(1, "1111....", Instruction::HLT, "hlt"));



    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////

    addressingModes.append(new AddressingMode("......00", AddressingMode::DIRECT,       AddressingMode::NO_PATTERN));
    addressingModes.append(new AddressingMode("......01", AddressingMode::INDIRECT,     "(.*),i"));
    addressingModes.append(new AddressingMode("......10", AddressingMode::IMMEDIATE,    "#(.*)"));
    addressingModes.append(new AddressingMode("......11", AddressingMode::INDEXED_BY_X, "(.*),x"));
}
