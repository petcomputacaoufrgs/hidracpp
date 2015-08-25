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

    //instructions.append(new Instruction(1, "00000000", Instruction::VOLTA_NOP, "nop"));




    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////

    addressingModes.append(new AddressingMode("......00", AddressingMode::DIRECT,       AddressingMode::NO_PATTERN));
    addressingModes.append(new AddressingMode("......01", AddressingMode::INDIRECT,     "(.*),i"));
    addressingModes.append(new AddressingMode("......10", AddressingMode::IMMEDIATE,    "#(.*)"));
    addressingModes.append(new AddressingMode("......11", AddressingMode::INDEXED_BY_PC, "(.*),pc"));
}
