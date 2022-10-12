#ifndef CESARMACHINE_H
#define CESARMACHINE_H

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
        int &addressingMode1,
        int &addressingMode2,  
        int &registerId1,
        int &registerId2,
        int &extraValues,
        int &immediateAddress);

    ///Execute the instruction
    void executeInstruction(Instruction *&instruction, 
        int addressingModeCode1,
        int addressingModeCode2, 
        int registerName1,
        int registerName2, 
        int extraValue,
        int immediateAddress);
    
    ///Extract addressing mode codes
    void extractAddressingModeCodes(int fetchedValue, AddressingMode::AddressingModeCode&);

};



#endif