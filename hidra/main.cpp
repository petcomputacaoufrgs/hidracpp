#include "hidragui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(argc == 0) {
        HidraGui w;
        w.show();
    } else {
        for (int i = 0; i < QApplication::argc(); i++) {
            if(QApplication::arguments().at(i) == "run") {
                QString targetMachine = QApplication::arguments().at(++i).toLower();
                Machine *machine;
                if("neander" == targetMachine) {
                    machine = new NeanderMachine();
                } else if("ahmes" == targetMachine) {
//                    machine = new AhmesMachine();
                } else if("ramses" == targetMachine) {
//                    machine = new RamsesMachine();
                } else if("cesar" == targetMachine) {
//                    machine = new CesarMachine(); //acho q esse aqui vai ficar comentado por um com tempo :p
                } else {
                    std::cout << "targetMachine desconhecida, por favor verifique se ela está na lista a seguir:" << std::endl \
                              << "Neander \nAhmes \nRamses \nCesar" << std::endl;
                }
                machine->load(QApplication::arguments().at(++i));
                machine->run();
                machine->printStatusDebug();
            } else if(QApplication::arguments().at(i) == "assembly") {
                QString targetMachine = QApplication::arguments().at(++i).toLower();
                Machine *machine;
                if("neander" == targetMachine) {
                    machine = new NeanderMachine();
                }/* else if("ahmes" == targetMachine) {
                    machine = new AhmesMachine();
                } else if("ramses" == targetMachine) {
                    machine = new RamsesMachine();
                } else if("cesar" == targetMachine) {
                    machine = new CesarMachine(); //acho q esse aqui vai ficar comentado por um com tempo :p
                }*/ else {
                    std::cout << "targetMachine desconhecida, por favor verifique se ela está na lista a seguir:" << std::endl \
                              << "Neander \nAhmes \nRamses \nCesar" << std::endl;
                }
                machine->assemble(QApplication::arguments().at(++i));
                machine->run();
                machine->printStatusDebug();
            } else if(QApplication::arguments().at(i) == "init") {
                //TODO
            }
        }
    }
    return a.exec();
}
