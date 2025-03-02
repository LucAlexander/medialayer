#include "state.h" 
#include "shape.h"
#include "user.h"

static color white = {255, 255, 255, 255};
static color black = {0, 0, 0, 255};
static color red = {255, 0, 0, 255};
static color green = {0, 255, 0, 255};
static color blue = {0, 0, 255, 255};

int16_t spawn_shape(state* const s, float x, float y, float rad, float ang, color col){
	int16_t id = next_id(&s->user.shape_ids);
	if (id == -1){
		return -1;
	}
	s->user.shapes[id] = shape_init(x, y, rad, ang, col);
	s->user.shape_alive |= ((uint64_t)(1)<<id);
	return id;
}

int16_t spawn_afterimage(state* const s, shape parent, color col){
	int16_t id = next_id(&s->user.image_ids);
	if (id == -1){
		return -1;
	}
	parent.col = col;
	s->user.afterimages[id] = parent;
	s->user.image_alive |= ((uint64_t)(1)<<id);
	return id;
}

shape shape_init(float x, float y, float rad, float ang, color col){
	shape ent = {
		.x=x,
		.y=y,
		.rot=0,
		.rad=rad,
		.ang=ang,
		.rotimer=0,
		.col=col
	};
	return ent;
}

void shape_mutate(state* const s, shape* const ent){
	ent->rotimer += s->tick;
	if (ent->rotimer < 10){
		return;
	}
	ent->rotimer -= 10;
	ent->rot += 1;
	while (ent->rot >= 360){
		ent->rot -= 360;
	}
	while (ent->rot < 0){
		ent->rot += 360;
	}
}

void shape_render(state* const s, shape* const ent){
	render_set_color(&s->render, ent->col);
	float inner = 0;
	v2 a = {
		.x=len_dir_x(ent->rad, inner+ent->rot),
		.y=len_dir_y(ent->rad, inner+ent->rot)
	};
	inner += ent->ang;
	v2 b = {
		.x=len_dir_x(ent->rad, inner+ent->rot),
		.y=len_dir_y(ent->rad, inner+ent->rot)
	};
	while (inner <= 360) {
		draw_line(&s->render, ent->x+a.x, ent->y+a.y, ent->x+b.x, ent->y+b.y);
		inner += ent->ang;
		a = b;
		b.x = len_dir_x(ent->rad, inner+ent->rot);
		b.y = len_dir_y(ent->rad, inner+ent->rot);
	} 
	render_set_color(&s->render, black);
}

int8_t collides_shape(state* const s, float x, float y){
	for (uint16_t i = 0;i<MAX_SHAPES;++i){
		if ((s->user.shape_alive & ((uint64_t)(1)<<i)) == 0){
			continue;
		}
		shape* figure = &s->user.shapes[i];
		if (distance_point(x, y, figure->x, figure->y)<figure->rad){
			return i;
		}
	}
	return -1;
}

int8_t bullet_collides(state* const s, float x, float y){
	for (uint16_t i = 0;i<MAX_SHAPES;++i){
		if ((s->user.bullet_alive & ((uint64_t)(1)<<i)) == 0){
			continue;
		}
		shape* figure = &s->user.bullets[i].figure;
		if (distance_point(x, y, figure->x, figure->y)<s->user.play.figure.rad){
			return i;
		}
	}
	return -1;
}

player player_init(float x, float y, float spd){
	player p = {
		.spd = spd
	};
	p.figure = shape_init(x, y, BASE_RAD, 72, green);
	p.cursor = shape_init(x, y, 4, 90, white);
	p.shot_x = x;
	p.shot_x = y;
	p.spawn_time = 3000;
	p.spawn_timer = 0;
	p.mult_time = 10000;
	p.mult_timer = 0;
	p.shot_time = 400;
	p.shot_timer = 0;
	p.shot = 0;
	p.mult = 1;
	p.max_mult = 5;
	return p;
}

