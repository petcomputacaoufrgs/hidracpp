#include "ramsesmachine.h"

RamsesMachine::RamsesMachine()
{
    identifier = "RAD";

    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    registers.append(new Register( "A", "....00..", 8));
    registers.append(new Register( "B", "....01..", 8));
    registers.append(new Register( "X", "....10..", 8));
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
    flags.append(new Flag("C"));



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

    addressingModes.append(DIRECT);
    addressingModes.append(INDIRECT);
    addressingModes.append(IMMEDIATE);
    addressingModes.append(INDEXED);

    clearCounters();
    running = false;
}

Machine::AddressingMode RamsesMachine::extractAddressingMode(int byteArray[])
{
    switch (byteArray[0] & 0b00000011) // Extract last two bits
    {
        case 0x01:
            return AddressingMode::INDIRECT;

        case 0x02:
            return AddressingMode::IMMEDIATE;

        case 0x03:
            return AddressingMode::INDEXED;

        default:
            return AddressingMode::DIRECT;
    }
}

int RamsesMachine::extractRegisterId(int byteArray[])
{
    return ((byteArray[0] & 0b00001100) >> 2) % 3; // Bits 2 and 3 indicate register (A, B or X)
}

void RamsesMachine::setCarry(bool state)
{
    setFlagValue("C", state);
}

void RamsesMachine::setBorrowOrCarry(bool borrowState)
{
    setFlagValue("C", !borrowState); // Use carry as not borrow
}



