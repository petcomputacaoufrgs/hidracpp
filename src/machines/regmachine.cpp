#include "regmachine.h"

RegMachine::RegMachine()
{
    identifier = "REG";

    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    for (int registerId = 0; registerId < 64; registerId++)
    {
        QString bitPattern = Conversion::valueToString(registerId).replace(0, 2, ".."); // "..000000" to "..111111"
        registers.append(new Register("R" + QString::number(registerId), bitPattern, 8));
    }

    registers.append(new Register("PC", "", 8));
    PC = registers.last();



    //////////////////////////////////////////////////
    // Initialize memory
    //////////////////////////////////////////////////

    setMemorySize(256);



    //////////////////////////////////////////////////
    // Initialize instructions
    //////////////////////////////////////////////////

    instructions.append(new Instruction(1, "00......", Instruction::REG_INC, "inc r"));
    instructions.append(new Instruction(1, "01......", Instruction::REG_DEC, "dec r"));
    instructions.append(new Instruction(3, "10......", Instruction::REG_IF,  "if r a0 a1"));
    instructions.append(new Instruction(1, "11......", Instruction::HLT,     "hlt"));




    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////

    addressingModes.append(new AddressingMode("........", AddressingMode::DIRECT, AddressingMode::NO_PATTERN)); // Used for "if r a0 a1"
}


void RegMachine::executeInstruction(Instruction *&instruction, AddressingMode::AddressingModeCode, QString registerName, int immediateAddress)
{
    Instruction::InstructionCode instructionCode;
    instructionCode = (instruction) ? instruction->getInstructionCode() : Instruction::NOP;

    switch (instructionCode)
    {
    case Instruction::REG_INC:
        setRegisterValue(registerName, getRegisterValue(registerName) + 1);
        break;

    case Instruction::REG_DEC:
        setRegisterValue(registerName, getRegisterValue(registerName) - 1);
        break;

    case Instruction::REG_IF:
        if (getRegisterValue(registerName) == 0)
            setPCValue(getMemoryValue(immediateAddress));
        else
            setPCValue(getMemoryValue(immediateAddress + 1));
        break;

    case Instruction::REG_HLT:
        setRunning(false);
        break;

    default:
        break;
    }
    instructionCount++;
}
