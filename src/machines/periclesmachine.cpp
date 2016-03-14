#include "periclesmachine.h"

PericlesMachine::PericlesMachine()
{
    identifier = "PRC";

    littleEndian = true;

    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    registers.append(new Register( "A", "....00..", 8));
    registers.append(new Register( "B", "....01..", 8));
    registers.append(new Register( "X", "....10..", 8));
    registers.append(new Register("PC", "", 12, false));

    PC = registers.last();



    //////////////////////////////////////////////////
    // Initialize memory
    //////////////////////////////////////////////////

    setMemorySize(4096);



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
    instructions.append(new Instruction(0, "0001....", Instruction::STR, "str r a"));
    instructions.append(new Instruction(0, "0010....", Instruction::LDR, "ldr r a"));
    instructions.append(new Instruction(0, "0011....", Instruction::ADD, "add r a"));
    instructions.append(new Instruction(0, "0100....", Instruction::OR,  "or r a"));
    instructions.append(new Instruction(0, "0101....", Instruction::AND, "and r a"));
    instructions.append(new Instruction(1, "0110....", Instruction::NOT, "not r"));
    instructions.append(new Instruction(0, "0111....", Instruction::SUB, "sub r a"));
    instructions.append(new Instruction(0, "1000....", Instruction::JMP, "jmp a"));
    instructions.append(new Instruction(0, "1001....", Instruction::JN,  "jn a"));
    instructions.append(new Instruction(0, "1010....", Instruction::JZ,  "jz a"));
    instructions.append(new Instruction(0, "1011....", Instruction::JC,  "jc a"));
    instructions.append(new Instruction(0, "1100....", Instruction::JSR, "jsr a"));
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

// Returns number of bytes reserved
int PericlesMachine::calculateBytesToReserve(QString addressArgument)
{
    AddressingMode::AddressingModeCode addressingModeCode;
    extractArgumentAddressingModeCode(addressArgument, addressingModeCode);
    return (addressingModeCode == AddressingMode::IMMEDIATE) ? 2 : 3; // Immediate requires only 2 bytes
}

void PericlesMachine::decodeInstruction(int fetchedValue, Instruction *&instruction, AddressingMode::AddressingModeCode &addressingModeCode, QString &registerName, int &immediateAddress)
{
    addressingModeCode = extractAddressingModeCode(fetchedValue);
    registerName = extractRegisterName(fetchedValue);

    if (instruction && instruction->getNumBytes() == 0) // If instruction has variable number of bytes
    {
        immediateAddress = getPCValue(); // Address that contains first argument byte

        // Skip argument bytes
        incrementPCValue();
        if (addressingModeCode != AddressingMode::IMMEDIATE) // Immediate argument has only 1 byte
            incrementPCValue();
    }
}

int PericlesMachine::memoryGetOperandAddress(int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode)
{
    switch (addressingModeCode)
    {
        case AddressingMode::DIRECT:
            return memoryReadTwoByteAddress(immediateAddress);

        case AddressingMode::INDIRECT:
            return memoryReadTwoByteAddress(memoryReadTwoByteAddress(immediateAddress));

        case AddressingMode::IMMEDIATE:
            return immediateAddress;

        case AddressingMode::INDEXED_BY_X:
            return address(memoryReadTwoByteAddress(immediateAddress) + getRegisterValue("X"));

        default:
            return 0;
    }
}

void PericlesMachine::getNextOperandAddress(int &intermediateAddress, int &intermediateAddress2, int &finalOperandAddress)
{
    int fetchedValue = getMemoryValue(PC->getValue());
    Instruction *instruction = getInstructionFromValue(fetchedValue);
    AddressingMode::AddressingModeCode addressingModeCode = extractAddressingModeCode(fetchedValue);
    int immediateAddress;

    intermediateAddress  = -1;
    intermediateAddress2 = -1;
    finalOperandAddress  = -1;

    if (!instruction || instruction->getNumBytes() != 0)
        return;

    immediateAddress = PC->getValue() + 1;

    switch (addressingModeCode)
    {
        case AddressingMode::DIRECT:
            finalOperandAddress = getMemoryTwoByteAddress(immediateAddress);
            break;

        case AddressingMode::INDIRECT:
            intermediateAddress  = getMemoryTwoByteAddress(immediateAddress);
            intermediateAddress2 = address(intermediateAddress + 1); // Second byte
            finalOperandAddress  = getMemoryTwoByteAddress(intermediateAddress);
            break;

        case AddressingMode::IMMEDIATE:
            finalOperandAddress = immediateAddress;
            break;

        case AddressingMode::INDEXED_BY_X:
            finalOperandAddress = address(getMemoryTwoByteAddress(immediateAddress) + getRegisterValue("X"));
            break;

        case AddressingMode::INDEXED_BY_PC:
            break;
    }
}

int PericlesMachine::memoryReadTwoByteAddress(int address)
{
    return memoryRead(address) + (memoryRead(address + 1) << 8);
}

int PericlesMachine::getMemoryTwoByteAddress(int address)
{
    return getMemoryValue(address) + (getMemoryValue(address + 1) << 8);
}
