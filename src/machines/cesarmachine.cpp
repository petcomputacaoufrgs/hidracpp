#include "cesarmachine.h"
#define INTE_THRESHOLD  0b10000010
#define INTS_THRESHOLD 0b10000000
#define IVET_LOCALE 65470
#define INTE 0xFFD9
#define INTS 0xFFD8

#define GEN_INT 0b10000000
#define TEC_INT 0b00000010
#define TIMER_INT 0b00000001

#define TEC_ENABLED 0b10000010
#define TIMER_ENABLED 0b10000001

#define TECDT 65495

CesarMachine::CesarMachine()
{
    identifier = "C16";
    fetchByteSize = 2;

    littleEndian = false;
    
    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    //Register bitpatterns are irrelevant; CESAR uses a different decoding method
    registers.append(new Register("R0", ".....000", 16));
    registers.append(new Register("R1", ".....001", 16));
    registers.append(new Register("R2", ".....010", 16));
    registers.append(new Register("R3", ".....011", 16));
    registers.append(new Register("R4", ".....100", 16));
    registers.append(new Register("R5", ".....101", 16));
    registers.append(new Register("R6", ".....110", 16, false));
    registers.append(new Register("R7", ".....111", 16, false));

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
    instructions.append(new Instruction(2, "10000000", Instruction::CESAR_CLR, "clr R1", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000001", Instruction::CESAR_NOT, "not R1", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000010", Instruction::CESAR_INC, "inc R1", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000011", Instruction::CESAR_DEC, "dec R1", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000100", Instruction::CESAR_NEG, "neg R1", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000101", Instruction::CESAR_TST, "tst R1", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000110", Instruction::CESAR_ROR, "ror R1", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10000111", Instruction::CESAR_ROL, "rol R1", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10001000", Instruction::CESAR_ASR, "asr R1", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10001001", Instruction::CESAR_ASL, "asl R1", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10001010", Instruction::CESAR_ADC, "adc R1", Instruction::GROUP_ONE_OPERAND));
    instructions.append(new Instruction(2, "10001011", Instruction::CESAR_SBC, "sbc R1", Instruction::GROUP_ONE_OPERAND));
    //////////////////////////////////////////////////
    //flow control
    //////////////////////////////////////////////////
    instructions.append(new Instruction(2, "0100....", Instruction::CESAR_JMP, "jmp R1", Instruction::GROUP_JUMP));
    instructions.append(new Instruction(2, "00110000", Instruction::CESAR_BR,  "br R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110001", Instruction::CESAR_BNE, "bne R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110010", Instruction::CESAR_BEQ, "beq R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110011", Instruction::CESAR_BPL, "bpl R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110100", Instruction::CESAR_BMI, "bmi R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110101", Instruction::CESAR_BVC, "bvc R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110110", Instruction::CESAR_BVS, "bvs R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00110111", Instruction::CESAR_BCC, "bcc R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111000", Instruction::CESAR_BCS, "bcs R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111001", Instruction::CESAR_BGE, "bge R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111010", Instruction::CESAR_BLT, "blt R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111011", Instruction::CESAR_BGT, "bgt R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111100", Instruction::CESAR_BLE, "ble R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111101", Instruction::CESAR_BHI, "bhi R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "00111110", Instruction::CESAR_BLS, "bls R1", Instruction::GROUP_CONDITIONAL_BRANCHES));
    instructions.append(new Instruction(2, "0110....", Instruction::CESAR_JSR, "jsr R1 R2", Instruction::GROUP_JUMP_SUBROUTINE));
    instructions.append(new Instruction(1, "0111....", Instruction::CESAR_RTS, "rts R1", Instruction::GROUP_RETURN_SUBROUTINE));
    ////////////////////////
    //special instructions
    ////////////////////////
    instructions.append(new Instruction(1, "0000....", Instruction::CESAR_NOP, "nop", Instruction::GROUP_NOP));
    instructions.append(new Instruction(1, "0001....", Instruction::CESAR_CCC, "ccc NZVC", Instruction::GROUP_CONDITIONAL_CODES));
    instructions.append(new Instruction(1, "0010....", Instruction::CESAR_SCC, "scc NZVC", Instruction::GROUP_CONDITIONAL_CODES));
    instructions.append(new Instruction(2, "0101....", Instruction::CESAR_SOB, "sob R1 R2", Instruction::GROUP_LOOP_CONTROL));
    instructions.append(new Instruction(1, "1111....", Instruction::CESAR_HLT, "hlt", Instruction::GROUP_HLT));

    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////
    
    addressingModes.append(new AddressingMode(".....000", AddressingMode::REGISTER,                     AddressingMode::NO_PATTERN));
    addressingModes.append(new AddressingMode(".....001", AddressingMode::AFTER_INCREMENTED,            "\\((R[0-7])\\)\\+"));
    addressingModes.append(new AddressingMode(".....010", AddressingMode::PRE_DECREMENTED,              "\\-\\((R[0-7])\\)"));
    addressingModes.append(new AddressingMode(".....011", AddressingMode::INDEXED_BY_REG,               "[0-9]{1,5}\\((R[0-7])\\)"));
    addressingModes.append(new AddressingMode(".....100", AddressingMode::INDIRECT_REGISTER,            "\\((R[0-7])\\)"));
    addressingModes.append(new AddressingMode(".....101", AddressingMode::AFTER_INCREMENTED_INDIRECT,   "\\(\\((R[0-7])\\)\\+\\)"));
    addressingModes.append(new AddressingMode(".....110", AddressingMode::PRE_DECREMENTED_INDIRECT,     "\\(\\-\\((R[0-7])\\)\\)"));
    addressingModes.append(new AddressingMode(".....111", AddressingMode::INDIRECT_INDEXED_BY_REG,      "\\([0-9]{1,5}\\((R[0-7])\\)\\)"));

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



bool CesarMachine::hasBeenInterrupted(){
    return keyboardInterrupted or timerInterrupted;
}

void CesarMachine::step(){
    Machine::step();

    // Checks if there were any unhandled interruptions
    // if(hasBeenInterrupted()){
    //     handleInterruption();
    // }
}


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
            decodedExtraValue = (0b1111 & fetchedValue);//Mask to get flag values
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
            decodedAddressingModeCode1 = default_am; //Adressing mode not used
            decodedAddressingModeCode2 = convertInstructionStringAddressingMode((fetchedValue >> 3) & 0b111);//Get adressing mode
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

    int reg_value1, reg_value2, src_addr, dst_addr, src_val, dst_val, result;
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
        src_val = GetCurrentOperandValue(1);
        if(decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode2, src_val);
            updateFlags(src_val);
        }
        else
        {
            // Get the address for writing
            dst_addr = GetCurrentOperandAddress(2);
            memoryWriteTwoByte(dst_addr, src_val);
            updateFlags(dst_addr);
        }

        setOverflow(false);
        
        break;

    case Instruction:: CESAR_SUB:
        src_val = GetCurrentOperandValue(1);   
        dst_val = GetCurrentOperandValue(2);
        dst_addr = GetCurrentOperandAddress(2);
        result = (dst_val - src_val) & 0xFFFF;
        if(decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode2, result);
        }
        else
        {
            memoryWriteTwoByte(dst_addr, result);
        }
        // REMEMBER TO REVIEW THE LOGIC BEHIND IT
        setCarry(toSigned(src_val) < toSigned(dst_val));
        setOverflow(toSigned(src_val) + toSigned(-dst_val) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction:: CESAR_ADD:
        src_val = GetCurrentOperandValue(1);   
        dst_val = GetCurrentOperandValue(2);
        dst_addr = GetCurrentOperandAddress(2);
        result = (dst_val + src_val) & 0xFFFF;
        if(decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode2, result);
        }
        else
        {
            memoryWriteTwoByte(dst_addr, result);
        }

        setCarry(src_val + dst_val > 0xFFFF);
        setOverflow(toSigned(src_val) + toSigned(dst_val) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction:: CESAR_CMP:
        src_val = GetCurrentOperandValue(1);   
        dst_val = GetCurrentOperandValue(2);
        result = src_val - dst_val;

        setCarry(toSigned(src_val) < toSigned(dst_val));
        setOverflow(toSigned(src_val) + toSigned(-dst_val) != toSigned(result));
        updateFlags(result);
        break;

    case Instruction::CESAR_AND:
        src_val = GetCurrentOperandValue(1);
        dst_val = GetCurrentOperandValue(2);
        dst_addr = GetCurrentOperandAddress(2);
        result = src_val & dst_val;
        if(decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode2, result);
        }
        else
        {
            memoryWriteTwoByte(dst_addr, result);
        }

        setOverflow(false);
        updateFlags(result);
        break;

    case Instruction::CESAR_OR:
        src_val = GetCurrentOperandValue(1);
        dst_val = GetCurrentOperandValue(2);
        dst_addr = GetCurrentOperandAddress(2);
        result = (src_val | dst_val) & 0xFFFF;
        if(decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode2, result);
        }
        else
        {
            memoryWriteTwoByte(dst_addr, result);
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
        src_val = getRegisterValue(decodedRegisterCode1);
        result = ~src_val & 0xFF;

        setRegisterValue(decodedRegisterCode1, result);
        setCarry(true);
        setOverflow(false);
        updateFlags(result);
        break;

    case Instruction::CESAR_INC:
        src_val = GetCurrentOperandValue(1);
        dst_addr = GetCurrentOperandAddress(1);
        result = (src_val + 1) & 0xFFFF;
        if (decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode1, result);
        }
        else
        {
            memoryWriteTwoByte(dst_addr, result);
        }
        setCarry((src_val + 1) > 0xFFFF);
        setOverflow(src_val == 0x7FFF);
        updateFlags(result);
        break;
        
    case Instruction::CESAR_DEC:
        src_val = GetCurrentOperandValue(1);
        dst_addr = GetCurrentOperandAddress(1);
        result = (src_val - 1) & 0xFFFF;
        if (decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode1,result);
            
        }
        else
        {
            memoryWriteTwoByte(dst_addr,result);
        }
        //setCarry(((src + 1) > 0xFFFF));
        setCarry(src_val == 0);
        //11111111
        //       1
        //is the only situation where there is carry (and thus, no borrow)
        setOverflow(src_val == 0x8000 );
        updateFlags(result);
        break;

    case Instruction::CESAR_NEG:
        src_val = GetCurrentOperandValue(1);
        dst_addr = GetCurrentOperandAddress(1);
        result = -toSigned(src_val);

        if (decodedAddressingModeCode2 == AddressingMode::REGISTER)
        {
            setRegisterValue(decodedRegisterCode1,result);
            
        }
        else
        {
            memoryWriteTwoByte(dst_addr,result);
        }
        
        setCarry(!(src_val == 0));
        setOverflow(src_val == 0x8000);
        updateFlags(result);
        break;

    case Instruction::CESAR_TST:
        updateFlags(getRegisterValue(decodedRegisterCode1));
        setOverflow(false);
        setCarry(false);
        break;

    case Instruction:: CESAR_ROR:
        src_val = getRegisterValue(decodedRegisterCode1);
        result = ((src_val >> 1) | (getFlagValue("C") == true ? 0x8000 : 0x00)) & 0xFFFF;
        setRegisterValue(decodedRegisterCode1,result);
        setCarry(src_val & 1);
        updateFlags(result);
        setOverflow(getFlagValue("N") ^ getFlagValue("C"));
        break;

    case Instruction:: CESAR_ROL:
        src_val = getRegisterValue(decodedRegisterCode1);
        result = ((src_val << 1) | (getFlagValue("C") == true ? 0x01 : 0x00)) & 0xFFFF;
        setRegisterValue(decodedRegisterCode1, result);
        setCarry(src_val & 0x8000);
        updateFlags(result);
        setOverflow(getFlagValue("N") ^ getFlagValue("C"));
        break;

    case Instruction::CESAR_ASL:
        src_val = getRegisterValue(decodedRegisterCode1);
        result = (src_val << 1) & 0xFFFF;
        setRegisterValue(decodedRegisterCode1,result);
        setCarry(src_val & 0X8000);
        updateFlags(result);
        setOverflow(getFlagValue("N") ^ getFlagValue("C"));
        break;
    
    case Instruction::CESAR_ASR:
        src_val = getRegisterValue(decodedRegisterCode1);
        result = (src_val >> 1);
        setRegisterValue(decodedRegisterCode1,result);
        setCarry(src_val & 1);
        updateFlags(result);
        setOverflow(getFlagValue("N") ^ getFlagValue("C"));
        break;

    case Instruction:: CESAR_ADC:
        int carry;
        src_val = GetCurrentOperandValue(1);
        carry = getFlagValue("C");
        src_val += carry;
        result = src_val & 0xFFFF;
        setCarry((src_val + 1) > 0xFFFF);
        setOverflow(src_val == 0x8000 && carry == true);
        updateFlags(result);
        setRegisterValue(decodedRegisterCode1,result);
        break;

    case Instruction::CESAR_SBC:
        src_val = GetCurrentOperandValue(1);
        carry = getFlagValue("C");
        result =  src_val - carry;
        result = result & 0xFFFF;
        setCarry(src_val == 0);
        setOverflow((result == 0x7FFF) && (carry = true));
        updateFlags(result);
        setRegisterValue(decodedRegisterCode1,result);
        
        break;

    case Instruction:: CESAR_JMP:
        if(decodedAddressingModeCode1 == AddressingMode::REGISTER){
            break; // Acts as NOP
        }
        
        result = GetCurrentOperandAddress(1);
        setPCValue(result);

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

        src_val = GetCurrentOperandValue(1) - 1;
        dst_addr = GetCurrentOperandAddress(1);
        if(decodedAddressingModeCode1 == AddressingMode::REGISTER){
            setRegisterValue(decodedRegisterCode1, src_val);
        }
        else{
            memoryWriteTwoByte(dst_addr, src_val);
        }


        if (src_val != 0)
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
        //Acts as NOP if am = register, as per specification
        if(decodedAddressingModeCode2 == AddressingMode::REGISTER) break;

        //Addressing mode must be executed prior for R7 to work
        int subroutine_address = GetCurrentOperandAddress(2);

        int returnPC = getPCValue();
        PutOnStack(decodedRegisterCode1);
        setRegisterValue(decodedRegisterCode1, returnPC);
        
        setPCValue(subroutine_address);
        break;
        }

    case Instruction:: CESAR_RTS:
        setRegisterValue("R7", getRegisterValue(decodedRegisterCode1));
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
    // Stack writes backwards
    memoryWrite(stackValue - 1, registerValueOffStack & 0x00FF);
    memoryWrite(stackValue, (registerValueOffStack & 0xFF00) >> 8);
    setRegisterValue("R6",stackValue - 2);
}

