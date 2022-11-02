#include "cesarmachine.h"


CesarMachine::CesarMachine()
{
    identifier = "C16";
    fetchByteSize = 2;

    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    //Register bitpatterns are irrelevant; CESAR uses a different decoding method
    registers.append(new Register("R0", "", 16));
    registers.append(new Register("R1", "", 16));
    registers.append(new Register("R2", "", 16));
    registers.append(new Register("R3", "", 16));
    registers.append(new Register("R4", "", 16));
    registers.append(new Register("R5", "", 16));
    registers.append(new Register("SP", "", 16, false));
    registers.append(new Register("PC", "", 16, false));

    PC = registers.last();
    //////////////////////////////////////////////////
    // Initialize memory
    //////////////////////////////////////////////////

    setMemorySize(65536);

    //////////////////////////////////////////////////
    // Initialize flags
    //////////////////////////////////////////////////

    flags.append(new Flag(Flag::NEGATIVE, "N"));
    flags.append(new Flag(Flag::ZERO, "Z", true));
    flags.append(new Flag(Flag::CARRY, "C"));
    flags.append(new Flag(Flag::OVERFLOW_FLAG, "V"));

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

    addressingModes.append(new AddressingMode("........", AddressingMode::DIRECT, AddressingMode::NO_PATTERN));

}
