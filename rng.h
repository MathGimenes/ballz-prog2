// GRR20211781 Matheus Gimenes da Silva Viana
#ifndef _RNG_
#define _RNG_

/* funcao que determina o numero de quadrados na proxima linha */
int aleatorizar_wave ();

/* retorna um inteiro aleatorio com base no maximo e minimo informado */
int aleatorio_range(int max, int min);

/* aleatoriza o numero de batidas do quadrado */
int aleatorio_batidas ();

/* aleatoriza a posicao do powerup */
int aleatorizar_powerup(int vetor_posicoes[], int quantidade);

/* varre um vetor em busca de um numero, se encontrar retorna 0, retorna 1 caso contrarios */
int existe_vetor (int vetor[], int numero_aleatorizado, int indice);

/* aleatoriza e retorna um vetor contendo as posições dos proximos quadrados */
void aleatorizar_posicoes (int vetor_posicoes[], int quantidade);

#endif