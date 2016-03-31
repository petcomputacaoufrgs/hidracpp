#include <QApplication>
#include "gui/hidragui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    HidraGui w;

    if (argc == 2)
        w.load(QString(argv[1]), false); // Load file

    w.show();
    return a.exec();
}
