#include "cesarmachine.h"



CesarMachine::CesarMachine()
{
    identifier = "C16";
    fetchByteSize = 2;

    littleEndian = false;

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

    instructions.append(new Instruction(2, "1001....", Instruction::MOV, "mov R1 R2", Instruction::GROUP_TWO_OPERAND));

    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////
    //to-do: check if is needed
    addressingModes.append(new AddressingMode("........", AddressingMode::DIRECT, AddressingMode::NO_PATTERN));

}


void CesarMachine::step()
{
    int fetchedValue, immediateAddress;
    int reg1, reg2;
    AddressingMode::AddressingModeCode am1, am2;
    int extra;
    Instruction *instruction = nullptr;

    fetchInstruction(fetchedValue, instruction);
    decodeInstruction(fetchedValue, instruction, am1, am2, reg1, reg2, extra, immediateAddress); // Outputs addressing mode, register and immediate address
    executeInstruction(instruction, am1, am2, reg1, reg2, extra, immediateAddress);

    if (getPCValue() == getBreakpoint())
        setRunning(false);    

}    



/*
Cesar's way to fetch, decode and execute instructions

-- Always fetches 2 bytes, even if the instruction only uses one

*/        

/*
bool CesarMachine::IsOneByteInstruction()
{

}*/

AddressingMode::AddressingModeCode CesarMachine::convertInstructionStringAddressingMode(int extract_am)
{
    switch(extract_am){
        case CESAR_ADDRESSING_MODE_REG:
            return AddressingMode::DIRECT;

        case CESAR_ADDRESSING_MODE_REG_POSTINC: 
            return AddressingMode::AFTER_INCREMENTED;

        case CESAR_ADDRESSING_MODE_REG_PREDEC:
            return AddressingMode::PRE_DECREMENTED;

        case CESAR_ADDRESSING_MODE_INDEX:
            return AddressingMode::INDEXED_BY_REG;

        case CESAR_ADDRESSING_MODE_INDIRECT_REG:
            return AddressingMode::INDIRECT_REGISTER;

        case CESAR_ADDRESSING_MODE_INDIRECT_REG_POSTINC:
            return AddressingMode::AFTER_INCREMENTED_INDIRECT;

        case CESAR_ADDRESSING_MODE_INDIRECT_REG_PREDEC:
            return AddressingMode::PRE_DECREMENTED_INDIRECT;

        case CESAR_ADDRESSING_MODE_INDIRECT_INDEX:
            return AddressingMode::INDIRECT_INDEXED_BY_REG;

        default:
            return AddressingMode::DIRECT; //Error

    }

}


void CesarMachine::fetchInstruction(int &fetchedValue, Instruction *&instruction)
{
    // Read first byte
    fetchedValue = memoryReadNext();  // has to read two bytes
    instruction = getInstructionFromValue(fetchedValue);
    if(instruction->getNumBytes() > 1){
        // Shift fetchedValue 1 byte to the left and read second byte using or mask
        fetchedValue = (fetchedValue << 8) | memoryReadNext();
    }
}    
    
