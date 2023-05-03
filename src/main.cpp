/********************************************************************************
 *
 * Copyright (C) 2014-2021 PET Computação UFRGS
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
#include <QPalette>
#include "gui/hidragui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("PET Computação UFRGS");
    a.setApplicationName("Hidra");

    HidraGui w;

    QPalette dark_mode = HidraGui::getDarkModePalette();

    // Dark mode configuration
    a.setStyle("Fusion");
    a.setPalette(dark_mode);
    a.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

    if (argc == 2)
        w.load(QString(argv[1]), false); // Load file

    w.show();
    return a.exec();
}
