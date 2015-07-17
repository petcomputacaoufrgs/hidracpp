#include <QApplication>
//#include <QTextCodec>
#include "gui/hidragui.h"
//#include "gui/hidracli.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(argc == 1) {
        HidraGui w;
        w.show();
        return a.exec();
    } else {
        //HidraCli c(QApplication::arguments(), argc);
        return 0;
    }
}
