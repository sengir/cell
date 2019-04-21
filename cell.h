#ifndef CELL_H
#define CELL_H

typedef struct Cell Cell;

struct Cell {
	SDL_Rect rect;
	uint16_t age;
	uint8_t r, g, b;
	uint8_t diseased;
	uint16_t brightness;
};

void usage(void);
void init(void);
void deinit(void);
void loop(void);
void keydown(SDL_KeyboardEvent);
void step(void);
void render(void);
int32_t rand32(int32_t, int32_t);
float randf(float, float);
void mix(Cell*, Cell*, Cell*);
void move(Cell*, Cell*);
uint8_t stronger(Cell*, Cell*);

#endif