void CesarMachine::decodeInstruction(int fetchedValue, Instruction *&instruction, 
        AddressingMode::AddressingModeCode &addressingMode1,
        AddressingMode::AddressingModeCode &addressingMode2, 
        int &registerId1,
        int &registerId2, 
        int &extraValues, 
        int &immediateAddress)
{

    static AddressingMode::AddressingModeCode default_am = AddressingMode::REGISTER;

    Instruction::InstructionGroup instructionGroup = instruction->getInstructionGroup();
    switch(instructionGroup){
        case Instruction::InstructionGroup::GROUP_NOP:
            addressingMode1 = default_am; //Adressing mode not used
            addressingMode2 = default_am; //
            registerId1 = 0;//Pass the first register, with no intention to use it
            registerId2 = 0;//
            extraValues = 0;// extraValues not used
            break;
        case Instruction::InstructionGroup::GROUP_CONDITIONAL_CODES:
            addressingMode1 = default_am; //Adressing mode not used
            addressingMode2 = default_am; //
            registerId1 = 0;//Pass the first register, with no intention to use it
            registerId2 = 0;//
            extraValues = ((1111 << 8) & fetchedValue) >> 8;//Mask to get flag values
            break; 

        case Instruction::InstructionGroup::GROUP_CONDITIONAL_BRANCHES:
            addressingMode1 = default_am; //Adressing mode not used
            addressingMode2 = default_am; //
            registerId1 = 0;//Pass the first register, with no intention to use it
            registerId2 = 0;//
            extraValues = fetchedValue & 0b11111111;//Mask to get branch adress to jump to
            break;

        case Instruction::InstructionGroup::GROUP_JUMP:
            addressingMode1 = convertInstructionStringAddressingMode((fetchedValue & 0b00111000) >> 3);
            addressingMode2 = default_am;
            registerId1 = (fetchedValue & 0b00000111);
            registerId2 = 0;
            break;
            
        case Instruction::InstructionGroup::GROUP_LOOP_CONTROL:
            addressingMode1 = default_am;//Adressing mode not used
            addressingMode2 = default_am;//
            registerId1 = (fetchedValue >> 8) & 0b111;//Shift 1 byte to the left and mask to get register
            registerId2 = 0;//Pass the second register, with no intention to use it
            extraValues = fetchedValue & 0b11111111;//Get displacement
            break;

        case Instruction::InstructionGroup::GROUP_JUMP_SUBROUTINE:
            addressingMode1 = convertInstructionStringAddressingMode((fetchedValue >> 3) & 0b111);//Get adressing mode
            addressingMode2 = default_am; //Adressing mode not used
            registerId1 = (fetchedValue >> 8) & 0b111;//Shift 1 byte to the left and mask to get register
            registerId2 = fetchedValue & 0b111;//mask to get register
            extraValues = 0;// extraValues not used
            break;

        case Instruction::InstructionGroup::GROUP_RETURN_SUBROUTINE:
            addressingMode1 = default_am;
            addressingMode2 = default_am;
            registerId1 = (fetchedValue  & 0b111);
            registerId2 = 0;
            extraValues = 0;
            break;

        case Instruction::InstructionGroup::GROUP_ONE_OPERAND:
            addressingMode1 = convertInstructionStringAddressingMode((fetchedValue >> 3) & 0b111);//Get adressing mode
            addressingMode2 = default_am;//Adressing mode not used
            registerId1 = (fetchedValue  & 0b111);//Mask to get register
            registerId2 = 0;//Pass the second register, with no intention to use it
            extraValues = 0;// extraValues not used
            break;

        case Instruction::InstructionGroup::GROUP_TWO_OPERAND:
            addressingMode1 = convertInstructionStringAddressingMode((fetchedValue >> 9) & 0b111);//
            addressingMode2 = convertInstructionStringAddressingMode((fetchedValue >> 3) & 0b111);
            registerId1 = (fetchedValue >> 6) & 0b111;
            registerId2 = (fetchedValue  & 0b111);
            break;
        /*
        case default:
            throw Instrução sem grupo n pode*/
    }


    //IF INSTRUCTION.GROUP = INSTRUCAO 1 PARAMETRO
    //Então o formato é cccc mmm aaa
    //IF OPCODE MATCHES 0001XXXX
    //IF OPECODE M

}
     
