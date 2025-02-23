#include "media.h"
#include <math.h>
#include <float.h>
#include <stdlib.h>

float approach(float value, float point){
	if (value > point){
		return value - 0.1;
	}
	if (value < point){
		return value + 0.1;
	}
	return value;
}

int8_t sign(double n){
	if (n > 0){
		return 1;
	}
	if (n < 0){
		return -1;
	}
	return 0;
}

float len_dir_x(float len, float dir){
	return len*cos(dir*PI/180.0);
}

float len_dir_y(float len, float dir){
	return len*(-sin(dir*PI/180.0));
}

float distance_point(float x, float y, float xx, float yy){
	x -= xx;
	y -= yy;
	x *= x;
	y *= y;
	return sqrt(x+y);
}

float distance_point_v2(v2 a, v2 b){
	return distance_point(a.x, a.y, b.x, b.y);
}

float direction_point(float x, float y, float xx, float yy){
	float res = (atan2(yy-y, xx-x)*180)/PI;
	return (yy>y) ? (360-res) : -res;
}

float direction_point_v2(v2 a, v2 b){
	return direction_point(a.x, a.y, b.x, b.y);
}

uint8_t point_in_rect(float x, float y, v4 r){
	if (r.w < 0){
		r.w *= -1;
		r.x -= r.w;
	}
	if (r.h < 0){
		r.h *= -1;
		r.y -= r.w;
	}
	return (x >= r.x && x <= r.x+r.w) && 
	       (y >= r.y && y <= r.y+r.h);
}

uint8_t point_in_rect_v2(v2 p, v4 r){
	return point_in_rect(p.x, p.y, r);
}

uint8_t rect_collides(v4 a, v4 b){
	return !(
		(a.x > (b.x+b.w)) ||
		((a.x+a.w) < b.x) ||
		(a.y > (b.y+b.h)) ||
		(a.y+a.h < b.y)
	);
}

uint8_t rect_contains(v4 a, v4 b){
	return (
		(b.x > a.x) &&
		(b.y > a.y) &&
		(b.x+b.w < a.x+a.w) &&
		(b.y+b.h < a.y+a.h)
	);
}

void input_init(user_input* inp){
	memset(inp->held_keys, 0, 256);
	memset(inp->pressed_keys, 0, 256);
	memset(inp->released_keys, 0, 256);
	memset(inp->held_buttons, 0, 5);
	memset(inp->pressed_buttons, 0, 5);
	memset(inp->released_buttons, 0, 5);
	inp->scroll_dir = 0;
}

void new_input_frame(user_input* inp){
	memset(inp->pressed_keys, 0, 256);
	memset(inp->released_keys, 0, 256);
	memset(inp->pressed_buttons, 0, 5);
	memset(inp->released_buttons, 0, 5);
	inp->scroll_dir = 0;
}

void mouse_scroll_event(user_input* inp, int8_t dir){
	inp->scroll_dir= dir;
}

void mouse_move_event(user_input* inp, int32_t x, int32_t y){
	inp->mouse_x = x;
	inp->mouse_y = y;
}

v2 mouse_pos(user_input* inp){
	v2 a = {inp->mouse_x, inp->mouse_y};
	return a;
}

void mouse_up_event(user_input* inp, const SDL_Event event){
	inp->released_buttons[event.button.button-1] = 1;
	inp->held_buttons[event.button.button-1] = 0;
}

void mouse_down_event(user_input* inp, const SDL_Event event){
	inp->pressed_buttons[event.button.button-1] = 1;
	inp->held_buttons[event.button.button-1] = 1;
}

void key_up_event(user_input* inp, const SDL_Event event){
	inp->released_keys[event.key.keysym.scancode] = 1;
	inp->held_keys[event.key.keysym.scancode] = 0;
}

void key_down_event(user_input* inp, const SDL_Event event){
	inp->pressed_keys[event.key.keysym.scancode] = 1;
	inp->held_keys[event.key.keysym.scancode] = 1;
}

uint8_t mouse_scrolled(user_input* inp, int8_t dir){
	return sign(dir)==sign(inp->scroll_dir);
}

