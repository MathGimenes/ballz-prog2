#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "libestruturas.h"
#include "graficos.h"

#define SQUARE_FONT_SIZE 26
#define SCORE_FONT_SIZE 49
#define BEST_FONT_SIZE 17
#define HISCORE_FONT_SIZE 33
#define PAUSE_HEIGHT 25
#define PAUSE_WIDTH 5
#define WIDTH 480
#define HEIGHT 854
#define SQUARE_EDGE 62
#define RAIO 8

void desenhar_powerups(nodo_powerup* aux_powerup, lista_powerup* powerups){
    aux_powerup = powerups->ini;
    for (int i = 0; i < powerups->tamanho; i++){
        al_draw_filled_circle (aux_powerup->powerup.cx, aux_powerup->powerup.cy, 15, al_map_rgb_f(1, 1, 1));
        aux_powerup = aux_powerup->prox;
    }

}


void desenhar_quadrados(nodo_l_t* aux, lista_t* quadrados, ALLEGRO_FONT* fonte_quadrado){
    aux = quadrados->ini;
    for (int i = 0; i < quadrados->tamanho; i++){
        al_draw_filled_rectangle(aux->quadrado.x, aux->quadrado.y, aux->quadrado.x + (SQUARE_EDGE), aux->quadrado.y + (SQUARE_EDGE), al_map_rgb_f(1, 0, 0));
        al_draw_textf (fonte_quadrado, al_map_rgb_f (0, 0, 0), aux->quadrado.x + SQUARE_EDGE/2, aux->quadrado.y + SQUARE_FONT_SIZE/2, 1, "%d", aux->quadrado.batidas);

        aux = aux->prox;  
    }
}

void desenhar_bolas(int quantidade_bolas, BOLA* bolas){

    for (int i = 0; i < quantidade_bolas; i++){
        al_draw_filled_circle (bolas[i].x, bolas[i].y, RAIO, al_map_rgb (223, 112, 0));

    }

}

void desenhar_caminho_cursor(int mousex, int mousey, BOLA* bolas){
    if (mousey < 680 && mousey > 100){
        al_draw_filled_circle (mousex, mousey, 5, al_map_rgb_f (1, 1, 1 ));
        al_draw_filled_circle ((mousex + bolas[0].x)/2, (mousey + bolas[0].y)/2, 5, al_map_rgb_f (1, 1, 1 ));
        al_draw_filled_circle ((((mousex + bolas[0].x)/2) + mousex)/2, (((mousey + bolas[0].y)/2) + mousey)/2, 5, al_map_rgb_f (1, 1, 1 ));
        al_draw_filled_circle ((bolas[0].x + ((mousex + bolas[0].x)/2))/2, (bolas[0].y + ((mousey + bolas[0].y)/2))/2, 5, al_map_rgb_f (1, 1, 1));
    }
}


void desenha_faixas(){

    /* faixa de cima */
    al_draw_filled_rectangle(0, 0, WIDTH, 100, al_map_rgb(38, 38, 38));

    /* faixa de baixo */
    al_draw_filled_rectangle(0, 698, 480, HEIGHT, al_map_rgb(38, 38, 38));


}

void desenha_ajuda(ALLEGRO_FONT* fonte_best){
    al_draw_filled_rectangle(WIDTH/8, 200, 7*WIDTH/8, 550, al_map_rgb (80, 80, 80));
    al_draw_multiline_text (fonte_best, al_map_rgb_f (1, 1, 1), WIDTH/8, 200, WIDTH/8 + 7*WIDTH/8, 0 , 0, "Controles:\nBotao esquerdo mouse: Lanca a bola\nMouse: Seleciona a direcao desejada\nF1: Abre esta janela de ajuda\nF2: Pausa o jogo\nESC: Termina a execucao do jogo\n\nCheatcode: essejogonaotemcheatcodes");
    al_draw_multiline_text (fonte_best, al_map_rgb_f (1, 1, 1), WIDTH/8, 550 - 3 * BEST_FONT_SIZE, WIDTH/8 + 7*WIDTH/8, 0, 0, "Autor:\nMatheus Gimenes da Silva Viana");
}

void desenha_pause(ALLEGRO_FONT* fonte_hiscore){
    /* fundo */
    al_draw_filled_rectangle(WIDTH/2 - WIDTH/4, HEIGHT/2 - HEIGHT/6, WIDTH/2 + WIDTH/4, HEIGHT/2 - HEIGHT/6 + 250, al_map_rgb (80, 80, 80));


    /* botoes */

    /* continue */
    al_draw_filled_rounded_rectangle (WIDTH/2 - WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 25, WIDTH/2 + WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 75, 25, 25, al_map_rgb (234, 34, 94));
    al_draw_text (fonte_hiscore, al_map_rgb_f (1, 1, 1), WIDTH/2, (((HEIGHT/2 - HEIGHT/6 + 25) + (HEIGHT/2 - HEIGHT/6 + 75))/2) - HISCORE_FONT_SIZE/1.5, 1, "CONTINUE");

    /* restart */
    al_draw_filled_rounded_rectangle (WIDTH/2 - WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 100, WIDTH/2 + WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 150, 25, 25, al_map_rgb (245, 181, 46));
    al_draw_text (fonte_hiscore, al_map_rgb_f (1, 1, 1), WIDTH/2, ((HEIGHT/2 - HEIGHT/6 + 100) + (HEIGHT/2 - HEIGHT/6 + 150))/2 - HISCORE_FONT_SIZE/1.5, 1, "RESTART");

    /* quit */
    al_draw_filled_rounded_rectangle (WIDTH/2 - WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 175, WIDTH/2 + WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 225, 25, 25, al_map_rgb (0, 163, 150));
    al_draw_text (fonte_hiscore, al_map_rgb_f (1, 1, 1), WIDTH/2, ((HEIGHT/2 - HEIGHT/6 + 175) + (HEIGHT/2 - HEIGHT/6 + 225))/2 - HISCORE_FONT_SIZE/1.5, 1, "QUIT");

}

void desenha_game_over(ALLEGRO_FONT* fonte_hiscore){

    /* fundo */
    al_draw_filled_rectangle(WIDTH/2 - WIDTH/4, HEIGHT/2 - HEIGHT/6 + 75, WIDTH/2 + WIDTH/4, HEIGHT/2 - HEIGHT/6 + 250, al_map_rgb (80, 80, 80));


    /* botoes */

    /* restart */
    al_draw_filled_rounded_rectangle (WIDTH/2 - WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 100, WIDTH/2 + WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 150, 25, 25, al_map_rgb (245, 181, 46));
    al_draw_text (fonte_hiscore, al_map_rgb_f (1, 1, 1), WIDTH/2, ((HEIGHT/2 - HEIGHT/6 + 100) + (HEIGHT/2 - HEIGHT/6 + 150))/2 - HISCORE_FONT_SIZE/1.5, 1, "RESTART");

    /* quit */
    al_draw_filled_rounded_rectangle (WIDTH/2 - WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 175, WIDTH/2 + WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 225, 25, 25, al_map_rgb (0, 163, 150));
    al_draw_text (fonte_hiscore, al_map_rgb_f (1, 1, 1), WIDTH/2, ((HEIGHT/2 - HEIGHT/6 + 175) + (HEIGHT/2 - HEIGHT/6 + 225))/2 - HISCORE_FONT_SIZE/1.5, 1, "QUIT");
}