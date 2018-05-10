Implementação sequencial da GRNN e gerado dos conjuntos de dados.

Para compilar, executar comando `make`.

As definições são alteradas diretamente no código-fonte. O arquivo `gerador.c` é o código reponsável por gerar as bases de dados de treinamento e teste, que serão utilizadas pelo `grnn.c` para fazer a regressão no conjunto de teste. A condição inicial das observações são curvas aleatórias dadas pelas função `inicial` em `gerador.c`, que pode ser modificada para outros tipos de condições iniciais.

O comando `grnn` vai carregar os conjuntos de dados e exibir os resultados para cada amostra do conjunto de testes. Além de exibir o estado inicial, estimativa e valores observados (estes gerados previamente pelo método de diferenças finitas), é informada a norma euclidiana da diferenças entre esses dois vetores (estimativa e observado). Ao final, é exibida a raiz da média do quadrado da diferença -- *Root Mean Square Error (RMSE)* -- para o conjunto de teste.

As bases de treinamento e teste são armazenadas nos arquivos binários `train_x.idx`, `train_y.idx`, `test_x.idx` e `test_y.idx`. O formato *idx* é composto por um cabeçalho de 4 bytes que contém o tipos dos dados (char, int, float ou double) e a quantidade de dimensões. Para cada dimensão, segue um inteiro de 4 bytes com a quantidade de itens contidos na mesma. Em seguida estão os dados, tal qual são alocados na memória por um array em linguagem C.

O script `verificar.sh` pode ser utilizado para testar os conjuntos. O comando `verificar.sh train` escolhe e exibe um par aleatório do conjunto de treinamento e `verificar.sh test` escolhe um par aleatório do conjunto de teste.
