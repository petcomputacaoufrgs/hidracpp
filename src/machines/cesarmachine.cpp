#include "cesarmachine.h"


CesarMachine::CesarMachine()
{
    identifier = "C16";

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


    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////


}
