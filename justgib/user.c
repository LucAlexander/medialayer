#include "user.h"
#include "state.h"

static color white = {255, 255, 255, 255};
static color red = {255, 0, 0, 255};
static color green = {0, 255, 0, 255};
static color blue = {0, 0, 255, 255};

int16_t next_id(id_stack** ids){
	if ((*ids)->next == NULL){
		if ((*ids)->id == MAX_SHAPES){
			return -1;
		}
		(*ids)->id += 1;
		return (*ids)->id-1;
	}
	uint16_t val = (*ids)->id;
	(*ids) = (*ids)->next;
	return val;
}

void id_push(id_stack** ids, uint16_t id){
	if ((*ids)->next == NULL){
		if (id == (*ids)->id-1){
			(*ids)->id -= 1;
			return;
		}
	}
	id_stack* top = pool_request((*ids)->mem, sizeof(id_stack));
	top->mem = (*ids)->mem;
	top->id= id;
	top->next = (*ids);
	(*ids) = top;
}

void user_init(state* const s){
	SDL_ShowCursor(SDL_DISABLE);
	s->user.shapes = pool_request(&s->main_mem, sizeof(shape)*MAX_SHAPES);
	s->user.afterimages = pool_request(&s->main_mem, sizeof(shape)*MAX_SHAPES);
	s->user.enemies = pool_request(&s->main_mem, sizeof(enemy)*MAX_SHAPES);
	s->user.bullets = pool_request(&s->main_mem, sizeof(bullet)*MAX_SHAPES);
	{
		s->user.shape_ids = pool_request(&s->main_mem, sizeof(id_stack));
		s->user.shape_ids->mem = &s->main_mem;
		s->user.shape_ids->next = NULL;
		s->user.shape_ids->id = 0;
	}
	{
		s->user.image_ids = pool_request(&s->main_mem, sizeof(id_stack));
		s->user.image_ids->mem = &s->main_mem;
		s->user.image_ids->next = NULL;
		s->user.image_ids->id = 0;
	}
	{
		s->user.bullet_ids= pool_request(&s->main_mem, sizeof(id_stack));
		s->user.bullet_ids->mem = &s->main_mem;
		s->user.bullet_ids->next = NULL;
		s->user.bullet_ids->id = 0;
	}

	s->user.shape_alive = 0;
	s->user.image_alive = 0;
	s->user.bullet_alive = 0;
	s->user.play = player_init(WINDOW_W/2, WINDOW_H/2, 0.1);
}

void user_mutate(state* const s){
	player_mutate(s, &s->user.play);
	for (uint64_t i = 0;i<MAX_SHAPES;++i){
		if (s->user.shape_alive & ((uint64_t)1<<i)){
			shape_mutate(s, &s->user.shapes[i]);
			enemy_mutate(s, &s->user.enemies[i], &s->user.shapes[i]);
		}
	}
	for (uint64_t i = 0;i<MAX_SHAPES;++i){
		if (s->user.image_alive & ((uint64_t)1<<i)){
			afterimage_mutate(s, i, &s->user.afterimages[i]);
		}
	}
	for (uint64_t i = 0;i<MAX_SHAPES;++i){
		if (s->user.bullet_alive & ((uint64_t)1<<i)){
			bullet_mutate(s, i, &s->user.bullets[i]);
		}
	}
}

void user_clean(state* const s){

}

void user_render(state* const s){
	for (uint64_t i = 0;i<MAX_SHAPES;++i){
		if (s->user.shape_alive & ((uint64_t)1<<i)){
			shape_render(s, &s->user.shapes[i]);
			enemy_render(s, &s->user.enemies[i], &s->user.shapes[i]);
		}
	}
	for (uint64_t i = 0;i<MAX_SHAPES;++i){
		if (s->user.image_alive & ((uint64_t)1<<i)){
			shape_render(s, &s->user.afterimages[i]);
		}
	}
	for (uint64_t i = 0;i<MAX_SHAPES;++i){
		if (s->user.bullet_alive & ((uint64_t)1<<i)){
			shape_render(s, &s->user.bullets[i].figure);
		}
	}
	player_render(s, &s->user.play);
}
