Hidra
=====

O Hidra é um ambiente de desenvolvimento integrado para as máquinas teóricas estudadas nas disciplinas de Arquitetura de Computadores da UFRGS, unificando as tarefas de montagem, simulação e depuração em um ambiente favorável ao desenvolvimento, permitindo que o aluno possa se concentrar nas diferentes características de cada arquitetura. 

Além das máquinas Neander, Ahmes e Ramses, o Hidra traz a simulação da máquina de registradores REG e da máquina de pilha Volta, bem como das demais arquiteturas hipotéticas descritas ao final do livro Fundamentos de Arquitetura de Computadores, do Prof. Raul Fernando Weber, com a notável exceção da máquina Cesar, a qual está atualmente em desenvolvimento.

Download
========

Versões pré-compiladas para Windows estão disponíveis em:<br/>
https://github.com/petcomputacaoufrgs/hidracpp/releases/

Para desenvolver
================

O projeto é desenvolvido na linguagem C++, utilizando o framework Qt versão 5.15.2. Atualmente, o desenvolvimento é realizado utilizando a plataforma [VS Code](https://code.visualstudio.com/) e a ferramenta de build utilizada é o [CMake](https://cmake.org/)

### Clonando o repositório



### Baixando o framework Qt
O [framework Qt](https://www.qt.io/product) é a base na qual o Hidra é construído, auxiliando na criação da interface gráfica, estruturação do programa, automatização de testes, entre outros.

#### Passos
* Baixe o [Qt Open Source](https://www.qt.io/download-qt-installer?hsCtaTracking=99d9dd4f-5681-48d2-b096-470725510d34%7C074ddad0-fdef-4e53-8aa8-5e8a876d6ab4)
  * Lembre que, para utilizar o Qt Open Source, é necessário seguir as obrigações das licenças GPL/LGPL
  * Caso queira montar as bibliotecas manualmente, [siga este guia](https://wiki.qt.io/Building_Qt_5_from_Git#Getting_the_source_code)
* Rode o instalador do Qt
  * Caso esteja utilizando Linux, é possível que seja necessário instalar o package "libxcb-xinerama0"
* Crie uma conta Qt, caso já não tenha uma, e insira o nome e senha na tela inicial
* No passo *Installation Folder*, determine o caminho onde o Qt será instalado e selecione a opção *Instalação Personalizada*
* No passo *Select Components*, clique na aba *Qt* e após na aba *Qt 5.15.2*
* Selecione o componente que reflita o compilador que você irá usar (por exemplo, *Desktop gcc 64-bit* no Ubuntu)
* Na aba *Developer and Designer Tools*, clique na caixa *Qt Design Studio*, assim como em qualquer outra ferramenta utilizada posteriormente já não instalada em seu computador (por exemplo, o CMake, ou o compilador que você selecionou)
* Prossiga com o resto da instalação até o fim.

### Fazendo o build do projeto com o VS Code
Ao abrir a pasta "hidracpp" com o VSCode, instale as extensões recomendadas, em especial a extensão [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools). Uma janela pedindo para configurar o projeto aparecerá; clique sim (ou, se quiser, use o comando *CMake: Configure*)
A extensão então pedirá que você selecione um kit; por padrão, ela irá buscar por compiladores em algumas pastas específicas. Selecione o kit que corresponde ao compilador que você escolheu durante a instalação do Qt.
* Se você instalou o compilador pelo Qt também, execute o comando da extensão *CMake: Edit User-Local CMake Kits*. Isso abrirá um arquivo JSON, onde você poderá manualmente adicionar um kit.
```
{
    "name": "Qt MinGW 64",
    "compilers": {
      "C": "C:\\Qt\\Tools\\mingw810_64\\bin\\gcc.exe",
      "CXX": "C:\\Qt\\Tools\\mingw810_64\\bin\\g++.exe"
    },
    "preferredGenerator": {
      "name": "MinGW Makefiles"
    },
    "environmentVariables": {
      "CMT_MINGW_PATH": "C:\\MinGW\\bin"
    }
  },
  ```



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

Vamos nos referir ao diretório do projeto do Hidra no seu computador como `$HIDRA`.
Por exemplo, se o Hidra está em "`/home/joaozinho/Documents/pet/hidracpp`", `$HIDRA` é "`/home/joaozinho/Documents/pet/hidracpp`".

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
Para tal, execute o seguinte (no diretório "`$MXE`"):
```
make qtbase
```

Certifique-se de que o passo anterior terminou em sucesso.

Note que o comando acima só constrói a parte mais essencial.
Se algum dia precisarmos de features fora do `qtbase`, é necessário também
executar `make qt5`.

Precisamos que as ferramentas do Qt que acabamos de construir estejam visíveis.
Para tal, você precisa executar o seguinte comando:
```
export PATH="$MXE/usr/bin:$PATH"
```

Agora, vamos ao diretório do Hidra ("`$HIDRA`").
```
cd $HIDRA
```

Vamos criar um diretório novo para colocar nossos artefatos.
Vamos chamá-lo "`$HIDRA/build-cross-compile`".
Execute o seguinte, no diretório "`$HIDRA`":
```
mkdir build-cross-compile

cd build-cross-compile
```

Agora, precisamos gerar artefatos de compilação do Hidra.
Execute o seguinte, no diretório "`$HIDRA/build-cross-compile`":
```
i686-w64-mingw32.static-qmake-qt5 ../src
```

Certifique-se de que o passo anterior terminou em sucesso.

Finalmente, vamos compilar o Hidra.
Execute o seguinte, novamente no diretório "`$HIDRA/build-cross-compile`":
```
make
```

Certifique-se de que o passo anterior terminou em sucesso.

Tudo está pronto agora.
O arquivo do hidra encontra-se em:
```
$HIDRA/build-cross-compile/release/hidra.exe
```
