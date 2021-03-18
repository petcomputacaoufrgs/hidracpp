Hidra
=====

O Hidra é um ambiente de desenvolvimento integrado para as máquinas teóricas estudadas nas disciplinas de Arquitetura de Computadores da UFRGS.

O software unifica as tarefas de montagem, simulação e depuração em um ambiente favorável ao desenvolvimento, permitindo que o estudante possa se concentrar nas diferentes características de cada arquitetura. 

Além das máquinas Neander, Ahmes e Ramses, o Hidra traz a simulação da máquina de registradores REG e da máquina de pilha Volta, bem como das demais arquiteturas hipotéticas descritas ao final do livro Fundamentos de Arquitetura de Computadores, do Prof. Raul Fernando Weber.

Download
========

Versões pré-compiladas para Windows estão disponíveis em:<br/>
https://github.com/petcomputacaoufrgs/hidracpp/releases/

Para desenvolver
================

O projeto é desenvolvido na linguagem C++, utilizando o framework Qt versão 5.3 e o ambiente QtCreator.

Desenvolver usando Windows
--------------------------

Para desenvolver em Windows, primeiro instale o Qt 5.3 com MinGW (que inclui o ambiente QtCreator), através do link:<br/>
https://download.qt.io/archive/qt/5.3/5.3.2/qt-opensource-windows-x86-mingw482_opengl-5.3.2.exe

Em seguida, clone o projeto através do software Git de sua preferência (ex: GitHub for Windows) e abra o arquivo <i>src/hidra.pro</i>.

Desenvolver usando Linux
------------------------

Para desenvolver usando Linux, faça o download do Qt através deste link:<br/>
https://download.qt.io/archive/qt/5.3/5.3.2/qt-opensource-linux-x86-5.3.2.run

Em seguida, siga o tutorial para instalação em sua distribuição Linux, por exemplo:<br/>
https://wiki.qt.io/Install_Qt_5_on_Ubuntu

Por fim, clone o projeto através do software Git de sua preferência (ex: Git GUI) e utilize o QtCreator instalado para abrir o arquivo <i>src/hidra.pro</i>.
