#include <allegro5/allegro5.h>
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

#define new_max(x,y) (((x) >= (y)) ? (x) : (y))
#define new_min(x,y) (((x) <= (y)) ? (x) : (y))



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

    if (distanciax > ((float)SQUARE_EDGE/2 + RAIO))
        return false;
    if (distanciay > ((float)SQUARE_EDGE/2 + RAIO + GAP_SIZE/2))
        return false;
    
    if ((distanciax <= ((float)SQUARE_EDGE/2 + RAIO)) && distanciay < distanciax){
        if (distanciax - distanciay > 3){
            bola->vx *= -1;
            return true;
        }
    }
    if (distanciay <= ((float)SQUARE_EDGE/2 + RAIO + GAP_SIZE/2)){
        if (distanciay - distanciax > 3){
            bola->vy *= -1;
            return true;
        }
    }
    
    float distancia_pontos = ((distanciax - SQUARE_EDGE/2) * (distanciax - SQUARE_EDGE/2)) + ((distanciay - SQUARE_EDGE/2 ) * (distanciay - SQUARE_EDGE/2 ));

    if (distancia_pontos <= (RAIO * RAIO )){
        if (fabs (bola->vx) > fabs (bola->vy))
            bola->vx *= -1;
        else if (fabs (bola->vy) > fabs (bola->vx))
            bola->vy *= -1;
        else{
            bola->vx *= -1;
            bola->vy *= -1;
        } 

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