void CesarMachine::GetOffStack(int registerId)
{
    int stackValue = getRegisterValue("R6");
    // Stack writes backwards, so it must also read backwards
    int memoryValue = (getMemoryValue(stackValue+2) << 8) |  getMemoryValue(stackValue+1);
    setRegisterValue(registerId, memoryValue);
    setRegisterValue("R6", stackValue + 2);
}


    
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
            // Read memory at Register value + value in next byte.
            r_value = memoryReadTwoByteAddress(memoryReadTwoByteAddress(getPCValue()) + getRegisterValue(registerCode));
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
            // Get register value
            registerValueByte1 = getMemoryValue(getRegisterValue(registerCode));
            registerValueByte2 = getMemoryValue(getRegisterValue(registerCode)+1);

            // shifts the result of byte 1 eight times to compreehend the full lenght of the word.
            r_value = registerValueByte1 << 8 | registerValueByte2;
            // Increment register value by 2
            registerValue = getRegisterValue(registerCode)+2;

            setRegisterValue(registerCode, registerValue);
            r_value = getMemoryTwoByteAddress(r_value);
            return r_value;

        //INDIRECT PRE DECREMENT
        case AddressingMode::PRE_DECREMENTED_INDIRECT:
            // decrement the register's value.
            setRegisterValue(registerCode,getRegisterValue(registerCode) - 2);
            // Get right operand within the memory.
            registerValueByte1 = getMemoryValue(getRegisterValue(registerCode));
            registerValueByte2 = getMemoryValue(getRegisterValue(registerCode)+1);

            //join the values
            r_value = registerValueByte1 << 8 | registerValueByte2;
            r_value = getMemoryTwoByteAddress(r_value);
            return r_value;
        
        //INDIRECT INDEXED

        case AddressingMode::INDIRECT_INDEXED_BY_REG:
            // Read memory at Register value + value in next byte.
            r_value = memoryReadTwoByteAddress(memoryReadTwoByteAddress(getPCValue()) + getRegisterValue(registerCode));
            // Increment PC by 2 bytes
            incrementPCValue(2);
        
            return memoryReadTwoByteAddress(r_value);
        
        default:
            return 0;
    }
}

