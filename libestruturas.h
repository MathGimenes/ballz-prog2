#ifndef _LIBESTRUTURAS_
#define _LIBESTRUTURAS_

#include <allegro5/allegro5.h>

typedef struct QUADRADO{
    /* posicao */
    float x, y;
    /* centro */
    float cx, cy;

    /* contador de batidas */
    int batidas;
} QUADRADO;

typedef struct BOLA{
    /* posicao */
    float x, y;
    /* velocidade */
    float vx, vy;

    bool colidiu, lancada; 
} BOLA;

typedef struct POWERUP{
    /* posicao powerup */
    float cx, cy;
} POWERUP;


struct nodo_l {
    int elemento;       /* lista de numeros inteiros */
    QUADRADO quadrado;
    struct nodo_l* prox;  /* ponteiro para o proximo   */
};
typedef struct nodo_l nodo_l_t;

struct lista {
    nodo_l_t* ini;      /* ponteiro para o inicio da lista */
    int tamanho;        /* numero de elementos na lista    */
};
typedef struct lista lista_t;

typedef struct nodo_powerup {
    int elemento;       /* lista de numeros inteiros */
    POWERUP powerup;
    struct nodo_powerup* prox;  /* ponteiro para o proximo   */
} nodo_powerup;

typedef struct lista_powerup {
    nodo_powerup* ini;      /* ponteiro para o inicio da lista */
    int tamanho;        /* numero de elementos na lista    */
} lista_powerup;


lista_powerup* lista_cria_powerup ();

/*
 * Cria uma lista vazia e a retorna, se falhar retorna NULL.
*/
lista_t* lista_cria ();

/*
 * Remove todos os elementos da lista, libera espaco e retorna NULL.
*/
lista_t* lista_destroi (lista_t* l);

/*
 * Retorna 1 se a lista esta vazia e 0 caso contrario.
*/
int lista_vazia (lista_t* l);

int lista_vazia_powerup (lista_powerup* l);

/*
 * Insere o elemento no inicio da lista.
 * Retorna 1 se a operacao foi bem sucedida e 0 caso contrario.
*/
int lista_insere_inicio_quadrado (lista_t* l, int elemento, int posicao, int wave_atual);

int lista_insere_inicio_powerup (lista_powerup* l, int elemento, int posicao);
/*
 * Remove o elemento do inicio da lista e o retorna em 'elemento'.
 * Retorna 1 se a operacao foi bem sucedida e 0 caso contrario.
*/
int lista_retira_inicio (lista_t* l, int* elemento);

int lista_retira_inicio_powerup (lista_powerup *l, int* elemento);

/*
 * Remove o elemento 'elemento' caso ele exista na lista.
 * Retorna 1 se a operacao foi bem sucedida e 0 caso contrario.
 * Se o elemento nao for encontrado na lista tambem retorna 0.
*/
int lista_retira_elemento (lista_t *l, int *elemento);

int lista_retira_elemento_powerup (lista_powerup *l, int *elemento);

#endif