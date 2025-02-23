#ifndef INPUTUTILS_H
#define INPUTUTILS_H

#include <SDL2/SDL.h>

#include <inttypes.h>

#ifndef PI
#define PI 3.14159265
#endif

typedef struct v2 {
	float x;
	float y;
} v2;

typedef struct v3 {
	float x;
	float y;
	float z;
} v3;

typedef struct v4 {
	float x;
	float y;
	float w;
	float h;
} v4;

float len_dir_x(float len, float dir);
float len_dir_y(float len, float dir);
float approach(float value, float point);
int8_t sign(double n);
float distance_point(float x, float y, float xx, float yy);
float distance_point_v2(v2 a, v2 b);
float direction_point(float x, float y, float xx, float yy);
float direction_point_v2(v2 a, v2 b);
uint8_t point_in_rect(float x, float y, v4 r);
uint8_t point_in_rect_v2(v2 p, v4 r);
uint8_t rect_collides(v4 a, v4 b);
uint8_t rect_contains(v4 a, v4 b);

typedef struct user_input{
	uint8_t held_keys[256];
	uint8_t pressed_keys[256];
	uint8_t released_keys[256];
	uint8_t held_buttons[5];
	uint8_t pressed_buttons[5];
	uint8_t released_buttons[5];
	int8_t scroll_dir;
	int32_t mouse_x;
	int32_t mouse_y;
} user_input;

void input_init(user_input* inp);

void new_input_frame(user_input* inp);

void mouse_move_event(user_input* inp, int32_t x, int32_t y);

int32_t mouse_x(user_input* inp);
int32_t mouse_y(user_input* inp);
v2 mouse_pos(user_input* inp);

void mouse_scroll_event(user_input* inp, int8_t);

void mouse_up_event(user_input* inp, const SDL_Event event);
void mouse_down_event(user_input* inp, const SDL_Event event);

void key_up_event(user_input* inp, const SDL_Event event);
void key_down_event(user_input* inp, const SDL_Event event);

uint8_t mouse_scrolled(user_input* inp, int8_t);

uint8_t mouse_held(user_input* inp, uint8_t);
uint8_t mouse_pressed(user_input* inp, uint8_t);
uint8_t mouse_released(user_input* inp, uint8_t);

uint8_t key_held(user_input* inp, const char*);
uint8_t key_pressed(user_input* inp, const char*);
uint8_t key_released(user_input* inp, const char*);

void mouse_interrupt(user_input* inp, uint8_t);
void key_interrupt(user_input* inp, const char*);

typedef struct view {
	float x;
	float y;
	float px;
	float py;
	float pw;
	float ph;
} view;

typedef enum RENDER_SCALE_QUALITY {
	RENDER_SCALE_NEAREST=0,
	RENDER_SCALE_LINEAR=1,
	RENDER_SCALE_ANISOTROPIC=2
} RENDER_SCALE_QUALITY;

typedef struct graphics {
	SDL_Window* window;
	SDL_Renderer* renderer;
	view render_view;
	uint16_t window_w;
	uint16_t window_h;
	float sprite_scale_x;
	float sprite_scale_y;
	RENDER_SCALE_QUALITY render_quality;
} graphics;

typedef struct color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} color;

void render_set_blend_mode(graphics* g, SDL_BlendMode b);
void render_init(graphics* g, uint16_t width, uint16_t height, const char* title);
void render_deinit(graphics* g);
void render_view_init(graphics* g);
uint8_t render_in_view(graphics* g, float x, float y, float x2, float y2);
void render_set_sprite_scale(graphics* g, float x, float y);
float scale_x(graphics* g, float val);
float scale_y(graphics* g, float val);
void toggle_fullscreen(graphics* g);
v2 view_to_world(graphics* g, float x, float y);
v2 world_to_view(graphics* g, float x, float y);
v2 view_to_world_v2(graphics* g, v2 p);
v2 world_to_view_v2(graphics* g, v2 p);
void render_flip(graphics* g);
void render_clear(graphics* g);
void render_set_view(graphics* g, view v);
void render_set_color(graphics* g, color c);
void render_set_target(graphics* g, SDL_Texture* t);
void format_dest_rect_to_view(graphics* g, SDL_Rect* dest);
void format_dest_frect_to_view(graphics* g, SDL_FRect* dest);

typedef enum BLITABLE_FLAGS{
	BLITABLE_VISIBLE=1,
	BLITABLE_FLIP_H=2,
	BLITABLE_FLIP_V=4
}BLITABLE_FLAGS;

typedef struct Blitable{
	SDL_Texture* texture;
	SDL_Rect drawBound;
	uint32_t displayW;
	uint32_t displayH;
	int32_t textureW;
	int32_t textureH;
	BLITABLE_FLAGS flags;
	double angle;
	SDL_FPoint center;
}Blitable;

//TODO blitable stuff once we know how to arenas for graphics
//animations too

void blit_surface(graphics* g, SDL_Texture* texture, SDL_Rect* src, SDL_Rect dst);
void blit_surface_ex(graphics* g, SDL_Texture* texture, SDL_Rect* src, SDL_Rect dst, double angle, SDL_Point* center, SDL_RendererFlip flip);
void blit_surface_f(graphics* g, SDL_Texture* texture, SDL_Rect* src, SDL_FRect dst);
void blit_surface_exf(graphics* g, SDL_Texture* texture, SDL_Rect* src, SDL_FRect dst, double angle, SDL_FPoint* center, SDL_RendererFlip flip);

typedef enum RECT_FILL_PARAM {
	RECT_FILL,
	RECT_OUTLINE
} RECT_FILL_PARAM;

void draw_line(graphics* g, float x, float y, float xx, float yy);
void draw_line_v2(graphics* g, v2 a, v2 b);
void draw_rect(graphics* g, float x, float y, float w, float h, RECT_FILL_PARAM p);
void draw_rect_v4(graphics* g, v4 rect, RECT_FILL_PARAM p);
void draw_rect_v2(graphics* g, v2 pt, v2 d, RECT_FILL_PARAM p);

//TODO font

#endif
