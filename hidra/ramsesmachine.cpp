#include "ramsesmachine.h"

RamsesMachine::RamsesMachine()
{
    registers = QVector<Register*>(4);
    QVector<Register*>::iterator i;
    for (i = registers.begin(); i != registers.end(); i++) {
        *i = new Register();
    }
    RA = registers[0];
    RB = registers[1];
    RX = registers[2];
    PC = registers[3];

    flags = QVector<Bit*>(3);
    QVector<Bit*>::iterator k;
    for (k = flags.begin(); k != flags.end(); k++) {
        *k = new Bit();
    }
    N = flags[0];
    Z = flags[1];
    C = flags[2];
    Z->setValue(true);
    C->setValue(true);

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

    instructions = QVector<Instruction*>(16);
    instructions[0]  = new Instruction("nop",   0, 0, 1);
    instructions[1]  = new Instruction("str",  16, 2, 2);
    instructions[2]  = new Instruction("ldr",  32, 2, 2);
    instructions[3]  = new Instruction("add",  48, 2, 2);
    instructions[4]  = new Instruction( "or",  64, 2, 2);
    instructions[5]  = new Instruction("and",  80, 2, 2);
    instructions[6]  = new Instruction("not",  96, 1, 1);
    instructions[7]  = new Instruction("sub", 112, 2, 2);
    instructions[8]  = new Instruction("jmp", 128, 1, 2);
    instructions[9]  = new Instruction( "jn", 144, 1, 2);
    instructions[10] = new Instruction( "jz", 160, 1, 2);
    instructions[11] = new Instruction( "jc", 176, 1, 2);
    instructions[12] = new Instruction("jsr", 192, 1, 2);
    instructions[13] = new Instruction("neg", 208, 1, 1);
    instructions[14] = new Instruction("shr", 224, 1, 1);
    instructions[15] = new Instruction("hlt", 240, 0, 1);
}

void RamsesMachine::printStatusDebug()
{
    std::cout << std::endl;
    std::cout << "RA: " << (int)RA->getValue() << std::endl;;
    std::cout << "RB: " << (int)RB->getValue() << std::endl;;
    std::cout << "RX: " << (int)RX->getValue() << std::endl;;
}

void RamsesMachine::load(QString filename)
{
    bool err = false;
    QFile memFile(filename);
    memFile.open(QFile::ReadOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);
    unsigned char buffer;
    unsigned char IDBuffer[4];
    unsigned char olderMachineIdentifier[4] = {3, 'N', 'D', 'R'};    //Ramses is code-compatible with Neander
    unsigned char machineIdentifier[4] = {3, 'R', 'M', 'S'};    //machine identifier
    stream >> IDBuffer[0];
    stream >> IDBuffer[1];
    stream >> IDBuffer[2];
    stream >> IDBuffer[3];
    int i;
    for(i = 0; i < 4; i++) {
        if(IDBuffer[i] != machineIdentifier[i]) {
            err = true;
            break;
        }
    }
    for(i = 0; i < 4; i++) {
        if(IDBuffer[i] != olderMachineIdentifier[i]) {
            err = true;
            break;
        }
    }
    i = 0;
    if(!err) {
        while(!stream.atEnd()) {
            stream >> buffer;
            memory[i++]->setValue((unsigned char)buffer);
            stream >> buffer;
        }
    }
    memFile.close();
}

