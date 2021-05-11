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

O projeto é desenvolvido na linguagem C++, utilizando o framework Qt versão 5.3 e o ambiente Qt Creator.

Desenvolver usando Windows
--------------------------

Para desenvolver em Windows, primeiro instale o Qt 5.3 com MinGW (que inclui o ambiente Qt Creator), através do link:<br/>
https://download.qt.io/archive/qt/5.3/5.3.2/qt-opensource-windows-x86-mingw482_opengl-5.3.2.exe

Em seguida, clone o projeto através do software Git de sua preferência (ex: GitHub for Windows) e abra o arquivo <i>src/hidra.pro</i>.

Desenvolver usando Linux
------------------------

Para desenvolver usando Linux, faça o download do Qt através deste link:<br/>
https://download.qt.io/archive/qt/5.3/5.3.2/qt-opensource-linux-x86-5.3.2.run

Em seguida, siga o tutorial para instalação em sua distribuição Linux, por exemplo:<br/>
https://wiki.qt.io/Install_Qt_5_on_Ubuntu

Por fim, clone o projeto através do software Git de sua preferência (ex: Git GUI) e utilize o Qt Creator instalado para abrir o arquivo <i>src/hidra.pro</i>.

Cross-Compile: compilar no Linux para Windows
---------------------------------------------

Aviso: o cross-compile polui o diretório do Hidra com arquivos que o Git, por enquanto, não vai ignorar.
Certifique-se de salvar seu trabalho, e limpar arquivos de cross-compile antes de registrar novas mudanças no Git (ou cuidar para não incluir arquvos de cross-compile).

Vamos nos referir ao diretório do projeto do Hidra no seu computador como `$HIDRA`.
Por exemplo, se o Hidra está em "`/home/joaozinho/Documents/pet/hidra`", `$HIDRA` é "`/home/joaozinho/Documents/pet/hidra`".

O projeto "mxe" provê ferramentas para fazer cross-compile do Qt.
Vamos baixar o projeto "mxe":
```
git clone https://github.com/mxe/mxe.git

cd mxe
```

Vamos nos referir ao diretório onde está o projeto "mxe" como `$MXE`.
Por exemplo, se o repositório foi baixado em "`/home/joaozinho/Documents/pet/mxe`", `$MXE` é "`/home/joaozinho/Documents/pet/mxe`".

Agora, é necessário instalar algumas dependências, listadas em https://mxe.cc/#requirements.
A forma exata de instalá-las vai depender da sua distribuição Linux, mas ao tentar executar o próximo passo, você deve ser avisado do que falta.
O próximo passo é fazer cross-compile da parte mais essencial do Qt, necessária para o Hidra.
Para tal, execute o seguinte (no diretório `$MXE`):
```
make qtbase
```

Certifique-se de que o passo anterior terminou em sucesso.
Note que o comando acima só constrói a parte mais essencial.
Se algum dia precisarmos de features fora do `qtbase`, é necessário também
executar `make qt5`.

Agora, vamos ao diretório do Hidra (`$HIDRA`/src).
```
cd $HIDRA/src
```

Precisamos que as ferramentas do Qt que acabamos de construir estejam visíveis.
Para tal, você precisa executar o seguinte comando:
```
export PATH="$MXE/usr/bin:$PATH"
```

E então, precisamos gerar artefatos de compilação do Hidra:
```
i686-w64-mingw32.static-qmake-qt5
```

Finalmente, vamos compilar o Hidra:
```
make
```

Agora o arquivo do hidra encontra-se em:
```
$HIDRA/src/release/hidra.exe
```
