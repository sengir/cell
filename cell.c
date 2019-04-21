#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <SDL2/SDL.h>
#include "cell.h"

#define WIDTH 640
#define HEIGHT 480
#define SIZE 4
uint8_t MATE = 64, FREN = 64, DISEASE = 1;
float RAND_GENE = 1.1f;

uint16_t winw = WIDTH, winh = HEIGHT;
uint8_t size = SIZE;
uint16_t w = WIDTH/SIZE, h = HEIGHT/SIZE;
int8_t running = 0;

SDL_Window *win;
SDL_Renderer *ren;

Cell **world;

int
main(int argc, char **argv)
{
	if (argc != 6)
		usage();
	srand(atoi(argv[1]));
	MATE = atoi(argv[2]);
	FREN = atoi(argv[3]);
	DISEASE = atoi(argv[4]);
	RAND_GENE = atof(argv[5]);
	
	init();
	render();
	loop();
	deinit();
	
	return 0;
}

void
usage(void)
{
	printf("usage: cell <seed> <mate range> <friend range> <disease chance> <offspring color>\n");
	printf("To start with, I would recommend 'cell 1337 64 32 1 1.1'.\n");
	printf("Afterwards, feel free to mess around with the parameters to test out different scenarios.\n");
	printf("Use the 'P' key to toggle automatic playback and the spacebar to advance one step.\n");
	exit(0);
}

void
init()
{
	uint16_t x, y;
	SDL_Init(SDL_INIT_EVERYTHING);
	win = SDL_CreateWindow("cell", 0, 0, winw, winh, 0);
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	
	world = malloc(h*sizeof(Cell*));
	for (y = 0; y < h; y++) {
		world[y] = malloc(w*sizeof(Cell));
		for (x = 0; x < w; x++) {
			Cell cell;
			cell.rect = (SDL_Rect){ x*size, y*size, size, size };
			if (rand32(0, 100) < 33)
				cell.age = rand32(0, 255);
			else
				cell.age = 0;
			cell.r = rand32(0, 255);
			cell.g = rand32(0, 255);
			cell.b = rand32(0, 255);
			cell.diseased = 0;
			cell.brightness = cell.r + cell.g + cell.b;
			world[y][x] = cell;
		}
	}
}

void
deinit()
{
	uint16_t y;
	for (y = 0; y < h; y++)
		free(world[y]);
	free(world);
	
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	exit(0);
}

void
loop()
{
	SDL_Event e;
	for (;;) {
		SDL_Delay(5);
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					deinit();
					break;
				case SDL_KEYDOWN:
					keydown(e.key);
					break;
			}
		}
		if (running)
			step();
		else {
			SDL_Delay(50);
			SDL_RenderPresent(ren);
		}
	}
}

void
keydown(SDL_KeyboardEvent e)
{
	switch (e.keysym.sym) {
		case SDLK_SPACE:
			step();
			break;
		case SDLK_p:
			running = !running;
			break;
	}
}

