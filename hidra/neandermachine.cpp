#include "neandermachine.h"

NeanderMachine::NeanderMachine()
{
    registers = new QVector<Register*>(2);
    registers->operator [](0) = new Register();
    registers->operator [](1) = new Register();
    PC = registers->at(1);
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
