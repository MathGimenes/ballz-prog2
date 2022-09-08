#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include "libestruturas.h"
#include "graficos.h"

void desenhar_powerups(nodo_powerup* aux_powerup, lista_powerup* powerups){
    aux_powerup = powerups->ini;
    for (int i = 0; i < powerups->tamanho; i++){
        al_draw_filled_circle (aux_powerup->powerup.cx, aux_powerup->powerup.cy, 15, al_map_rgb_f(1, 1, 1));
        aux_powerup = aux_powerup->prox;
    }

}