// GRR20211781 Matheus Gimenes da Silva Viana
#ifndef _GRAFICOS_
#define _GRAFICOS_

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "libestruturas.h"

/* desenha os powerups na tela */
void desenhar_powerups(nodo_powerup* aux_powerup, lista_powerup* powerups);

/*determina a cor do quadrado  */
ALLEGRO_COLOR cor_quadrado(int batidas);

/* desenha os quadrados que estao em jogo na tela */
void desenhar_quadrados(nodo_l_t* aux, lista_t* quadrados, ALLEGRO_FONT* fonte_quadrado);

/* desenha as bolas */
void desenhar_bolas(int quantidade_bolas, BOLA* bolas);

/* desenha o caminho de mira */
void desenhar_caminho_cursor(int mousex, int mousey, BOLA* bolas);

/* desenha faixas de baixo e de cima */
void desenha_faixas();

/* desenha menu de ajuda */
void desenha_ajuda(ALLEGRO_FONT* fonte_best);

/* desenha menu de pause */
void desenha_pause(ALLEGRO_FONT* fonte_hiscore);

/* desena menu de game over */
void desenha_game_over(ALLEGRO_FONT* fonte_hiscore);

/* desenha fontes */
void desenha_fontes(ALLEGRO_FONT* fonte_hiscore, ALLEGRO_FONT* fonte_score, ALLEGRO_FONT* fonte_best, int wave_atual, int hiscore);

/* desenha o botao de pause */
void desenha_botao_pause();

#endif