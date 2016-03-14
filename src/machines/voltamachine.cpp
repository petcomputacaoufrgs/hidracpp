#include "voltamachine.h"

VoltaMachine::VoltaMachine()
{
    identifier = "VLT"; // TODO: Confirmar identificador

    //////////////////////////////////////////////////
    // Initialize registers
    //////////////////////////////////////////////////

    registers.append(new Register("PC", "", 8, false));
    registers.append(new Register("SP", "", 6, false));

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
    instructions.append(new Instruction(2, "100000..", Instruction::VOLTA_POP, "pop a"));
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
    int value1, value2, result, bit;
    Instruction::InstructionCode instructionCode;
    instructionCode = (instruction) ? instruction->getInstructionCode() : Instruction::NOP;

    switch (instructionCode)
    {

    //////////////////////////////////////////////////
    // Arithmethic and logic (two operands)
    //////////////////////////////////////////////////

    case Instruction::VOLTA_ADD:
        value1 = stackPop();
        value2 = stackPop();
        result = value2 + value1;
        stackPush(result);
        break;

    case Instruction::VOLTA_SUB:
        value1 = stackPop();
        value2 = stackPop();
        result = value2 - value1;
        stackPush(result);
        break;

    case Instruction::VOLTA_AND:
        value1 = stackPop();
        value2 = stackPop();
        result = value2 & value1;
        stackPush(result);
        break;

    case Instruction::VOLTA_OR:
        value1 = stackPop();
        value2 = stackPop();
        result = value2 | value1;
        stackPush(result);
        break;



    //////////////////////////////////////////////////
    // Arithmethic and logic (one operand)
    //////////////////////////////////////////////////

    case Instruction::VOLTA_CLR:
        setStackValue(SP->getValue(), 0); // Replace top of stack
        accessCount++; // Count single access
        break;

    case Instruction::VOLTA_NOT:
        value1 = stackPop();
        result = ~value1;
        stackPush(result);
        break;

    case Instruction::VOLTA_NEG:
        value1 = toSigned(stackPop());
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
        result |= (value1 & 0x80); // Preserve sign (arithmetic shift)
        stackPush(result);
        break;

    case Instruction::VOLTA_ASL:
        value1 = stackPop();
        result = value1 << 1;
        stackPush(result);
        break;

    case Instruction::VOLTA_ROR:
        value1 = stackPop();
        bit = value1 & 0x01; // Extract least significant bit
        result = (value1 >> 1) | (bit << 7);
        stackPush(result);
        break;

    case Instruction::VOLTA_ROL:
        value1 = stackPop();
        bit = value1 & 0x80; // Extract most significant bit
        result = (value1 << 1) | (bit >> 7);
        stackPush(result);
        break;



    //////////////////////////////////////////////////
    // Conditionals (one operand)
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
        value1 = toSigned(stackPop());
        if (value1 > 0)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SMI:
        value1 = toSigned(stackPop());
        if (value1 < 0)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SPZ:
        value1 = toSigned(stackPop());
        if (value1 >= 0)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SMZ:
        value1 = toSigned(stackPop());
        if (value1 <= 0)
            skipNextInstruction();
        break;



    //////////////////////////////////////////////////
    // Conditionals (two operands)
    //////////////////////////////////////////////////

    case Instruction::VOLTA_SEQ:
        value1 = stackPop();
        value2 = stackPop();
        if (value2 == value1)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SNE:
        value1 = stackPop();
        value2 = stackPop();
        if (value2 != value1)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SGR:
        value1 = toSigned(stackPop());
        value2 = toSigned(stackPop());
        if (value2 > value1)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SLS:
        value1 = toSigned(stackPop());
        value2 = toSigned(stackPop());
        if (value2 < value1)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SGE:
        value1 = toSigned(stackPop());
        value2 = toSigned(stackPop());
        if (value2 >= value1)
            skipNextInstruction();
        break;

    case Instruction::VOLTA_SLE:
        value1 = toSigned(stackPop());
        value2 = toSigned(stackPop());
        if (value2 <= value1)
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

    default: // NOP etc.
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

void VoltaMachine::generateDescriptions()
{
    descriptions["nop"] = "Nenhuma operação.";

    // Arithmetic and logic (two operands)
    descriptions["add"] = "Desempilha A e B, empilha B + A.";
    descriptions["sub"] = "Desempilha A e B, empilha B - A.";
    descriptions["and"] = "Desempilha A e B, empilha resultado do 'e' lógico entre seus bits.";
    descriptions["or"]  = "Desempilha A e B, empilha resultado do 'ou' lógico entre seus bits.";

    // Arithmetic and logic (one operand)
    descriptions["clr"] = "Zera o valor no topo da pilha.";
    descriptions["not"] = "Inverte (complementa) o valor dos bits do topo da pilha.";
    descriptions["neg"] = "Troca o sinal do valor em complemento de 2 do topo da pilha entre positivo e negativo.";
    descriptions["inc"] = "Incrementa em uma unidade o topo da pilha.";
    descriptions["dec"] = "Decrementa de uma unidade o topo da pilha.";
    descriptions["asr"] = "Realiza shift aritmético dos bits do topo da pilha para a direita, mantendo seu sinal em complemento de dois (bit mais significativo).";
    descriptions["asl"] = "Realiza shift aritmético dos bits do topo da pilha para a esquerda, preenchendo com zero o bit menos significativo.";
    descriptions["ror"] = "Realiza rotação para a direita dos bits do topo da pilha.";
    descriptions["rol"] = "Realiza rotação para a esquerda dos bits do topo da pilha.";

    // Conditionals (one operand)
    descriptions["sz"]  = "Retira o topo da pilha e pula a próxima instrução se for igual a zero (skip on zero).";
    descriptions["snz"] = "Retira o topo da pilha e pula a próxima instrução se for diferente de zero (skip on not zero).";
    descriptions["spl"] = "Retira o topo da pilha e pula a próxima instrução se for positivo (skip on plus).";
    descriptions["smi"] = "Retira o topo da pilha e pula a próxima instrução se for negativo (skip on minus).";
    descriptions["spz"] = "Retira o topo da pilha e pula a próxima instrução se for maior ou igual a zero (skip on plus/zero).";
    descriptions["smz"] = "Retira o topo da pilha e pula a próxima instrução se for menor ou igual a zero (skip on minus/zero).";

    // Conditionals (two operands)
    descriptions["seq"] = "Retira A e B da pilha e pula a próxima instrução se B = A (skip if equal).";
    descriptions["sne"] = "Retira A e B da pilha e pula a próxima instrução se B ≠ A (skip if not equal).";
    descriptions["sgr"] = "Retira A e B da pilha e pula a próxima instrução se B &gt; A (skip if greater than).";
    descriptions["sls"] = "Retira A e B da pilha e pula a próxima instrução se B &lt; A (skip if less than).";
    descriptions["sge"] = "Retira A e B da pilha e pula a próxima instrução se B ≥ A (skip if greater than/equal to).";
    descriptions["sle"] = "Retira A e B da pilha e pula a próxima instrução se B ≤ A (skip if less than/equal to).";

    // Others
    descriptions["rts"]   = "Desvia para o endereço indicado pelo topo da pilha, desempilhando-o (retorno de sub-rotina).";
    descriptions["psh a"] = "Empilha o valor do endereço de memória 'a'.";
    descriptions["pop a"] = "Desempilha o topo da pilha, armazenando-o no endereço de memória 'a'.";
    descriptions["jmp a"] = "Desvia a execução para o endereço 'a' (desvio incondicional).";
    descriptions["jsr a"] = "Empilha PC e desvia para o endereço 'a' (desvio para sub-rotina).";
    descriptions["hlt"]   = "Termina a execução.";
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
    accessCount++;
    SP->incrementValue();
    setStackValue(SP->getValue(), value);
}

int VoltaMachine::stackPop()
{
    accessCount++;
    int value = getStackValue(SP->getValue());
    SP->setValue(SP->getValue() - 1); // Decrement SP
    return value;
}



//////////////////////////////////////////////////
// Getters/setters, clear
//////////////////////////////////////////////////

static bool isPowerOfTwo(unsigned int value)
{
    while (((value % 2) == 0) && value > 1) // While value is even and greater than one
        value /= 2;

    return (value == 1);
}

void VoltaMachine::setStackSize(int size)
{
    stack.fill(nullptr, size);
    stackChanged.fill(true, size);

    for (int i=0; i<stack.size(); i++)
        stack[i] = new Byte();

    Q_ASSERT(isPowerOfTwo(size)); // Size must be a power of two for the mask to work
    stackMask = (size - 1);
}

int VoltaMachine::getStackValue(int address)
{
    return stack[address & stackMask]->getValue();
}

void VoltaMachine::setStackValue(int address, int value)
{
    stack[address & stackMask]->setValue(value & 0xFF);
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

void VoltaMachine::clearAfterBuild()
{
    clearStack();
    Machine::clearAfterBuild();
}