uint8_t mouse_held(user_input* inp, uint8_t button){
	return inp->held_buttons[button-1];
}

uint8_t mouse_pressed(user_input* inp, uint8_t button){
	return inp->pressed_buttons[button-1];
}

uint8_t mouse_released(user_input* inp, uint8_t button){
	return inp->released_buttons[button-1];
}

uint8_t key_held(user_input* inp, const char* key){
	SDL_Scancode k = SDL_GetScancodeFromName(key);
	return inp->held_keys[k];
}

uint8_t key_pressed(user_input* inp, const char* key){
	SDL_Scancode k = SDL_GetScancodeFromName(key);
	return inp->pressed_keys[k];
}

uint8_t key_released(user_input* inp, const char* key){
	SDL_Scancode k = SDL_GetScancodeFromName(key);
	return inp->released_keys[k];
}

void mouse_interrupt(user_input* inp, uint8_t b){
	inp->pressed_buttons[b-1] = 0;
	inp->held_buttons[b-1] = 0;
}

void key_interrupt(user_input* inp, const char* key){
	SDL_Scancode k = SDL_GetScancodeFromName(key);
	inp->pressed_keys[k] = 0;
	inp->held_keys[k] = 0;
}

void render_set_blend_mode(graphics* g, SDL_BlendMode b){
	SDL_SetRenderDrawBlendMode(g->renderer, b);
}

void render_init(graphics* g, uint16_t width, uint16_t height, const char* title){
	g->render_quality = RENDER_SCALE_NEAREST;
	g->window = NULL;
	g->renderer = NULL;
	render_view_init(g);
	g->sprite_scale_x = 1;
	g->sprite_scale_y = 1;
	SDL_Init(SDL_INIT_EVERYTHING);
	if (g->window != NULL){
		SDL_DestroyWindow(g->window);
	}
	if (g->renderer != NULL){
		SDL_DestroyRenderer(g->renderer);
	}
	SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_OPENGL, &(g->window), &(g->renderer));
	SDL_SetWindowTitle(g->window, title);
	view default_view = {0, 0, 0, 0, width, height};
	g->window_w = width;
	g->window_h = height;
	render_set_view(g, default_view);
	SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);
}

void render_deinit(graphics* g){
	SDL_DestroyWindow(g->window);
	SDL_DestroyRenderer(g->renderer);
	g->window = NULL;
	g->renderer = NULL;
	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
	SDL_Quit();
}

void render_view_init(graphics* g){
	g->render_view.x = 0;
	g->render_view.y = 0;
	g->render_view.px = 0;
	g->render_view.py = 0;
	g->render_view.pw = 0;
	g->render_view.ph = 0;
}

uint8_t render_in_view(graphics* g, float x, float y, float x2, float y2){
	v4 port = {
		g->render_view.x,
		g->render_view.y,
		g->render_view.pw,
		g->render_view.ph
	};
	v4 rect = {x, y, x2, y2};
	return rect_collides(port, rect);
}

void render_set_sprite_scale(graphics* g, float x, float y){
	g->sprite_scale_x = x;
	g->sprite_scale_y = y;
	SDL_RenderSetScale(g->renderer, g->sprite_scale_x, g->sprite_scale_y);
}

float scale_x(graphics* g, float val){
	return val / g->sprite_scale_x;
}

float scale_y(graphics* g, float val){
	return val / g->sprite_scale_y;
}

void render_set_view(graphics* g, view v){
	g->render_view = v;
	const SDL_Rect port = {v.px, v.py, v.pw, v.ph};
	SDL_RenderSetViewport(g->renderer, &port);
	SDL_RenderSetLogicalSize(g->renderer, v.pw, v.ph);
}

