#ifndef USER_HEADER
#define USER_HEADER

#include "media.h"
#include "evo.h"

#define WINDOW_W 320
#define WINDOW_H 180
#define WINDOW_TITLE "template"

typedef struct state state;

typedef struct node_circle {
	float x;
	float y;
	float rad;
} node_circle;

typedef struct user_state {
	network net;
	uint8_t* rendered;
	node_circle* positions;
} user_state;

void user_init(state* const s);
void user_mutate(state* const s);
void user_clean(state* const s);
void user_render(state* const s);

#endif
