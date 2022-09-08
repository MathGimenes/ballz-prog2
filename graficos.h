#ifndef _GRAFICOS_
#define _GRAFICOS_

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "libestruturas.h"

/* desenha os powerups na tela */
void desenhar_powerups(nodo_powerup* aux_powerup, lista_powerup* powerups);

/* desenha os quadrados que estao em jogo na tela */
void desenhar_quadrados(nodo_l_t* aux, lista_t* quadrados, ALLEGRO_FONT* fonte_quadrado);

/* desenha as bolas */
void desenhar_bolas(int quantidade_bolas, BOLA* bolas);

/* desenha o caminho de mira */
void desenhar_caminho_cursor(int mousex, int mousey, BOLA* bolas);

void desenha_faixas();

void desenha_ajuda(ALLEGRO_FONT* fonte_best);

void desenha_pause(ALLEGRO_FONT* fonte_hiscore);

void desenha_game_over(ALLEGRO_FONT* fonte_hiscore);


#endif