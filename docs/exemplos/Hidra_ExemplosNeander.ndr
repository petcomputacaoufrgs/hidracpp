;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Dicas de uso
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Passe o mouse sobre as diferentes áreas do programa para obter informações
;; úteis na barra de status.

;; Para rodar um programa, clique em Montar, e em seguida em Rodar. Você pode
;; realizar a execução passo-a-passo com o comando Passo (tecla F8).

;; Para rodar o programa até uma linha específica do código, utilize o comando
;; Máquina -> Inserir/remover breakpoint (tecla F12), e execute o programa.

;; Passe o mouse sobre instruções ou modos de acesso (canto inferior direito)
;; para visualizar a sintaxe e o funcionamento de cada um.

;; Passe o mouse sobre valores da memória ou registradores para visualizá-los em
;; hexadecimal ou binário.

;; Dê um clique duplo na tabela de memória esquerda (instruções) para mover a
;; execução (PC) para o endereço escolhido.

;; Dê um clique duplo em um label na tabela de memória direita (dados) para
;; visualizar o código correspondente.

;; De um clique duplo em um valor de memória para alterá-lo.



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 01: Soma de valores
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Carrega o valor presente no end. 128 da memória para o acumulador.
; Adiciona o valor presente no end. 129 ao acumulador.
; Armazena o valor do acumulador no end. 130.

	LDA 128
	ADD 129
	STA 130



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 02: Labels, Complemento de 2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Carrega o valor indicado pelo label Dois (ver área de Dados, final do código)
; Adiciona o valor indicado pelo label MenosUm
; Armazena o resultado na posicao indicada por ResSoma

	LDA Dois
	ADD MenosUm
	STA Soma02

; Assim como em computadores modernos, o valor -1 é representado em complemento
; de 2 (valor 255 neste caso), de forma que a soma 2 + 255 resulta no valor
; correto (1), devido ao estouro de valores (overflow).
; O bit mais significativo (carry) é descartado.

;   00000010 (2)
; + 11111111 (255, interpretado como -1)
;   --------
;   100000001 (257, ou 1 após descarte de carry)

; Experimente alterar os valores Valor1 e Valor2 e observe o resultado da soma:

	LDA Valor1
	ADD Valor2
	STA ResSoma



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 03: Troca de sinal
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Converte um número positivo em um número negativo, e vice-versa, utilizando
; aritmética de complemento de 2 (experimente alterar os valores Entrada e Saida)

	LDA Entrada
	NOT ; Inverte o valor de cada bit
	ADD Um ; Adiciona a constante 1
	STA Saida



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 04: Condicional (If)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Verifica se ValBool é verdadeiro.
; Se sim, segue a execução normalmente. Do contrário, desvia para False.
; Convenciona-se que 0 representa o valor booleano falso e 1 o valor booleano
; verdadeiro, embora qualquer valor diferente de 0 seja considerado verdadeiro.

	LDA ValBool
	JZ False
; True:
	NOP ; Se a execução chegou aqui, valor é verdadeiro (dif. de 0)
	JMP EndIf
False:
	NOP ; Se a execução chegou aqui, valor é falso (igual a 0)
	JMP EndIf
EndIf:
	NOP ; Ambos os caminhos continuam aqui



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 05: Comparações (>, <, =)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Compara os valores X e Y (em complemento de dois), subtraindo o segundo do
; primeiro.
; Como não há instrução SUB em Neander, é necessário inverter o sinal de Y para
; então realizar uma adição.
; Se os valores forem iguais (X - Y = 0), desvia a execução para Iguais.
; Se o primeiro valor for menor que o segundo (X - Y < 0), desvia para XMenor.
; Se nenhum desses casos ocorreu, desvia para XMaior (X - Y > 0).

	LDA Y
	NOT
	Add Um ; Troca de sinal (-Y)
	
	ADD X      ; -Y + X (ou X - Y)
	JZ Iguais  ; X - Y = 0 (X = Y)
	JN XMenor  ; X - Y < 0 (X < Y)
	JMP XMaior ; X - Y > 0 (X > Y)

Iguais:
	NOP ; Se a execução chegou aqui, valores são iguais
	JMP EndComp
XMenor:
	NOP ; Se a execução chegou aqui, X é menor que Y
	JMP EndComp
XMaior:
	NOP ; Se a execução chegou aqui, X é maior que Y
	JMP EndComp
EndComp:
	NOP ; Ambos os caminhos seguem aqui

; Experimente alterar os valores de X e Y e observe o caminho de execução
; passo-a-passo.



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 06: Laços (For)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Repete uma seção do código um número determinado de vezes.
; Neste exemplo, é realizado um somatório simples de NumIter até um.

	LDA Zero
	STA Valor   ; Inicializa Valor com 0
Loop:
	LDA NumIter ; Carrega o número de iterações no acumulador
	JZ FimLoop  ; Se já acabaram as iterações, pula para o fim
	
	; Código a ser repetido
	LDA Valor
	ADD NumIter
	STA Valor
	; Fim do código a ser repetido

	LDA NumIter
	ADD MenosUm ; Subtrai uma iteração
	STA NumIter ; Guarda o novo número de iterações	
	JMP Loop
FimLoop:
	LDA Valor   ; Coloca o resultado no acumulador



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 07: Multiplicação
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Executa multiplicação como uma série de somas
; Mul1 * Mul2 = Mul2 + Mul2 + Mul2 + ... (Mul1 vezes)

	LDA Zero
	STA ResMul ; Inicializa o resultado com zero
LoopMul:
	LDA Mul1   ; Mul1 controla o número de iterações
	JZ FimMul
	ADD MenosUm
	STA Mul1
	
	LDA ResMul
	ADD Mul2
	STA ResMul ; Adiciona Mul2 ao resultado (executado Mul1 vezes)
	
	JMP LoopMul
FimMul:
	LDA ResMul ; Coloca o resultado no acumulador

; Experimente também multiplicar valores negativos (em complemento de dois).



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 08: OR, AND e HLT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; AND: Operação "E" lógica bit-a-bit. Resulta 1 para cada novo bit quando ambos
; os bits correspondentes à posição têm valor 1.

	LDA Val7   ;   00000111
	AND Val22  ; & 00001110
	STA ResAnd ; = 00000110

; OR: Operação "OU" lógica bit-a-bit. Resulta 1 para cada novo bit se pelo menos
; um dos bits correspondentes tem valor um.

	LDA Val7   ;   00000111
	OR Val22   ; | 00001110
	STA ResOr ; = 00001111

; HLT: Termina a execução (halt)

	HLT



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ÁREA DE DADOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ORG 128 ; Diretiva do montador, que posiciona a montagem na pos. 128 da memória

         DB 1 ; Posição 128 (inicializada com 1 durante a montagem)
         DB 2 ; Posição 129 (inicializada com 2)
         DB   ; Posição 130 (não-inicializada)

Zero:    DB 0
Um:      DB 1
Dois:    DB 2
MenosUm: DB -1 ; Armazenado como 255 (complemento de 2)
Soma02:  DB

Valor1:  DB 5
Valor2:  DB -5
ResSoma: DB

Entrada: DB -1
Saida:   DB

ValBool: DB 0

X:       DB 3
Y:       DB 4

Valor:	 DB
NumIter: DB 5

Mul1:    DB 5
Mul2:    DB 3
ResMul:  DB

Val7:    DB 7
Val22:   DB 22
ResAnd:  DB
ResOr:   DB

DAB 'ABCDE' ; Exemplo de declaração de array de caracteres.
            ; Experimente a opção Exibir->Interpretar caracteres
