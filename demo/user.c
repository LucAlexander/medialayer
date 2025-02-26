#include "user.h"
#include "state.h"

static color COLOR_RED = {255, 0, 0, 255};
static color COLOR_WHITE = {255, 255, 255, 255};
static color COLOR_BLACK = {0, 0, 0, 255};

void user_init(state* const s){}
void user_mutate(state* const s){}
void user_clean(state* const s){}
void user_render(state* const s){
	render_set_color(&s->render, COLOR_RED);
	draw_rect(&s->render, 10, 10, 50, 50, RECT_FILL);
	render_set_color(&s->render, COLOR_WHITE);
	draw_rect(&s->render, 10, 10, 50, 50, RECT_OUTLINE);
	render_set_color(&s->render, COLOR_BLACK);
}
