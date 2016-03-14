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

    registers.append(new Register("PC", "", 8, false));
    PC = registers.last();



    //////////////////////////////////////////////////
    // Initialize memory
    //////////////////////////////////////////////////

    setMemorySize(256);



    //////////////////////////////////////////////////
    // Initialize instructions
    //////////////////////////////////////////////////

    instructions.append(new Instruction(1, "00......", Instruction::INC,    "inc r"));
    instructions.append(new Instruction(1, "01......", Instruction::DEC,    "dec r"));
    instructions.append(new Instruction(3, "10......", Instruction::REG_IF, "if r a0 a1"));
    instructions.append(new Instruction(1, "11......", Instruction::HLT,    "hlt"));



    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////

    addressingModes.append(new AddressingMode("........", AddressingMode::DIRECT, AddressingMode::NO_PATTERN)); // Used for "if r a0 a1"
}
