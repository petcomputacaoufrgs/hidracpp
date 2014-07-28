#include "ahmesmachine.h"

AhmesMachine::AhmesMachine()
{
    registers = QVector<Register*>(2);
    for (int k = 0; k < registers.size(); k++) registers[k] = new Register();

    PC = registers[1];
    AC = registers[0];
    AC->setValue(0);

    memory = QVector<Byte*>(256);
    for (int k = 0; k < memory.size(); k++) memory[k] = new Byte(0);

    flags = QVector<Bit*>(5);
    for (int k = 0; k < flags.size(); k++) flags[k] = new Bit();

    N = flags[0];
    Z = flags[1];
    V = flags[2];
    C = flags[3];
    B = flags[4];

   // std::cout << "\n----->>>> " << N->getValue();

    //test code
    memory[0]->setValue(32);
    memory[1]->setValue(128);
    memory[2]->setValue(227);
    memory[3]->setValue(227);
//    memory[4]->setValue(16);
//    memory[5]->setValue(130);
    memory[4]->setValue(240);
    memory[128]->setValue(255);
    memory[129]->setValue(255);
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
    char tempAC = AC->getValue();
    if(actualInstruction->getNumberOfArguments() == 1) {
        PC->setValue(PC->getValue() + 1);
        operand = memory[PC->getValue()];
    }

    std::cout << "\n-----> " << actualInstruction->getValue();
    switch (actualInstruction->getValue()) {
        case 0:
            break;
        case 0x10:
            memory[operand->getValue()]->setValue((unsigned char)AC->getValue());
            break;
        case 0x20:
            AC->setValue(memory[operand->getValue()]->getValue());
            break;
        case 0x30:
            AC->setValue((AC->getValue() + memory[operand->getValue()]->getValue()) & 0xFF);
            updateFlags(tempAC, operand->getValue(), false, false);
            break;
        case 0x40:
            AC->setValue(AC->getValue() | memory[operand->getValue()]->getValue());
            updateFlags(tempAC, operand->getValue(), false, false);
            break;
        case 0x50:
            AC->setValue(AC->getValue() & memory[operand->getValue()]->getValue());
            updateFlags(tempAC, operand->getValue(), false, false);
            break;
        case 0x60:
            AC->setValue(AC->getValue() ^ 0xFF);
            updateFlags(tempAC, operand->getValue(), false, false);
            break;
        case 0x70:
            AC->setValue((AC->getValue() - memory[operand->getValue()]->getValue()) & 0xFF);
            updateFlags(tempAC, operand->getValue(), false, true);
            break;
        case 0x80:
            PC->setValue(operand->getValue());
            break;
        case 0x90:
            if(N->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0x94:
            if(!N->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0x98:
            if(V->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0x9C:
            if(!V->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xA0:
            if(Z->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xA4:
            if(!Z->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xB0:
            if(C->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xB4:
            if(!C->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xB8:
            if(B->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xBC:
            if(!B->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xE0:
            AC->setValue(AC->getValue() >> 1);
            updateFlags(tempAC, operand->getValue(), true, false);
            break;
        case 0xE1:
            AC->setValue(AC->getValue() << 1);
            updateFlags(tempAC, operand->getValue(), true, false);
            break;
        case 0xE2:
            AC->setValue((AC->getValue() >> 1) & 0xFF);
            if (C->getValue()) AC->setValue(AC->getValue() | 0x80);
            updateFlags(tempAC, operand->getValue(), true, false);
            break;
        case 0xE3:
            AC->setValue((AC->getValue() << 1) & 0xFF);
            if (C->getValue()) AC->setValue(AC->getValue() | 0x1);
            updateFlags(tempAC, operand->getValue(), true, false);
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
    while (this->running && this->PC->getValue() <= 255) this->step();

    std::cout << "\n";
    std::cout << "\n ----> A: " << AC->getValue();
    std::cout << "\n ----> N: " << N->getValue();
    std::cout << "\n ----> Z: " << Z->getValue();
    std::cout << "\n ----> V: " << V->getValue();
    std::cout << "\n ----> C: " << C->getValue();
    std::cout << "\n ----> B: " << B->getValue();
}

void AhmesMachine::updateFlags(unsigned char preAC, unsigned char operand, bool sour, bool sub)
{
    if (AC->getValue() == 0) Z->setValue(1);
    else Z->setValue(0);
    if (AC->getValue() >= 128) N->setValue(1);
    else N->setValue(0);

    if (sub)
    {
        if (preAC < 128 && AC->getValue() >= 128) B->setValue(1);
        else B->setValue(0);
    }

    if (preAC >= 128)
    {
        if (AC->getValue() < 128)
        {
            V->setValue(0);
            C->setValue(1);
        }
        else
        {
            V->setValue(0);
            C->setValue(0);
        }
    }
    else
    {
        if (AC->getValue() >= 128)
        {
            V->setValue(1);
            C->setValue(0);
        }
        else
        {
            V->setValue(0);
            C->setValue(0);
        }
    }

    if (preAC >= 128 && operand >= 128)
    {
        if (AC->getValue() < 128)
        {
            C->setValue(1);
            V->setValue(1);
        }
        else
        {
            C->setValue(1);
            V->setValue(0);
        }
        if (sub) V->setValue(0);
    }

    if (sour && AC->getValue() == 0) C->setValue(1);
}


void AhmesMachine::assemble(QString filename) {
       Machine *outputMachine = new AhmesMachine();
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
               atual = (Instruction*) getInstructionFromMnemonic(line.at(0));
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
               atual = (Instruction*) getInstructionFromMnemonic(line.at(0));
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

const Instruction* AhmesMachine::getInstructionFromValue(int desiredInstruction) {
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getValue() == desiredInstruction) {
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
