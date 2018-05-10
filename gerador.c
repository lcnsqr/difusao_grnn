#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "include/matrix.h"
#include "include/difusao.h"
#include "include/idxio.h"

// Constante da equação do calor
#define ALFA 10

// Gerar as soluções por diferenças finitas, após T segundos
#define T 10

// Valores padrão
// Extremidade inicial e final
#define POS0 0
#define POS1 127

// Valores padrão
// Menor e maior valores de leitura
#define MINVAL 0
#define MAXVAL 127

// Total de amostras
#define TOTAL 500000

// Total de amostras de teste
#define TOTALTEST 10

// Dimensões da amostra
#define DIM 10

// Arquivo das amostras
#define TRAIN_X "train_x.idx"
#define TRAIN_Y "train_y.idx"
// Arquivo das amostras de teste
#define TEST_X "test_x.idx"
#define TEST_Y "test_y.idx"

// Variável aleatória uniforme em [0,1)
#define RAND ((float)(rand() >> 1)/((RAND_MAX >> 1) + 1))

// Função de preenchimento da condição inicial.
void inicial(Matrix *w){
	// Gerar uma curva aleatória
	w->_[0] = RAND * MAXVAL;
	for (int i = 1; i < DIM; i++){
		// A cada passo, a curva muda até 15 unidades para baixo ou para cima
		w->_[i] = w->_[i-1] - 15 + RAND * 30;
		if ( w->_[i] < MINVAL ) w->_[i] = MINVAL;
		if ( w->_[i] > MAXVAL ) w->_[i] = MAXVAL;
	}
	// Valor da derivada em relação a x no extremo 
	// b definido pelo último valor do vetor (coluna)
	w->_[DIM] = 0;
}

int main (int argc, char **argv){
	srand((unsigned int)time(NULL));

	// Cada linha da matriz train[0] constitui a variável independente e
	// cada linha da matrix train[1] a variável dependente correspondente
	Matrix train[2], test[2];
	// Alocar matriz das amostras
	mtrxBuild(&train[0], TOTAL, DIM);
	printf("Memória ocupada por %s: %d bytes\n", TRAIN_X, train[0].memsize);
	mtrxBuild(&train[1], TOTAL, DIM);
	printf("Memória ocupada por %s: %d bytes\n", TRAIN_Y, train[1].memsize);

	// Alocar matriz das amostras de teste
	mtrxBuild(&test[0], TOTALTEST, DIM);
	mtrxBuild(&test[1], TOTALTEST, DIM);
	
	// Vetores antes e depois (condição inicial e vetor resultante)
	Matrix w[2];
	// Neste caso, os vetores não são transpostos (coluna)
	mtrxBuild(&w[0], DIM + 1, 1);
	// Vetor do resultado
	mtrxBuild(&w[1], DIM + 1, 1);
	// Buffer para armazenar o vetor transposto para cópia
	Matrix buf[2];
	mtrxBuild(&buf[0], 1, DIM + 1);
	mtrxBuild(&buf[1], 1, DIM);
	// Vetor de posição para recorte e inserção de matriz
	int p[] = {0, 0};
	// Gerar cada amostra e copiar para a matriz de amostras
	printf("Gerando %d amostras...\n", TOTAL);
	for (int i = 0; i < TOTAL; i++){
		// Preencher condições iniciais
		inicial(&w[0]);
		// Gerar as soluções por diferenças finitas, após T segundos
		gerar(w, ALFA, POS0, POS1, T);
		// Transpor vetor independente para vetor linha
		mtrxTranspose(&buf[0], &w[0]);
		// Reduzir uma dimensão (da condição de Neumann)
		p[0] = 0;
		mtrxCopy(&buf[1], &buf[0], p);
		// Inserir na matriz de amostras
		p[0] = i;
		mtrxPaste(&train[0], &buf[1], p);
		// Executar os mesmos procedimentos para o vetor dependente
		mtrxTranspose(&buf[0], &w[1]);
		p[0] = 0;
		mtrxCopy(&buf[1], &buf[0], p);
		p[0] = i;
		mtrxPaste(&train[1], &buf[1], p);
		// Exibir progresso
		if ( (i+1) % 10000 == 0 ) printf("%d\n", i+1);
	}

	// Salvar matrizes de amostras
	struct Idx idx;
	// Tipo float (4 bytes)
	idx.type = 0x0d;
	idx.dimCount = 2;
	idx.dimSizes = malloc(idx.dimCount*WSIZE);
	idx.dimSizes[0] = TOTAL;
	idx.dimSizes[1] = DIM;
	idx.size = 1;
	for (int d = 0; d < idx.dimCount; d++) idx.size *= idx.dimSizes[d];
	// Variáveis independentes
	idx.data.f = train[0]._;
	idxSave(TRAIN_X, &idx);
	// Variáveis dependentes
	idx.data.f = train[1]._;
	idxSave(TRAIN_Y, &idx);

	// Gerar cada amostra de teste e copiar para a matriz de amostras
	printf("Gerando %d amostras de teste ...\n", TOTALTEST);
	for (int i = 0; i < TOTALTEST; i++){
		// Preencher condições iniciais
		inicial(&w[0]);
		// Gerar as soluções por diferenças finitas, após T segundos
		gerar(w, ALFA, POS0, POS1, T);
		// Transpor vetor independente para vetor linha
		mtrxTranspose(&buf[0], &w[0]);
		// Reduzir uma dimensão (da condição de Neumann)
		p[0] = 0;
		mtrxCopy(&buf[1], &buf[0], p);
		// Inserir na matriz de amostras de teste
		p[0] = i;
		mtrxPaste(&test[0], &buf[1], p);
		// Executar os mesmos procedimentos para o vetor dependente
		mtrxTranspose(&buf[0], &w[1]);
		p[0] = 0;
		mtrxCopy(&buf[1], &buf[0], p);
		p[0] = i;
		mtrxPaste(&test[1], &buf[1], p);
	}

	// Salvar matrizes de amostras
	// Tipo float (4 bytes)
	idx.type = 0x0d;
	idx.dimCount = 2;
	idx.dimSizes = malloc(idx.dimCount*WSIZE);
	idx.dimSizes[0] = TOTAL;
	idx.dimSizes[1] = DIM;
	idx.size = 1;
	for (int d = 0; d < idx.dimCount; d++) idx.size *= idx.dimSizes[d];
	// Variáveis independentes
	idx.data.f = train[0]._;
	idxSave(TRAIN_X, &idx);
	// Variáveis dependentes
	idx.data.f = train[1]._;
	idxSave(TRAIN_Y, &idx);

	// Salvar matrizes de amostras de teste
	idx.dimSizes[0] = TOTALTEST;
	idx.size = 1;
	for (int d = 0; d < idx.dimCount; d++) idx.size *= idx.dimSizes[d];
	// Variáveis independentes
	idx.data.f = test[0]._;
	idxSave(TEST_X, &idx);
	// Variáveis dependentes
	idx.data.f = test[1]._;
	idxSave(TEST_Y, &idx);

	return 0;
}
