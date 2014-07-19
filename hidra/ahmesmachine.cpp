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

    instructions = QVector<Instruction*>(11);
    instructions[0] = new Instruction("nop", 0, 0);
    instructions[1] = new Instruction("sta", 16, 1);
    instructions[2] = new Instruction("lda", 32, 1);
    instructions[3] = new Instruction("add", 48, 1);
    instructions[4] = new Instruction( "or", 64, 1);
    instructions[5] = new Instruction("and", 80, 1);
    instructions[6] = new Instruction("not", 96, 0);
    instructions[7] = new Instruction("jmp", 128, 1);
    instructions[8] = new Instruction( "jn", 144, 1);
    instructions[9] = new Instruction( "jz", 160, 1);
    instructions[10] = new Instruction("hlt", 240, 0);
}

void Machine::load(QString filename) {

}

void Machine::save(QString filename){

}

void Machine::step() {

}

void Machine::run() {

}

void Machine::assemble(QString filename) {

}
