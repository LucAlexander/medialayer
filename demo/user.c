#include "user.h"
#include "state.h"

static color COLOR_RED = {255, 0, 0, 255};
static color COLOR_WHITE = {255, 255, 255, 255};
static color COLOR_BLACK = {0, 0, 0, 255};

void user_init(state* const s){
	s->user.sprite = load_sprite(&s->render, "spr/catpon.png", 32, 32);
	s->user.idle = animation_init(10, 21, 50);
	set_animation(&s->user.root, &s->user.idle, &s->user.sprite);
}

void user_mutate(state* const s){
	mutate_animation(s->user.root, &s->user.sprite, s->tick);
}

void user_clean(state* const s){}

void user_render(state* const s){
	render_set_color(&s->render, COLOR_RED);
	draw_rect(&s->render, 10, 10, 50, 50, RECT_FILL);
	render_set_color(&s->render, COLOR_WHITE);
	draw_rect(&s->render, 10, 10, 50, 50, RECT_OUTLINE);
	render_blitable(&s->render, &s->user.sprite, 128, 128);
	render_set_color(&s->render, COLOR_BLACK);
}
