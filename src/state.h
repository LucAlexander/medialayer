#ifndef LAYER_STATE
#define LAYER_STATE

#include "media.h"
#include "hashmap.h"
#include "user.h"

#define MAIN_MEM_SIZE 0x1000000
#define FRAME_MEM_SIZE 0x10000

typedef struct state {
	user_state user;
	user_input input;
	graphics render;
	pool main_mem;
	pool frame_mem;
	uint32_t base_time;
	uint32_t tick;
	SDL_Event event;
	uint8_t running;
} state;

void tick_reset(state* s);

#ifdef __EMSCRIPTEN__
void process_frame(void* args);
#else
void process_frame(state* s);
#endif

#endif