void CesarMachine::executeInstruction (Instruction *&instruction, 
        AddressingMode::AddressingModeCode addressingModeCode1,
        AddressingMode::AddressingModeCode addressingModeCode2, 
        int registerName1,
        int registerName2, 
        int extraValue,
        int immediateAddress){

    int reg_value1, reg_value2, src, dst, result;
    Instruction::InstructionCode instructionCode;
    instructionCode = (instruction) ? instruction->getInstructionCode() : Instruction::NOP;
    bool isImmediate = (addressingModeCode1 == AddressingMode::IMMEDIATE); // Used to invalidate immediate jumps

    switch (instructionCode)
    {

    //////////////////////////////////////////////////
    // Move
    //////////////////////////////////////////////////
    case Instruction::MOV:
        reg_value1 = getRegisterValue(registerName1);
        reg_value2 = getRegisterValue(registerName2);
        
        dst = memoryGetOperandValue(reg_value2, addressingModeCode2, registerName2);
        
        //Move the value from dst to a register
        if(addressingModeCode1 == AddressingMode::REGISTER){
            setRegisterValue(registerName1, dst);
        }
        //Move the value from dst to a memory address
        else{
            src = memoryGetOperandValue(reg_value1, addressingModeCode1, registerName1);
            setMemoryValue(src, dst);
        }
        break;



    //////////////////////////////////////////////////
    // Arithmetic and logic
    //////////////////////////////////////////////////

    /*case Instruction::ADD:
        value1 = getRegisterValue(registerName1);
        value2 = memoryGetOperandValue(immediateAddress, addressingModeCode1);
        result = (value1 + value2) & 0xFF;

        setRegisterValue(registerName, result);
        setCarry(value1 + value2 > 0xFF);
        setOverflow(toSigned(value1) + toSigned(value2) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction::OR:
        value1 = getRegisterValue(registerName);
        value2 = memoryGetOperandValue(immediateAddress, addressingModeCode);
        result = (value1 | value2);

        setRegisterValue(registerName, result);
        updateFlags(result);
        break;

    case Instruction::AND:
        value1 = getRegisterValue(registerName);
        value2 = memoryGetOperandValue(immediateAddress, addressingModeCode);
        result = (value1 & value2);

        setRegisterValue(registerName, result);
        updateFlags(result);
        break;

    case Instruction::NOT:
        value1 = getRegisterValue(registerName);
        result = ~value1 & 0xFF;

        setRegisterValue(registerName, result);
        updateFlags(result);
        break;

    case Instruction::SUB:
        value1 = getRegisterValue(registerName);
        value2 = memoryGetOperandValue(immediateAddress, addressingModeCode);
        result = (value1 - value2) & 0xFF;

        setRegisterValue(registerName, result);
        setBorrowOrCarry(value1 < value2);
        setOverflow(toSigned(value1) - toSigned(value2) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction::NEG:
        value1 = getRegisterValue(registerName);
        result = (-value1) & 0xFF;

        setRegisterValue(registerName, result);
        updateFlags(result);
        break;

    case Instruction::SHR:
        value1 = getRegisterValue(registerName);
        result = (value1 >> 1) & 0xFF; // Logical shift (unsigned)

        setRegisterValue(registerName, result);
        setCarry(value1 & 0x01);
        updateFlags(result);
        break;

    case Instruction::SHL:
        value1 = getRegisterValue(registerName);
        result = (value1 << 1) & 0xFF;

        setRegisterValue(registerName, result);
        setCarry((value1 & 0x80) ? 1 : 0);
        updateFlags(result);
        break;

    case Instruction::ROR:
        value1 = getRegisterValue(registerName);
        result = ((value1 >> 1) | (getFlagValue("C") == true ? 0x80 : 0x00)) & 0xFF;

        setRegisterValue(registerName, result);
        setCarry(value1 & 0x01);
        updateFlags(result);
        break;

    case Instruction::ROL:
        value1 = getRegisterValue(registerName);
        result = ((value1 << 1) | (getFlagValue("C") == true ? 0x01 : 0x00)) & 0xFF;

        setRegisterValue(registerName, result);
        setCarry((value1 & 0x80) ? 1 : 0);
        updateFlags(result);
        break;

    case Instruction::INC:
        setRegisterValue(registerName, getRegisterValue(registerName) + 1);
        break;

    case Instruction::DEC:
        setRegisterValue(registerName, getRegisterValue(registerName) - 1);
        break;



    //////////////////////////////////////////////////
    // Jumps
    //////////////////////////////////////////////////

    case Instruction::JMP:
        if (!isImmediate) // Invalidate immediate jumps
            setPCValue(memoryGetJumpAddress(immediateAddress, addressingModeCode));
        break;

    case Instruction::JN:
        if (getFlagValue("N") == true && !isImmediate)
            setPCValue(memoryGetJumpAddress(immediateAddress, addressingModeCode));
        break;

    case Instruction::JP:
        if (getFlagValue("N") == false && !isImmediate)
            setPCValue(memoryGetJumpAddress(immediateAddress, addressingModeCode));
        break;

    case Instruction::JV:
        if (getFlagValue("V") == true && !isImmediate)
            setPCValue(memoryGetJumpAddress(immediateAddress, addressingModeCode));
        break;

    case Instruction::JNV:
        if (getFlagValue("V") == false && !isImmediate)
            setPCValue(memoryGetJumpAddress(immediateAddress, addressingModeCode));
        break;

    case Instruction::JZ:
        if (getFlagValue("Z") == true && !isImmediate)
            setPCValue(memoryGetJumpAddress(immediateAddress, addressingModeCode));
        break;

    case Instruction::JNZ:
        if (getFlagValue("Z") == false && !isImmediate)
            setPCValue(memoryGetJumpAddress(immediateAddress, addressingModeCode));
        break;

    case Instruction::JC:
        if (getFlagValue("C") == true && !isImmediate)
            setPCValue(memoryGetJumpAddress(immediateAddress, addressingModeCode));
        break;

    case Instruction::JNC:
        if (getFlagValue("C") == false && !isImmediate)
            setPCValue(memoryGetJumpAddress(immediateAddress, addressingModeCode));
        break;

    case Instruction::JB:
        if (getFlagValue("B") == true && !isImmediate)
            setPCValue(memoryGetJumpAddress(immediateAddress, addressingModeCode));
        break;

    case Instruction::JNB:
        if (getFlagValue("B") == false && !isImmediate)
            setPCValue(memoryGetJumpAddress(immediateAddress, addressingModeCode));
        break;

    case Instruction::JSR:
        if (!isImmediate)
        {
            int jumpAddress = memoryGetJumpAddress(immediateAddress, addressingModeCode);
            memoryWrite(jumpAddress, getPCValue());
            setPCValue(jumpAddress+1);
        }
        break;

    case Instruction::REG_IF:
        if (getRegisterValue(registerName) == 0)
            setPCValue(getMemoryValue(immediateAddress));
        else
            setPCValue(getMemoryValue(immediateAddress + 1));
        break;



    //////////////////////////////////////////////////
    // Halt
    //////////////////////////////////////////////////

    case Instruction::HLT:
        setRunning(false);
        break;

    default: // NOP etc.
        break;
    */
    }
    instructionCount++;
}


    

