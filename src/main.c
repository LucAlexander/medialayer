#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

#include "state.h"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

#include <SDL2/SDL.h>

void tick_reset(state* s){
	s->tick = SDL_GetTicks()-s->base_time;
	s->base_time = SDL_GetTicks();
}

#ifdef __EMSCRIPTEN__
void process_frame(void* args){
#else
void process_frame(state* s){
#endif
	while (SDL_PollEvent(&s->event)){
		switch(s->event.type){
		case SDL_QUIT:
			s->running = 0;
			break;
		case SDL_KEYDOWN:
			if (s->event.key.repeat == 0){
				key_down_event(&s->input, s->event);
			}
			break;
		case SDL_KEYUP:
			key_up_event(&s->input, s->event);
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (!mouse_held(&s->input, s->event.button.button)){
				mouse_down_event(&s->input, s->event);
			}
			break;
		case SDL_MOUSEBUTTONUP:
			mouse_up_event(&s->input, s->event);
			break;
		case SDL_MOUSEWHEEL:
			mouse_scroll_event(&s->input, s->event.wheel.y);
			break;
		case SDL_MOUSEMOTION:
			mouse_move_event(&s->input, s->event.motion.x, s->event.motion.y);
		}
	}
	pool_empty(&s->frame_mem);
	user_mutate(s);
	user_clean(s);
	render_clear(&s->render);
	user_render(s);
	render_flip(&s->render);
	new_input_frame(&s->input);
	tick_reset(s);
#ifdef __EMSCRIPTEN__
	if (state->running) return;
	pool_dealloc(s->frame_mem);
	pool_dealloc(s->main_mem);
	emscripten_cancel_main_loop();
#endif
}

int main(){
	srand(time(NULL));
	state s = {
		.main_mem = pool_alloc(MAIN_MEM_SIZE, POOL_STATIC),
		.frame_mem = pool_alloc(FRAME_MEM_SIZE, POOL_STATIC),
		.base_time = 0,
		.tick = 0,
		.running = 1
	};
	render_init(&s.render, &s.main_mem, WINDOW_W, WINDOW_H, WINDOW_TITLE);
	input_init(&s.input);
	user_init(&s);
	tick_reset(&s);
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(process_frame, &s, 0, 1);
#else
	while (s.running == 1){
		process_frame(&s);
	}
	pool_dealloc(&s.frame_mem);
	pool_dealloc(&s.main_mem);
#endif
	return 0;
}
