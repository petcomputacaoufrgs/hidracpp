#include "voltamachine.h"

VoltaMachine::VoltaMachine()
{
    identifier = "VLT"; // TODO: Confirmar identificador

    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    registers.append(new Register("PC", "", 8));
    registers.append(new Register("SP", "", 6));

    PC = registers.first();
    SP = registers.last();



    //////////////////////////////////////////////////
    // Initialize memory and stack
    //////////////////////////////////////////////////

    setMemorySize(256);
    setStackSize(64);



    //////////////////////////////////////////////////
    // Initialize instructions
    //////////////////////////////////////////////////

    instructions.append(new Instruction(1, "00000000", Instruction::VOLTA_NOP, "nop"));
    instructions.append(new Instruction(1, "00010001", Instruction::VOLTA_ADD, "add"));
    instructions.append(new Instruction(1, "00010010", Instruction::VOLTA_SUB, "sub"));
    instructions.append(new Instruction(1, "00010011", Instruction::VOLTA_AND, "and"));
    instructions.append(new Instruction(1, "00010100", Instruction::VOLTA_OR,  "or"));
    instructions.append(new Instruction(1, "00100001", Instruction::VOLTA_CLR, "clr"));
    instructions.append(new Instruction(1, "00100010", Instruction::VOLTA_NOT, "not"));
    instructions.append(new Instruction(1, "00100011", Instruction::VOLTA_NEG, "neg"));
    instructions.append(new Instruction(1, "00100100", Instruction::VOLTA_INC, "inc"));
    instructions.append(new Instruction(1, "00100101", Instruction::VOLTA_DEC, "dec"));
    instructions.append(new Instruction(1, "00110001", Instruction::VOLTA_ASR, "asr"));
    instructions.append(new Instruction(1, "00110010", Instruction::VOLTA_ASL, "asl"));
    instructions.append(new Instruction(1, "00110011", Instruction::VOLTA_ROR, "ror"));
    instructions.append(new Instruction(1, "00110100", Instruction::VOLTA_ROL, "rol"));
    instructions.append(new Instruction(1, "01000001", Instruction::VOLTA_SZ,  "sz"));
    instructions.append(new Instruction(1, "01000010", Instruction::VOLTA_SNZ, "snz"));
    instructions.append(new Instruction(1, "01000011", Instruction::VOLTA_SPL, "spl"));
    instructions.append(new Instruction(1, "01000100", Instruction::VOLTA_SMI, "smi"));
    instructions.append(new Instruction(1, "01000101", Instruction::VOLTA_SPZ, "spz"));
    instructions.append(new Instruction(1, "01000110", Instruction::VOLTA_SMZ, "smz"));
    instructions.append(new Instruction(1, "01010001", Instruction::VOLTA_SEQ, "seq"));
    instructions.append(new Instruction(1, "01010010", Instruction::VOLTA_SNE, "sne"));
    instructions.append(new Instruction(1, "01010011", Instruction::VOLTA_SGR, "sgr"));
    instructions.append(new Instruction(1, "01010100", Instruction::VOLTA_SLS, "sls"));
    instructions.append(new Instruction(1, "01010101", Instruction::VOLTA_SGE, "sge"));
    instructions.append(new Instruction(1, "01010110", Instruction::VOLTA_SLE, "sle"));
    instructions.append(new Instruction(1, "01100110", Instruction::VOLTA_RTS, "rts"));
    instructions.append(new Instruction(2, "011100..", Instruction::VOLTA_PSH, "psh a"));
    instructions.append(new Instruction(2, "011101..", Instruction::VOLTA_POP, "pop a")); // TODO: Código estava repetido, falar com o Weber
    instructions.append(new Instruction(2, "100100..", Instruction::VOLTA_JMP, "jmp a"));
    instructions.append(new Instruction(2, "101000..", Instruction::VOLTA_JSR, "jsr a"));
    instructions.append(new Instruction(1, "1111....", Instruction::VOLTA_HLT, "hlt"));



    //////////////////////////////////////////////////
    // Initialize addressing modes
    //////////////////////////////////////////////////

    addressingModes.append(new AddressingMode("......00", AddressingMode::DIRECT,       AddressingMode::NO_PATTERN));
    addressingModes.append(new AddressingMode("......01", AddressingMode::INDIRECT,     "(.*),i"));
    addressingModes.append(new AddressingMode("......10", AddressingMode::IMMEDIATE,    "#(.*)"));
    addressingModes.append(new AddressingMode("......11", AddressingMode::INDEXED_BY_PC, "(.*),pc"));
}

