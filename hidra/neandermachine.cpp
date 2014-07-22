#include "neandermachine.h"

NeanderMachine::NeanderMachine()
{
    registers = QVector<Register*>(2);
    QVector<Register*>::iterator i;
    for(i = registers.begin(); i != registers.end(); i++) {
        *i = new Register();
    }
    PC = registers[1];
    AC = registers[0];
    AC->setValue(77);

    memory = QVector<Byte*>(MEM_SIZE);
    QVector<Byte*>::iterator j;
    int k;
    for(k = 0, j = memory.begin(); j != memory.end();k++, j++) {
        *j = new Byte();
    }

    //test code
    memory[0]->setValue(32);
    memory[1]->setValue(128);
    memory[2]->setValue(48);
    memory[3]->setValue(129);
    memory[4]->setValue(16);
    memory[5]->setValue(130);
    memory[6]->setValue(240);
    memory[128]->setValue(4);
    memory[129]->setValue(8);
    memory[130]->setValue(2);

    // initialize instructions
    instructions = QVector<Instruction*>(11);
    instructions[0]  = new Instruction("nop",   0, 0);
    instructions[1]  = new Instruction("sta",  16, 1);
    instructions[2]  = new Instruction("lda",  32, 1);
    instructions[3]  = new Instruction("add",  48, 1);
    instructions[4]  = new Instruction( "or",  64, 1);
    instructions[5]  = new Instruction("and",  80, 1);
    instructions[6]  = new Instruction("not",  96, 0);
    instructions[7]  = new Instruction("jmp", 128, 1);
    instructions[8]  = new Instruction( "jn", 144, 1);
    instructions[9]  = new Instruction( "jz", 160, 1);
    instructions[10] = new Instruction("hlt", 240, 0);

}

/**
 * @brief NeanderMachine::printStatusDebug
 * debug function to see a status of the machine on the terminal, without gui implementations
 */
void NeanderMachine::printStatusDebug()
{
    std::cout << "PC: " << PC->getValue() << std::endl;
    std::cout << "AC: " << AC->getValue() << std::endl;
    int j;
    QVector<Byte*>::iterator i;
    for(j = 0, i = memory.begin(); i != memory.end(); i++, j=j+1) {
        const Instruction *actual = getInstructionFromValue((int)(*i)->getValue());
        if(actual != NULL) {
            std::cout << j << ": " << actual->getMnemonic().toStdString() << " " << (int)(*i)->getValue() << std::endl;
            for(int k = 0; k < actual->getNumberOfArguments(); k++) {
                i++;
                j++;
                std::cout << j << ": " << (int)(*i)->getValue() << std::endl;
            }
        } else {
            std::cout << j << ": " << (int)(*i)->getValue() << std::endl;
        }
    }

    QVector<Instruction*>::iterator i2;
    for(j = 0, i2 = instructions.begin(); i2 != instructions.end(); i2++, j=j+1) {
        std::cout << j << ": " << (*i2)->getMnemonic().toStdString() << " " << (*i2)->getValue() <<  std::endl;
    }
}

void NeanderMachine::load(QString filename) {

}

void NeanderMachine::save(QString filename){

}

void NeanderMachine::step() {
    const Instruction* actualInstruction = getInstructionFromValue(memory[PC->getValue()]->getValue());
    Byte *operand;
    if(actualInstruction->getNumberOfArguments() == 1) {
        PC->setValue(PC->getValue() + 1);
        operand = memory[PC->getValue()];
    }
    switch (actualInstruction->getValue() & 0xF0) {
    case 0:
        break;
    case 0x10:
        memory[operand->getValue()]->setValue((unsigned char)AC->getValue());
        break;
    case 0x20:
        AC->setValue(memory[operand->getValue()]->getValue());
        break;
    case 0x30:
        AC->setValue(AC->getValue() + memory[operand->getValue()]->getValue());
        break;
    case 0x40:
        AC->setValue(AC->getValue() | memory[operand->getValue()]->getValue());
        break;
    case 0x50:
        AC->setValue(AC->getValue() & memory[operand->getValue()]->getValue());
        break;
    case 0x60:
        AC->setValue(AC->getValue() ^ 0xFF);
        break;
    case 0x80:
        PC->setValue(operand->getValue());
        break;
    case 0x90:
        if(flags[0]) {
            PC->setValue(operand->getValue());
        }
        break;
    case 0xA0:
        if(flags[1]) {
            PC->setValue(operand->getValue());
        }
        break;
    case 0xF0:
        this->running = false;
        break;
    default:
        break;
    }
    PC->setValue(PC->getValue() + 1);
    if(PC->getValue() ==  MEM_SIZE) {
        this->running = false;
    }
}

void NeanderMachine::run() {
    this->running = true;
    while (this->running) {
        this->step();
    }
}

void NeanderMachine::assemble(QString filename) {
    NeanderMachine *outputMachine = new NeanderMachine();


    QString outputFilename = filename.section('.', 0) + ".mem";
    outputMachine->save(outputFilename);
}

bool NeanderMachine::validateInstructions(QStringList instructionList)
{

}

const Instruction* NeanderMachine::getInstructionFromValue(int desiredInstruction) {
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getValue() == (desiredInstruction & 0xF0)) {
            return (*i);
        }
    }
    return NULL;
}
const Instruction* NeanderMachine::getInstructionFromMnemonic(QString desiredInstruction) {
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getMnemonic() == desiredInstruction) {
            return (*i);
        }
    }
    return NULL;
}
