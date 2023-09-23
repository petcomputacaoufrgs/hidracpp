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


    public:
    CesarMachine();
    

    void step();

    void fetchInstruction();
    void decodeInstruction();
    void executeInstruction();

    void handleKeyPress(int key);
    void handleTimerEvent();
    
    void PutOnStack(int registerValue);
    void GetOffStack(int registerId);
    
    // Calculates the address and returns the value it holds
    int GetCurrentOperandValue(int operand = 1);
    // Calculates the address and returns it
    int GetCurrentOperandAddress(int operand = 1);
    
    int toSigned(int unsignedByte);

    void translateLabelToValue(QString& argument);
    int calculateBytesToReserve(const Instruction* instruction, QStringList const& arguments);
    void buildInstruction(Instruction* instruction, QStringList argumentList);
    
    QString getCurrentInstructionString(int &argumentBytes);
    void wrapAddressingModeToRegister(AddressingMode::AddressingModeCode const& am, QString& registerString, int extraValue);

    ///Necessary Conversion methods
    AddressingMode::AddressingModeCode convertInstructionStringAddressingMode(int extract_am);
    void extractInstructionRegisterParameter(QString& param, int& reg_code, int& am_code, int& am_offset);

    int getTimerValue();
};

#endif