#ifndef SHAPE_H
#define SHAPE_H

#include "media.h"

#define RAY_MAX_DISTANCE 256
#define RAY_STEP RAY_MAX_DISTANCE/128

#define BASE_RAD 12

typedef struct state state;

typedef struct shape {
	float x;
	float y;
	float rot;
	float rad;
	float ang;
	float rotimer;
	color col;
} shape;

int16_t spawn_shape(state* const s, float x, float y, float rad, float ang, color col);
int16_t spawn_afterimage(state* const s, shape parent, color col);
shape shape_init(float x, float y, float rad, float ang, color col);
void shape_mutate(state* const s, shape* const figure);
void shape_render(state* const s, shape* const figure);

int8_t collides_shape(state* const s, float x, float y);
int8_t bullet_collides(state* const s, float x, float y);

typedef struct player {
	shape figure;
	shape cursor;
	float spawn_timer;
	float spawn_time;
	float mult_timer;
	float mult_time;
	float spd;
	float shot_x_origin;
	float shot_y_origin;
	float shot_x;
	float shot_y;
	float shot_time;
	float shot_timer;
	uint16_t shot;
	uint8_t mult;
	uint8_t max_mult;
} player;

player player_init(float x, float y, float spd);
void player_mutate(state* const s, player* const p);
void player_render(state* const s, player* const p);

typedef enum FACE_EXPR {
	ANGRY_EXPR,
	GATO_EXPR
} FACE_EXPR;

typedef struct face {
	v2* segments;
	color col;
	uint8_t segment_count;
} face;

face face_init(pool* const mem, shape parent, color col, FACE_EXPR);
void face_render(state* const s, shape* const figure, face* const f);

typedef struct enemy {
	float max_spd;
	float acc;
	float xspd;
	float yspd;
	float xspd_max;
	float yspd_max;
	float target_x;
	float target_y;
	face expr;
	enum {
		TRI, QUAD, PENTA, HEXA, CIRC
	} tag;
	float shoot_timer;
	float shoot_time;
} enemy;

int16_t spawn_tri(state* const s, float x, float y, color col);
int16_t spawn_quad(state* const s, float x, float y, color col);
int16_t spawn_penta(state* const s, float x, float y, color col);
int16_t spawn_hexa(state* const s, float x, float y, color col);
int16_t spawn_circ(state* const s, float x, float y, color col);
void enemy_mutate(state* const s, enemy* const penta, shape* const figure);
void enemy_render(state* const s, enemy* const penta, shape* const figure);

void afterimage_mutate(state* const s, uint16_t id, shape* const image);

typedef struct bullet {
	shape figure;
	float spd;
	float dir;
} bullet;

int16_t spawn_bullet(state* const s, float x, float y, float dir);
void bullet_mutate(state* const s, uint16_t id, bullet* const shot);

#endif