void player_mutate(state* const s, player* const p){
	shape_mutate(s, &p->figure);
	p->cursor.x = s->input.mouse_x;
	p->cursor.y = s->input.mouse_y;
	if (p->shot_timer > 0){
		p->shot_timer -= s->tick;
	}
	if (mouse_pressed(&s->input, 1) && (p->shot_timer <= 0)){
		p->shot_timer = p->shot_time;
		float mdir = direction_point(p->figure.x, p->figure.y, p->cursor.x, p->cursor.y);
		float xstep = len_dir_x(RAY_STEP, mdir);
		float ystep = len_dir_y(RAY_STEP, mdir);
		float xoff = p->figure.x+xstep;
		float yoff = p->figure.y+ystep;
		p->shot = 255;
		p->shot_x_origin = p->figure.x;
		p->shot_y_origin = p->figure.y;
		p->shot_x = xoff + (xstep*RAY_MAX_DISTANCE);
		p->shot_y = yoff + (ystep*RAY_MAX_DISTANCE);
		for (uint16_t i = 0;i<RAY_MAX_DISTANCE;i += RAY_STEP){
			int8_t id = collides_shape(s, xoff, yoff);
			xoff += xstep;
			yoff += ystep;
			if (id == -1){
				continue;
			}
			s->user.shape_alive &= ~((uint64_t)(1)<<id);
			id_push(&s->user.shape_ids, id);
			shape parent = s->user.shapes[id];
			spawn_afterimage(s, parent, white);
			p->shot_x = xoff;
			p->shot_y = yoff;
			break;
		}
	}
	if (p->mult_timer > 0){
		p->mult_timer -= s->tick;
	}
	else{
		if (p->mult < p->max_mult){
			p->mult_timer = p->mult_time;
			p->mult += 1;
		}
	}
	if (p->spawn_timer > 0){
		p->spawn_timer -= s->tick;
	}
	else{
		p->spawn_timer = p->spawn_time;
		uint16_t m = p->mult;
		while (m > 0){
			m -= 1;
			float spawn_x = 64;
			float spawn_y = 64;
			if (rand()%2){
				spawn_x += WINDOW_W;
			}
			else{
				spawn_x = 0 - spawn_x;
			}
			if (rand()%2){
				spawn_y += WINDOW_H;
			}
			else{
				spawn_y = 0 - spawn_y;
			}
			uint8_t type = rand() % p->mult;
			switch (type){
			case 0:
				spawn_tri(s, spawn_x, spawn_y, red);
				break;
			case 1:
				spawn_quad(s, spawn_x, spawn_y, red);
				break;
			case 2:
				spawn_penta(s, spawn_x, spawn_y, red);
				break;
			case 3:
				spawn_hexa(s, spawn_x, spawn_y, red);
				break;
			case 4:
				spawn_circ(s, spawn_x, spawn_y, red);
				break;
			}
		}
	}
	int16_t bullet_id = bullet_collides(s, p->figure.x, p->figure.y);
   	if (bullet_id != -1){
		s->user.bullet_alive &= ~((uint64_t)(1)<<bullet_id);
		id_push(&s->user.bullet_ids, bullet_id);
		s->running = 0;
	}
	uint8_t up = key_held(&s->input, "W");
	uint8_t down = key_held(&s->input, "S");
	uint8_t left = key_held(&s->input, "A");
	uint8_t right = key_held(&s->input, "D");
	int8_t vdir = down-up;
	int8_t hdir = right-left;
	if (vdir == 0 && hdir == 0){
		return;
	}
	float dir = direction_point(0, 0, hdir, vdir);
	float hsp = len_dir_x(p->spd, dir);
	float vsp = len_dir_y(p->spd, dir);
	p->figure.x += s->tick*hsp;
	p->figure.y += s->tick*vsp;
}

void player_render(state* const s, player* const p){
	shape_render(s, &p->figure);
	shape_render(s, &p->cursor);
	if (p->shot > 0){
		color shotcolor = {255, 255, 255, p->shot};
		render_set_color(&s->render, shotcolor);
		draw_line(&s->render, p->shot_x_origin, p->shot_y_origin, p->shot_x, p->shot_y);
		p->shot -= s->tick;
		render_set_color(&s->render, black);
	}
}

int16_t spawn_tri(state* const s, float x, float y, color col){
	int16_t id = spawn_shape(s, x, y, BASE_RAD, 120, col);
	if (id == -1){
		return -1;
	}
	enemy e = {
		.tag=TRI,
		.target_x=x,
		.target_y=y,
		.xspd = 0,
		.xspd_max = 0,
		.yspd = 0,
		.yspd_max = 0,
		.max_spd=0.1,
		.acc=0.0001,
		.shoot_timer=0,
		.shoot_time=1000,
		.expr=face_init(&s->main_mem, s->user.shapes[id], red, rand()%2)
	};
	s->user.enemies[id] = e;
	return id;
}

int16_t spawn_quad(state* const s, float x, float y, color col){
	int16_t id = spawn_shape(s, x, y, BASE_RAD, 90, col);
	if (id == -1){
		return -1;
	}
	enemy e = {
		.tag=QUAD,
		.target_x=x,
		.target_y=y,
		.xspd = 0,
		.xspd_max = 0,
		.yspd = 0,
		.yspd_max = 0,
		.max_spd=0.1,
		.acc=0.0005,
		.shoot_timer=0,
		.shoot_time=1000,
		.expr=face_init(&s->main_mem, s->user.shapes[id], red, rand()%2)
	};
	s->user.enemies[id] = e;
	return id;
}

