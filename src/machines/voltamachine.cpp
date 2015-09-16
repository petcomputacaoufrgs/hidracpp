#include "voltamachine.h"

VoltaMachine::VoltaMachine()
{
    identifier = "VLT"; // TODO: Confirmar identificador

    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    registers.append(new Register("PC", "", 8));
    registers.append(new Register("SP", "", 8));

    PC = registers.last();
    SP = registers.first();



    //////////////////////////////////////////////////
    // Initialize memory
    //////////////////////////////////////////////////

    setMemorySize(256);



    //////////////////////////////////////////////////
    // Initialize instructions
    //////////////////////////////////////////////////

    instructions.append(new Instruction(1, "00000000", Instruction::VOLTA_NOP, "nop"));
    instructions.append(new Instruction(1, "00010001", Instruction::VOLTA_ADD, "add"));
    instructions.append(new Instruction(1, "00010010", Instruction::VOLTA_SUB, "sub"));
    instructions.append(new Instruction(1, "00010011", Instruction::VOLTA_AND, "and"));
    instructions.append(new Instruction(1, "00010100", Instruction::VOLTA_OR,  "or"));
    instructions.append(new Instruction(1, "00100001", Instruction::VOLTA_CLR, "clr"));
    instructions.append(new Instruction(1, "00100010", Instruction::VOLTA_NOT, "not"));
    instructions.append(new Instruction(1, "00100011", Instruction::VOLTA_REG, "neg"));
    instructions.append(new Instruction(1, "00100100", Instruction::VOLTA_INC, "inc"));
    instructions.append(new Instruction(1, "00100101", Instruction::VOLTA_DEC, "dec"));
    instructions.append(new Instruction(1, "00110001", Instruction::VOLTA_ASR, "asr"));
    instructions.append(new Instruction(1, "00110010", Instruction::VOLTA_ASL, "asl"));
    instructions.append(new Instruction(1, "00110011", Instruction::VOLTA_ROR, "ror"));
    instructions.append(new Instruction(1, "00110100", Instruction::VOLTA_ROL, "rol"));
    instructions.append(new Instruction(1, "01000001", Instruction::VOLTA_SZ,  "sz"));
    instructions.append(new Instruction(1, "01000010", Instruction::VOLTA_SNZ, "snz"));
    instructions.append(new Instruction(1, "01000011", Instruction::VOLTA_SPL, "spl"));
    instructions.append(new Instruction(1, "01000100", Instruction::VOLTA_SMI, "smi"));
    instructions.append(new Instruction(1, "01000101", Instruction::VOLTA_SPZ, "spz"));
    instructions.append(new Instruction(1, "01000110", Instruction::VOLTA_SMZ, "smz"));
    instructions.append(new Instruction(1, "01010001", Instruction::VOLTA_SEQ, "seq"));
    instructions.append(new Instruction(1, "01010010", Instruction::VOLTA_SNE, "sne"));
    instructions.append(new Instruction(1, "01010011", Instruction::VOLTA_SGR, "sgr"));
    instructions.append(new Instruction(1, "01010100", Instruction::VOLTA_SLS, "sls"));
    instructions.append(new Instruction(1, "01010101", Instruction::VOLTA_SGE, "sge"));
    instructions.append(new Instruction(1, "01010110", Instruction::VOLTA_SLE, "sle"));
    instructions.append(new Instruction(1, "01100110", Instruction::VOLTA_RTS, "rts"));
    instructions.append(new Instruction(2, "011100..", Instruction::VOLTA_PSH, "psh a"));
    instructions.append(new Instruction(2, "011101..", Instruction::VOLTA_POP, "pop a")); // TODO: Código estava repetido, falar com o Weber
    instructions.append(new Instruction(2, "100100..", Instruction::VOLTA_JMP, "jmp a"));
    instructions.append(new Instruction(2, "101000..", Instruction::VOLTA_JSR, "jsr a"));
    instructions.append(new Instruction(1, "1111....", Instruction::VOLTA_HLT, "hlt"));


    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////

    addressingModes.append(new AddressingMode("......00", AddressingMode::DIRECT,       AddressingMode::NO_PATTERN));
    addressingModes.append(new AddressingMode("......01", AddressingMode::INDIRECT,     "(.*),i"));
    addressingModes.append(new AddressingMode("......10", AddressingMode::IMMEDIATE,    "#(.*)"));
    addressingModes.append(new AddressingMode("......11", AddressingMode::INDEXED_BY_PC, "(.*),pc"));
}
