#ifndef USER_HEADER
#define USER_HEADER

#include "media.h"
#include "shape.h"

#define WINDOW_W 320
#define WINDOW_H 180
#define WINDOW_TITLE "justgib"

#define MAX_SHAPES 64

typedef struct state state;
typedef struct id_stack id_stack;

typedef struct id_stack {
	pool* mem;
	id_stack* next;
	uint16_t id;
} id_stack;

int16_t next_id(id_stack** ids);
void id_push(id_stack** ids, uint16_t idd);

typedef struct user_state {
	shape* shapes;
	shape* afterimages;
	enemy* enemies;
	bullet* bullets;
	id_stack* shape_ids;
	id_stack* image_ids;
	id_stack* bullet_ids;
	player play;
	uint64_t shape_alive;
	uint64_t image_alive;
	uint64_t bullet_alive;
} user_state;

void user_init(state* const s);
void user_mutate(state* const s);
void user_clean(state* const s);
void user_render(state* const s);

#endif
