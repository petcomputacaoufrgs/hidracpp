#include "cromagmachine.h"

CromagMachine::CromagMachine()
{
    identifier = "CMG"; // TODO: Confirmar c/Weber

    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    registers.append(new Register("A", "........", 8));
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
    flags.append(new Flag(Flag::CARRY, "C"));



    //////////////////////////////////////////////////
    // Initialize instructions
    //////////////////////////////////////////////////

    instructions.append(new Instruction(1, "0000....", Instruction::NOP, "nop"));
    instructions.append(new Instruction(2, "0001....", Instruction::STR, "str a"));
    instructions.append(new Instruction(2, "0010....", Instruction::LDR, "ldr a"));
    instructions.append(new Instruction(2, "0011....", Instruction::ADD, "add a"));
    instructions.append(new Instruction(2, "0100....", Instruction::OR,  "or a"));
    instructions.append(new Instruction(2, "0101....", Instruction::AND, "and a"));
    instructions.append(new Instruction(1, "0110....", Instruction::NOT, "not"));
    instructions.append(new Instruction(2, "1000....", Instruction::JMP, "jmp a"));
    instructions.append(new Instruction(2, "1001....", Instruction::JN,  "jn a"));
    instructions.append(new Instruction(2, "1010....", Instruction::JZ,  "jz a"));
    instructions.append(new Instruction(2, "1011....", Instruction::JC,  "jc a"));
    instructions.append(new Instruction(1, "1110....", Instruction::SHR, "shr"));
    instructions.append(new Instruction(1, "1111....", Instruction::HLT, "hlt"));



    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////

    addressingModes.append(new AddressingMode(".......0", AddressingMode::DIRECT, AddressingMode::NO_PATTERN));
    addressingModes.append(new AddressingMode(".......1", AddressingMode::INDIRECT,     "(.*),i"));
}
