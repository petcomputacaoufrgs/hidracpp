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
    instructions.append(new Instruction(2, "10000000", Instruction::CESAR_CLR, "clr R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000001", Instruction::CESAR_NOT, "not R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000010", Instruction::CESAR_INC, "inc R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000011", Instruction::CESAR_DEC, "dec R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000100", Instruction::CESAR_NEG, "neg R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000101", Instruction::CESAR_TST, "tst R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000110", Instruction::CESAR_ROR, "ror R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000111", Instruction::CESAR_ROL, "rol R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10001000", Instruction::CESAR_ASR, "asr R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10001001", Instruction::CESAR_ASL, "asl R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10001010", Instruction::CESAR_ADC, "adc R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10001011", Instruction::CESAR_SBC, "sbc R1 ", Instruction::GROUP_ONE_OPERAND));
    //////////////////////////////////////////////////
    //flow control
    //////////////////////////////////////////////////
    instructions.append(new Instruction(2, "0100....", Instruction::CESAR_JMP, "jmp R1 ", Instruction::GROUP_JUMP));
    instructions.append(new Instruction(2, "00110000", Instruction::CESAR_BR,  "br R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110001", Instruction::CESAR_BNE, "bne R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110010", Instruction::CESAR_BEQ, "beq R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110011", Instruction::CESAR_BPL, "bpl R1 ", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110100", Instruction::CESAR_BMI, "bmi R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
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
    instructions.append(new Instruction(1, "0000....", Instruction::CESAR_NOP, "nop", Instruction::GROUP_NOP));
    instructions.append(new Instruction(1, "0001....", Instruction::CESAR_CCC, "ccc NZCV ", Instruction::GROUP_CONDITIONAL_CODES));
    instructions.append(new Instruction(1, "0010....", Instruction::CESAR_SCC, "scc NZVC ", Instruction::GROUP_CONDITIONAL_CODES));
    instructions.append(new Instruction(2, "0101....", Instruction::CESAR_SOB, "sob R1 ", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(1, "1111....", Instruction::CESAR_HLT, "hlt", Instruction::GROUP_NOP));

    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////
    //to-do: check if is needed
    addressingModes.append(new AddressingMode("........", AddressingMode::DIRECT, AddressingMode::NO_PATTERN));

}


int CesarMachine::toSigned(int unsignedByte)
{
    if (unsignedByte <= 32767) // Max signed byte
        return unsignedByte;
    else
        return unsignedByte - 65536;
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
            decodedExtraValue = (0b1111  & fetchedValue) ;//Mask to get flag values
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
    int carry, overflow, zero, negative;
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
            updateFlags(src);
        }
        else
        {
            dst = GetCurrentOperandValue(2);
            memoryWriteTwoByte(dst, src);
            updateFlags(dst);
        }

        setOverflow(false);
        
        break;

    case Instruction:: CESAR_SUB:
        src = GetCurrentOperandValue(1);   
        dst = GetCurrentOperandValue(2);
        result = (src - dst) & 0xFFFF;
        if(decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode2, result);
        }
        else
        {
            memoryWriteTwoByte(dst, result);
        }
        // REMEMBER TO REVIEW THE LOGIC BEHIND IT
        setCarry(toSigned(src) < toSigned(dst));
        setOverflow(toSigned(src) + toSigned(-dst) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction:: CESAR_ADD:
        src = GetCurrentOperandValue(1);   
        dst = GetCurrentOperandValue(2);
        result = (dst + src) & 0xFFFF;
        if(decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode2, result);
        }
        else
        {
            memoryWriteTwoByte(dst, result);
        }

        setCarry(src + dst > 0xFFFF);
        setOverflow(toSigned(src) + toSigned(dst) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction:: CESAR_CMP:
        src = GetCurrentOperandValue(1);   
        dst = GetCurrentOperandValue(2);
        result = src - dst;

        setCarry(toSigned(src) < toSigned(dst));
        setOverflow(toSigned(src) + toSigned(-dst) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction::CESAR_AND:
        src = GetCurrentOperandValue(1);
        dst = GetCurrentOperandValue(2);
        result = src & dst;
        if(decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode2, result);
        }
        else
        {
            memoryWriteTwoByte(dst, result);
        }

        setOverflow(false);
        updateFlags(result);
        break;

    case Instruction::CESAR_OR:
        src = GetCurrentOperandValue(1);
        dst = GetCurrentOperandValue(2);
        result = (src | dst) & 0xFFFF;
        if(decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode2, result);
        }
        else
        {
            memoryWriteTwoByte(dst, result);
        }
        
        setOverflow(false);
        updateFlags(result);
        break;

    case Instruction:: CESAR_CLR:
        setRegisterValue(decodedRegisterCode1,0);
        updateFlags(0);
        setOverflow(false);
        setCarry(false);
        break;

    case Instruction:: CESAR_NOT:
        src = getRegisterValue(decodedRegisterCode1);
        result = ~src & 0xFF;

        setRegisterValue(decodedRegisterCode1, result);
        setCarry(true);
        setOverflow(false);
        updateFlags(result);
        break;

    case Instruction::CESAR_INC:
        src = GetCurrentOperandValue(1);
        result = (src + 1) & 0xFFFF;
        if (decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode1, result);
        }
        else
        {
            memoryWriteTwoByte(src, result);
        }
        setCarry((src + 1) > 0xFFFF);
        setOverflow(src == 0x7FFF);
        updateFlags(result);
        break;
        
    case Instruction::CESAR_DEC:
        src = GetCurrentOperandValue(1);
        result = (src - 1) & 0xFFFF;
        if (decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode1,result);
            
        }
        else
        {
            memoryWriteTwoByte(src,result);
        }
        //setCarry(((src + 1) > 0xFFFF));
        setCarry(src == 0);
        //11111111
        //       1
        //is the only situation where there is carry (and thus, no borrow)
        setOverflow(src == 0x8000 );
        updateFlags(result);
        break;

    case Instruction::CESAR_NEG:
        src = GetCurrentOperandValue(1);
        result = -toSigned(src);

        if (decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode1,result);
            
        }
        else
        {
            memoryWriteTwoByte(src,result);
        }
        
        setCarry(!(src == 0));
        setOverflow(src == 0x8000);
        updateFlags(result);
        break;

    case Instruction::CESAR_TST:
        updateFlags(getRegisterValue(decodedRegisterCode1));
        setOverflow(false);
        setCarry(false);
        break;

    case Instruction:: CESAR_ROR:
        src = getRegisterValue(decodedRegisterCode1);
        result = ((src >> 1) | (getFlagValue("C") == true ? 0x8000 : 0x00)) & 0xFFFF;
        setRegisterValue(decodedRegisterCode1,result);
        setCarry(src & 1);
        updateFlags(result);
        setOverflow(getFlagValue("N") ^ getFlagValue("C"));
        break;

    case Instruction:: CESAR_ROL:
        src = getRegisterValue(decodedRegisterCode1);
        result = ((src << 1) | (getFlagValue("C") == true ? 0x01 : 0x00)) & 0xFFFF;
        setRegisterValue(decodedRegisterCode1, result);
        setCarry(src & 0x8000);
        updateFlags(result);
        setOverflow(getFlagValue("N") ^ getFlagValue("C"));
        break;

    case Instruction::CESAR_ASL:
        src = getRegisterValue(decodedRegisterCode1);
        result = (src << 1) & 0xFFFF;
        setRegisterValue(decodedRegisterCode1,result);
        setCarry(src & 0X8000);
        updateFlags(result);
        setOverflow(getFlagValue("N") ^ getFlagValue("C"));
        break;
    
    case Instruction::CESAR_ASR:
        src = getRegisterValue(decodedRegisterCode1);
        result = (src >> 1);
        setRegisterValue(decodedRegisterCode1,result);
        setCarry(src & 1);
        updateFlags(result);
        setOverflow(getFlagValue("N") ^ getFlagValue("C"));
        break;

    case Instruction:: CESAR_ADC:
        int carry;
        src = GetCurrentOperandValue(1);
        carry = getFlagValue("C");
        src += carry;
        result = src & 0xFFFF;
        setCarry((src + 1) > 0xFFFF);
        setOverflow(src == 0x8000 && carry == true);
        updateFlags(result);
        setRegisterValue(decodedRegisterCode1,result);
        break;

    case Instruction::CESAR_SBC:
        src = GetCurrentOperandValue(1);
        carry = getFlagValue("C");
        result =  src - carry;
        result = result & 0xFFFF;
        setCarry(src == 0);
        setOverflow((result == 0x7FFF) && (carry = true));
        updateFlags(result);
        setRegisterValue(decodedRegisterCode1,result);
        
        break;

    case Instruction:: CESAR_JMP:
        if (!isImmediate) // Invalidate immediate jumps
        {
            setPCValue(fetchedValue & 0b0000000011111111);
        }
        break;

    case Instruction:: CESAR_BR:
        setPCValue(getPCValue() + Machine::toSigned(decodedExtraValue)); // Calling machine::toSigned from parent becuse we use it for 1byte
        break;

    case Instruction:: CESAR_BNE:
        if(getFlagValue("Z") == false)
            setPCValue(getPCValue() + Machine::toSigned(decodedExtraValue));
        break;

    case Instruction:: CESAR_BEQ:
        if(getFlagValue("Z") == true)
            setPCValue(getPCValue() + Machine::toSigned(decodedExtraValue));
        break;

    case Instruction:: CESAR_BPL:
        if(getFlagValue("N") == false)
            setPCValue(getPCValue() + Machine::toSigned(decodedExtraValue));
        break;

    case Instruction:: CESAR_BMI:
        if(getFlagValue("N") == true)
            setPCValue(getPCValue() + Machine::toSigned(decodedExtraValue));
        break;

    case Instruction:: CESAR_BVC:
        if(getFlagValue("V") == false)
            setPCValue(getPCValue() + Machine::toSigned(decodedExtraValue));
        break;

    case Instruction:: CESAR_BVS:
        if(getFlagValue("V") == true)
            setPCValue(getPCValue() + Machine::toSigned(decodedExtraValue));
        break;

    case Instruction:: CESAR_BCC:
        if(getFlagValue("C") == false)
            setPCValue(getPCValue() + Machine::toSigned(decodedExtraValue));
        break;

    case Instruction::CESAR_BCS:

        if(getFlagValue("C") == true)
        {
            int pc_jmp = Machine::toSigned(getMemoryValue(getPCValue()-1));
            setPCValue(getPCValue()+pc_jmp);
        }
        break;

    case Instruction::CESAR_BGE:

        if(getFlagValue("N") == getFlagValue("V"))
        {
            int pc_jmp = Machine::toSigned(getMemoryValue(getPCValue()-1));
            setPCValue(getPCValue()+pc_jmp);
        }
        break;
    
    case Instruction::CESAR_BLT:

        if(getFlagValue("N") != getFlagValue("V"))
        {
            int pc_jmp = Machine::toSigned(getMemoryValue(getPCValue()-1));
            setPCValue(getPCValue()+pc_jmp);
        }
        break;

    case Instruction::CESAR_BGT:

        if((getFlagValue("N") == getFlagValue("V")) && (getFlagValue("Z") == 0))
        {
            int pc_jmp = Machine::toSigned(getMemoryValue(getPCValue()-1));
            setPCValue(getPCValue()+pc_jmp);
        }
        break;

    case Instruction::CESAR_BLE:

        if((getFlagValue("N") != getFlagValue("V")) || (getFlagValue("Z") == 1))
        {
            int pc_jmp = Machine::toSigned(getMemoryValue(getPCValue()-1));
            setPCValue(getPCValue()+pc_jmp);
        }
        break;

    case Instruction::CESAR_BHI:

        if((getFlagValue("C") == 0) && (getFlagValue("Z") == 0))
        {
            int pc_jmp = Machine::toSigned(getMemoryValue(getPCValue()-1));
            setPCValue(getPCValue()+pc_jmp);
        }
        break;

    case Instruction::CESAR_BLS:

        if((getFlagValue("C") == 1) || (getFlagValue("Z") == 1))
        {
            int pc_jmp = Machine::toSigned(getMemoryValue(getPCValue()-1));
            setPCValue(getPCValue()+pc_jmp);
        }
        break;

    case Instruction::CESAR_SOB:

        src = GetCurrentOperandValue(1) - 1;
        setRegisterValue(decodedRegisterCode1, src);

        if (src != 0)
        {
            int pc_jmp = Machine::toSigned(getMemoryValue(getPCValue()-1));
            setPCValue(getPCValue()-pc_jmp);
        }
        break;
        

    case Instruction::CESAR_NOP:
        break;

    case Instruction:: CESAR_CCC:
        
        carry = decodedExtraValue & 0b1;
        overflow = decodedExtraValue & 0b10;
        zero = decodedExtraValue & 0b100;
        negative = decodedExtraValue & 0b1000;
        
        if (negative)
            setFlagValue("N", !negative);
        if (zero)
            setFlagValue("Z", !zero);
        if (overflow)
            setFlagValue("V", !overflow);         
        if (carry)
            setCarry(!carry);
        break;

    case Instruction:: CESAR_SCC:
        carry = decodedExtraValue & 0b1;
        overflow = decodedExtraValue & 0b10;
        zero = decodedExtraValue & 0b100;
        negative = decodedExtraValue & 0b1000;
        
        if (negative)
            setFlagValue("N", negative);
        if (zero)
            setFlagValue("Z", zero);
        if (overflow)
            setFlagValue("V", overflow);         
        if (carry)
            setCarry(carry);
        break;

    case Instruction:: CESAR_JSR:
        {
        int currentPC = getPCValue();
        PutOnStack(decodedRegisterCode1);
        setRegisterValue(decodedRegisterCode1, currentPC);
        setPCValue(GetCurrentOperandValue(2));
        break;
        }

    case Instruction:: CESAR_RTS:
        setRegisterValue("R7", decodedRegisterCode1);
        GetOffStack(decodedRegisterCode1);
        break;

    case Instruction:: CESAR_HLT:
        {
            setRunning(false);
            setPCValue(getPCValue() - 1);
            break;
        }

    }
    

    instructionCount++;
}

