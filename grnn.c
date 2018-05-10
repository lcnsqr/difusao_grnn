#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "include/idxio.h"

// Arquivo das amostras
#define TRAIN_X "train_x.idx"
#define TRAIN_Y "train_y.idx"
// Arquivos de teste
#define TEST_X "test_x.idx"
#define TEST_Y "test_y.idx"

// Implementação da distância entre dois vetores
float dist(float *v, float *w, int n){
	// Quadrado da distância euclidiana entre o vetores v e w
	float d = 0;
	for (int i = 0; i < n; i++){
		d += pow(w[i]-v[i], 2);
	}
	return d;
}

// Estimar a variável dependente unidimensional
// mem_x: Memória das variáveis independentes
// mem_y: Memória das variáveis dependentes
// m: Número de amostras
// x: Variável independente lida
// y: Estimativa da variável dependente
// n: Dimensões das amostras
// sigma: Parâmetro da regressão
// Retorna -1 em caso de erro
int estim(float *mem_x, float *mem_y, int m, float *x, float *y, int n, float sigma){
	// Numerador e denominador do estimador
	float numer, denom;
	// Fator comum nas operações
	float f;
	// Iterar para cada componente de y
	for (int c = 0; c < n; c++){
		numer = 0;
		denom = 0;
		// Iterar em cada amostra de treinamento
		/*
		 * Este é o loop a ser paralelizado
		 */
		for (int i = 0; i < m; i++){
			// Fator comum
			f = exp( -dist(x, &mem_x[i*n], n) / 2*pow(sigma,2) );
			// Numerador da fração para o c-ésimo componente
			numer += mem_y[i*n+c] * f;
			// Denominador da fração
			denom += f;
		}
		// Estimativa com verificação de divisão por zero
		if ( denom != 0 ){
			y[c] = numer / denom;
		}
		else {
			// Falha na operação
			return -1;
		}
	}
	// Sem erro
	return 0;
}

int testar(struct Idx *train_x, struct Idx *train_y, struct Idx *test_x, struct Idx *test_y, float sigma, float *errsum){
	// Vetor da estimativa 
	float *y = malloc(sizeof(float)*train_y->dimSizes[1]);
	// Verificação de falha na operação
	int ret = 0;
	// Erro da estimativa
	float err = 0;
	// Iterar em todo o conjunto de teste
	for (int i = 0; i < test_x->dimSizes[0]; i++){
		printf("Teste %d:\nInicial  \tEstimativa  \tObservado\n", i);
		ret = estim(train_x->data.f, train_y->data.f, train_x->dimSizes[0], &test_x->data.f[i*test_x->dimSizes[1]], y, train_x->dimSizes[1], sigma);
		if ( ret == -1 ){
			printf("Falha na operação\n");
			return ret;
		}
		for (int c = 0; c < test_y->dimSizes[1]; c++){
			printf("%.6f\t%.6f\t%.6f\n", test_x->data.f[i*test_x->dimSizes[1] + c], y[c], test_y->data.f[i*test_y->dimSizes[1] + c]);
		}
		// Erro da estimativa
		err = dist(y, &test_y->data.f[i*test_y->dimSizes[1]], test_y->dimSizes[1]);
		printf("Norma euclidiana da diferença: %f\n", sqrt(err));
		printf("\n");
		// Erro acumulado (sem raiz)
		*errsum += err;
	}
	free(y);
	// Sem erro
	return 0;
}

int main(int argc, char **argv){

	struct Idx train_x, train_y, test_x, test_y;
	// Carregar arquivos das amostras
	idxLoad(TRAIN_X, &train_x);
	idxLoad(TRAIN_Y, &train_y);

	// Arquivos de teste
	idxLoad(TEST_X, &test_x);
	idxLoad(TEST_Y, &test_y);

	printf("Conjunto de treinamento: %d amostras\n", train_x.dimSizes[0]);

	// Parâmetro sigma da regressão
	float sigma;
	// Erro acumulado
	float errsum;
	// Código de retorno da operação
	int ret;

	// Testar
	printf("Calculando estimativas para o conjunto teste (%d amostras)...\n\n", test_x.dimSizes[0]);

	// Candidato a melhor parâmetro sigma
	sigma = M_E/log(train_x.dimSizes[0]);
	errsum = 0;
	ret = 0;
	printf("Parâmetro sigma = e/log(n): %f\n", sigma);
	ret = testar(&train_x, &train_y, &test_x, &test_y, sigma, &errsum);
	if ( ret == 0 ){
		printf("Root Mean Square Error (RMSE): %f\n\n", sqrt(errsum / test_x.dimSizes[0]));
	}
	else {
		printf("Erro na operação\n");
		return ret;
	}
	return 0;
}
