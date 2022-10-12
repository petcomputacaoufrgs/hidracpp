#include "cesarmachine.h"


CesarMachine::CesarMachine()
{
    identifier = "C16";
    fetchByteSize = 2;

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

    instructions.append(new Instruction(1, "0000....", Instruction::NOP, "nop"));
    instructions.append(new Instruction(2, "0001....", Instruction::STR, "sta a"));
    instructions.append(new Instruction(2, "0010....", Instruction::LDR, "lda a"));
    instructions.append(new Instruction(2, "0011....", Instruction::ADD, "add a"));
    instructions.append(new Instruction(2, "0100....", Instruction::OR,  "or a"));
    instructions.append(new Instruction(2, "0101....", Instruction::AND, "and a"));
    instructions.append(new Instruction(1, "0110....", Instruction::NOT, "not"));
    instructions.append(new Instruction(2, "0111....", Instruction::SUB, "sub a"));
    instructions.append(new Instruction(2, "1000....", Instruction::JMP, "jmp a"));
    instructions.append(new Instruction(2, "100100..", Instruction::JN,  "jn a"));
    instructions.append(new Instruction(2, "100101..", Instruction::JP,  "jp a"));
    instructions.append(new Instruction(2, "100110..", Instruction::JV,  "jv a"));
    instructions.append(new Instruction(2, "100111..", Instruction::JNV, "jnv a"));
    instructions.append(new Instruction(2, "101000..", Instruction::JZ,  "jz a"));
    instructions.append(new Instruction(2, "101001..", Instruction::JNZ, "jnz a"));
    instructions.append(new Instruction(2, "101100..", Instruction::JC,  "jc a"));
    instructions.append(new Instruction(2, "101101..", Instruction::JNC, "jnc a"));
    instructions.append(new Instruction(2, "101110..", Instruction::JB,  "jb a"));
    instructions.append(new Instruction(2, "101111..", Instruction::JNB, "jnb a"));
    instructions.append(new Instruction(1, "1110..00", Instruction::SHR, "shr"));
    instructions.append(new Instruction(1, "1110..01", Instruction::SHL, "shl"));
    instructions.append(new Instruction(1, "1110..10", Instruction::ROR, "ror"));
    instructions.append(new Instruction(1, "1110..11", Instruction::ROL, "rol"));
    instructions.append(new Instruction(1, "1111....", Instruction::HLT, "hlt"));

    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////

    addressingModes.append(new AddressingMode("........", AddressingMode::DIRECT, AddressingMode::NO_PATTERN));

}


void CesarMachine::step()
{
    int fetchedValue, immediateAddress;
    int reg1, reg2;
    int am1, am2;
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
    
void CesarMachine::fetchInstruction(int &fetchedValue, Instruction *&instruction)
{
    // Read first byte
    fetchedValue = memoryReadNext();  // has to read two bytes
    instruction = getInstructionFromValue(fetchedValue);
    // Shift fetchedValue 1 byte to the left and read second byte using or mask
    fetchedValue = fetchedValue << 8 | memoryReadNext();
}    
    
void CesarMachine::decodeInstruction(int fetchedValue, Instruction *&instruction, int &addressingMode1,
        int &addressingMode2, int &registerId1,
        int &registerId2, int &extraValues, int &immediateAddress)
{

    Instruction::InstructionGroup instructionGroup = instruction->getInstructionGroup();
    switch(instructionGroup){
        case Instruction::InstructionGroup::GROUP_NOP:
            addressingMode1 = 0; //Adressing mode not used
            addressingMode2 = 0; //
            registerId1 = 0;//Pass the first register, with no intention to use it
            registerId2 = 0;//
            extraValues = 0;// extraValues not used
            break;
        case Instruction::InstructionGroup::GROUP_CONDITIONAL_CODES:
            addressingMode1 = 0; //Adressing mode not used
            addressingMode2 = 0; //
            registerId1 = 0;//Pass the first register, with no intention to use it
            registerId2 = 0;//
            extraValues = ((1111 << 8) & fetchedValue) >> 8;//Mask to get flag values
            break; 

        case Instruction::InstructionGroup::GROUP_CONDITIONAL_BRANCHES:
            addressingMode1 = 0; //Adressing mode not used
            addressingMode2 = 0; //
            registerId1 = 0;//Pass the first register, with no intention to use it
            registerId2 = 0;//
            extraValues = fetchedValue & 0b11111111;//Mask to get branch adress to jump to
            break;

        case Instruction::InstructionGroup::GROUP_JUMP:
            addressingMode1 = (fetchedValue & 0b00111000) >> 3;
            addressingMode2 = 0;
            registerId1 = (fetchedValue & 0b00000111);
            registerId2 = 0;
            break;
            
        case Instruction::InstructionGroup::GROUP_LOOP_CONTROL:
            addressingMode1 = 0;//Adressing mode not used
            addressingMode2 = 0;//
            registerId1 = (fetchedValue >> 8) & 0b111;//Shift 1 byte to the left and mask to get register
            registerId2 = 0;//Pass the second register, with no intention to use it
            extraValues = fetchedValue & 0b11111111;//Get displacement
            break;

        case Instruction::InstructionGroup::GROUP_JUMP_SUBROUTINE:
            addressingMode1 = (fetchedValue >> 3) & 0b111;//Get adressing mode
            addressingMode2 = 0; //Adressing mode not used
            registerId1 = (fetchedValue >> 8) & 0b111;//Shift 1 byte to the left and mask to get register
            registerId2 = fetchedValue & 0b111;//mask to get register
            extraValues = 0;// extraValues not used
            break;

        case Instruction::InstructionGroup::GROUP_RETURN_SUBROUTINE:
            addressingMode1 = 0;
            addressingMode2 = 0;
            registerId1 = (fetchedValue  & 0b111);
            registerId2 = 0;
            extraValues = 0;
            break;

        case Instruction::InstructionGroup::GROUP_ONE_OPERAND:
            addressingMode1 = (fetchedValue >> 3) & 0b111;//Get adressing mode
            addressingMode2 = 0;//Adressing mode not used
            registerId1 = (fetchedValue  & 0b111);//Mask to get register
            registerId2 = 0;//Pass the second register, with no intention to use it
            extraValues = 0;// extraValues not used
            break;

        case Instruction::InstructionGroup::GROUP_TWO_OPERAND:
            addressingMode1 = (fetchedValue >> 9) & 0b111;//
            addressingMode2 = (fetchedValue >> 3) & 0b111;
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
     
void CesarMachine::executeInstruction(Instruction *&instruction, 
        int addressingModeCode1,
        int addressingModeCode2, 
        int registerName1,
        int registerName2, 
        int extraValue,
        int immediateAddress)
{

    
}

void CesarMachine::extractAddressingModeCodes(int fetchedValue, AddressingMode::AddressingModeCode&)
{

}
           
        
