/********************************************************************************
 *
 * Copyright (C) 2015-2020 PET Computação UFRGS
 *
 * Este arquivo é parte do programa Hidra.
 *
 * Hidra é um software livre; você pode redistribuí-lo e/ou modificá-lo
 * dentro dos termos da Licença Pública Geral GNU como publicada pela
 * Fundação do Software Livre (FSF); na versão 3 da Licença, ou
 * (de opção sua) qualquer versão posterior.
 *
 *******************************************************************************/

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