void VoltaMachine::executeInstruction(Instruction *&instruction, AddressingMode::AddressingModeCode addressingModeCode, QString, int immediateAddress)
{
    int value1, value2, result, carry;
    Instruction::InstructionCode instructionCode;
    instructionCode = (instruction) ? instruction->getInstructionCode() : Instruction::NOP;

    switch (instructionCode)
    {

    //////////////////////////////////////////////////
    // Arithmethic and logic
    //////////////////////////////////////////////////

    case Instruction::VOLTA_ADD:
        value1 = stackPop();
        value2 = stackPop();
        result = value1 + value2;
        stackPush(result);
        break;

    case Instruction::VOLTA_SUB:
        value1 = stackPop();
        value2 = stackPop();
        result = value1 - value2;
        stackPush(result);
        break;

    case Instruction::VOLTA_AND:
        value1 = stackPop();
        value2 = stackPop();
        result = value1 & value2;
        stackPush(result);
        break;

    case Instruction::VOLTA_OR:
        value1 = stackPop();
        value2 = stackPop();
        result = value1 | value2;
        stackPush(result);
        break;

    case Instruction::VOLTA_CLR:
        stackPop();
        stackPush(0);
        break;

    case Instruction::VOLTA_NOT:
        value1 = stackPop();
        result = ~value1;
        stackPush(result);
        break;

    case Instruction::VOLTA_NEG:
        value1 = stackPop();
        result = -value1;
        stackPush(result);
        break;

    case Instruction::VOLTA_INC:
        value1 = stackPop();
        result = value1 + 1;
        stackPush(result);
        break;

    case Instruction::VOLTA_DEC:
        value1 = stackPop();
        result = value1 - 1;
        stackPush(result);
        break;

    case Instruction::VOLTA_ASR:
        value1 = stackPop();
        result = value1 >> 1;
        stackPush(result);
        break;

    case Instruction::VOLTA_ASL:
        value1 = stackPop();
        result = value1 << 1;
        stackPush(result);
        break;

    case Instruction::VOLTA_ROR:
        value1 = stackPop();
        carry = value1 & 0x01; // Extract least significant bit
        result = (value1 >> 1) | (carry << 7);
        stackPush(result);
        break;

    case Instruction::VOLTA_ROL:
        value1 = stackPop();
        carry = value1 & 0x80; // Extract most significant bit
        result = (value1 << 1) | (carry << 7);
        stackPush(result);
        break;



    //////////////////////////////////////////////////
    // Conditionals
    //////////////////////////////////////////////////

    case Instruction::VOLTA_SZ:
        value1 = stackPop();
        if (value1 == 0)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SNZ:
        value1 = stackPop();
        if (value1 != 0)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SPL:
        value1 = stackPop();
        if (value1 > 0)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SMI:
        value1 = stackPop();
        if (value1 < 0)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SPZ:
        value1 = stackPop();
        if (value1 >= 0)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SMZ:
        value1 = stackPop();
        if (value1 <= 0)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SEQ:
        value1 = stackPop();
        value2 = stackPop();
        if (value1 == value2)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SNE:
        value1 = stackPop();
        value2 = stackPop();
        if (value1 != value2)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SGR:
        value1 = stackPop();
        value2 = stackPop();
        if (value1 > value2)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SLS:
        value1 = stackPop();
        value2 = stackPop();
        if (value1 < value2)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SGE:
        value1 = stackPop();
        value2 = stackPop();
        if (value1 >= value2)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SLE:
        value1 = stackPop();
        value2 = stackPop();
        if (value1 <= value2)
            skipNextInstruction();
        break;



    //////////////////////////////////////////////////
    // Others
    //////////////////////////////////////////////////

    case Instruction::VOLTA_RTS:
        value1 = stackPop();
        setPCValue(value1);
        break;

    case Instruction::VOLTA_PSH:
        value1 = memoryGetOperandValue(immediateAddress, addressingModeCode);
        stackPush(value1);
        break;

    case Instruction::VOLTA_POP:
        value1 = stackPop();
        memoryWrite(memoryGetOperandAddress(immediateAddress, addressingModeCode), value1);
        break;

    case Instruction::VOLTA_JMP:
        setPCValue(memoryGetJumpAddress(immediateAddress, addressingModeCode));
        break;

    case Instruction::VOLTA_JSR:
        stackPush(getPCValue());
        setPCValue(memoryGetJumpAddress(immediateAddress, addressingModeCode));
        break;

    case Instruction::VOLTA_HLT:
        setRunning(false);
        break;

    default:
        break;
    }
    instructionCount++;
}

void VoltaMachine::skipNextInstruction()
{
    // Read first byte
    int fetchedValue = memoryReadNext();
    Instruction *instruction = getInstructionFromValue(fetchedValue);

    // Skip next byte
    if (instruction && instruction->getNumBytes() == 2)
        incrementPCValue();
}



//////////////////////////////////////////////////
// Stack push/pop with access count
//////////////////////////////////////////////////

int VoltaMachine::getStackSize()
{
    return stack.size();
}

void VoltaMachine::stackPush(int value)
{
    accessCount++; // TODO: Stack conta acessos?
    SP->setValue(SP->getValue() - 1); // Decrement SP
    setStackValue(SP->getValue(), value);
}

int VoltaMachine::stackPop()
{
    accessCount++;
    int value = getStackValue(SP->getValue());
    SP->incrementValue();
    return value;
}



//////////////////////////////////////////////////
// Getters/setters, clear
//////////////////////////////////////////////////

void VoltaMachine::setStackSize(int size)
{
    stack.fill(nullptr, size);
    stackChanged.fill(true, size);

    for (int i=0; i<stack.size(); i++)
        stack[i] = new Byte();
}

int VoltaMachine::getStackValue(int address)
{
    return stack[address]->getValue();
}

void VoltaMachine::setStackValue(int address, int value)
{
    stack[address]->setValue(value & 0xFF);
}

void VoltaMachine::clearStack()
{
    for (int i=0; i<stack.size(); i++)
        setStackValue(i, 0);
}

int VoltaMachine::getSPValue()
{
    return SP->getValue();
}

void VoltaMachine::clear()
{
    clearStack();
    Machine::clear();
}
