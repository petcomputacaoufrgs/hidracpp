#include "ahmesmachine.h"

AhmesMachine::AhmesMachine()
{
    registers = QVector<Register*>(2);
    QVector<Register*>::iterator i;
    for(i = registers.begin(); i != registers.end(); i++) {
        *i = new Register();
        (*i)->setNumOfBits(8);
    }
    PC = registers[1];
    AC = registers[0];

    memory = QVector<Byte*>(MEM_SIZE);

    assemblerMemory = QVector<Byte*>(MEM_SIZE);
    reserved = QVector<bool>(MEM_SIZE);
    correspondingLine = QVector<int>(MEM_SIZE);

    QVector<Byte*>::iterator j;
    for(j = memory.begin(); j != memory.end();j++) {
        *j = new Byte();
    }
    for(j = assemblerMemory.begin(); j != assemblerMemory.end();j++) {
        *j = new Byte();
    }

    flags = QVector<Bit*>(5);
    for (int k = 0; k < flags.size(); k++) flags[k] = new Bit();

    N = flags[0];
    Z = flags[1];
    V = flags[2];
    C = flags[3];
    B = flags[4];

    Z->setValue(true);

    instructions = QVector<Instruction*>(24);
    instructions[0]  = new Instruction("nop",   0, 0, 1);
    instructions[1]  = new Instruction("sta",  16, 1, 2);
    instructions[2]  = new Instruction("lda",  32, 1, 2);
    instructions[3]  = new Instruction("add",  48, 1, 2);
    instructions[4]  = new Instruction( "or",  64, 1, 2);
    instructions[5]  = new Instruction("and",  80, 1, 2);
    instructions[6]  = new Instruction("not",  96, 0, 1);
    instructions[7]  = new Instruction("sub", 112, 1, 2);
    instructions[8]  = new Instruction("jmp", 128, 1, 2);
    instructions[9]  = new Instruction( "jn", 144, 1, 2);
    instructions[10] = new Instruction( "jp", 148, 1, 2);
    instructions[11] = new Instruction( "jv", 152, 1, 2);
    instructions[12] = new Instruction("jnv", 156, 1, 2);
    instructions[13] = new Instruction( "jz", 160, 1, 2);
    instructions[14] = new Instruction("jnz", 164, 1, 2);
    instructions[15] = new Instruction( "jc", 176, 1, 2);
    instructions[16] = new Instruction("jnc", 180, 1, 2);
    instructions[17] = new Instruction( "jb", 184, 1, 2);
    instructions[18] = new Instruction("jnb", 188, 1, 2);
    instructions[20] = new Instruction("shl", 225, 0, 1);
    instructions[21] = new Instruction("ror", 226, 0, 1);
    instructions[22] = new Instruction("rol", 227, 0, 1);
    instructions[23] = new Instruction("hlt", 240, 0, 1);
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
    bool err = false;
    QFile memFile(filename);
    memFile.open(QFile::ReadOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);
    unsigned char buffer;
    unsigned char machineIdentifier[4] = {3, 'A', 'H', 'D'};    //machine identifier
    int i;
    for(i = 0; i < 4; i++) {
        stream >> buffer;
        if(buffer != machineIdentifier[i]) {
            err = true;
            break;
        }
    }
    i = 0;
    if(!err) {
        while(!stream.atEnd()) {
            stream >> buffer;
            memory[i++]->setValue((unsigned char)buffer);
            stream >> buffer;   //ignore the byte 0
        }
    }
    memFile.close();
}

