#include "hidracli.h"

HidraCli::HidraCli(QStringList argv, int argc, QObject *parent) :
    QObject(parent)
{
    for (int i = 0; i < argc; i++) {
        if(argv.at(i) == "run") {
            QString targetMachine = argv.at(++i).toLower();
            if("neander" == targetMachine) {
                machine = new NeanderMachine();
            } else if("ahmes" == targetMachine) {
                machine = new AhmesMachine();
            } else if("ramses" == targetMachine) {
                machine = new RamsesMachine();
            } /*else if("cesar" == targetMachine) {
                machine = new CesarMachine(); //acho q esse aqui vai ficar comentado por um com tempo :p
            }*/ else {
                std::cout << "targetMachine desconhecida, por favor verifique se ela está na lista a seguir:" << std::endl \
                          << "Neander \nAhmes \nRamses \nCesar" << std::endl;
            }
            machine->importMemory(argv.at(++i));
            //machine->run();
            machine->printStatusDebug();
        } else if(argv.at(i) == "assembly") {
            QString targetMachine = argv.at(++i).toLower();
            if("neander" == targetMachine) {
                machine = new NeanderMachine();
            } else if("ahmes" == targetMachine) {
                machine = new AhmesMachine();
            }/* else if("ramses" == targetMachine) {
                machine = new RamsesMachine();
            } else if("cesar" == targetMachine) {
                machine = new CesarMachine(); //acho q esse aqui vai ficar comentado por um com tempo :p
            }*/ else {
                std::cout << "targetMachine desconhecida, por favor verifique se ela está na lista a seguir:" << std::endl \
                          << "Neander \nAhmes \nRamses \nCesar" << std::endl;
            }
            machine->assemble(argv.at(++i));
            machine->printStatusDebug();
        } else if(argv.at(i) == "init") {
            QString targetMachine = argv.at(++i).toLower();
            if("neander" == targetMachine) {
                machine = new NeanderMachine();
            } else if("ahmes" == targetMachine) {
                machine = new AhmesMachine();
            }/* else if("ramses" == targetMachine) {
                machine = new RamsesMachine();
            } else if("cesar" == targetMachine) {
                machine = new CesarMachine(); //acho q esse aqui vai ficar comentado por um com tempo :p
            }*/ else {
                std::cout << "targetMachine desconhecida, por favor verifique se ela está na lista a seguir:" << std::endl \
                          << "Neander \nAhmes \nRamses \nCesar" << std::endl;
            }
            handleCommandPrompt();
        }
    }
}

void HidraCli::handleCommandPrompt()
{
    bool running = true;
    while(running) {
        std::string buffer;
        std::cout << ">>> ";
        getline(std::cin, buffer);
        QString actualCmd(buffer.c_str());
        actualCmd = actualCmd.simplified();
        QStringList cmdList = actualCmd.toLower().split(" ", QString::SkipEmptyParts);
        if(cmdList.at(0) ==  "set") {
            // TODO: auxMemory = machine->getMemory();
            auxMemory[cmdList.at(1).toInt()]->setValue((unsigned char) cmdList.at(2).toShort());
            // TODO: machine->setMemory(auxMemory);
        } else if(cmdList.at(0) ==  "setr") {

        } else if(cmdList.at(0) ==  "get") {
            // TODO: auxMemory = machine->getMemory();
            std::cout << (int)auxMemory[cmdList.at(1).toInt()]->getValue() << std::endl;
        }else if(cmdList.at(0) ==  "getr") {
            for (int i=0; i<machine->getNumberOfRegisters(); i++) {
                std::cout << machine->getRegisterValue(i) << " ";
            }
            std::cout << std::endl;
        } else if(cmdList.at(0) ==  "step") {
            machine->step();
        } else if(cmdList.at(0) ==  "run") {
            //machine->run();
        } else {
            std::cout << "Comando inválido" << std::endl;
            continue;
        }
    }
}