int CesarMachine::GetCurrentOperandAddress(int operand)
{
    int registerValueByte1,registerValueByte2,registerValue;
    int addressRight;
    int registerCode = (operand == 1 ? decodedRegisterCode1 : decodedRegisterCode2);
    int addressingModeCode = (operand == 1 ? decodedAddressingModeCode1 : decodedAddressingModeCode2);
    int r_value; // The memory address acessed by the addressing mode

    switch (addressingModeCode)
    {
        //REGISTER
        case AddressingMode::REGISTER:
            return 0; // No address. This value should be ignored by instructions that use this function

        //POST INCREMENTED
        case AddressingMode::AFTER_INCREMENTED:
            r_value = getRegisterValue(registerCode);

            // Increment register value by 2
            registerValue = getRegisterValue(registerCode)+2;
            setRegisterValue(registerCode, registerValue);
            return r_value;

        //PRE DECREMENTED
        case AddressingMode::PRE_DECREMENTED:
            // decrement the register's value.
            setRegisterValue(registerCode,getRegisterValue(registerCode) - 2);
            
            r_value = getRegisterValue(registerCode);

            return r_value;

        //INDEXED
        case AddressingMode::INDEXED_BY_REG:
            // Address acessed is calculated as Register value + value in next byte.
            r_value = memoryReadTwoByteAddress(getPCValue()) + getRegisterValue(registerCode);
            // Increment PC by 2 bytes
            incrementPCValue(2);
        
            return r_value;

        //INDIRECT REGISTER
        case AddressingMode::INDIRECT_REGISTER:
            //Acessed address is the register value.
            r_value = getRegisterValue(registerCode);
            return r_value;

        //INDIRECT POST INCREMENT
        case AddressingMode::AFTER_INCREMENTED_INDIRECT:
            // Get register value
            registerValueByte1 = getMemoryValue(getRegisterValue(registerCode));
            registerValueByte2 = getMemoryValue(getRegisterValue(registerCode)+1);

            // shifts the result of byte 1 eight times to compreehend the full lenght of the word.
            r_value = registerValueByte1 << 8 | registerValueByte2;
            // Increment register value by 2
            registerValue = getRegisterValue(registerCode)+2;

            setRegisterValue(registerCode, registerValue);

            // r_value already contains the memory address acessed
            return r_value;

        //INDIRECT PRE DECREMENT
        case AddressingMode::PRE_DECREMENTED_INDIRECT:
            // decrement the register's value.
            setRegisterValue(registerCode,getRegisterValue(registerCode) - 2);
            // Get right operand within the memory.
            registerValueByte1 = getMemoryValue(getRegisterValue(registerCode));
            registerValueByte2 = getMemoryValue(getRegisterValue(registerCode)+1);

            //join the values
            r_value = registerValueByte1 << 8 | registerValueByte2;

            // r_value already contains the memory address acessed
            return r_value;
        
        //INDIRECT INDEXED

        case AddressingMode::INDIRECT_INDEXED_BY_REG:
            // Address acessed is calculated as Register value + value in next byte.
            r_value = memoryReadTwoByteAddress(getPCValue()) + getRegisterValue(registerCode);
            // Increment PC by 2 bytes
            incrementPCValue(2);
        
            return memoryReadTwoByteAddress(r_value);
        
        default:
            return 0;
    }
}

