#include "ahmesmachine.h"

AhmesMachine::AhmesMachine()
{
    registers = QVector<Register*>(2);
    QVector<Register*>::iterator i;
    for(i = registers.begin(); i != registers.end(); i++) {
        *i = new Register();
    }
    PC = registers[1];
    AC = registers[0];
    AC->setValue(0);

    memory = QVector<Byte*>(256);
    QVector<Byte*>::iterator j;
    int k;
    for(k = 0, j = memory.begin(); j != memory.end();k++, j++) {
        *j = new Byte();
    }

    // 0 - n; 1 - z; 2 - v; 3 - c; 4 - b;
    flags = QVector<Bit*>(5, false);

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

/**
 * @brief AhmesMachine::printStatusDebug
 * debug function to see a status of the machine on the terminal, without gui implementations
 */
void AhmesMachine::printStatusDebug()
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

void AhmesMachine::load(QString filename) {

}

void AhmesMachine::save(QString filename){

}

void AhmesMachine::step() {
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
        case 0x70:
            AC->setValue(AC->getValue() - memory[operand->getValue()]->getValue());
            break;
        case 0x80:
            PC->setValue(operand->getValue());
            break;
        case 0x90:
            if(this->flags[0]) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0x94:
            if(!this->flags[0]) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0x98:
            if(this->flags[2]) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0x9C:
            if(!this->flags[2]) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xA0:
            if(this->flags[1]) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xA4:
            if(!this->flags[1]) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xB0:
            if(this->flags[3]) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xB4:
            if(!this->flags[3]) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xB8:
            if(this->flags[4]) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xBC:
            if(!this->flags[4]) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xF0:
            this->running = false;
        default:
            break;
        }
        PC->setValue(PC->getValue() + 1);
}

void AhmesMachine::run() {
    this->running = true;
    while (this->running) {
        this->step();
    }
}

// Returne true if the two bytes has the same sign. false if don't
bool AhmesMachine::hasSameSign(Byte n1, Byte n2)
{
    if ((n1->getValue() >= 128 && n2->getValue() >= 128) || (n1->getValue() <= 127 && n2->getValue() <= 127)) return true;
    else return false;
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
const Instruction* AhmesMachine::getInstructionFromMnemonic(QString desiredInstruction) {
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getMnemonic() == desiredInstruction) {
            return (*i);
        }
    }
    return NULL;
}
