#include <QApplication>
#include <QTextCodec>
#include "hidragui.h"
#include "hidracli.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName("utf8") );
    if(argc == 1) {
        HidraGui w;
        w.show();
        return a.exec();
    } else {
        HidraCli c(QApplication::arguments(), argc);
        return 0;
    }
}
