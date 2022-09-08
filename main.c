#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include "libestruturas.h"
#include "rng.h"
#include "logica_jogo.h"

#define WIDTH 480
#define HEIGHT 854
#define SQUARE_EDGE 62
#define RAIO 8
#define GAP_SIZE 5
#define SIDE_GAP 8
#define ROW_SIZE 7
#define FRAMES 60
#define VELOCIDADE 11
#define SQUARE_FONT_SIZE 26
#define SCORE_FONT_SIZE 49
#define BEST_FONT_SIZE 17
#define HISCORE_FONT_SIZE 33
#define PAUSE_HEIGHT 25
#define PAUSE_WIDTH 5

void inicializar (bool teste, const char *descricao){
    if(teste) 
        return;

    printf("Nao foi possivel inicializar %s\n", descricao);
    exit(1);
}

bool existe_hiscore (){
    FILE *hiscore = fopen ("resources/.hiscore", "r");
    if (hiscore == NULL){
        return false;
    }
    else{
        fclose (hiscore);
        return true;
    }

}


int main() {
    srand (time(NULL));

    /* iniciando allegro */
    inicializar (al_init(), "allegro");

    /* iniciando teclado e mouse */
    inicializar (al_install_keyboard(), "keyboard");
    inicializar (al_install_mouse(), "mouse");

    /* iniciando timer, 1 evento a cada 1/60 segundos */
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FRAMES);
    inicializar (timer, "timer");

    /* fila de eventos */
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    inicializar (queue, "queue");


    /* iniciando anti-aliasing */
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    
    
    /* criando display */
    ALLEGRO_DISPLAY* disp = al_create_display(WIDTH, HEIGHT);
    inicializar (disp, "display");

    /* addon de fontes */
    inicializar (al_init_font_addon(), "addon font");
    inicializar (al_init_ttf_addon(), "addon ttf");

    /* inicializando fonte dos numeros dos quadrados */
    ALLEGRO_FONT* fonte_quadrado = al_load_ttf_font("resources/font.otf", SQUARE_FONT_SIZE, 0);
    inicializar (fonte_quadrado, "fonte");

    /* inicilizando fonte score */
    ALLEGRO_FONT* fonte_score = al_load_ttf_font("resources/font.otf", SCORE_FONT_SIZE, 0);
    inicializar (fonte_score, "fonte");

    /* inicializando fonte "best" */
    ALLEGRO_FONT* fonte_best = al_load_ttf_font("resources/font.otf", BEST_FONT_SIZE, 0);
    inicializar (fonte_best, "fonte");

    /* inicializando fonte do hiscore */
    ALLEGRO_FONT* fonte_hiscore = al_load_ttf_font("resources/font.otf", HISCORE_FONT_SIZE, 0);
    inicializar (fonte_hiscore, "fonte");


    /* iniciando addon de primitivas */
    inicializar (al_init_primitives_addon(), "primitives");

    /* registrando as fontes de eventos na fila de eventos */
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source()); 

    /* escondendo o cursor */
    al_hide_mouse_cursor(disp);


    /* booleans para desenhar na tela e terminar execucao */
    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    /* vetor de posicoes dos quadrados aleatorizados */    
    int *vetor_posicoes = malloc (sizeof (int) * ROW_SIZE);
    if (!vetor_posicoes){
        fprintf (stderr, "Falha ao alocar vetor de posições, terminando execucao...\n ");
        return 1;
    }

    /* lista dos quadrados que ainda estão no jogo */
    lista_t *quadrados = lista_cria ();
    if (!quadrados){
        fprintf (stderr, "falha ao alocar lista de quadrados, terminando execucao...\n");
        return 1;
    }

    /* lista dos powerups */
    lista_powerup *powerups = lista_cria_powerup();
    if (!powerups){
        fprintf (stderr, "Falha ao alocar lista de powerups, terminando execucao...\n");
        return 1;
    }

    int tamanho_vetor_bolas = 10;
    /* vetor de bolas */
    BOLA *bolas = malloc (sizeof (BOLA) * tamanho_vetor_bolas);
    if (!bolas){
        fprintf (stderr, "Falha ao alocar vetor de bolas, terminando execucao...\n");
        return 1;
    }

    /* inicializando a posicao da primeira bola */
    bolas[0].x = WIDTH/2;
    bolas[0].y = 690;
    bolas[0].colidiu = false;
    bolas[0].lancada = false;

    /* variaveis da posicao do mouse */
    int mousex = 0, mousey = 0;
    float constante;

    /* booleans que controlam os estados */
    bool new_wave = true, refresh = false, game_over = false, bola_andando = false, help_window = false, restart = false, pause = false;
    int quantidade_aleatorizada;
    int quadrados_atuais = 0, powerups_atuais = 0;

    /* lendo o hiscore (cria um arquivo de hiscore caso nao exista) */
    int hiscore;
    FILE *hiscore_file = NULL;
    if (existe_hiscore()){
        char *str = malloc (sizeof(char) * 32);
        if (!str){
            fprintf (stderr, "Falha ao alocar vetor que auxilia na leitura do hiscore, terminando execucao...\n");
            return 1;
        }
        hiscore_file = fopen ("resources/.hiscore", "r+");
        fgets (str, 31, hiscore_file);
        hiscore = atoi (str);
        free (str);
    }else{
        hiscore_file = fopen ("resources/.hiscore", "w+");
        fprintf (hiscore_file, "0");
        hiscore = 0;
    }


    /* booleanos auxiliares */
    bool primeiro = false;
    int primeiro_indice, indice_bolas = 0, quantidade_bolas = 1;

    int wave_atual = 0;

    int elemento = 0;

    /* ponteiros auxiliares */
    nodo_l_t *aux;
    nodo_powerup *aux_powerup;


    /* determinando a velocidade com que os quadrados descem */
    int iteracoes = FRAMES / 2;
    float speed_quadrados = (float)(SQUARE_EDGE + GAP_SIZE)/ (float)iteracoes;

    /* inicializando timer que controla os frames */
    al_start_timer(timer);
    
    while(1) {
        al_wait_for_event(queue, &event);

        switch(event.type){
            case ALLEGRO_EVENT_TIMER:
                
            
                if (new_wave){
                    quantidade_aleatorizada = aleatorizar_wave();
                    aleatorizar_posicoes (vetor_posicoes, quantidade_aleatorizada);
                    wave_atual++;
                    for (int i = 0; i < quantidade_aleatorizada; i++){
                        if (!wave_atual)
                            lista_insere_inicio_quadrado (quadrados, quadrados_atuais++, vetor_posicoes[i], wave_atual );
                        else
                            lista_insere_inicio_quadrado (quadrados, quadrados_atuais++, vetor_posicoes[i], wave_atual);
                    }

                    if (wave_atual > 1){
                        lista_insere_inicio_powerup (powerups, powerups_atuais++, aleatorizar_powerup(vetor_posicoes,quantidade_aleatorizada));
                    }
                    new_wave = false;
                    refresh = true;

                }
                
                if (refresh){
                    aux = quadrados->ini;
                    for (int i = 0; i < quadrados->tamanho; i++){
                        aux->quadrado.y += speed_quadrados;
                        aux->quadrado.cy += speed_quadrados;
                        aux = aux->prox;
                    }

                    if (wave_atual > 1){
                        aux_powerup = powerups->ini;
                        for (int i = 0; i < powerups->tamanho; i++){
                            aux_powerup->powerup.cy += speed_quadrados;
                            aux_powerup = aux_powerup->prox;
                        }
                    }
                    
                    if (iteracoes != 1){
                        iteracoes--;
                    }
                    else{
                        refresh = false;
                        iteracoes = FRAMES/2;
                    }
                
                }

                if (game_over || restart){
                    if (wave_atual > hiscore){
                        rewind (hiscore_file);
                        fprintf (hiscore_file, "%d", wave_atual);
                    }
                }
            
                
                if (restart){
                    new_wave = true;
                    refresh = false;
                    game_over = false; 
                    bola_andando = false; 
                    help_window = false;
                    restart = false;
                    quadrados_atuais = 0; 
                    powerups_atuais = 0;

                    primeiro = false;
                    indice_bolas = 0;
                    quantidade_bolas = 1;
                    wave_atual = 0;
                    iteracoes = FRAMES / 2;


                    primeiro = false;



                    
                    lista_destroi (quadrados);
                    quadrados = lista_cria ();
                    if (!quadrados){
                        fprintf (stderr, "falha ao alocar lista de quadrados, terminando execucao...\n");
                        return 1;
                    }

                    lista_destroi_powerup (powerups);
                    powerups = lista_cria_powerup();
                    if (!powerups){
                        fprintf (stderr, "Falha ao alocar lista de powerups, terminando execucao...\n");
                        return 1;
                    }

                    bolas[0].x = WIDTH/2;
                    bolas[0].y = 690;
                    bolas[0].colidiu = false;
                    bolas[0].lancada = false;

                    char *str = malloc (sizeof(char) * 32);
                    if (!str){
                        fprintf (stderr, "Falha ao alocar vetor que auxilia na leitura do hiscore, terminando execucao...\n");
                        return 1;
                    }
                    rewind (hiscore_file);
                    fgets (str, 31, hiscore_file);
                    hiscore = atoi (str);
                    free (str);
                }




                if (bola_andando){
                    int contador_colisoes = 0; 
                    
                    for (int j = 0; j < quantidade_bolas; j++){
                        bool colidiu_laco = false;
                        if (colide_parede (&bolas[j].x)){
                            bolas[j].vx *= -1;
                            
                            }
                        if (colide_teto (&bolas[j].y))
                            bolas[j].vy *= -1;
                        if (!bolas[j].colidiu && colide_piso (bolas[j].y)){
                            contador_colisoes++;
                            if (!primeiro){
                                primeiro_indice = j;
                                primeiro = true;
                                corrigir_x (&bolas[j].x);

                            }
                            bolas[j].colidiu = true;
                            bolas[j].lancada = false;
                            bolas[j].y = 690;
                            bolas[j].x = bolas[primeiro_indice].x;
                        }else if (bolas[j].colidiu){
                            contador_colisoes++;
                        }
                        if (contador_colisoes == quantidade_bolas){
                            for (int i = 0; i < quantidade_bolas; i++){
                                bolas[i].colidiu = false;
                            }
                            refresh = true;
                            bola_andando = false;
                            new_wave = true;
                            primeiro = false;
                            init_bolas (bolas, indice_bolas, primeiro_indice);
                            quantidade_bolas = indice_bolas + 1;
                        }
                        aux = quadrados->ini;
                        for (int i = 0, k = quadrados->tamanho; i < k; i++){
                            bool aux_colisao, zerou = false;
                            if (!colidiu_laco){
                                aux_colisao = colide_quadrado (&bolas[j], &aux->quadrado);
                                if (aux_colisao){
                                    aux->quadrado.batidas--;
                                    elemento = aux->elemento;
                                    colidiu_laco = true;

                                    zerou = aux->quadrado.batidas == 0;
                                    if (zerou){
                                        aux = aux->prox;
                                        lista_retira_elemento (quadrados, &elemento);
                                    }
                                }
                            }
                            if (!zerou)
                                aux = aux->prox;
                            zerou = false;
                        }

                        aux_powerup = powerups->ini;
                        for (int i = 0; i < powerups->tamanho; i++){
                            bool aux_colisao = colide_powerup (&bolas[j], &aux_powerup->powerup);
                            if (aux_colisao){
                                elemento = aux_powerup->elemento;
                                aux_powerup = aux_powerup->prox;
                                lista_retira_elemento_powerup (powerups, &elemento);
                                add_bola (&bolas, &tamanho_vetor_bolas, ++indice_bolas);
                                
                            }
                            if (!aux_colisao)
                                aux_powerup = aux_powerup->prox;
                        }
                    }

                }


                if (bola_andando){
                for (int i = 0; i < quantidade_bolas; i++){
                    if (i){
                        if (bolas[i - 1].lancada){
                            float distancia_quadrada = (float)(bolas[i - 1].x - bolas[i].x) * (bolas[i - 1].x - bolas[i].x) + (bolas[i - 1].y - bolas[i].y) * (bolas[i - 1].y - bolas[i].y);
                            if (distancia_quadrada > (RAIO * 2 + 25) * (RAIO * 2 + 25))
                                bolas[i].lancada = true;
                        }
                    }
                    if (!bolas[i].colidiu && bolas[i].lancada){
                        bolas[i].x += bolas[i].vx;
                        bolas[i].y += bolas[i].vy;
                    }
                }
            }
                redraw = true;
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                bolas[0].lancada = true;
                if (!bola_andando  && mousey < 680 && mousey > 100 && !help_window && !pause && !game_over){
                    constante = VELOCIDADE * VELOCIDADE / (((mousex - bolas[0].x) * (mousex - bolas[0].x)) + ((mousey - bolas[0].y) * (mousey - bolas[0].y))); 
                    constante = sqrtf(constante);
                    for (int i = 0; i < quantidade_bolas; i++){
                        bolas[i].vx = (mousex - bolas[i].x) * constante;
                        bolas[i].vy = (mousey - bolas[i].y) * constante;
                        bola_andando = true;
                    }
                }

                if (pause){
                    /* botao continue */
                    float posx1 = WIDTH/2 - WIDTH/4.5, posx2 = WIDTH/2 + WIDTH/4.5; 
                    if ((mousex > posx1 && mousex < posx2) && (mousey > HEIGHT/2 - HEIGHT/6 + 25 && mousey < HEIGHT/2 - HEIGHT/6 + 75))
                        pause = false;
                    
                    /* botao restart */
                    if ((mousex > posx1 && mousex < posx2) && (mousey > HEIGHT/2 - HEIGHT/6 + 100 && mousey < HEIGHT/2 - HEIGHT/6 + 150)){
                        pause = false;
                        restart = true;
                    }

                    /* botao quit */
                    if ((mousex > posx1 && mousex < posx2) && (mousey > HEIGHT/2 - HEIGHT/6 + 175 && mousey < HEIGHT/2 - HEIGHT/6 + 225))
                        done = true;
                }

                if (game_over){
                    float posx1 = WIDTH/2 - WIDTH/4.5, posx2 = WIDTH/2 + WIDTH/4.5; 
                    /* botao restart */
                    if ((mousex > posx1 && mousex < posx2) && (mousey > HEIGHT/2 - HEIGHT/6 + 100 && mousey < HEIGHT/2 - HEIGHT/6 + 150)){
                        pause = false;
                        restart = true;
                    }

                    /* botao quit */
                    if ((mousex > posx1 && mousex < posx2) && (mousey > HEIGHT/2 - HEIGHT/6 + 175 && mousey < HEIGHT/2 - HEIGHT/6 + 225))
                        done = true;
                }

                

                if ((mousex > WIDTH/20  &&  mousex < WIDTH/20 + 2 * PAUSE_WIDTH + 10) && (mousey > 20 && mousey < 20 + PAUSE_HEIGHT))
                    pause = pause ? false:true;


                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    done = true;
                if (event.keyboard.keycode == ALLEGRO_KEY_F2)
                    pause = pause ? false:true;
                if (event.keyboard.keycode == ALLEGRO_KEY_F1){
                    if (help_window){
                        help_window = false;
                        al_hide_mouse_cursor (disp);
                    }else{
                        help_window = true;
                        if (!pause)
                            al_show_mouse_cursor (disp);
                    }
                }
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;

            case ALLEGRO_EVENT_MOUSE_AXES:
                mousex = event.mouse.x;
                mousey = event.mouse.y;
                if (mousey > 100 && !help_window){
                    al_hide_mouse_cursor (disp);
                }else
                    al_show_mouse_cursor (disp);
                break;
        }
        


        if(done) 
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(32,32, 32));

            
            

            aux_powerup = powerups->ini;
            for (int i = 0; i < powerups->tamanho; i++){
                al_draw_filled_circle (aux_powerup->powerup.cx, aux_powerup->powerup.cy, 15, al_map_rgb_f(1, 1, 1));
                aux_powerup = aux_powerup->prox;
            }


            aux = quadrados->ini;
            for (int i = 0; i < quadrados->tamanho; i++){
                al_draw_filled_rectangle(aux->quadrado.x, aux->quadrado.y, aux->quadrado.x + (SQUARE_EDGE), aux->quadrado.y + (SQUARE_EDGE), al_map_rgb_f(1, 0, 0));
                al_draw_textf (fonte_quadrado, al_map_rgb_f (0, 0, 0), aux->quadrado.x + SQUARE_EDGE/2, aux->quadrado.y + SQUARE_FONT_SIZE/2, 1, "%d", aux->quadrado.batidas);
                if (aux->quadrado.y > 630)
                    game_over = true;
                aux = aux->prox;  
            }

            




            for (int i = 0; i < quantidade_bolas; i++){
                al_draw_filled_circle (bolas[i].x, bolas[i].y, RAIO, al_map_rgb_f (1, 0, 1 ));
            
            }

            if (!bola_andando && !help_window && !pause && !game_over){
                if (mousey < 680 && mousey > 100){
                    al_draw_filled_circle (mousex, mousey, 5, al_map_rgb_f (1, 1, 0 ));
                    al_draw_filled_circle ((mousex + bolas[0].x)/2, (mousey + bolas[0].y)/2, 5, al_map_rgb_f (1, 1, 0 ));
                    al_draw_filled_circle ((((mousex + bolas[0].x)/2) + mousex)/2, (((mousey + bolas[0].y)/2) + mousey)/2, 5, al_map_rgb_f (1, 1, 0 ));
                }
                al_draw_textf (fonte_best, al_map_rgb_f (1, 1, 1), bolas[0].x, bolas[0].y - BEST_FONT_SIZE * 1.8, 1, "x%d", quantidade_bolas);
            }
            /* faixa de cima */
            al_draw_filled_rectangle(0, 0, WIDTH, 100, al_map_rgb(38, 38, 38));

            /* faixa de baixo */
            al_draw_filled_rectangle(0, 698, 480, HEIGHT, al_map_rgb(38, 38, 38));

            al_draw_textf (fonte_score, al_map_rgb_f (1, 1, 1), WIDTH/2, 0, 1, "%d", wave_atual);
            al_draw_text (fonte_best, al_map_rgb_f (1, 1, 1), WIDTH/6, 5, 1, "BEST");
            al_draw_textf (fonte_hiscore, al_map_rgb_f (1, 1, 1), WIDTH/6, 15, 1, "%d", hiscore);


            for (int i = 0; i < 11; i += 10){
                al_draw_filled_rounded_rectangle (WIDTH/20 + i, 20, WIDTH/20 + PAUSE_WIDTH + i, 20 + PAUSE_HEIGHT , PAUSE_WIDTH/2, PAUSE_WIDTH/2, al_map_rgb (70, 70, 70));
            }
            
            

            if (help_window){
                al_draw_filled_rectangle(WIDTH/8, 200, 7*WIDTH/8, 550, al_map_rgb (80, 80, 80));
                al_draw_multiline_text (fonte_best, al_map_rgb_f (1, 1, 1), WIDTH/8, 200, WIDTH/8 + 7*WIDTH/8, 0 , 0, "Controles:\nBotao esquerdo mouse: Lanca a bola\nMouse: Seleciona a direcao desejada\nF1: Abre esta janela de ajuda\nF2: Pausa o jogo\nESC: Termina a execucao do jogo\n\nCheatcode: essejogonaotemcheatcodes");
                al_draw_multiline_text (fonte_best, al_map_rgb_f (1, 1, 1), WIDTH/8, 550 - 3 * BEST_FONT_SIZE, WIDTH/8 + 7*WIDTH/8, 0, 0, "Autor:\nMatheus Gimenes da Silva Viana");
            }

            if (pause){
                /* fundo */
                al_draw_filled_rectangle(WIDTH/2 - WIDTH/4, HEIGHT/2 - HEIGHT/6, WIDTH/2 + WIDTH/4, HEIGHT/2 - HEIGHT/6 + 250, al_map_rgb (80, 80, 80));
                al_show_mouse_cursor (disp);

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

            if (game_over){
                /* fundo */
                al_draw_filled_rectangle(WIDTH/2 - WIDTH/4, HEIGHT/2 - HEIGHT/6 + 75, WIDTH/2 + WIDTH/4, HEIGHT/2 - HEIGHT/6 + 250, al_map_rgb (80, 80, 80));
                al_show_mouse_cursor (disp);

                /* botoes */

                /* restart */
                al_draw_filled_rounded_rectangle (WIDTH/2 - WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 100, WIDTH/2 + WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 150, 25, 25, al_map_rgb (245, 181, 46));
                al_draw_text (fonte_hiscore, al_map_rgb_f (1, 1, 1), WIDTH/2, ((HEIGHT/2 - HEIGHT/6 + 100) + (HEIGHT/2 - HEIGHT/6 + 150))/2 - HISCORE_FONT_SIZE/1.5, 1, "RESTART");

                /* quit */
                al_draw_filled_rounded_rectangle (WIDTH/2 - WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 175, WIDTH/2 + WIDTH/4.5, HEIGHT/2 - HEIGHT/6 + 225, 25, 25, al_map_rgb (0, 163, 150));
                al_draw_text (fonte_hiscore, al_map_rgb_f (1, 1, 1), WIDTH/2, ((HEIGHT/2 - HEIGHT/6 + 175) + (HEIGHT/2 - HEIGHT/6 + 225))/2 - HISCORE_FONT_SIZE/1.5, 1, "QUIT");

            }

            al_flip_display(); 

            redraw = false;
        }
    }

    free (vetor_posicoes);
    free (bolas);
    lista_destroi (quadrados); 
    lista_destroi_powerup (powerups);



    al_destroy_font(fonte_quadrado);
    al_destroy_font(fonte_best);
    al_destroy_font(fonte_hiscore);
    al_destroy_font(fonte_score);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    fclose (hiscore_file);

    return 0;
}