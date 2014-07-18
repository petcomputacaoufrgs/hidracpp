#include "hidragui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HidraGui w;
    w.show();

    return a.exec();
}
