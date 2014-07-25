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

    flags = QVector<Bit*>(2);
    QVector<Bit*>::iterator k;
    for(k = flags.begin(); k != flags.end();k++) {
        *k = new Bit();
    }
    N = flags[0];
    Z = flags[1];
    Z->setValue(true);

    memory = QVector<Byte*>(MEM_SIZE);
    QVector<Byte*>::iterator j;
    for(j = memory.begin(); j != memory.end();j++) {
        *j = new Byte();
    }

    //test code
//    memory[0]->setValue(32);
//    memory[1]->setValue(128);
//    memory[2]->setValue(48);
//    memory[3]->setValue(129);
//    memory[4]->setValue(16);
//    memory[5]->setValue(130);
//    memory[6]->setValue(240);
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
    std::cout << "N: "  << (N->getValue()? "1" : "0") << "\t";
    std::cout << "Z: "  << (Z->getValue()? "1" : "0") << std::endl;
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
}

void NeanderMachine::load(QString filename) {

}

void NeanderMachine::save(QString filename){

}

void NeanderMachine::step() {
    const Instruction* actualInstruction = getInstructionFromValue(memory[PC->getValue()]->getValue());
    Byte *operand;
    if(actualInstruction->getNumberOfArguments() == 1) {    //se a instrucao tiver no end 255 vai dar bug, mas como
        PC->setValue(PC->getValue() + 1);                   //o objetivo eh utilizar o montador, ele soh faz isso editando
        operand = memory[PC->getValue()];                   //o binario diretamente, portanto, deixei propositalmente :p
    }
    switch (actualInstruction->getValue() & 0xF0) {
    case 0:
        break;
    case 0x10:
        memory[operand->getValue()]->setValue((unsigned char)AC->getValue());
        break;
    case 0x20:
        AC->setValue(memory[operand->getValue()]->getValue());
        N->setValue(AC->getValue() > MAX_VALUE_SIGN);
        Z->setValue(AC->getValue() == 0);
        break;
    case 0x30:
        AC->setValue((AC->getValue() + memory[operand->getValue()]->getValue()) & MAX_VALUE);
        N->setValue(AC->getValue() > MAX_VALUE_SIGN);
        Z->setValue(AC->getValue() == 0);
        break;
    case 0x40:
        AC->setValue(AC->getValue() | memory[operand->getValue()]->getValue());
        N->setValue(AC->getValue() > MAX_VALUE_SIGN);
        Z->setValue(AC->getValue() == 0);
        break;
    case 0x50:
        AC->setValue(AC->getValue() & memory[operand->getValue()]->getValue());
        N->setValue(AC->getValue() > MAX_VALUE_SIGN);
        Z->setValue(AC->getValue() == 0);
        break;
    case 0x60:
        AC->setValue(AC->getValue() ^ 0xFF);
        N->setValue(AC->getValue() > MAX_VALUE_SIGN);
        Z->setValue(AC->getValue() == 0);
        break;
    case 0x80:
        PC->setValue(operand->getValue());
        break;
    case 0x90:  //JN
        if(N->getValue()) {
            PC->setValue(operand->getValue());
        }
        break;
    case 0xA0:  //JZ
        if(Z->getValue()) {
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
    if(PC->getValue() >=  MEM_SIZE) {
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

    QFile sourceFile(filename);
    sourceFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString source = sourceFile.readAll();
    QStringList sourceLines = source.split("\n", QString::SkipEmptyParts);  //separa o arquivo fonte por linhas de codigo
    QStringList::iterator i;
    for(i = sourceLines.begin(); i != sourceLines.end(); i++) {
        (*i) = (*i).split(';').at(0).toLower().simplified();    //elimina os comentarios do codigo
    }
    sourceLines.removeAll("");  //remove as linhas em branco

    std::cout << "1=========" << std::endl;
    foreach (QString tmp, sourceLines) {
        std::cout << tmp.toStdString() << std::endl;
    }
    std::cout << "2=========" << std::endl;
    //tipo aqui tenho que tratar os DB, DW, DAB, DAW e etc
    int pc = 0;
    QVector<Byte *> memory = outputMachine->getMemory();
    for(i = sourceLines.begin(); i != sourceLines.end(); i++) {
        Instruction *atual;

        QStringList line = (*i).split(" ");
        if(line.at(0) == "org") {
            pc = line.at(1).toInt();
        } else if(line.at(0) == "db") {
            memory[pc++]->setValue((unsigned char)line.last().toInt());
        } else if(line.at(0) == "dw") {
            int word = line.last().toInt();
            memory[pc++]->setValue((unsigned char)((word & 0xFF00)>>8) );
            memory[pc++]->setValue((unsigned char)(word & 0x00FF) );
        } else if(line.at(0) == "dab") {

        } else if(line.at(0) == "daw") {

        } else {
            atual = getInstructionFromMnemonic(line.at(0));
            line.replace(0, QString::number(atual->getValue()));
            foreach (QString byte, line) {
                memory[pc++]->setValue((unsigned char)byte.toInt());
            }
        }
    }

    foreach (QString tmp, sourceLines) {
        std::cout << tmp.toStdString() << std::endl;
    }

    outputMachine->setMemory(memory);
    outputMachine->printStatusDebug();
    QString outputFilename = filename.split('.').at(0) + ".mem";
    outputMachine->save(outputFilename);
}

const bool NeanderMachine::validateInstructions(QStringList instructionList)
{
    return true;
}

Instruction* NeanderMachine::getInstructionFromValue(int desiredInstruction) {
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getValue() == (desiredInstruction & 0xF0)) {
            return (*i);
        }
    }
    return NULL;
}
Instruction* NeanderMachine::getInstructionFromMnemonic(QString desiredInstruction) {
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getMnemonic() == desiredInstruction) {
            return (*i);
        }
    }
    return NULL;
}
