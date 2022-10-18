#ifndef CESARMACHINE_H
#define CESARMACHINE_H

#define CESAR_ADDRESSING_MODE_REG 0
#define CESAR_ADDRESSING_MODE_REG_POSTINC 1
#define CESAR_ADDRESSING_MODE_REG_PREDEC 2
#define CESAR_ADDRESSING_MODE_INDEX 3
#define CESAR_ADDRESSING_MODE_INDIRECT_REG 4
#define CESAR_ADDRESSING_MODE_INDIRECT_REG_POSTINC 5
#define CESAR_ADDRESSING_MODE_INDIRECT_REG_PREDEC 6
#define CESAR_ADDRESSING_MODE_INDIRECT_INDEX 7

#include "core/machine.h"

class CesarMachine : public Machine
{
    public:
    CesarMachine();

    void step();

    ///Get next instruction
    void fetchInstruction(int &fetchedValue, Instruction *&instruction);
    ///Decode the instruction
    void decodeInstruction(int fetchedValue, Instruction *&instruction, 
        AddressingMode::AddressingModeCode &addressingMode1,
        AddressingMode::AddressingModeCode &addressingMode2,  
        int &registerId1,
        int &registerId2,
        int &extraValues,
        int &immediateAddress);

    ///Execute the instruction
    void executeInstruction (Instruction *&instruction, 
        AddressingMode::AddressingModeCode addressingModeCode1,
        AddressingMode::AddressingModeCode addressingModeCode2, 
        int registerName1,
        int registerName2, 
        int extraValue,
        int immediateAddress);
    

    int memoryGetOperandAddress(int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode,  int registerName);
    int memoryGetOperandValue(int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode, int registerName);

    ///Necessary Conversion methods
    AddressingMode::AddressingModeCode convertInstructionStringAddressingMode(int extract_am);


};



#endif