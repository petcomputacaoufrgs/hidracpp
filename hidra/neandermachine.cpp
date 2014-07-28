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

    memory = QVector<Byte*>(256);
    QVector<Byte*>::iterator j;
    int k;
    for(k = 0, j = memory.begin(); j != memory.end();k++, j++) {
        *j = new Byte(k);
    }

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

void NeanderMachine::printStatusDebug()
{
    std::cout << "PC: " << PC->getValue() << std::endl;
    std::cout << "AC: " << AC->getValue() << std::endl;
    int j;
    QVector<Byte*>::iterator i;
    for(j = 0, i = memory.begin(); i != memory.end(); i++, j=j+1) {
        const Instruction *actual = getInstructionFromValue((int)(*i)->getValue());
        if(actual != NULL) {
            std::cout << j << ": " << actual->getMnemonic().toStdString() << std::endl;
            for(int k = 0; k < actual->getNumberOfArguments(); k++) {
                i++;
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

void Machine::load(QString filename) {

}

void Machine::save(QString filename){

}

void NeanderMachine::assemble(QString filename) {
    NeanderMachine *outputMachine = new NeanderMachine();
    QHash<QString, int> labelsMap;
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
    int pc = 0;
    QVector<Byte *> memory = outputMachine->getMemory();
    for(i = sourceLines.begin(); i != sourceLines.end(); i++) {
        QStringList line = (*i).split(" ", QString::SkipEmptyParts);
         std::cout << line.join(" ").toStdString() << std::endl;
        Instruction *atual;
        if (line.at(0).contains(QRegExp("(.*:)"))) {
            QString key = line.first();
            (*i).replace(key, "");
            key.chop(1);
            labelsMap.insert(key, pc);
        } else if(line.at(0) == "org") {
            pc = line.at(1).toInt();
        } else if(line.at(0) == "db") {
            pc++;
        } else if(line.at(0) == "dw") {
            pc += 2;
        } else if(line.at(0) == "dab") {
            if(line.at(1).contains(QRegExp("(\\d+\\(\\d+\\))"))) {
                QStringList dabValue = line.at(1).split("(");
                dabValue.last().chop(1);
                pc += dabValue.first().toInt();
            }
        } else if(line.at(0) == "daw") {
            if(line.at(1).contains(QRegExp("(\\d+\\(\\d+\\))"))) {
                QStringList dawValue = line.at(1).split("(");
                dawValue.last().chop(1);
                pc += dawValue.first().toInt() * 2;
            }
        } else {
            atual = getInstructionFromMnemonic(line.at(0));
            pc += 1 + atual->getNumberOfArguments();
        }
    }
    sourceLines.removeAll("");  //remove as linhas em branco
    foreach(QString key, labelsMap.keys()) {
        std::cout << key.toStdString() << ": " << labelsMap.value(key) << std::endl;
    }
    foreach(QString key, labelsMap.keys()) {
        sourceLines.replaceInStrings(QString(key), QString::number(labelsMap.value(key)));
    }
    foreach (QString tmp, sourceLines) {
        std::cout << tmp.toStdString() << std::endl;
    }
    std::cout << "3=========" << std::endl;
    pc = 0;
    for(i = sourceLines.begin(); i != sourceLines.end(); i++) {
        Instruction *atual;

        QStringList line = (*i).split(" ", QString::SkipEmptyParts);
        std::cout << line.join(" ").toStdString() << std::endl;
        if (line.at(0).contains(QRegExp("(.*:)"))) {
            //skip
        } else if(line.at(0) == "org") {
            pc = line.at(1).toInt();
        } else if(line.at(0) == "db") {
            memory[pc++]->setValue((unsigned char)line.last().toInt());
        } else if(line.at(0) == "dw") {
            int word = line.last().toInt();
            memory[pc++]->setValue((unsigned char)((word & 0xFF00)>>8) );
            memory[pc++]->setValue((unsigned char)(word & 0x00FF) );
        } else if(line.at(0) == "dab") {
            if(line.at(1).contains(QRegExp("(\\d+\\(\\d+\\))"))) {
                QStringList dabValue = line.at(1).split("(");
                dabValue.last().chop(1);
                for(int i = 0; i < dabValue.first().toInt(); i++) {
                    memory[pc++]->setValue((unsigned char) dabValue.last().toInt());
                }
            }
        } else if(line.at(0) == "daw") {
            if(line.at(1).contains(QRegExp("(\\d+\\(\\d+\\))"))) {
                QStringList dabValue = line.at(1).split("(");
                dabValue.last().chop(1);
                for(int i = 0; i < dabValue.first().toInt(); i++) {
                    int word = dabValue.last().toInt();
                    memory[pc++]->setValue((unsigned char)((word & 0xFF00)>>8));
                    memory[pc++]->setValue((unsigned char)(word & 0x00FF) );
                }
            }
        } else {
            atual = getInstructionFromMnemonic(line.at(0));
            line.replace(0, QString::number(atual->getValue()));
            foreach (QString byte, line) {
                memory[pc++]->setValue((unsigned char)byte.toInt());
            }
        }
    }
}

void Machine::run() {

}

void Machine::assemble(QString filename) {

}

const Instruction* Machine::getInstructionFromValue(int desiredInstruction) {
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getValue() == (desiredInstruction & 0xF0)) {
            return (*i);
        }
    }
    return NULL;
}