// void CesarMachine::extractAddressingModeCodes(int fetchedValue, AddressingMode::AddressingModeCode&)
// {

// }

//Returns an address       
int CesarMachine::memoryGetOperandAddress(int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode,  int registerName)
{
    int registerValue = getRegisterValue(registerName);
    int r_value;

    switch (addressingModeCode)
    {
        //REGISTER
        case AddressingMode::REGISTER:
            return registerValue;

        //POST INCREMENTED
        case AddressingMode::AFTER_INCREMENTED:
            // Get register value
            registerValue = getRegisterValue(registerName);
            // Read 2 bytes
            r_value = registerValue;
            // Increment register value by 2
            registerValue += 2;

            setRegisterValue(registerName, registerValue);
            return r_value;

        //PRE DECREMENTED
        case AddressingMode::PRE_DECREMENTED:
            // Get register value
            registerValue = getRegisterValue(registerName);
            // Decrement registerValue by 2
            registerValue -= 2;

            setRegisterValue(registerName,registerValue);
            r_value = registerValue;
            return r_value;

        //INDEXED
        case AddressingMode::INDEXED_BY_REG:
            // Register value + value in next byte
            r_value = memoryReadTwoByteAddress(getPCValue()) + getRegisterValue(registerName);
            // Increment PC by 2 bytes
            incrementPCValue(2);
        
            return r_value;

        //INDIRECT REGISTER
        case AddressingMode::INDIRECT_REGISTER:
            return memoryRead(registerValue);

        //INDIRECT POST INCREMENT
        case AddressingMode::AFTER_INCREMENTED_INDIRECT:
             // Read 2 bytes
             r_value = memoryReadTwoByteAddress(registerValue);
             // Increment registerValue by 2
             registerValue += 2;
             
             setRegisterValue(registerName,registerValue);
             return r_value;

        //INDIRECT PRE DECREMENT
        case AddressingMode::PRE_DECREMENTED_INDIRECT:
            // Get register value
            registerValue = getRegisterValue(registerName);
            // Decrement registerValue by 2
            registerValue -= 2;
    
            r_value = memoryReadTwoByteAddress(registerValue);
            return r_value;
        
        //INDIRECT INDEXED

        case AddressingMode::INDIRECT_INDEXED_BY_REG:
            // Register value + value in next byte
            r_value = memoryReadTwoByteAddress(getPCValue()) + getRegisterValue(registerName);
            // Increment PC by 2 bytes
            incrementPCValue(2);
        
            return memoryReadTwoByteAddress(r_value);
        
        default:
            return 0;
    }
}

int CesarMachine::memoryGetOperandValue(int immediateAddress, AddressingMode::AddressingModeCode addressingModeCode, int registerName)
{
    //When getting an operand value from a register using the "register" addressing mode,
    //we don't need to access memory.
    //Ex: MOV R5 R6 places the values of R5 in R6
    //However, were R5 to be in any other addressing mode, we'd use the contents of R5
    //as an address
    if(addressingModeCode == AddressingMode::REGISTER){
        return memoryGetOperandAddress(immediateAddress, addressingModeCode, registerName)
    }
    else{
        return memoryReadTwoByteAddress(memoryGetOperandAddress(immediateAddress, addressingModeCode, registerName));
    }

}
