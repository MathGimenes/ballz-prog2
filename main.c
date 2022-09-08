#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "libestruturas.h"
#include "rng.h"
#include "logica_jogo.h"
#include "graficos.h"

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

int main() {
    srand (time(NULL));

    /* iniciando allegro */
    inicializar (al_init(), "allegro");

    /* iniciando teclado e mouse */
    inicializar (al_install_keyboard(), "teclado");
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

    /* iniciando audio */
    inicializar (al_install_audio(), "audio");
    inicializar (al_init_acodec_addon(), "codeec de audio");
    inicializar (al_reserve_samples(5), "reserve samples");

    /* iniciando efeitos de som */
    ALLEGRO_SAMPLE* game_over_som = al_load_sample("resources/game_over.wav");
    inicializar (game_over_som, "som game over");

    ALLEGRO_SAMPLE* clique = al_load_sample("resources/clique.wav");
    inicializar (clique, "som clique");

    ALLEGRO_SAMPLE*  new_game = al_load_sample("resources/new_game.ogg");
    inicializar (new_game, "som new_game");

    ALLEGRO_SAMPLE*  lancamento = al_load_sample("resources/lancamento.wav");
    inicializar (lancamento, "som lancamento");

    ALLEGRO_SAMPLE*  powerup_som = al_load_sample("resources/powerup.wav");
    inicializar (powerup_som, "som powerup");

    ALLEGRO_SAMPLE*  chao = al_load_sample("resources/chao.wav");
    inicializar (chao, "som chao");
    
    
    /* criando display */
    ALLEGRO_DISPLAY* disp = al_create_display(WIDTH, HEIGHT);
    inicializar (disp, "display");

    /* addon de fontes */
    inicializar (al_init_font_addon(), "addon de fontes");
    inicializar (al_init_ttf_addon(), "addon de fontes ttf");

    /* inicializando fonte dos numeros dos quadrados */
    ALLEGRO_FONT* fonte_quadrado = al_load_ttf_font("resources/font.otf", SQUARE_FONT_SIZE, 0);
    inicializar (fonte_quadrado, "fonte_quadrado");

    /* inicilizando fonte score */
    ALLEGRO_FONT* fonte_score = al_load_ttf_font("resources/font.otf", SCORE_FONT_SIZE, 0);
    inicializar (fonte_score, "fonte_score");

    /* inicializando fonte "best" */
    ALLEGRO_FONT* fonte_best = al_load_ttf_font("resources/font.otf", BEST_FONT_SIZE, 0);
    inicializar (fonte_best, "fonte_best");

    /* inicializando fonte do hiscore */
    ALLEGRO_FONT* fonte_hiscore = al_load_ttf_font("resources/font.otf", HISCORE_FONT_SIZE, 0);
    inicializar (fonte_hiscore, "fonte_hiscore");


    /* iniciando addon de primitivas */
    inicializar (al_init_primitives_addon(), "addon de primitivas");

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
    bool new_wave = true, refresh = false, game_over = false, bola_andando = false, help_window = false, restart = false, pause = false, cheatcode = false, digitando_cheatcode = false;
    int quantidade_aleatorizada;
    int quadrados_atuais = 0, powerups_atuais = 0;

    /* string que verifica o cheatcode */
    char* string_cheatcode = malloc(sizeof(char)*25);
    if (!string_cheatcode){
        fprintf (stderr, "Falha ao alocar string de cheatcode, terminando execucao...\n");
        exit(1);
    }
    string_cheatcode[0] = 'e';
    int caracteres_cheatcode;

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
    
    al_play_sample(new_game, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    while(1) {
        al_wait_for_event(queue, &event);

        switch(event.type){
            case ALLEGRO_EVENT_TIMER:
                
                /* aleatoriza as novas posicoes de powerups/quadrados */
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
                
                /* atualiza a tela */
                if (refresh){
                    aux = quadrados->ini;
                    for (int i = 0; i < quadrados->tamanho; i++){
                        aux->quadrado.y += speed_quadrados;
                        aux->quadrado.cy += speed_quadrados;
                        if (aux->quadrado.y > 630){
                            game_over = true;
                            al_play_sample(game_over_som, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        }
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
                
                /* atualizando hiscore caso ele seja maior */
                if (game_over || restart){
                    if (wave_atual > hiscore){
                        rewind (hiscore_file);
                        fprintf (hiscore_file, "%d", wave_atual);
                    }
                }
            
                /* voltando ao estado inicial */
                if (restart){
                    al_stop_samples();
                    al_play_sample(new_game, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
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



                /* parte que toma conta das colisoes */
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
                            al_stop_samples();
                            al_play_sample(chao, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
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
                                    aux->quadrado.batidas = cheatcode ? 0:aux->quadrado.batidas - 1;
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
                                al_stop_samples();
                                al_play_sample(powerup_som, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
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

                /* parte que calcula as novas posicoes */
                if (bola_andando){
                    posicao_bolas (bolas, quantidade_bolas);
                }
                redraw = true;
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                /* calcula a direcao quando o usuario aperta o botao do mouse */
                if (!bola_andando  && mousey < 680 && mousey > 100 && !help_window && !pause && !game_over){
                    al_stop_samples();
                    al_play_sample(lancamento, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    bolas[0].lancada = true;
                    constante = VELOCIDADE * VELOCIDADE / (((mousex - bolas[0].x) * (mousex - bolas[0].x)) + ((mousey - bolas[0].y) * (mousey - bolas[0].y))); 
                    constante = sqrtf(constante);
                    for (int i = 0; i < quantidade_bolas; i++){
                        bolas[i].vx = (mousex - bolas[i].x) * constante;
                        bolas[i].vy = (mousey - bolas[i].y) * constante;
                        bola_andando = true;
                    }
                }

                /* teste se o usuario apertou o botao na tela de pause*/
                if (pause){
                    /* botao continue */
                    float posx1 = WIDTH/2 - WIDTH/4.5, posx2 = WIDTH/2 + WIDTH/4.5; 
                    if ((mousex > posx1 && mousex < posx2) && (mousey > HEIGHT/2 - HEIGHT/6 + 25 && mousey < HEIGHT/2 - HEIGHT/6 + 75)){
                        pause = false;
                        al_play_sample(clique, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    }
                    
                    /* botao restart */
                    if ((mousex > posx1 && mousex < posx2) && (mousey > HEIGHT/2 - HEIGHT/6 + 100 && mousey < HEIGHT/2 - HEIGHT/6 + 150)){
                        pause = false;
                        restart = true;
                        al_play_sample(clique, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    }

                    /* botao quit */
                    if ((mousex > posx1 && mousex < posx2) && (mousey > HEIGHT/2 - HEIGHT/6 + 175 && mousey < HEIGHT/2 - HEIGHT/6 + 225)){
                        al_play_sample(clique, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        done = true;
                    }
                }

                /* teste se o usuario apertou o botao na tela de game over */
                if (game_over){
                    float posx1 = WIDTH/2 - WIDTH/4.5, posx2 = WIDTH/2 + WIDTH/4.5; 
                    /* botao restart */
                    if ((mousex > posx1 && mousex < posx2) && (mousey > HEIGHT/2 - HEIGHT/6 + 100 && mousey < HEIGHT/2 - HEIGHT/6 + 150)){
                        al_stop_samples ();
                        al_play_sample(clique, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        pause = false;
                        restart = true;
                    }

                    /* botao quit */
                    if ((mousex > posx1 && mousex < posx2) && (mousey > HEIGHT/2 - HEIGHT/6 + 175 && mousey < HEIGHT/2 - HEIGHT/6 + 225)){
                        al_stop_samples ();
                        al_play_sample(clique, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        done = true;
                    }
                }

                
                /* testa se o usuario apertou o botao de pause */
                if ((mousex > WIDTH/20  &&  mousex < WIDTH/20 + 2 * PAUSE_WIDTH + 10) && (mousey > 20 && mousey < 20 + PAUSE_HEIGHT)){
                    al_play_sample(clique, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    pause = pause ? false:true;
                }


                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                /* sai do jogo */
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    done = true;
                
                /* inicia a tela de pause */
                if (event.keyboard.keycode == ALLEGRO_KEY_F2)
                    pause = pause ? false:true;
                

                /* mostra a tela de ajuda */
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


                if (event.keyboard.keycode == ALLEGRO_KEY_E && !digitando_cheatcode){
                    caracteres_cheatcode = 1;
                    digitando_cheatcode = true;
                }else if (digitando_cheatcode){
                    caracteres_cheatcode++;
                    if (checar_cheatcode (string_cheatcode, caracteres_cheatcode, event.keyboard.keycode + 96)){
                        if (caracteres_cheatcode == 24){
                            cheatcode = cheatcode ? false : true;
                            digitando_cheatcode = false;
                        }
                    }else
                        digitando_cheatcode = false;

                }

                
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;

            case ALLEGRO_EVENT_MOUSE_AXES:
                mousex = event.mouse.x;
                mousey = event.mouse.y;

                /* escondendo o mouse na area de jogo */
                if (mousey > 100 && !help_window){
                    al_hide_mouse_cursor (disp);
                }else
                    al_show_mouse_cursor (disp);
                break;
        }
        


        if(done) 
            break;

        if(redraw && al_is_event_queue_empty(queue)) {
            
            /* cor do fundo */
            al_clear_to_color(al_map_rgb(32,32, 32));
            
            desenhar_powerups (aux_powerup, powerups);
            desenhar_quadrados (aux, quadrados, fonte_quadrado);
            desenhar_bolas (quantidade_bolas, bolas);

            if (!bola_andando && !help_window && !pause && !game_over){
                desenhar_caminho_cursor (mousex, mousey, bolas);
                al_draw_textf (fonte_best, al_map_rgb_f (1, 1, 1), bolas[0].x, bolas[0].y - BEST_FONT_SIZE * 1.8, 1, "x%d", quantidade_bolas);
            }
            
            desenha_faixas();
            desenha_fontes (fonte_hiscore, fonte_score, fonte_best, wave_atual, hiscore);
            desenha_botao_pause();

            if (help_window){
                desenha_ajuda (fonte_best);
            }

            if (pause){
                al_show_mouse_cursor (disp);
                desenha_pause (fonte_hiscore);

            }

            if (game_over){
                al_show_mouse_cursor (disp);
                desenha_game_over (fonte_hiscore);
            }

            al_flip_display(); 

            redraw = false;
        }
    }

    free (vetor_posicoes);
    free (bolas);
    free (string_cheatcode);
    fclose (hiscore_file);
    lista_destroi (quadrados); 
    lista_destroi_powerup (powerups);




    al_destroy_font(fonte_quadrado);
    al_destroy_font(fonte_best);
    al_destroy_font(fonte_hiscore);
    al_destroy_font(fonte_score);
    al_destroy_sample(game_over_som);
    al_destroy_sample(new_game);
    al_destroy_sample(clique);
    al_destroy_sample(lancamento);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);


    return 0;
}