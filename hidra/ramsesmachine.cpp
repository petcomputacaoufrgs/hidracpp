#include "ramsesmachine.h"

RamsesMachine::RamsesMachine()
{
    registers = QVector<Register*>(4);
    QVector<Register*>::iterator i;
    for (i = registers.begin(); i != registers.end(); i++) {
        *i = new Register();
    }
    RA = registers[0];
    RB = registers[1];
    RX = registers[2];
    PC = registers[3];

    flags = QVector<Bit*>(3);
    QVector<Bit*>::iterator k;
    for (k = flags.begin(); k != flags.end(); k++) {
        *k = new Bit();
    }
    N = flags[0];
    Z = flags[1];
    C = flags[2];
    Z->setValue(true);
    C->setValue(true);

    memory = QVector<Byte*>(MEM_SIZE);
    QVector<Byte*>::iterator j;
    for (j = memory.begin(); j != memory.end(); j++) {
        *j = new Byte();
    }

    instructions = QVector<Instruction*>(16);
    instructions[0]  = new Instruction("nop",   0, 0);
    instructions[1]  = new Instruction("str",  16, 1);
    instructions[2]  = new Instruction("lda",  32, 1);
    instructions[3]  = new Instruction("add",  48, 1);
    instructions[4]  = new Instruction( "or",  64, 1);
    instructions[5]  = new Instruction("and",  80, 1);
    instructions[6]  = new Instruction("not",  96, 0);
    instructions[7]  = new Instruction("sub", 112, 1);
    instructions[8]  = new Instruction("jmp", 128, 1);
    instructions[9]  = new Instruction( "jn", 144, 1);
    instructions[10] = new Instruction( "jz", 160, 1);
    instructions[11] = new Instruction( "jc", 176, 1);
    instructions[12] = new Instruction("jsr", 192, 1);
    instructions[13] = new Instruction("neg", 208, 0);
    instructions[14] = new Instruction("shr", 224, 0);
    instructions[15] = new Instruction("hlt", 240, 0);
}

void RamsesMachine::printStatusDebug()
{
    std::cout << std::endl;
    std::cout << "RA: " << (int)RA->getValue() << std::endl;;
    std::cout << "RB: " << (int)RB->getValue() << std::endl;;
    std::cout << "RX: " << (int)RX->getValue() << std::endl;;
}

void RamsesMachine::load(QString filename)
{
    bool err = false;
    QFile memFile(filename);
    memFile.open(QFile::ReadOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);
    unsigned char buffer;
    unsigned char IDBuffer[4];
    unsigned char olderMachineIdentifier[4] = {3, 'N', 'D', 'R'};    //Ramses is code-compatible with Neander
    unsigned char machineIdentifier[4] = {3, 'R', 'M', 'S'};    //machine identifier
    stream >> IDBuffer[0];
    stream >> IDBuffer[1];
    stream >> IDBuffer[2];
    stream >> IDBuffer[3];
    int i;
    for(i = 0; i < 4; i++) {
        if(IDBuffer[i] != machineIdentifier[i]) {
            err = true;
            break;
        }
    }
    for(i = 0; i < 4; i++) {
        if(IDBuffer[i] != olderMachineIdentifier[i]) {
            err = true;
            break;
        }
    }
    i = 0;
    if(!err) {
        while(!stream.atEnd()) {
            stream >> buffer;
            memory[i++]->setValue((unsigned char)buffer);
            stream >> buffer;
        }
    }
    memFile.close();
}

void RamsesMachine::save(QString filename)
{
    QFile memFile(filename);
    memFile.open(QFile::WriteOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << (unsigned char)3 << (unsigned char)'R' << (unsigned char)'M' << (unsigned char)'S'; //prefixo identificador da maquina (basicamente o que muda em cada maquina

    foreach (Byte *byte, memory) {
        stream << byte->getValue() << (unsigned char)0;
    }
    memFile.close();
}

void RamsesMachine::step() {
    const Instruction *currentInstruction = getInstructionFromValue(memory[PC->getValue()]->getValue());
    Byte *operand;

    if(currentInstruction->getNumberOfArguments() == 1) {
        PC->incrementValue();

        Byte *endOperand = NULL;
        switch (currentInstruction->getValue() & 0x03) {
        case 0x00: // modo de enderecamento direto
            endOperand = memory[PC->getValue()];
            break;
        case 0x01: // modo indireto
            Byte *endPointer;
            endPointer = memory[PC->getValue()];
            endOperand = memory[endPointer->getValue()];
            break;
        case 0x02: // modo imediato
            endOperand->setValue(PC->getValue());
            break;
        case 0x03: // modo indexado
            endOperand = memory[PC->getValue() + RX->getValue()];
            break;
        }

        operand = memory[endOperand->getValue()];
    }

    int reg = ((currentInstruction->getValue() & 0x0C) >> 2); // os bits 2 e 3 da instrucao indicam o registrador a ser usado;

    PC->incrementValue();
    if(PC->getValue() == 0) { // ADICIONAR BREAKPOINT
        this->running = false;
    }

    switch (currentInstruction->getValue() & 0xF0) {
    case 0x00: // nop
        break;
    case 0x10: // str
        operand->setValue(registers[reg]->getValue());
        break;
    case 0x20: // ldr
        registers[reg]->setValue(operand->getValue());
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        C->setValue(false);
        break;
    case 0x30: // add
        registers[reg]->setValue((operand->getValue() + registers[reg]->getValue()) & MAX_VALUE);
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        C->setValue((registers[reg]->getValue() + operand->getValue()) > MAX_VALUE);
        break;
    case 0x40: // or
        registers[reg]->setValue(operand->getValue() | registers[reg]->getValue());
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        C->setValue(false);
        break;
    case 0x50: // and
        registers[reg]->setValue(operand->getValue() & registers[reg]->getValue());
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        C->setValue(false);
        break;
    case 0x60: // not
        registers[reg]->setValue(~registers[reg]->getValue());
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        C->setValue(false);
        break;
    case 0x70: // sub
        registers[reg]->setValue((registers[reg]->getValue() - operand->getValue()) & MAX_VALUE);
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        C->setValue((registers[reg]->getValue() - operand->getValue()) < 0);
        break;
    case 0x80: // jmp
        PC->setValue(operand->getValue());
        break;
    case 0x90: // jn
        if (N) PC->setValue(operand->getValue());
        break;
    case 0xA0: // jz
        if (Z) PC->setValue(operand->getValue());
        break;
    case 0xB0: // jc
        if (C) PC->setValue(operand->getValue());
        break;
    case 0xC0: // jsr
        memory[operand->getValue()]->setValue(PC->getValue());
        PC->setValue(operand->getValue() + 1);
        break;
    case 0xD0: // neg
        registers[reg]->setValue(~registers[reg]->getValue() + 1);
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        break;
    case 0xE0: // shr
        C->setValue((registers[reg]->getValue() & 0x01) == 1);
        registers[reg]->setValue(registers[reg]->getValue() >> 1);
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        break;
    case 0xF0: // hlt
        this->running = false;
        break;
    }
}

void RamsesMachine::run()
{
    this->running = true;
    while (this->running) {
        this->step();
    }
}

void RamsesMachine::assemble(QString filename)
{

}

const Instruction* RamsesMachine::getInstructionFromValue(int desiredInstruction) {
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getValue() == (desiredInstruction & 0xF0)) {
            return (*i);
        }
    }
    return NULL;
}
const Instruction* RamsesMachine::getInstructionFromMnemonic(QString desiredInstruction) {
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getMnemonic() == desiredInstruction) {
            return (*i);
        }
    }
    return NULL;
}
