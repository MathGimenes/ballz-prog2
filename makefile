#makefile sumarizaçao de dados gps


#flags de compilacao
CFLAGS = -Wall -std=c99 -g

ALLEGRO = -lallegro_image  -lallegro_primitives  -lallegro_dialog  -lallegro_ttf -lallegro_font -lallegro_ttf -lallegro -lm
OBJS = libestruturas.o rng.o main.o logica_jogo.o graficos.o

PROGS = main

all: $(PROGS)

$(PROGS): $(OBJS)
	gcc -o $(PROGS) $(OBJS) $(CFLAGS) $(ALLEGRO)

libestruturas.o: libestruturas.c libestruturas.h
	gcc -c $< $(CFLAGS)

rng.o: rng.c rng.h
	gcc -c $< $(CFLAGS)

logica_jogo.o: logica_jogo.c logica_jogo.h
	gcc -c $< $(CFLAGS)

graficos.o: graficos.c graficos.h
	gcc -c $< $(CFLAGS)

clean : 
	rm -f *~ *.o *.gch

purge : clean
	rm -f $(PROGS)
