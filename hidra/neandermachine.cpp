#include "neandermachine.h"

NeanderMachine::NeanderMachine()
{
    registers = QVector<Register*>(2);
    QVector<Register*>::iterator i;
    for(i = registers.begin(); i != registers.end(); i++) {
        *i = new Register();
    }
    PC = registers[1];

    memory = QVector<Byte*>(256);
    QVector<Byte*>::iterator j;
    for(j = memory.begin(); j != memory.end(); j++) {
        *j = new Byte();
    }
}

void NeanderMachine::printStatusDebug()
{
    std::cout << "PC: " << PC->getValue() << std::endl;
    int j;
    QVector<Byte*>::iterator i;
    for(j = 0, i = memory.begin(); i != memory.end(); i++, j=j+1) {
        (*i)->setValue(j);
        std::cout << j << ": " << (int)(*i)->getValue() << std::endl;
    }
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
