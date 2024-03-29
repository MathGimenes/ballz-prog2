#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "rng.h"

#define CHANCE2 9
#define CHANCE3 44
#define CHANCE4 79
#define CHANCE5 97
#define CHANCE6 99


/* funcao que determina o numero de quadrados na proxima linha */
int aleatorizar_wave (){
    int tmp = rand()%100;

    if (tmp <= CHANCE2)
        return 2;
    else if (tmp <= CHANCE3)
        return 3;
    else if (tmp <= CHANCE4)
        return 4;
    else if (tmp <= CHANCE5)
        return 5;
    else
        return 6;
}


/* retorna um inteiro aleatorio com base no maximo e minimo informado */
int aleatorio_range(int max, int min){
    return rand() % (max - min + 1) + min;
}

int aleatorio_batidas (){
    int tmp = rand () % 100;

    if (tmp < 15)
        return 2;
    
    return 1;
}

int aleatorizar_powerup(int vetor_posicoes[], int quantidade){
    int tmp;
    do{
        tmp = aleatorio_range (6, 0);
    } while(!(existe_vetor(vetor_posicoes, tmp, quantidade)));

    return tmp;
}


/* varre um vetor em busca de um numero, se encontrar retorna 0, retorna 1 caso contrarios */
int existe_vetor (int vetor[], int numero_aleatorizado, int indice){
    for (int i = 0; i < indice; i++){

        if (vetor[i] == numero_aleatorizado)
            return 0;
    }

    return 1;
}


/* aleatoriza e retorna um vetor contendo as posições dos proximos quadrados */
void aleatorizar_posicoes (int vetor_posicoes[], int quantidade){
   
    int i = 0;
    vetor_posicoes[i] = aleatorio_range (6, 0);
    i++;

    while (i < quantidade){
        vetor_posicoes[i] = aleatorio_range (6, 0);
        if (existe_vetor(vetor_posicoes, vetor_posicoes[i], i))
            i++;
    }

}