void toggle_fullscreen(graphics* g){
	uint32_t flags = (SDL_GetWindowFlags(g->window) ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (SDL_SetWindowFullscreen(g->window, flags) < 0){
		fprintf(stderr, "[!] Toggling fullscreen failed\n");
		return;
	}
	render_set_view(g, g->render_view);
	if ((flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0){
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, (const char* )g->render_quality);
	}
}

v2 view_to_world(graphics* g, float x, float y){
	v2 coords = {
		x+g->render_view.x,
		y+g->render_view.y
	};
	return coords;
}

v2 world_to_view(graphics* g, float x, float y){
	v2 coords = {
		x-g->render_view.x,
		y-g->render_view.y
	};
	return coords;
}

v2 view_to_world_v2(graphics* g, v2 p){
	p.x += g->render_view.x;
	p.y += g->render_view.y;
	return p;
}

v2 world_to_view_v2(graphics* g, v2 p){
	p.x -= g->render_view.x;
	p.y -= g->render_view.y;
	return p;
}

void render_flip(graphics* g){
	SDL_RenderPresent(g->renderer);
}

void render_clear(graphics* g){
	SDL_RenderClear(g->renderer);
}

void render_set_color(graphics* g, color c){
	SDL_SetRenderDrawColor(g->renderer, c.r, c.g, c.b, c.a);
}

void render_set_target(graphics* g, SDL_Texture* t){
	SDL_SetRenderTarget(g->renderer, t);
}

void format_dest_rect_to_view(graphics* g, SDL_Rect* dest){
	dest->x -= g->render_view.x;
	dest->y -= g->render_view.y;
}

void format_dest_frect_to_view(graphics* g, SDL_FRect* dest){
	dest->x -= g->render_view.x;
	dest->y -= g->render_view.y;
}

void blit_surface(graphics* g, SDL_Texture* texture, SDL_Rect* src, SDL_Rect dst){
	if (!render_in_view(g, dst.x, dst.y, dst.w, dst.h)){
		return;
	}
	format_dest_rect_to_view(g, &dst);
	SDL_RenderCopy(g->renderer, texture, src, &dst);
}

void blit_surface_ex(graphics* g, SDL_Texture* texture, SDL_Rect* src, SDL_Rect dst, double angle, SDL_Point* center, SDL_RendererFlip flip){
	if (!render_in_view(g, dst.x, dst.y, dst.w, dst.h)){
		return;
	}
	format_dest_rect_to_view(g, &dst);
	SDL_RenderCopyEx(g->renderer, texture, src, &dst, angle, center, flip);
}

void blit_surface_f(graphics* g, SDL_Texture* texture, SDL_Rect* src, SDL_FRect dst){
	if (!render_in_view(g, dst.x, dst.y, dst.w, dst.h)){
		return;
	}
	format_dest_frect_to_view(g, &dst);
	SDL_RenderCopyF(g->renderer, texture, src, &dst);
}

void blit_surface_exf(graphics* g, SDL_Texture* texture, SDL_Rect* src, SDL_FRect dst, double angle, SDL_FPoint* center, SDL_RendererFlip flip){
	if (!render_in_view(g, dst.x, dst.y, dst.w, dst.h)){
		return;
	}
	format_dest_frect_to_view(g, &dst);
	SDL_RenderCopyExF(g->renderer, texture, src, &dst, angle, center, flip);
}

void draw_line(graphics* g, float x, float y, float xx, float yy){
	SDL_RenderDrawLine(g->renderer, x-g->render_view.x, y-g->render_view.y, xx-g->render_view.x, yy-g->render_view.y);
}

void draw_line_v2(graphics* g, v2 a, v2 b){
	draw_line(g, a.x, a.y, b.x, b.y);
}

void draw_rect(graphics* g, float x, float y, float w, float h, RECT_FILL_PARAM p){
	SDL_FRect bound = {
		x-g->render_view.x,
		y-g->render_view.y,
		w,
		h
	};
	if (p & RECT_FILL){
		SDL_RenderFillRectF(g->renderer, &bound);
	}
	if (p & RECT_OUTLINE){
		SDL_RenderDrawRectF(g->renderer, &bound);
	}
}

void draw_rect_v4(graphics* g, v4 rect, RECT_FILL_PARAM p){
	draw_rect(g, rect.x, rect.y, rect.w, rect.h, p);
}

void draw_rect_v2(graphics* g, v2 pt, v2 d, RECT_FILL_PARAM p){
	draw_rect(g, pt.x, pt.y, d.x, d.y, p);
}