void CesarMachine::handleKeyPress(int key)
{
    if ((getMemoryValue(INTE) & (GEN_INT | TEC_INT) == TEC_ENABLED) && !(getMemoryValue(INTS) & (GEN_INT)))
        {
            setMemoryValue(INTS, getMemoryValue(INTS) | TEC_INT | GEN_INT);
            setPCValue(getMemoryTwoByteAddress(IVET_LOCALE));
        }
}

void CesarMachine::handleTimerEvent()
{
    if ((getMemoryValue(INTE) & (GEN_INT | TIMER_INT) == TIMER_ENABLED) && !(getMemoryValue(INTS) & TIMER_INT))
        {
            setMemoryValue(INTS, getMemoryValue(INTS) | TIMER_INT | GEN_INT);
            setPCValue(getMemoryTwoByteAddress(IVET_LOCALE));
        }
}

int CesarMachine::getTimerValue()
{
    return getMemoryValue(TECDT);
}



/*
...../ )
.....' /
---' (_____
......... ((__)
..... _ ((___)
....... -'((__)
--.___((_)

ok
boa sorte
BOA NOITE PESSOAL
*/

/*

    Assembler functions

*/

int CesarMachine::calculateBytesToReserve(const Instruction* instruction, QStringList const& arguments){
    //TO-DO: MAKE THE SPECIAL ADDRESSING MODE CHECKS INDIVIDUAL METHODS TO AVOID REPETITION
    static QRegExp regex_immediate("([0-9]{1,5})");
    static QRegExp regex_absolute("#([0-9]{1,5})");
    
    // Eugh...
    QRegExp regex_idx_reg = addressingModes[CESAR_ADDRESSING_MODE_INDEX]->getAssemblyRegExp();
    QRegExp regex_indirect_idx_reg = addressingModes[CESAR_ADDRESSING_MODE_INDIRECT_INDEX]->getAssemblyRegExp();

    //Adds additional offsets caused by special addressing modes
    int n_bytes = instruction->getNumBytes();

    for(int i = 0; i < arguments.length(); i++){
        QString param = arguments[i];

        // It doesn't matter if the assigned value is right or not; as long as the label is
        // already on the map, we can consider the fact that it will require 2 bytes (due to the R7 AMs)
        translateLabelToValue(param);

        if(
            regex_immediate.exactMatch(param) ||
            regex_absolute.exactMatch(param) ||
            regex_idx_reg.exactMatch(param) ||
            regex_indirect_idx_reg.exactMatch(param)
            )
        {
            n_bytes += 2;
        }
    }

    return n_bytes;
}

