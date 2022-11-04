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

    instructions.append(new Instruction(2, "1001....", Instruction::MOV,       "mov R1 R2", Instruction::GROUP_TWO_OPERAND));
    instructions.append(new Instruction(2, "1010....", Instruction::CESAR_ADD, "add R1 R2", Instruction::GROUP_TWO_OPERAND));
    instructions.append(new Instruction(2, "1011....", Instruction::CESAR_SUB, "sub R1 R2", Instruction::GROUP_TWO_OPERAND));
    instructions.append(new Instruction(2, "1100....", Instruction::CESAR_CMP, "cmp R1 R2", Instruction::GROUP_TWO_OPERAND));
    instructions.append(new Instruction(2, "1101....", Instruction::CESAR_AND, "and R1 R2", Instruction::GROUP_TWO_OPERAND));
    instructions.append(new Instruction(2, "1110....", Instruction::CESAR_OR,  "or R1 R2" , Instruction::GROUP_TWO_OPERAND));
    ///////////////////////////////////////////////////////////////////////
    //one operand instructions   //funcoes ainda nao feitas, arrumar grupos
    ///////////////////////////////////////////////////////////////////////
    instructions.append(new Instruction(1, "10000000", Instruction::CESAR_CLR, "clr R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(1, "10000001", Instruction::CESAR_NOT, "not R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(1, "10000010", Instruction::CESAR_INC, "inc R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(1, "10000011", Instruction::CESAR_DEC, "dec R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(1, "10000100", Instruction::CESAR_NEG, "neg R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(1, "10000101", Instruction::CESAR_TST, "tst R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(1, "10000110", Instruction::CESAR_ROR, "ror R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(1, "10000111", Instruction::CESAR_ROL, "rol R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(1, "10001000", Instruction::CESAR_ASR, "asr R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(1, "10001001", Instruction::CESAR_ASL, "asl R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(1, "10001010", Instruction::CESAR_ADC, "adc R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(1, "10001011", Instruction::CESAR_SDC, "sdc R1 ", Instruction::GROUP_ONE_OPERAND));
    //////////////////////////////////////////////////
    //flow control
    //////////////////////////////////////////////////
    instructions.append(new Instruction(2, "0100....", Instruction::CESAR_JMP, "jmp R1 ", Instruction::GROUP_JUMP));
    instructions.append(new Instruction(2, "00110000", Instruction::CESAR_BR,  "br R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110001", Instruction::CESAR_BNE, "bne R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110010", Instruction::CESAR_BEQ, "beq R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110100", Instruction::CESAR_BPL, "bpl R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110101", Instruction::CESAR_BVC, "bvc R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110110", Instruction::CESAR_BVS, "bvs R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110111", Instruction::CESAR_BCC, "bcc R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111000", Instruction::CESAR_BCS, "bcs R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111001", Instruction::CESAR_BGE, "bge R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111010", Instruction::CESAR_BLT, "blt R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111011", Instruction::CESAR_BGT, "bgt R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111100", Instruction::CESAR_BLE, "ble R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111101", Instruction::CESAR_BHI, "bhi R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111110", Instruction::CESAR_BLS, "bls R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "0110....", Instruction::CESAR_JSR, "jsr R1 ", Instruction::GROUP_JUMP_SUBROUTINE));
    instructions.append(new Instruction(2, "0111....", Instruction::CESAR_RTS, "rts R1 ", Instruction::GROUP_RETURN_SUBROUTINE));
    ////////////////////////
    //special instructions
    ////////////////////////
    instructions.append(new Instruction(2, "0001....", Instruction::CESAR_CCC, "ccc NZCV ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "0010....", Instruction::CESAR_SCC, "scc NZVC ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "0101....", Instruction::CESAR_SOB, "sob R1 ", Instruction::GROUP_ONE_OPERAND));

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

    case Instruction:: CESAR_SUB:
        src = GetCurrentOperandValue(1);   
        dst = GetCurrentOperandValue(2);
        result = dst - src;
        setRegisterValue(decodedRegisterCode2, result);
        updateFlags(result);
        break;

    case Instruction:: CESAR_ADD:
        src = GetCurrentOperandValue(1);   
        dst = GetCurrentOperandValue(2);
        result = dst + src;
        setRegisterValue(decodedRegisterCode2, result);
        updateFlags(result);
        break;

    case Instruction:: CESAR_CMP:
        src = GetCurrentOperandValue(1);   
        dst = GetCurrentOperandValue(2);
        result = dst + src;
        updateFlags(result);
        break;

    case Instruction::CESAR_AND:
        src = GetCurrentOperandValue(1);
        dst = GetCurrentOperandValue(2);
        result = src & dst;
        setRegisterValue(decodedRegisterCode2,result);
        updateFlags(result);
        break;

    case Instruction::CESAR_OR:
        src = GetCurrentOperandValue(1);
        dst = GetCurrentOperandValue(2);
        result = src | dst;
        setRegisterValue(decodedRegisterCode2,result);
        updateFlags(result);
        break;

    case Instruction:: CESAR_CLR:
        setRegisterValue(decodedRegisterCode1,0);
        updateFlags(0);
        break;

    case Instruction:: CESAR_NOT:
        src = getRegisterValue(decodedRegisterCode1);
        result = ~src & 0xFF;

        setRegisterValue(decodedRegisterCode1, result);
        updateFlags(result);
        break;

    case Instruction::CESAR_INC:
        src = GetCurrentOperandValue(1);
        result += src;
        if (decodedAddressingModeCode2 == AddressingMode::REGISTER)
            setRegisterValue(decodedRegisterCode1,result);
        else
            dst = GetCurrentOperandValue(2);
            memoryWriteTwoByte(dst, result);
        break;

    case Instruction::CESAR_DEC:
        src = GetCurrentOperandValue(1);
        result -= src;
        if (decodedAddressingModeCode2 == AddressingMode::REGISTER)
            setRegisterValue(decodedRegisterCode1,result);
        else
            dst = GetCurrentOperandValue(2);
            memoryWriteTwoByte(dst, result);
        break;

    case Instruction::CESAR_NEG:
        src = GetCurrentOperandValue(1);
        result = (-src) & 0xFF;

        setRegisterValue(src, result);
        updateFlags(result);
        break;

    case Instruction::CESAR_TST:
        updateFlags(getRegisterValue(decodedRegisterCode1));
        break;

    case Instruction:: CESAR_ROR:
        src = getRegisterValue(decodedRegisterCode1);
        result = ((src >> 1) | (getFlagValue("C") == true ? 0x80 : 0x00)) & 0xFF;
        setRegisterValue(decodedRegisterCode1,result);
        setCarry(src & 0b00000001);
        break;

    case Instruction:: CESAR_ROL:
        src = getRegisterValue(decodedRegisterCode1);
        result = ((src << 1) | (getFlagValue("C") == true ? 0x01 : 0x00)) & 0xFF;
        setRegisterValue(decodedRegisterCode1, result);
        setCarry((src & 0x80) ? 1 : 0);
        updateFlags(result);
        break;

    case Instruction::CESAR_ASL:
        src = getRegisterValue(decodedRegisterCode1);
        result = (src << 1);
        setRegisterValue(decodedRegisterCode1,result);
        break;
    
    case Instruction::CESAR_ASR:
        src = getRegisterValue(decodedRegisterCode1);
        result = (src >> 1);
        setRegisterValue(decodedRegisterCode1,result);
        break;

    case Instruction:: CESAR_ADC:
        int carry;
        src = GetCurrentOperandValue(1);
        carry = getFlagValue("C");
        src += carry;
        setRegisterValue(decodedRegisterCode1,src);
        break;

    case Instruction::CESAR_SDC:
        src = GetCurrentOperandValue(1);
        carry = getFlagValue("C");
        src -= carry;
        setRegisterValue(decodedRegisterCode1,src);
        break;

    case Instruction:: CESAR_JMP:
         if (!isImmediate) // Invalidate immediate jumps
            setPCValue(fetchedValue & 0b0000000011111111);
        break;

    case Instruction:: CESAR_BR:
        break;
        
    }

    //////////////////////////////////////////////////
    // Arithmetic and logic
    //////////////////////////////////////////////////

    
    
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