void RamsesMachine::save(QString filename)
{
    QFile memFile(filename);
    memFile.open(QFile::WriteOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << (unsigned char)3 << (unsigned char)'R' << (unsigned char)'M' << (unsigned char)'S'; //prefixo identificador da maquina (basicamente o que muda em cada maquina

    foreach (Byte *byte, memory) {
        stream << byte->getValue() << (unsigned char)0;
    }
    memFile.close();
}

void RamsesMachine::step() {
    const Instruction *currentInstruction = getInstructionFromValue(memory[PC->getValue()]->getValue());
    Byte *operand;

    if(currentInstruction->getSize() == 2) {
        PC->incrementValue();

        Byte *endOperand = NULL;
        switch (currentInstruction->getValue() & 0x03) {
        case 0x00: // modo de enderecamento direto
            endOperand = memory[PC->getValue()];
            break;
        case 0x01: // modo indireto
            Byte *endPointer;
            endPointer = memory[PC->getValue()];
            endOperand = memory[endPointer->getValue()];
            break;
        case 0x02: // modo imediato
            endOperand->setValue(PC->getValue());
            break;
        case 0x03: // modo indexado
            endOperand = memory[PC->getValue() + RX->getValue()];
            break;
        }

        operand = memory[endOperand->getValue()];
    }

    int reg = ((currentInstruction->getValue() & 0x0C) >> 2); // os bits 2 e 3 da instrucao indicam o registrador a ser usado;

    PC->incrementValue();
    if(PC->getValue() == 0) { // ADICIONAR BREAKPOINT
        this->running = false;
    }

    switch (currentInstruction->getValue()) {
    case 0x00: // nop
        break;
    case 0x10: // str
        operand->setValue(registers[reg]->getValue());
        break;
    case 0x20: // ldr
        registers[reg]->setValue(operand->getValue());
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        C->setValue(false);
        break;
    case 0x30: // add
        registers[reg]->setValue((operand->getValue() + registers[reg]->getValue()) & MAX_VALUE);
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        C->setValue((registers[reg]->getValue() + operand->getValue()) > MAX_VALUE);
        break;
    case 0x40: // or
        registers[reg]->setValue(operand->getValue() | registers[reg]->getValue());
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        C->setValue(false);
        break;
    case 0x50: // and
        registers[reg]->setValue(operand->getValue() & registers[reg]->getValue());
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        C->setValue(false);
        break;
    case 0x60: // not
        registers[reg]->setValue(~registers[reg]->getValue());
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        C->setValue(false);
        break;
    case 0x70: // sub
        registers[reg]->setValue((registers[reg]->getValue() - operand->getValue()) & MAX_VALUE);
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        C->setValue((registers[reg]->getValue() - operand->getValue()) < 0);
        break;
    case 0x80: // jmp
        PC->setValue(operand->getValue());
        break;
    case 0x90: // jn
        if (N) PC->setValue(operand->getValue());
        break;
    case 0xA0: // jz
        if (Z) PC->setValue(operand->getValue());
        break;
    case 0xB0: // jc
        if (C) PC->setValue(operand->getValue());
        break;
    case 0xC0: // jsr
        memory[operand->getValue()]->setValue(PC->getValue());
        PC->setValue(operand->getValue() + 1);
        break;
    case 0xD0: // neg
        registers[reg]->setValue(~registers[reg]->getValue() + 1);
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        break;
    case 0xE0: // shr
        C->setValue((registers[reg]->getValue() & 0x01) == 1);
        registers[reg]->setValue(registers[reg]->getValue() >> 1);
        N->setValue(registers[reg]->getValue() > MAX_VALUE_SIGN);
        Z->setValue(registers[reg]->getValue() == 0);
        break;
    case 0xF0: // hlt
        this->running = false;
        break;
    }
}

void RamsesMachine::run()
{
    this->running = true;
    while (this->running) {
        this->step();
    }
}

int RamsesMachine::getMemorySize()
{
    return MEM_SIZE;
}

Machine::ErrorCode RamsesMachine::mountInstruction(QString mnemonic, QString arguments, QHash<QString, int> &labelPCMap)
{
    Instruction *instruction = getInstructionFromMnemonic(mnemonic);
    QStringList argumentList = arguments.split(" ", QString::SkipEmptyParts);
    int numberOfArguments = instruction->getNumberOfArguments();
    bool ok;

    int registerID = 0;
    int addressingMode = 0;

    // Check if correct number of arguments:
    if (argumentList.size() != numberOfArguments)
        return wrongNumberOfArguments;

    // If first argument is a register (every instruction that has arguments, except jumps):
    if (numberOfArguments > 0 && !(mnemonic.startsWith("j")))
    {
        if (argumentList.first() == "a")
            registerID = 0;
        else if (argumentList.first() == "b")
            registerID = 1;
        else if (argumentList.first() == "x")
            registerID = 2;
        else
            return invalidArgument;
    }

    // If last argument is an address (every two-byte instruction):
    if (instruction->getSize() == 2)
    {
        // Extract and remove addressing mode:
        if (argumentList.last().endsWith(",i"))
        {
            argumentList.last() = argumentList.last().replace(",i", "");
            addressingMode = 1; // Direct
        }
        else if (argumentList.last().startsWith("#"))
        {
            argumentList.last() = argumentList.last().replace("#", "");
            addressingMode = 2; // Indirect
        }
        else if (argumentList.last().endsWith(",x"))
        {
            argumentList.last() = argumentList.last().replace(",x", "");
            addressingMode = 3; // Indexed
        }
    }

    // Write first byte (instruction with register and addressing mode):
    assemblerMemory[PC->getValue()]->setValue(instruction->getValue() + (registerID << 2) + addressingMode);
    PC->incrementValue();

    // If instruction has two bytes, write second byte:
    if (instruction->getSize() > 1)
    {
        // Convert possible label to number:
        if (labelPCMap.contains(argumentList.last()))
            argumentList.last() = QString::number(labelPCMap.value(argumentList.last()));

        // Check if valid address:
        if (!isValidAddress(argumentList.last()))
            return invalidArgument;

        // Write address argument:
        assemblerMemory[PC->getValue()]->setValue(argumentList.last().toInt(&ok, 0));
        PC->incrementValue();
    }

    return noError;
}
/*
void RamsesMachine::assemble(QString sourceCode)
{
//    NeanderMachine *outputMachine = new NeanderMachine();
    QHash<QString, int> labelsMap;
    QFile sourceFile(sourceCode);
    sourceFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString source = sourceFile.readAll();
    QStringList sourceLines = source.split("\n", QString::SkipEmptyParts);  //separa o arquivo fonte por linhas de codigo
    QStringList::iterator i;
    for(i = sourceLines.begin(); i != sourceLines.end(); i++) {
        (*i) = (*i).split(';').at(0).toLower().simplified();    //elimina os comentarios do codigo
    }
    unsigned int sumAux;
    sourceLines.removeAll("");  //remove as linhas em branco

    int pc = 0;
    //QVector<Byte *> memory = outputMachine->getMemory();
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
        sourceLines.replaceInStrings(QString(key), QString::number(labelsMap.value(key)));
    }
    pc = 0;
    for(i = sourceLines.begin(); i != sourceLines.end(); i++) {
        Instruction *atual;

        QStringList line = (*i).split(" ", QString::SkipEmptyParts);

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
            memory[pc]->setValue((unsigned char)line[0].toInt());
            for(int i=1; i<line.length();i++)
            {

              //  if(line[i]=="a" || line[i]=="A" && i==1)
              //{
              //    sumAux=memory[pc]->getValue();
              //    memory[pc]->setValue();
              //}
              //else if((line[i]=="b" || line[i]=="B") && i==1)
                {
                    sumAux= (int)memory[pc]->getValue() + 4;
                    memory[pc]->setValue((unsigned char)sumAux);
                    sumAux= (int)memory[pc]->getValue();
                }
                else if((line[i]=="x" || line[i]=="X") && i==1)
                {
                    memory[pc++]->setValue(memory[pc]->getValue()+5);
                }
                else if(line[i].contains("#"))
                {
                    sumAux= (int)memory[pc]->getValue() + 2;
                    memory[pc++]->setValue((unsigned char)sumAux);
                    line[i].replace("#","");
                    memory[pc++]->setValue((unsigned char)line[i].toInt());
                }
                else if(line[i].contains(",i$") || line[i].contains(",I$"))
                {
                    memory[pc]->setValue(memory[pc]->getValue()+(unsigned char)1);
                    memory[pc++]->setValue((unsigned char)line[i].toInt());
                }
                else if(line[i].contains(",x$") || line[i].contains(",X$"))
                {
                    memory[pc]->setValue(memory[pc]->getValue()+(unsigned char)3);
                    memory[pc++]->setValue((unsigned char)line[i].toInt());
                }



            //foreach (QString byte, line) {
                //
            }
        }

    }
    //outputMachine->setMemory(memory);
    //outputMachine->printStatusDebug();
    QString outputFilename = sourceCode.split('.').at(0) + ".mem";
    save(outputFilename);
}
*/
Instruction* RamsesMachine::getInstructionFromValue(int value)
{
    QVector<Instruction*>::iterator i;
    value &= 0b11110000; // Filter bits

    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getValue() == value) {
            return (*i);
        }
    }
    return NULL;
}
Instruction* RamsesMachine::getInstructionFromMnemonic(QString desiredInstruction) {
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getMnemonic() == desiredInstruction) {
            return (*i);
        }
    }
    return NULL;
}
