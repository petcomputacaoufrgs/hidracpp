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

    private:
    // Internal class repre

    bool timerInterrupted;
    bool keyboardInterrupted;
    char keyboardInterruptionChar;

    // Wheter there was an 
    bool hasBeenInterrupted();
    // Updates

    public:
    CesarMachine();
    

    void step();

    void fetchInstruction();
    void decodeInstruction();
    void executeInstruction();

    void handleKeyPress(int key);
    
    int GetCurrentOperandValue(int operand = 1);
    int toSigned(int unsignedByte);
    
    void PutOnStack(int registerValue);
    void GetOffStack(int registerId);

    ///Necessary Conversion methods
    AddressingMode::AddressingModeCode convertInstructionStringAddressingMode(int extract_am);


};



#endif