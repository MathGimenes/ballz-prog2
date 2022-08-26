#ifndef _LOGICA_JOGO_
#define _LOGICA_JOGO_

#include "libestruturas.h"

bool colide_parede (float *x);

bool colide_teto (float *y);

bool colide_piso (float y);

bool colide_quadrado (BOLA *bola, QUADRADO* quadrado);
bool colide_powerup (BOLA* bola, POWERUP *powerup);

void add_bola (BOLA *bolas[], int *tamanho, int indice);

void init_bolas (BOLA bolas [], int indice, int indice_colidiu);

void corrigir_x (float *x);

#endif 