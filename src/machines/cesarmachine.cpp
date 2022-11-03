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
            return AddressingMode::REGISTER;

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


void CesarMachine::fetchInstruction()
{
    // Read first byte
    
    fetchedValue = memoryReadNext();  // has to read two bytes
    currentInstruction = getInstructionFromValue(fetchedValue);
    if(currentInstruction->getNumBytes() > 1){
        // Shift fetchedValue 1 byte to the left and read second byte using or mask
        fetchedValue = (fetchedValue << 8) | memoryReadNext();
    }
}    
    
void CesarMachine::decodeInstruction()
{

    static AddressingMode::AddressingModeCode default_am = AddressingMode::REGISTER;

    Instruction::InstructionGroup instructionGroup = currentInstruction->getInstructionGroup();
    switch(instructionGroup){
        case Instruction::InstructionGroup::GROUP_NOP:
            decodedAddressingModeCode1 = default_am; //Adressing mode not used
            decodedAddressingModeCode2 = default_am; //
            decodedRegisterCode1 = 0;//Pass the first register, with no intention to use it
            decodedRegisterCode2 = 0;//
            decodedExtraValue = 0;// decodedExtraValue not used
            break;
        case Instruction::InstructionGroup::GROUP_CONDITIONAL_CODES:
            decodedAddressingModeCode1 = default_am; //Adressing mode not used
            decodedAddressingModeCode2 = default_am; //
            decodedRegisterCode1 = 0;//Pass the first register, with no intention to use it
            decodedRegisterCode2 = 0;//
            decodedExtraValue = ((1111 << 8) & fetchedValue) >> 8;//Mask to get flag values
            break; 

        case Instruction::InstructionGroup::GROUP_CONDITIONAL_BRANCHES:
            decodedAddressingModeCode1 = default_am; //Adressing mode not used
            decodedAddressingModeCode2 = default_am; //
            decodedRegisterCode1 = 0;//Pass the first register, with no intention to use it
            decodedRegisterCode2 = 0;//
            decodedExtraValue = fetchedValue & 0b11111111;//Mask to get branch adress to jump to
            break;

        case Instruction::InstructionGroup::GROUP_JUMP:
            decodedAddressingModeCode1 = convertInstructionStringAddressingMode((fetchedValue & 0b00111000) >> 3);
            decodedAddressingModeCode2 = default_am;
            decodedRegisterCode1 = (fetchedValue & 0b00000111);
            decodedRegisterCode2 = 0;
            break;
            
        case Instruction::InstructionGroup::GROUP_LOOP_CONTROL:
            decodedAddressingModeCode1 = default_am;//Adressing mode not used
            decodedAddressingModeCode2 = default_am;//
            decodedRegisterCode1 = (fetchedValue >> 8) & 0b111;//Shift 1 byte to the left and mask to get register
            decodedRegisterCode2 = 0;//Pass the second register, with no intention to use it
            decodedExtraValue = fetchedValue & 0b11111111;//Get displacement
            break;

        case Instruction::InstructionGroup::GROUP_JUMP_SUBROUTINE:
            decodedAddressingModeCode1 = convertInstructionStringAddressingMode((fetchedValue >> 3) & 0b111);//Get adressing mode
            decodedAddressingModeCode2 = default_am; //Adressing mode not used
            decodedRegisterCode1 = (fetchedValue >> 8) & 0b111;//Shift 1 byte to the left and mask to get register
            decodedRegisterCode2 = fetchedValue & 0b111;//mask to get register
            decodedExtraValue = 0;// decodedExtraValue not used
            break;

        case Instruction::InstructionGroup::GROUP_RETURN_SUBROUTINE:
            decodedAddressingModeCode1 = default_am;
            decodedAddressingModeCode2 = default_am;
            decodedRegisterCode1 = (fetchedValue  & 0b111);
            decodedRegisterCode2 = 0;
            decodedExtraValue = 0;
            break;

        case Instruction::InstructionGroup::GROUP_ONE_OPERAND:
            decodedAddressingModeCode1 = convertInstructionStringAddressingMode((fetchedValue >> 3) & 0b111);//Get adressing mode
            decodedAddressingModeCode2 = default_am;//Adressing mode not used
            decodedRegisterCode1 = (fetchedValue  & 0b111);//Mask to get register
            decodedRegisterCode2 = 0;//Pass the second register, with no intention to use it
            decodedExtraValue = 0;// decodedExtraValue not used
            break;

        case Instruction::InstructionGroup::GROUP_TWO_OPERAND:
            decodedAddressingModeCode1 = convertInstructionStringAddressingMode((fetchedValue >> 9) & 0b111);//
            decodedAddressingModeCode2 = convertInstructionStringAddressingMode((fetchedValue >> 3) & 0b111);
            decodedRegisterCode1 = (fetchedValue >> 6) & 0b111;
            decodedRegisterCode2 = (fetchedValue & 0b111);
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
     
void CesarMachine::executeInstruction(){

    int reg_value1, reg_value2, src, dst, result;
    Instruction::InstructionCode instructionCode;
    instructionCode = (currentInstruction) ? currentInstruction->getInstructionCode() : Instruction::NOP;
    bool isImmediate = (decodedAddressingModeCode1 == AddressingMode::IMMEDIATE); // Used to invalidate immediate jumps

    switch (instructionCode)
    {

    //////////////////////////////////////////////////
    // Move
    //////////////////////////////////////////////////
    case Instruction::MOV:
        src = GetCurrentOperandValue(1);
        if(decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode2, src);
        }
        else
        {
            dst = GetCurrentOperandValue(2);
            memoryWriteTwoByte(dst, src);
        }
        break;

    //////////////////////////////////////////////////
    // Arithmetic and logic
    //////////////////////////////////////////////////

    /*case Instruction::ADD:
        value1 = getRegisterValue(decodedRegisterCode1);
        value2 = GetCurrentOperandValue(immediateAddress, addressingModeCode1);
        result = (value1 + value2) & 0xFF;

        setRegisterValue(registerCode, result);
        setCarry(value1 + value2 > 0xFF);
        setOverflow(toSigned(value1) + toSigned(value2) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction::OR:
        value1 = getRegisterValue(registerCode);
        value2 = GetCurrentOperandValue(immediateAddress, addressingModeCode);
        result = (value1 | value2);

        setRegisterValue(registerCode, result);
        updateFlags(result);
        break;

    case Instruction::AND:
        value1 = getRegisterValue(registerCode);
        value2 = GetCurrentOperandValue(immediateAddress, addressingModeCode);
        result = (value1 & value2);

        setRegisterValue(registerCode, result);
        updateFlags(result);
        break;

    case Instruction::NOT:
        value1 = getRegisterValue(registerCode);
        result = ~value1 & 0xFF;

        setRegisterValue(registerCode, result);
        updateFlags(result);
        break;

    case Instruction::SUB:
        value1 = getRegisterValue(registerCode);
        value2 = GetCurrentOperandValue(immediateAddress, addressingModeCode);
        result = (value1 - value2) & 0xFF;

        setRegisterValue(registerCode, result);
        setBorrowOrCarry(value1 < value2);
        setOverflow(toSigned(value1) - toSigned(value2) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction::NEG:
        value1 = getRegisterValue(registerCode);
        result = (-value1) & 0xFF;

        setRegisterValue(registerCode, result);
        updateFlags(result);
        break;

    case Instruction::SHR:
        value1 = getRegisterValue(registerCode);
        result = (value1 >> 1) & 0xFF; // Logical shift (unsigned)

        setRegisterValue(registerCode, result);
        setCarry(value1 & 0x01);
        updateFlags(result);
        break;

    case Instruction::SHL:
        value1 = getRegisterValue(registerCode);
        result = (value1 << 1) & 0xFF;

        setRegisterValue(registerCode, result);
        setCarry((value1 & 0x80) ? 1 : 0);
        updateFlags(result);
        break;

    case Instruction::ROR:
        value1 = getRegisterValue(registerCode);
        result = ((value1 >> 1) | (getFlagValue("C") == true ? 0x80 : 0x00)) & 0xFF;

        setRegisterValue(registerCode, result);
        setCarry(value1 & 0x01);
        updateFlags(result);
        break;

    case Instruction::ROL:
        value1 = getRegisterValue(registerCode);
        result = ((value1 << 1) | (getFlagValue("C") == true ? 0x01 : 0x00)) & 0xFF;

        setRegisterValue(registerCode, result);
        setCarry((value1 & 0x80) ? 1 : 0);
        updateFlags(result);
        break;

    case Instruction::INC:
        setRegisterValue(registerCode, getRegisterValue(registerCode) + 1);
        break;

    case Instruction::DEC:
        setRegisterValue(registerCode, getRegisterValue(registerCode) - 1);
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
        if (getRegisterValue(registerCode) == 0)
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
  
int CesarMachine::GetCurrentOperandValue(int operand)
{
    int registerValue;
    int registerCode = (operand == 1 ? decodedRegisterCode1 : decodedRegisterCode2);
    int addressingModeCode = (operand == 1 ? decodedAddressingModeCode1 : decodedAddressingModeCode2);
    int r_value;

    switch (addressingModeCode)
    {
        //REGISTER
        case AddressingMode::REGISTER:
            registerValue = getRegisterValue(registerCode);
            return registerValue;

        //POST INCREMENTED
        case AddressingMode::AFTER_INCREMENTED:
            // Get register value
            registerValue = getRegisterValue(registerCode);
            // Read 2 bytes
            r_value = registerValue;
            // Increment register value by 2
            registerValue += 2;

            setRegisterValue(registerCode, registerValue);
            return r_value;

        //PRE DECREMENTED
        case AddressingMode::PRE_DECREMENTED:
            // Get register value
            registerValue = getRegisterValue(registerCode);
            // Decrement registerValue by 2
            registerValue -= 2;

            setRegisterValue(registerCode,registerValue);
            r_value = registerValue;
            return r_value;

        //INDEXED
        case AddressingMode::INDEXED_BY_REG:
            // Register value + value in next byte
            r_value = memoryReadTwoByteAddress(getPCValue()) + getRegisterValue(registerCode);
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
             
             setRegisterValue(registerCode,registerValue);
             return r_value;

        //INDIRECT PRE DECREMENT
        case AddressingMode::PRE_DECREMENTED_INDIRECT:
            // Get register value
            registerValue = getRegisterValue(registerCode);
            // Decrement registerValue by 2
            registerValue -= 2;
    
            r_value = memoryReadTwoByteAddress(registerValue);
            return r_value;
        
        //INDIRECT INDEXED

        case AddressingMode::INDIRECT_INDEXED_BY_REG:
            // Register value + value in next byte
            r_value = memoryReadTwoByteAddress(getPCValue()) + getRegisterValue(registerCode);
            // Increment PC by 2 bytes
            incrementPCValue(2);
        
            return memoryReadTwoByteAddress(r_value);
        
        default:
            return 0;
    }
}