void
step()
{
	uint16_t x, y;
	
	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++) {
			Cell *l, *r, *u, *d, *ul, *ur, *dl, *dr, *p1, *p2, *p3, *p4;
			uint8_t nneighbors;
			l = (x>0 ? &world[y][x-1] : &world[y][w-1]);
			r = (x<w-1 ? &world[y][x+1] : &world[y][0]);
			u = (y>0 ? &world[y-1][x] : &world[h-1][x]);
			d = (y<h-1 ? &world[y+1][x] : &world[0][x]);
			
			p1 = NULL;
			if (l->age)
				p1 = l;
			if (r->age && (!p1 || r->brightness > p1->brightness))
				p1 = r;
			if (u->age && (!p1 || u->brightness > p1->brightness))
				p1 = u;
			if (d->age && (!p1 || d->brightness > p1->brightness))
				p1 = d;
			
			p2 = NULL;
			if (l->age && l != p1)
				p2 = l;
			if (r->age && r != p1 && (!p2 || r->brightness > p2->brightness))
				p2 = r;
			if (u->age && u != p1 && (!p2 || u->brightness > p2->brightness))
				p2 = u;
			if (d->age && d != p1 && (!p2 || d->brightness > p2->brightness))
				p2 = d;
			
			p3 = NULL;
			if (l->age && l != p1 && l != p2)
				p3 = l;
			if (r->age && r != p1 && r != p2 && (!p3 || r->brightness > p3->brightness))
				p3 = r;
			if (u->age && u != p1 && u != p2 && (!p3 || u->brightness > p3->brightness))
				p3 = u;
			if (d->age && d != p1 && d != p2 && (!p3 || d->brightness > p3->brightness))
				p3 = d;
			
			p4 = NULL;
			if (l->age && l != p1 && l != p2 && l != p3)
				p4 = l;
			if (r->age && r != p1 && r != p2 && r != p3 && (!p4 || r->brightness > p4->brightness))
				p4 = r;
			if (u->age && u != p1 && u != p2 && u != p3 && (!p4 || u->brightness > p4->brightness))
				p4 = u;
			if (d->age && d != p1 && d != p2 && d != p3 && (!p4 || d->brightness > p4->brightness))
				p4 = d;
			
			nneighbors = 0;
			if (p1) nneighbors++;
			if (p2) nneighbors++;
			if (p3) nneighbors++;
			if (p4) nneighbors++;
			
			#define cell world[y][x]
			if (cell.age) {
				if ((cell.diseased && cell.age >= 127 - (cell.brightness>>6)) || (cell.age >= 255 - (cell.brightness>>6)))
					cell.age = 0;
				else {
					cell.age += (cell.brightness>>6);
					
					if (nneighbors > 1) {
						if (stronger(p1, &cell))
							move(p1, &cell);
						else if (stronger(p2, &cell))
							move(p2, &cell);
						else if (stronger(p3, &cell))
							move(p3, &cell);
						else if (stronger(p4, &cell))
							move(p4, &cell);
					} else if (nneighbors == 1) {
						if (l->age)
							move(r, &cell);
						else if (r->age)
							move(l, &cell);
						else if (u->age)
							move(d, &cell);
						else if (d->age)
							move(u, &cell);
					} else {
						switch (rand32(0, 3)) {
							case 0:
								move(l, &cell);
								break;
							case 1:
								move(r, &cell);
								break;
							case 2:
								move(u, &cell);
								break;
							case 3:
								move(d, &cell);
								break;
						}
					}
				}
			} else {
				if (!p1 || !p2 || p1->age < 2 || p2->age < 2)
					continue;
				
				if (abs(p1->r - p2->r) < MATE &&
					abs(p1->g- p2->g) < MATE &&
					abs(p1->b - p2->b) < MATE)
					mix(&cell, p1, p2);
				else if (p3 && p3->age >= 2 &&
					abs(p1->r - p2->r) < MATE &&
					abs(p1->g- p2->g) < MATE &&
					abs(p1->b - p2->b) < MATE)
					mix(&cell, p1, p3);
			}
		}
	
	render();
}

void
render()
{
	uint16_t x, y;
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
	SDL_RenderClear(ren);
	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++) {
			#define cell world[y][x]
			if (!cell.age)
				continue;
			SDL_SetRenderDrawColor(ren, cell.r, cell.g, cell.b, SDL_ALPHA_OPAQUE);
			SDL_RenderFillRect(ren, &cell.rect);
			
			if (cell.diseased) {
				SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
				SDL_RenderFillRect(ren, &(SDL_Rect){cell.rect.x+1,cell.rect.y+1,2,2});
			}
		}
	SDL_RenderPresent(ren);
}

int32_t
rand32(int32_t low, int32_t high)
{
	return rand() % (high - low + 1) + low;
}

float
randf(float low, float high)
{
	return low + (rand() / (float)RAND_MAX) * (high - low);
}

void
mix(Cell *dest, Cell *p1, Cell *p2)
{
	float rnd;
	float rndf;
	uint8_t r, g, b;
	uint8_t diseased;
	
	rndf = randf(0.f, RAND_GENE);
	r = p1->r*rndf + p2->r*(RAND_GENE-rndf);
	rndf = randf(0.f, RAND_GENE);
	g = p1->g*rndf + p2->g*(RAND_GENE-rndf);
	rndf = randf(0.f, RAND_GENE);
	b = p1->b*rndf + p2->b*(RAND_GENE-rndf);
	
	rnd = rand32(0, 99 + DISEASE);
	if (rnd < 50)
		diseased = p1->diseased;
	else if (rnd < 100)
		diseased = p2->diseased;
	else
		diseased = 1;
	
	dest->age = 1;
	dest->r = r;
	dest->g = g;
	dest->b = b;
	dest->diseased = diseased;
	dest->brightness = r + g + b;
}

void
move(Cell *dest, Cell *src)
{
	dest->age = src->age;
	dest->r = src->r;
	dest->g = src->g;
	dest->b = src->b;
	dest->diseased = src->diseased;
	dest->brightness = src->brightness;
	src->age = 0;
}

uint8_t
stronger(Cell *dest, Cell *src)
{
	int8_t c;
	
	if (!dest || !dest->age)
		return 0;
	
	if (abs(dest->r - src->r) <= FREN && abs(dest->g- src->g) <= FREN && abs(dest->b - src->b) <= FREN)
		return 0;
	
	c = 0;
	if (src->r > dest->g)
		c++;
	if (src->g > dest->b)
		c++;
	if (src->b > dest->r)
		c++;
	
	return c > 1;
}