void CesarMachine::translateLabelToValue(QString& argument)
{
    // Allows the '#' special character to precede the label
    if(argument[0] == '#'){
        argument.remove(0, 1);
        Machine::translateLabelToValue(argument);
        argument.push_front("#");
    }
    else{
        Machine::translateLabelToValue(argument);
    }
    
}

void CesarMachine::buildInstruction(Instruction* instruction, QStringList argumentList)
{

    static QRegExp conditional_codes_regex("N?Z?V?C?");

    // CESAR does not rely on the instruction description to determine
    // how it should build the instruction, using GROUPS instead
    // TO-DO: Rewrite other machines to rely on groups
    int byte1, byte2;
    int reg1, reg2, am1, am2;
    int offset1, offset2;
    offset1 = -1;
    offset2 = -1;
    
    QString aux_str;

    switch (instruction->getInstructionGroup())
    {
    case Instruction::InstructionGroup::GROUP_NOP:
        setAssemblerMemoryNext(0);
        break;

    case Instruction::InstructionGroup::GROUP_CONDITIONAL_CODES:
        byte1 = instruction->getByteValue();
        aux_str = argumentList.first().toUpper();
        
        if(!conditional_codes_regex.exactMatch(aux_str))
        {
            throw invalidArgument;
        }

        if(aux_str.contains("N")){byte1 |= 0b1000;}
        if(aux_str.contains("Z")){byte1 |= 0b0100;}
        if(aux_str.contains("V")){byte1 |= 0b0010;}
        if(aux_str.contains("C")){byte1 |= 0b0001;}

        setAssemblerMemoryNext(byte1);
        break;

    case Instruction::InstructionGroup::GROUP_CONDITIONAL_BRANCHES:
        byte1 = instruction->getByteValue();
        byte2 = argumentToValue(argumentList[0], true, 2);
        byte2 = byte2 - PC->getValue(); //BUG TA AQUI AQUI AQUI AQUI

        // Value must range from -128 to 127
        if(byte2 < -128 || byte2 > 127)
        {
            throw invalidArgument;
        }        

        setAssemblerMemoryNext(byte1);
        setAssemblerMemoryNext(byte2);
        break;

    case Instruction::InstructionGroup::GROUP_JUMP:
        extractInstructionRegisterParameter(argumentList[0], reg1, am1, offset1);
        
        //Can't use register mode
        if(am1 == CESAR_ADDRESSING_MODE_REG)
        {
            throw invalidArgument;
        }        

        byte1 = instruction->getByteValue();
        byte2 = (am1 << 3) | reg1; 

        setAssemblerMemoryNext(byte1);
        setAssemblerMemoryNext(byte2);

        break;

    case Instruction::InstructionGroup::GROUP_LOOP_CONTROL:
        byte1 = instruction->getByteValue();
        extractInstructionRegisterParameter(argumentList[0], reg1, am1, offset1);        
        byte1 |= reg1;
        //Check if it's a label; if so, get the difference between (previous) PC and it
        // TO-DO: REFACTOR TO USE A METHOD FOR CHECKING IF A STRING IS A LABEL
        if(labelPCMap.contains(argumentList[1].toLower())){
            byte2 = PC->getValue() - argumentToValue(argumentList[1], true, 2) + 2;
        }
        else{
            byte2 = argumentToValue(argumentList[1], true, 2);
        }
        

        // Value must range from between 0 and 65535
        if(byte2 > 65535)
        {
            throw invalidArgument;
        }        

        setAssemblerMemoryNext(byte1);
        setAssemblerMemoryNext(byte2);
        break;

    case Instruction::InstructionGroup::GROUP_JUMP_SUBROUTINE:
        byte1 = instruction->getByteValue();
        extractInstructionRegisterParameter(argumentList[0], reg1, am1, offset1);
        extractInstructionRegisterParameter(argumentList[1], reg2, am2, offset2);         

        // First register must be in reg am and the second in any other except reg
        if(am1 != CESAR_ADDRESSING_MODE_REG || am2 == CESAR_ADDRESSING_MODE_REG)
        {
            throw invalidArgument;  //TO-DO: MORE DETAILED ERRORS FOR ASSEMBLER
        }

        byte1 |= reg1;
        byte2 = (am2 << 3) | reg2;
        setAssemblerMemoryNext(byte1);
        setAssemblerMemoryNext(byte2);
        
        break;

    case Instruction::InstructionGroup::GROUP_RETURN_SUBROUTINE:
        byte1 = instruction->getByteValue();
        extractInstructionRegisterParameter(argumentList.first(), reg1, am1, offset1);        
        
        if(am1 != CESAR_ADDRESSING_MODE_REG)
        {
            throw invalidArgument;
        }

        byte1 |= reg1;
        setAssemblerMemoryNext(byte1);

        break;

    case Instruction::InstructionGroup::GROUP_ONE_OPERAND:
        byte1 = instruction->getByteValue();
        
        extractInstructionRegisterParameter(argumentList.first(), reg1, am1, offset1);
        byte2 = (am1 << 3) | reg1;

        setAssemblerMemoryNext(byte1);
        setAssemblerMemoryNext(byte2);

        break;

    case Instruction::InstructionGroup::GROUP_TWO_OPERAND:
        byte1 = instruction->getByteValue();
        
        extractInstructionRegisterParameter(argumentList[0], reg1, am1, offset1);
        extractInstructionRegisterParameter(argumentList[1], reg2, am2, offset2);
        
        byte1 |= (am1 << 1) | ((reg1 & 0b100) >> 2);
        byte2 = ((reg1 & 0b011) << 6) | (am2 << 3) | reg2;

        setAssemblerMemoryNext(byte1);
        setAssemblerMemoryNext(byte2);
        
        break;
    
    case Instruction::InstructionGroup::GROUP_HLT:
        setAssemblerMemoryNext(0xF0);
        break;

    default:
        break;
    }

    // Add offsets
    if(offset1 != -1){ 
        setAssemblerMemoryNextTwoByte(offset1);
    }
    if(offset2 != -1){
        setAssemblerMemoryNextTwoByte(offset2);
    }

}