void CesarMachine::PutOnStack (int registerId)
{
    int stackValue = getRegisterValue("R6");
    int registerValueOffStack = getRegisterValue(registerId);
    memoryWrite(stackValue, registerValueOffStack);
    setRegisterValue("R6",stackValue - 2);
}

void CesarMachine::GetOffStack(int registerId)
{
    int stackValue = getRegisterValue("R6");
    int memoryValue = getMemoryValue(stackValue);
    setRegisterValue(registerId, memoryValue);
    setRegisterValue("R6", stackValue + 2);
}


    

// void CesarMachine::extractAddressingModeCodes(int fetchedValue, AddressingMode::AddressingModeCode&)
// {

// }
  
int CesarMachine::GetCurrentOperandValue(int operand)
{
    int registerValueByte1,registerValueByte2,registerValue;
    int addressRight;
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
            registerValueByte1 = getMemoryValue(getRegisterValue(registerCode));
            registerValueByte2 = getMemoryValue(getRegisterValue(registerCode)+1);

            // shifts the result of byte 1 eight times to compreehend the full lenght of the word.
            r_value = registerValueByte1 << 8 | registerValueByte2;
            // Increment register value by 2
            registerValue = getRegisterValue(registerCode)+2;

            setRegisterValue(registerCode, registerValue);
            return r_value;

        //PRE DECREMENTED
        case AddressingMode::PRE_DECREMENTED:
            // decrement the register's value.
            setRegisterValue(registerCode,getRegisterValue(registerCode) - 2);
            // Get right operand within the memory.
            registerValueByte1 = getMemoryValue(getRegisterValue(registerCode));
            registerValueByte2 = getMemoryValue(getRegisterValue(registerCode)+1);

            //join the values
            r_value = registerValueByte1 << 8 | registerValueByte2;
            return r_value;

        //INDEXED
        case AddressingMode::INDEXED_BY_REG:
            // Register value + value in next byte.
            r_value = memoryReadTwoByteAddress(getPCValue()) + getRegisterValue(registerCode);
            // Increment PC by 2 bytes
            incrementPCValue(2);
        
            return r_value;

        //INDIRECT REGISTER
        case AddressingMode::INDIRECT_REGISTER:
            //get register value to use as index.
            registerValue = getRegisterValue(registerCode);
            return memoryRead(registerValue) << 8 | memoryRead(registerValue+1);

        //INDIRECT POST INCREMENT
        case AddressingMode::AFTER_INCREMENTED_INDIRECT:
             // get's the right operand value within the memory.
             r_value = getMemoryValue(getMemoryValue(getRegisterValue(registerCode))) << 8 | getMemoryValue(getMemoryValue(getRegisterValue(registerCode) + 1)) ;
             // Increment registerValue by 2
             registerValue += 2;
             
             setRegisterValue(registerCode,registerValue);
             return r_value;

        //INDIRECT PRE DECREMENT
        case AddressingMode::PRE_DECREMENTED_INDIRECT:
            // Decrement registerValue by 2
            setRegisterValue(registerCode,getRegisterValue(registerCode) - 2);
            
            
            //takes the operand within the memory held by the register value
             r_value = getMemoryValue(getMemoryValue(getRegisterValue(registerCode))) << 8 | getMemoryValue(getMemoryValue(getRegisterValue(registerCode) + 1)) ;
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