int16_t spawn_penta(state* const s, float x, float y, color col){
	int16_t id = spawn_shape(s, x, y, BASE_RAD, 72, col);
	if (id == -1){
		return -1;
	}
	enemy e = {
		.tag=PENTA,
		.target_x=x,
		.target_y=y,
		.xspd = 0,
		.xspd_max = 0,
		.yspd = 0,
		.yspd_max = 0,
		.max_spd=0.1,
		.acc=0.0001,
		.shoot_timer=0,
		.shoot_time=1000,
		.expr=face_init(&s->main_mem, s->user.shapes[id], red, rand()%2)
	};
	s->user.enemies[id] = e;
	return id;
}

int16_t spawn_hexa(state* const s, float x, float y, color col){
	int16_t id = spawn_shape(s, x, y, BASE_RAD, 60, col);
	if (id == -1){
		return -1;
	}
	enemy e = {
		.tag=HEXA,
		.target_x=x,
		.target_y=y,
		.xspd = 0,
		.xspd_max = 0,
		.yspd = 0,
		.yspd_max = 0,
		.max_spd=0.1,
		.acc=0.0005,
		.shoot_timer=0,
		.shoot_time=1000,
		.expr=face_init(&s->main_mem, s->user.shapes[id], red, rand()%2)
	};
	s->user.enemies[id] = e;
	return id;
}

int16_t spawn_circ(state* const s, float x, float y, color col){
	int16_t id = spawn_shape(s, x, y, BASE_RAD, 4, col);
	if (id == -1){
		return -1;
	}
	enemy e = {
		.tag=CIRC,
		.target_x=x,
		.target_y=y,
		.xspd = 0,
		.xspd_max = 0,
		.yspd = 0,
		.yspd_max = 0,
		.max_spd=0.1,
		.acc=0.0001,
		.shoot_timer=0,
		.shoot_time=1000,
		.expr=face_init(&s->main_mem, s->user.shapes[id], red, rand()%2)
	};
	s->user.enemies[id] = e;
	return id;
}

void enemy_mutate(state* const s, enemy* const nme, shape* const figure){
	if (distance_point(figure->x, figure->y, nme->target_x, nme->target_y) < figure->rad){
		switch (nme->tag){
		case TRI:
		case QUAD:
			if (rand()%2){
				nme->target_x = ((rand() % 16) * 18)+16;
			}
			else{
				nme->target_y = ((rand() % 9) * 18)+9;
			}
			break;
		case PENTA:
		case HEXA:
			nme->target_x = ((rand() % 16) * 20)+16;
			nme->target_y = ((rand() % 9) * 20)+9;
			break;
		case CIRC:
			nme->target_x = s->user.play.figure.x+((rand()%64)-32);
			nme->target_y = s->user.play.figure.y+((rand()%64)-32);
			break;
		}
	}
	if (nme->shoot_timer > 0){
		nme->shoot_timer -= s->tick;
	}
	else{
		nme->shoot_timer = nme->shoot_time;
		switch (nme->tag){
		case TRI:
			float tridir = (rand() % 12) * 30;
			for (uint32_t i = 0;i<3;++i){
				spawn_bullet(s, figure->x, figure->y, tridir);
				tridir += 120;
			}
			break;
		case QUAD:
			float base_dir = (rand() % 12) * 30;
			for (uint32_t i = 0;i<4;++i){
				spawn_bullet(s, figure->x, figure->y, base_dir);
				base_dir += 90;
			}
			break;
		case PENTA:
			spawn_bullet(s, figure->x, figure->y, direction_point(figure->x, figure->y, s->user.play.figure.x, s->user.play.figure.y));
			break;
		case HEXA:
			float center_dir = direction_point(figure->x, figure->y, s->user.play.figure.x, s->user.play.figure.y);
			spawn_bullet(s, figure->x, figure->y, center_dir+30);
			spawn_bullet(s, figure->x, figure->y, center_dir-30);
			break;
		case CIRC:
			float circ_dir = direction_point(figure->x, figure->y, s->user.play.figure.x, s->user.play.figure.y);
			spawn_bullet(s, figure->x, figure->y, circ_dir+45);
			spawn_bullet(s, figure->x, figure->y, circ_dir);
			spawn_bullet(s, figure->x, figure->y, circ_dir-45);
			break;
		}
	}
	float dir = direction_point(figure->x, figure->y, nme->target_x, nme->target_y);
	nme->xspd_max = len_dir_x(nme->max_spd, dir);
	nme->yspd_max = len_dir_y(nme->max_spd, dir);
	if (nme->xspd < nme->xspd_max){
		nme->xspd += nme->acc * s->tick;
	}
	else if (nme->xspd > nme->xspd_max){
		nme->xspd -= nme->acc * s->tick;
	}
	if (nme->yspd < nme->yspd_max){
		nme->yspd += nme->acc * s->tick;
	}
	else if (nme->yspd > nme->yspd_max){
		nme->yspd -= nme->acc * s->tick;
	}
	figure->x += nme->xspd * s->tick;
	figure->y += nme->yspd * s->tick;
}

