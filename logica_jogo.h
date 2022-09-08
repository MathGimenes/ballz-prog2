#ifndef _LOGICA_JOGO_
#define _LOGICA_JOGO_

#include "libestruturas.h"

/* verifica se existe o arquivo de hiscore */
bool existe_hiscore ();

/* verifica se foi propriamente inicializado */
void inicializar (bool teste, const char *descricao);

/* testa colisao com as paredes */
bool colide_parede (float *x);

/* testa colisao com o teto */
bool colide_teto (float *y);

/* testa colisao com o piso */
bool colide_piso (float y);

/* testa se colidiu com os quadrados, e ajusta a veleocidade caso sim */
bool colide_quadrado (BOLA *bola, QUADRADO* quadrado);

/* testa se colidiiu coms powerups */
bool colide_powerup (BOLA* bola, POWERUP *powerup);

/* adiciona as bolas coletadas no vetor */
void add_bola (BOLA *bolas[], int *tamanho, int indice);

/* ionicia os valores das bolas */
void init_bolas (BOLA bolas [], int indice, int indice_colidiu);

/* corrige a posicao no eixo x, evitando out of bounds */
void corrigir_x (float *x);

/* atualiza a posica das bolas */
void posicao_bolas (BOLA* bolas, int quantidade_bolas);

bool checar_cheatcode (char string_cheatcode[], int caracteres_cheatcode, int letra_atual);

#endif 