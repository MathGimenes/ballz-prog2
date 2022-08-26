#include <stdlib.h>
#include <stdio.h>

#include "libestruturas.h"
#include "rng.h"

#define WIDTH 480
#define HEIGHT 854
#define SQUARE_EDGE 62
#define GAP_SIZE 5
#define SIDE_GAP 8
#define ROW_SIZE 7
#define FIRST_ROW_Y 100

lista_powerup* lista_cria_powerup (){
    lista_powerup *tmp = malloc (sizeof(lista_powerup));
    if (tmp == NULL)
        return tmp;
    
    tmp->tamanho = 0;
    tmp->ini = NULL;
    return tmp;
}

lista_t* lista_cria (){
    lista_t *tmp = malloc (sizeof(lista_t));
    if (tmp == NULL)
        return tmp;
    
    tmp->tamanho = 0;
    tmp->ini = NULL;
    return tmp;
}

lista_powerup* lista_destroi_powerup (lista_powerup* l){
    nodo_powerup *tmp = l->ini;
    int i;
    for (i=1; i <= l->tamanho; i++){
        l->ini = l->ini->prox;
        free (tmp);
        tmp = l->ini;
    }
    free (l);
    l = NULL;
    return l;
}

lista_t* lista_destroi (lista_t* l){
    nodo_l_t *tmp = l->ini;
    int i;
    for (i=1; i <= l->tamanho; i++){
        l->ini = l->ini->prox;
        free (tmp);
        tmp = l->ini;
    }
    free (l);
    l = NULL;
    return l;
}

int lista_vazia (lista_t* l){
    if (l->tamanho == 0)
        return 1;
    return 0;
}

int lista_vazia_powerup (lista_powerup* l){
    if (l->tamanho == 0)
        return 1;
    return 0;
}


int lista_insere_inicio_quadrado (lista_t* l, int elemento, int posicao, int wave_atual){
    nodo_l_t *tmp = malloc (sizeof(nodo_l_t));
    if (tmp == NULL)
        return 0;
    
    tmp->prox = l->ini;
    tmp->elemento = elemento;
    tmp->quadrado.x = SIDE_GAP + (posicao * (SQUARE_EDGE + GAP_SIZE));
    tmp->quadrado.y = FIRST_ROW_Y;
    tmp->quadrado.cx = tmp->quadrado.x + SQUARE_EDGE/2;
    tmp->quadrado.cy = tmp->quadrado.y + SQUARE_EDGE/2;
    tmp->quadrado.batidas = wave_atual * aleatorio_batidas ();

    l->ini = tmp;
    l->tamanho++;
    
    return 1; 
}

int lista_insere_inicio_powerup (lista_powerup* l, int elemento, int posicao){
    nodo_powerup *tmp = malloc (sizeof(nodo_powerup));
    if (tmp == NULL)
        return 0;
    
    tmp->prox = l->ini;
    tmp->elemento = elemento;

    tmp->powerup.cx = (SIDE_GAP + SQUARE_EDGE/2) + (SQUARE_EDGE + GAP_SIZE) *posicao;
    tmp->powerup.cy = FIRST_ROW_Y + SQUARE_EDGE/2;

    l->ini = tmp;
    l->tamanho++;

    return 1;
}

int lista_retira_inicio (lista_t* l, int* elemento){
    if (lista_vazia (l) == 1 )
        return 0;
    *elemento = l->ini->elemento;
    nodo_l_t *tmp = l->ini;
    l->ini = l->ini->prox;
    l->tamanho--;
    free (tmp);
    tmp = NULL;
    
    return 1;
}

int lista_retira_inicio_powerup (lista_powerup *l, int* elemento){
    if (lista_vazia_powerup (l) == 1 )
        return 0;
    *elemento = l->ini->elemento;
    nodo_powerup *tmp = l->ini;
    l->ini = l->ini->prox;
    l->tamanho--;
    free (tmp);
    tmp = NULL;
    
    return 1;
}

int lista_retira_elemento (lista_t *l, int *elemento){
    if (lista_vazia (l) == 1)
        return 0;
    nodo_l_t *tmp = l->ini, *aux = l->ini;
    int i;
    for (i = 1; i < l->tamanho && tmp->elemento != *elemento; i++){
        aux = tmp;
        tmp = tmp->prox;
    }
    if (tmp->elemento == *elemento){
        if (l->tamanho == 1 || i ==1){
            lista_retira_inicio (l, elemento);
            return 1;
        }
        aux->prox = tmp->prox;
        free (tmp);
        tmp = NULL;
        l->tamanho--;
        return 1;
    }
    return 0;
}

int lista_retira_elemento_powerup (lista_powerup *l, int *elemento){
    if (lista_vazia_powerup (l) == 1)
        return 0;
    nodo_powerup *tmp = l->ini, *aux = l->ini;
    int i;
    for (i = 1; i < l->tamanho && tmp->elemento != *elemento; i++){
        aux = tmp;
        tmp = tmp->prox;
    }
    if (tmp->elemento == *elemento){
        if (l->tamanho == 1 || i ==1){
            lista_retira_inicio_powerup (l, elemento);
            return 1;
        }
        aux->prox = tmp->prox;
        free (tmp);
        tmp = NULL;
        l->tamanho--;
        return 1;
    }
    return 0;
}