void enemy_render(state* const s, enemy* const enemy, shape* const figure){
	face_render(s, figure, &enemy->expr);
}

void face_render(state* const s, shape* const figure, face* const f){
	render_set_color(&s->render, f->col);
	for (uint16_t i = 0;i<f->segment_count;++i){
		float x = figure->x+f->segments[i*2].x;
		float y = figure->y+f->segments[i*2].y;
		float xx = figure->x+f->segments[(i*2)+1].x;
		float yy = figure->y+f->segments[(i*2)+1].y;
		draw_line(&s->render, x, y, xx, yy);
	}
	render_set_color(&s->render, black);
}

void afterimage_mutate(state* const s, uint16_t id, shape* const image){
	if (image->col.a > 0){
		image->col.a -= s->tick;
		return;
	}
	s->user.image_alive &= ~((uint64_t)(1)<<id);
	id_push(&s->user.image_ids, id);
}

face face_init(pool* const mem, shape parent, color col, FACE_EXPR e){
	face expr;
	expr.col = col;
	float rad = parent.rad/12;
	switch (e){
	case ANGRY_EXPR:
		expr.segment_count = 2;
		expr.segments = pool_request(mem, sizeof(v2)*2*expr.segment_count);
		{//left brow
			expr.segments[0].x = -rad*4;
			expr.segments[0].y = -rad;
			expr.segments[1].x = -rad;
			expr.segments[1].y = 0;
		}
		{//right brow
			expr.segments[2].x = rad*4;
			expr.segments[2].y = -rad;
			expr.segments[3].x = rad;
			expr.segments[3].y = 0;
		}
		break;
	case GATO_EXPR:
		expr.segment_count = 6;
		expr.segments = pool_request(mem, sizeof(v2)*2*expr.segment_count);
		{//left brow
			expr.segments[0].x = -rad*4;
			expr.segments[0].y = -rad;
			expr.segments[1].x = -rad;
			expr.segments[1].y = 0;
		}
		{//right brow
			expr.segments[2].x = rad*4;
			expr.segments[2].y = -rad;
			expr.segments[3].x = rad;
			expr.segments[3].y = 0;
		}
		{//left ear
			expr.segments[4].x = len_dir_x(rad*14, 135);
			expr.segments[4].y = len_dir_y(rad*14, 135);
			expr.segments[5].x = expr.segments[4].x+len_dir_x(rad*4, 345);
			expr.segments[5].y = expr.segments[4].y+len_dir_y(rad*4, 345);
			expr.segments[6].x = len_dir_x(rad*14, 135);
			expr.segments[6].y = len_dir_y(rad*14, 135);
			expr.segments[7].x = expr.segments[6].x+len_dir_x(rad*4, 285);
			expr.segments[7].y = expr.segments[6].y+len_dir_y(rad*4, 285);
		}
		{//right ear
			expr.segments[8].x = len_dir_x(rad*14, 45);
			expr.segments[8].y = len_dir_y(rad*14, 45);
			expr.segments[9].x = expr.segments[8].x+len_dir_x(rad*4, 195);
			expr.segments[9].y = expr.segments[8].y+len_dir_y(rad*4, 195);
			expr.segments[10].x = len_dir_x(rad*14, 45);
			expr.segments[10].y = len_dir_y(rad*14, 45);
			expr.segments[11].x = expr.segments[10].x+len_dir_x(rad*4, 255);
			expr.segments[11].y = expr.segments[10].y+len_dir_y(rad*4, 255);
		}
		break;
	}
	return expr;
}

int16_t spawn_bullet(state* const s, float x, float y, float dir){
	int16_t id = next_id(&s->user.bullet_ids);
	if (id == -1){
		return -1;
	}
	color cyan = {37, 184, 252, 255};
	bullet shot = {
		.figure=shape_init(x, y, 1, 12, cyan),
		.spd=0.05,
		.dir = dir
	};
	s->user.bullets[id] = shot;
	s->user.bullet_alive |= ((uint64_t)(1)<<id);
	return id;
}

void bullet_mutate(state* const s, uint16_t id, bullet* const shot){
	shot->figure.x += len_dir_x(shot->spd, shot->dir)*s->tick;
	shot->figure.y += len_dir_y(shot->spd, shot->dir)*s->tick;
	if ((shot->figure.x > WINDOW_W) || (shot->figure.x < 0) || (shot->figure.y > WINDOW_H) || (shot->figure.y < 0)){
		s->user.bullet_alive &= ~((uint64_t)(1)<<id);
		id_push(&s->user.bullet_ids, id);
	}
}