//Extracts information from a register parameter
void CesarMachine::extractInstructionRegisterParameter(QString& param, int& reg_code, int& am_code, int& am_offset)
{
    AddressingMode::AddressingModeCode aux;
    static QRegExp number_regex("\\(?([0-9]{1,5})\\(.*");
    static QRegExp regex_immediate("#([0-9]{1,5})");
    static QRegExp regex_absolute("([0-9]{1,5})");

    //Replaces labels with values
    translateLabelToValue(param);

    //Converts the R7 addressing modes to their literals
    if(regex_immediate.exactMatch(param))
    {
        param = "(R7)+";
        am_offset = regex_immediate.cap(1).toInt();
    }
    else if(regex_absolute.exactMatch(param))
    {
        param = "((R7)+)";
        am_offset = regex_absolute.cap(1).toInt();  
    }

    if(number_regex.exactMatch(param))
    {
        am_offset = number_regex.cap(1).toInt();
        if (am_offset > 65535){
            throw invalidArgument;
        }
    }

    extractArgumentAddressingModeCode(param, aux);
    am_code = getAddressingModeBitCode(aux); //To-do: extract argument addressing mode bit code; skip the extra step
    reg_code = getRegisterBitCode(param);
    
    if (reg_code == Register::NO_BIT_CODE)
        throw invalidArgument; // Register not found (or invisible)
}

