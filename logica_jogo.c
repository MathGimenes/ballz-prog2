// GRR20211781 Matheus Gimenes da Silva Viana
#include <allegro5/allegro5.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "logica_jogo.h"
#include "libestruturas.h"

#define WIDTH 480
#define YTOP 100
#define YBOTTOM 698
#define RAIO 8
#define RAIO_POWERUP 15
#define SQUARE_EDGE 62
#define GAP_SIZE 5
#define DIAGONAL 43.84062

bool existe_hiscore (){
    FILE *hiscore = fopen ("resources/.hiscore", "r");
    if (hiscore != NULL){
        fclose (hiscore);
        return true;
    }
    else{
        return false;
    }

}


void inicializar (bool teste, const char *descricao){
    if(teste) 
        return;

    printf("Nao foi possivel inicializar %s\n", descricao);
    exit(1);
}




bool colide_parede (float *x){
    if (*x + RAIO > WIDTH){
        *x = WIDTH - RAIO;
        return true;
    }
    if (*x - RAIO < 0){
        *x = RAIO;
        return true;
    }

    
    return false;
}

bool colide_teto (float *y){
    if (*y - RAIO < YTOP){
        *y = YTOP + RAIO;
        return true;
    }

    return false;
}

bool colide_piso (float y){
    if (y + RAIO > YBOTTOM){
        return true;
    }
         
    return false;
}

bool colide_quadrado (BOLA *bola, QUADRADO* quadrado){
    float distanciax = fabs (bola->x - quadrado->cx), distanciay = fabs (bola->y - quadrado->cy);

    if (distanciax > (SQUARE_EDGE/2 + RAIO + GAP_SIZE/2))
        return false;
    if (distanciay > (SQUARE_EDGE/2 + RAIO + GAP_SIZE/2))
        return false;
    
    if ((distanciax <= ((float)SQUARE_EDGE/2 + RAIO + GAP_SIZE/2)) && distanciay < distanciax){
        if (fabs(distanciax - distanciay) > 5){
            bola->vx *= -1;
            return true;
        }
    }
    if (distanciay <= ((float)SQUARE_EDGE/2 + RAIO + GAP_SIZE/2)&& distanciay > distanciax){
        if (fabs(distanciay - distanciax) > 5){
            bola->vy *= -1;
            return true;
        }
    }
    
    float distancia_pontos = ((distanciax) * (distanciax )) + ((distanciay  ) * (distanciay ));
    if (distancia_pontos <= ((RAIO + DIAGONAL) * (RAIO + DIAGONAL) )){
        if ((bola->vx > 0) && (bola->vy < 0)){
            if ((bola->y < quadrado->cy) && (bola->x < quadrado->cx)){
                bola->vx *= -1;
                }
            else if ((bola->y > quadrado->cy) && (bola->x > quadrado->cx))
                bola->vy *= -1;
            else {
                if (fabs(bola->vx) > fabs(bola->vy))
                    bola->vx *= -1;
                else
                    bola->vy *= -1;
            } 
        }else if ((bola->vx >0) && (bola->vy > 0)){
            if ((bola->x > quadrado->cx) && (bola->y < quadrado->cy))
                bola->vy *= -1;
            else if ((bola->x < quadrado->cx) && (bola->y > quadrado->cy))
                bola->vx *= -1;
            else{
                if (fabs(bola->vx) > fabs(bola->vy))
                    bola->vx *= -1;
                else
                    bola->vy *= -1;
            }
        }else if ((bola->vx < 0) && (bola->vy > 0)){
            if ((bola->x > quadrado->cx) && (bola->y > quadrado->cy))
                bola->vx *= -1;
            else if ((bola->x < quadrado->cx) && (bola->y < quadrado->cy))
                bola->vy *=-1;
            else{
                if (fabs(bola->vx) > fabs(bola->vy))
                    bola->vx *= -1;
                else
                    bola->vy *=-1;
            }
        }else if ((bola->vx < 0) && (bola->vy < 0)){
            if ((bola->x < quadrado->cx) && (bola->y > quadrado->cy))
                bola->vy *= -1;
            else if ((bola->x > quadrado->cx) && (bola->y < quadrado->cy))
                bola->vx *= -1;
            else{
                if (fabs(bola->vx) > fabs(bola->vy))
                    bola->vx *= -1;
                else
                    bola->vy *= -1; 
            }

        }else if (bola->vx == bola->vy){
            bola->vy *= -1;
            bola->vx *= -1;
        }else
            bola->vy *= -1;

        return true;
    }

    return false;
}


bool colide_powerup (BOLA* bola, POWERUP *powerup){
    float distanciax = fabs (bola->x - powerup->cx), distanciay = fabs (bola->y - powerup->cy);

    if (distanciax > RAIO + RAIO_POWERUP)
        return false;
    if (distanciay > RAIO + RAIO_POWERUP)
        return false;
    
    float distancia_centros = distanciax * distanciax + distanciay * distanciay;

    if (distancia_centros <= (float)(RAIO + RAIO_POWERUP) * (float)(RAIO + RAIO_POWERUP))
        return true;

    return false;
}

void add_bola (BOLA *bolas[], int *tamanho, int indice){
    if (indice > *tamanho -1){
        *tamanho += 5;
        *bolas = realloc (*bolas, sizeof (BOLA) * *tamanho);
    }

}

void init_bolas (BOLA bolas [], int indice, int indice_colidiu){
    for (int i = 0; i <= indice; i++){
        bolas[i].x = bolas[indice_colidiu].x;
        bolas[i].y = YBOTTOM - RAIO;
        bolas[i].colidiu = false;
        bolas[i].lancada = false;

    }

}

void corrigir_x (float *x){
    if (*x < RAIO)
        *x = RAIO;
    else if (*x > WIDTH - RAIO)
        *x = WIDTH - RAIO;
}

void posicao_bolas (BOLA* bolas, int quantidade_bolas){
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

bool checar_cheatcode (char string_cheatcode[], int caracteres_cheatcode, int letra_atual){
    string_cheatcode[caracteres_cheatcode - 1] = letra_atual;
    
    if (!strncmp (string_cheatcode, "essejogonaotemcheatcodes", caracteres_cheatcode))
        return true;
    
    return false;
}   