void AhmesMachine::save(QString filename){
    QFile memFile(filename);
    memFile.open(QFile::WriteOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << (unsigned char)3 << (unsigned char)'A' << (unsigned char)'H' << (unsigned char)'D'; //prefixo identificador da maquina (basicamente o que muda em cada maquina

    foreach (Byte *byte, memory) {
        stream << byte->getValue() << (unsigned char)0;
    }
    memFile.close();
}

void AhmesMachine::step() {
    const Instruction* currentInstruction = getInstructionFromValue(memory[PC->getValue()]->getValue());
    Byte *operand;

    unsigned char previousAC = AC->getValue();
    if(currentInstruction->getNumberOfArguments() == 1) {
        PC->incrementValue();
        operand = memory[PC->getValue()];
    }

    PC->incrementValue();
    if(PC->getValue() == 0) { // ADICIONAR BREAKPOINT
        this->running = false;
    }

    switch (currentInstruction->getValue()) { // Arredondar para baixo
        case 0x00: // NOP
            break;
        case 0x10: // STA
            memory[operand->getValue()]->setValue((unsigned char)AC->getValue());
            break;
        case 0x20: // LDA
            AC->setValue(memory[operand->getValue()]->getValue());
            break;
        case 0x30: // ADD
            AC->setValue((AC->getValue() + memory[operand->getValue()]->getValue()) & 0xFF);
            updateFlags(previousAC, operand->getValue(), true, false, false, false);
            break;
        case 0x40: // OR
            AC->setValue(AC->getValue() | memory[operand->getValue()]->getValue());
            updateFlags(previousAC, operand->getValue(), false, false, false, false);
            break;
        case 0x50: // AND
            AC->setValue(AC->getValue() & memory[operand->getValue()]->getValue());
            updateFlags(previousAC, operand->getValue(), false, false, false, false);
            break;
        case 0x60: // NOT
            AC->setValue(AC->getValue() ^ 0xFF);
            updateFlags(previousAC, operand->getValue(), false, false, false, false);;
            break;
        case 0x70: // SUB
            AC->setValue((AC->getValue() - memory[operand->getValue()]->getValue()) & 0xFF);
            updateFlags(previousAC, operand->getValue(), false, true, false, false);
            break;
        case 0x80: // JMP
            PC->setValue(operand->getValue());
            break;
        case 0x90: // JN
            if(N->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0x94: // JP
            if(!N->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0x98: // JV
            if(V->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0x9C: // JNV
            if(!V->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xA0: // JZ
            if(Z->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xA4: // JNZ
            if(!Z->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xB0: // JC
            if(C->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xB4: // JNC
            if(!C->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xB8: // JB
            if(B->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xBC: // JNB
            if(!B->getValue()) {
                PC->setValue(operand->getValue());
            }
            break;
        case 0xE0: // SHR
            AC->setValue(AC->getValue() >> 1);
            updateFlags(previousAC, operand->getValue(), false, false, true, false);
            break;
        case 0xE1: // SHL
            AC->setValue(AC->getValue() << 1);
            updateFlags(previousAC, operand->getValue(), false, false, true, false);
            break;
        case 0xE2: // ROR
            AC->setValue((AC->getValue() >> 1) & 0xFF);
            updateFlags(previousAC, operand->getValue(), false, false, false, true);
            break;
        case 0xE3: // ROL
            AC->setValue((AC->getValue() << 1) & 0xFF);
            if (C->getValue()) AC->setValue(AC->getValue() | 0x1);
            updateFlags(previousAC, operand->getValue(), false, false, false, true);
            break;
        case 0xF0: // HLT
            this->running = false;
        default:
            break;
        }
}

void AhmesMachine::run() {
    this->running = true;
    while (this->running && this->PC->getValue() <= 255) {
        this->step();
    }
}

int AhmesMachine::getMemorySize()
{
    return MEM_SIZE;
}

void AhmesMachine::updateFlags(unsigned char previous_AC, unsigned char operand, bool addition, bool subtraction, bool shift_rotate_left, bool shift_rotate_right)
{
    Z->setValue(AC->getValue() == 0);
    N->setValue(AC->getValue() >= 128);

    if (addition) // Update carry
    {
        C->setValue(AC->getValue() < previous_AC); // Result is less than initial value (Flag C is unsigned)
    }

    if (subtraction) // Update borrow
    {
        B->setValue(previous_AC < operand); // Subtraction needs borrow (Flag B is unsigned)
    }

    if (addition || subtraction)
    {
        if ((previous_AC >= 128 && operand >= 128 && AC->getValue()  < 128) || // Overflow: negative operands with positive result
            (previous_AC  < 128 && operand  < 128 && AC->getValue() >= 128))   // Overflow: positive operands with negative result
        {
            V->setValue(1);
        }
        else
        {
            V->setValue(0);
        }
    }

    if (shift_rotate_left)
    {
        C->setValue((previous_AC & 0b10000000) != 0);
    }

    if (shift_rotate_right)
    {
        C->setValue((previous_AC & 0b00000001) != 0);
    }
}


//void AhmesMachine::assemble(QString filename) {
//       Machine *outputMachine = new AhmesMachine();
//       QHash<QString, int> labelsMap;
//       QFile sourceFile(filename);
//       sourceFile.open(QIODevice::ReadOnly | QIODevice::Text);
//       QString source = sourceFile.readAll();
//       QStringList sourceLines = source.split("\n", QString::SkipEmptyParts);  //separa o arquivo fonte por linhas de codigo
//       QStringList::iterator i;
//       for(i = sourceLines.begin(); i != sourceLines.end(); i++) {
//           (*i) = (*i).split(';').at(0).toLower().simplified();    //elimina os comentarios do codigo
//       }
//       sourceLines.removeAll("");  //remove as linhas em branco

//       int pc = 0;
//       QVector<Byte *> memory = outputMachine->getMemory();
//       for(i = sourceLines.begin(); i != sourceLines.end(); i++) {
//           QStringList line = (*i).split(" ", QString::SkipEmptyParts);
//            std::cout << line.join(" ").toStdString() << std::endl;
//           Instruction *atual;
//           if (line.at(0).contains(QRegExp("(.*:)"))) {
//               QString key = line.first();
//               (*i).replace(key, "");
//               key.chop(1);
//               labelsMap.insert(key, pc);
//           } else if(line.at(0) == "org") {
//               pc = line.at(1).toInt();
//           } else if(line.at(0) == "db") {
//               pc++;
//           } else if(line.at(0) == "dw") {
//               pc += 2;
//           } else if(line.at(0) == "dab") {
//               if(line.at(1).contains(QRegExp("(\\d+\\(\\d+\\))"))) {
//                   QStringList dabValue = line.at(1).split("(");
//                   dabValue.last().chop(1);
//                   pc += dabValue.first().toInt();
//               }
//           } else if(line.at(0) == "daw") {
//               if(line.at(1).contains(QRegExp("(\\d+\\(\\d+\\))"))) {
//                   QStringList dawValue = line.at(1).split("(");
//                   dawValue.last().chop(1);
//                   pc += dawValue.first().toInt() * 2;
//               }
//           } else {
//               atual = (Instruction*) getInstructionFromMnemonic(line.at(0));
//               pc += 1 + atual->getNumberOfArguments();
//           }
//       }
//       sourceLines.removeAll("");  //remove as linhas em branco
//       foreach(QString key, labelsMap.keys()) {
//           sourceLines.replaceInStrings(QString(key), QString::number(labelsMap.value(key)));
//       }
//       pc = 0;
//       for(i = sourceLines.begin(); i != sourceLines.end(); i++) {
//           Instruction *atual;

//           QStringList line = (*i).split(" ", QString::SkipEmptyParts);

//           if (line.at(0).contains(QRegExp("(.*:)"))) {
//               //skip
//           } else if(line.at(0) == "org") {
//               pc = line.at(1).toInt();
//           } else if(line.at(0) == "db") {
//               memory[pc++]->setValue((unsigned char)line.last().toInt());
//           } else if(line.at(0) == "dw") {
//               int word = line.last().toInt();
//               memory[pc++]->setValue((unsigned char)((word & 0xFF00)>>8) );
//               memory[pc++]->setValue((unsigned char)(word & 0x00FF) );
//           } else if(line.at(0) == "dab") {
//               if(line.at(1).contains(QRegExp("(\\d+\\(\\d+\\))"))) {
//                   QStringList dabValue = line.at(1).split("(");
//                   dabValue.last().chop(1);
//                   for(int i = 0; i < dabValue.first().toInt(); i++) {
//                       memory[pc++]->setValue((unsigned char) dabValue.last().toInt());
//                   }
//               }
//           } else if(line.at(0) == "daw") {
//               if(line.at(1).contains(QRegExp("(\\d+\\(\\d+\\))"))) {
//                   QStringList dabValue = line.at(1).split("(");
//                   dabValue.last().chop(1);
//                   for(int i = 0; i < dabValue.first().toInt(); i++) {
//                       int word = dabValue.last().toInt();
//                       memory[pc++]->setValue((unsigned char)((word & 0xFF00)>>8));
//                       memory[pc++]->setValue((unsigned char)(word & 0x00FF) );
//                   }
//               }
//           } else {
//               atual = (Instruction*) getInstructionFromMnemonic(line.at(0));
//               line.replace(0, QString::number(atual->getValue()));
//               foreach (QString byte, line) {
//                   memory[pc++]->setValue((unsigned char)byte.toInt());
//               }
//           }
//       }

//       outputMachine->setMemory(memory);
//       outputMachine->printStatusDebug();
//       QString outputFilename = filename.split('.').at(0) + ".mem";
//       outputMachine->save(outputFilename);
//}

Instruction* AhmesMachine::getInstructionFromValue(int value)
{
    QVector<Instruction*>::iterator i;

    // Ignore "don't care" bits:
    if (value < 0x90 || value >= 0xF0)
        value &= 0b11110000;
    else if (value < 0xC0)
        value &= 0b11111100; // Special jumps
    else
        value &= 0b11110011; // Shift and rotate

    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getValue() == value) {
            return (*i);
        }
    }
    return NULL;
}
Instruction* AhmesMachine::getInstructionFromMnemonic(QString desiredInstruction) {
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getMnemonic() == desiredInstruction) {
            return (*i);
        }
    }
    return NULL;
}