QString CesarMachine::getCurrentInstructionString(int &argumentBytes)
{
    QString memoryString;
    int address = getPCValue();

    // Fetch and decode instruction
    Instruction *instruction = currentInstruction;
    AddressingMode::AddressingModeCode addressingModeCode1 = decodedAddressingModeCode1;
    AddressingMode::AddressingModeCode addressingModeCode2 = decodedAddressingModeCode2;
    QString registerName1 = getRegisterName(decodedRegisterCode1);
    QString registerName2 = getRegisterName(decodedRegisterCode2);
    int extraValue = decodedExtraValue;

    // Bytes to be skipped
    argumentBytes = instruction->getNumBytes() - 1;
    
    // Special addressing modes
    if((addressingModeCode1 == AddressingMode::AFTER_INCREMENTED || addressingModeCode1 == AddressingMode::AFTER_INCREMENTED_INDIRECT) &&
        decodedRegisterCode1 == PC->getBitCode()){
            argumentBytes += 2;
        }
    if((addressingModeCode2 == AddressingMode::AFTER_INCREMENTED || addressingModeCode2 == AddressingMode::AFTER_INCREMENTED_INDIRECT) &&
        decodedRegisterCode2 == PC->getBitCode()){
            argumentBytes += 2;
        }
    
    // Instruction name
    memoryString = instruction->getMnemonic().toUpper();

    if (instruction->getArguments().contains("NZVC")){
        memoryString += " ";
        if (extraValue & 0b1000){ 
            memoryString += "N";
        }
        if (extraValue & 0b0100){ 
            memoryString += "Z";
        }
        if (extraValue & 0b0010){ 
            memoryString += "V";
        }
        if (extraValue & 0b0001){ 
            memoryString += "C";
        }
    }

    if (instruction->getArguments().contains("R1")){
        wrapAddressingModeToRegister(addressingModeCode1, registerName1, extraValue);
        memoryString += " " + registerName1;
    }

    if (instruction->getArguments().contains("R2")){
        wrapAddressingModeToRegister(addressingModeCode2, registerName2, extraValue);
        memoryString += " " + registerName2;
    }

    return memoryString;
}

void CesarMachine::wrapAddressingModeToRegister(AddressingMode::AddressingModeCode const& am, QString& registerString, int extraValue){
    switch(am){
        case AddressingMode::REGISTER:
        break;
        case AddressingMode::AFTER_INCREMENTED:
        registerString = "(" + registerString + ")+";
        break;
        case AddressingMode::PRE_DECREMENTED:
        registerString = "-(" + registerString + ")";
        break;
        case AddressingMode::INDEXED_BY_REG:
        registerString = registerString + "(" + QString::number(extraValue) + ")";
        break;
        case AddressingMode::INDIRECT_REGISTER:
        registerString = "(" + registerString + ")";
        break;
        case AddressingMode::AFTER_INCREMENTED_INDIRECT:
        registerString = "((" + registerString + ")+)";
        break;
        case AddressingMode::PRE_DECREMENTED_INDIRECT:
        registerString = "(-(" + registerString + "))";
        break;
        case AddressingMode::INDIRECT_INDEXED_BY_REG:
        registerString = "(" + registerString + "(" + QString::number(extraValue) + ")" + ")";
        break;
    }

}