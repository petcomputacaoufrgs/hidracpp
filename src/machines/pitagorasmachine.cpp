#include "pitagorasmachine.h"

PitagorasMachine::PitagorasMachine()
{
    identifier = "PTG"; // TODO: Confirmar c/Weber

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

    instructions.append(new Instruction(1, "00000000", Instruction::NOP, "nop"));
    instructions.append(new Instruction(2, "00010000", Instruction::STR, "sta a"));
    instructions.append(new Instruction(2, "00100000", Instruction::LDR, "lda a"));
    instructions.append(new Instruction(2, "00110000", Instruction::ADD, "add a"));
    instructions.append(new Instruction(2, "01000000", Instruction::OR,  "or a"));
    instructions.append(new Instruction(2, "01010000", Instruction::AND, "and a"));
    instructions.append(new Instruction(1, "01100000", Instruction::NOT, "not"));
    instructions.append(new Instruction(2, "01110000", Instruction::SUB, "sub a"));
    instructions.append(new Instruction(2, "10000000", Instruction::JMP, "jmp a"));
    instructions.append(new Instruction(2, "10010000", Instruction::JN,  "jn a"));
    instructions.append(new Instruction(2, "10011100", Instruction::JP,  "jp a"));
    instructions.append(new Instruction(2, "10100000", Instruction::JZ,  "jz a"));
    instructions.append(new Instruction(2, "10101100", Instruction::JNZ, "jd a"));
    instructions.append(new Instruction(2, "10110000", Instruction::JC,  "jc a"));
    instructions.append(new Instruction(2, "10111100", Instruction::JB,  "jb a"));
    instructions.append(new Instruction(1, "11100000", Instruction::SHR, "shr"));
    instructions.append(new Instruction(1, "11100001", Instruction::SHL, "shl"));
    instructions.append(new Instruction(1, "11100010", Instruction::ROR, "ror"));
    instructions.append(new Instruction(1, "11100011", Instruction::ROL, "rol"));
    instructions.append(new Instruction(1, "1111....", Instruction::HLT, "hlt"));



    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////

    addressingModes.append(new AddressingMode("........", AddressingMode::DIRECT, AddressingMode::NO_PATTERN));
}
