#include "ahmesmachine.h"

AhmesMachine::AhmesMachine()
{
    registers = QVector<Register*>(2);
    QVector<Register*>::iterator i;
    for(i = registers.begin(); i != registers.end(); i++) {
        *i = new Register();
    }
    PC = registers[1];

    memory = QVector<Byte*>(256);
    QVector<Byte*>::iterator j;
    int k;
    for(k = 0, j = memory.begin(); j != memory.end();k++, j++) {
        *j = new Byte(k);
    }

    instructions = QVector<Instruction*>(24);
    instructions[0] = new Instruction("nop", 0, 0);
    instructions[1] = new Instruction("sta", 16, 1);
    instructions[2] = new Instruction("lda", 32, 1);
    instructions[3] = new Instruction("add", 48, 1);
    instructions[4] = new Instruction( "or", 64, 1);
    instructions[5] = new Instruction("and", 80, 1);
    instructions[6] = new Instruction("not", 96, 0);
    instructions[7] = new Instruction("sub", 112, 1);
    instructions[8] = new Instruction("jmp", 128, 1);
    instructions[9] = new Instruction( "jn", 144, 1);
    instructions[10] = new Instruction(  "jp", 148, 1);
    instructions[11] = new Instruction(  "jv", 152, 1);
    instructions[12] = new Instruction( "jnv", 156, 1);
    instructions[13] = new Instruction(  "jz", 160, 1);
    instructions[14] = new Instruction( "jnz", 164, 1);
    instructions[15] = new Instruction(  "jc", 176, 1);
    instructions[16] = new Instruction( "jnc", 180, 1);
    instructions[17] = new Instruction(  "jb", 184, 1);
    instructions[18] = new Instruction( "jnb", 188, 1);
    instructions[19] = new Instruction( "shr", 224, 0);
    instructions[20] = new Instruction( "shl", 225, 0);
    instructions[21] = new Instruction( "ror", 226, 0);
    instructions[22] = new Instruction( "rol", 227, 0);
    instructions[23] = new Instruction( "hlt", 240, 0);
}

void AhmesMachine::load(QString filename) {

}

void AhmesMachine::save(QString filename){

}

void AhmesMachine::step() {
    const Instruction *actualInstruction = getInstructionFromValue(memory[PC->getValue()]->getValue());
    Byte *operand;

    if (actualInstruction->getNumberOfArguments() == 1)
    {
        PC->setValue(PC->getValue() + 1);
        operand = memory[PC->getValue()];
    }
}

void AhmesMachine::run() {

}

void AhmesMachine::assemble(QString filename) {

}

const Instruction* AhmesMachine::getInstructionFromValue(int desiredInstruction) {
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getValue() == (desiredInstruction & 0xF0)) {
            return (*i);
        }
    }
    return NULL;
}
