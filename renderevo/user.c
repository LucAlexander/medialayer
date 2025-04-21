#include "user.h"
#include "state.h"
#include "evo.h"

static color white = {255, 255, 255, 255};
static color red = {255, 0, 0, 255};
static color green = {0, 255, 0, 255};
static color black = {0, 0, 0, 255};

void user_init(state* const s){
	set_seed(time(NULL));
	pool* mem = &s->main_mem;
	layer* input = input_init_evo(mem, 8);
	layer* output = layer_init(mem, 8, ACTIVATION_SIGMOID, 0);
	network net = network_init(
		mem,
		input, output,
		WEIGHT_INITIALIZATION_XAVIER,
		BIAS_INITIALIZATION_ZERO,
		LAYER_WEIGHT_INITIALIZATION_NORMAL,
		ACTIVATION_RELU,
		1, 2,
		0, 0,
		0, 0,
		0,
		16, 0.001,
		5,
		LOSS_MSE
	);
	uint64_t samples = 512;
	double** training_data = pool_request(mem, sizeof(double*)*samples);
	uint8_t pos = 0;
	for (uint32_t i = 0;i<samples;++i){
		training_data[i] = pool_request(mem, sizeof(double)*net.input->data.input.width);
		for (uint32_t k = 0;k<net.input->data.input.width;++k){
			training_data[i][k] = 0;
		}
		training_data[i][pos] = 1;
		pos += 1;
		if (pos == 8){
			pos = 0;
		}
	}
	pos = 0;
	double** expected = pool_request(mem, sizeof(double*)*samples);
	for (uint32_t i = 0;i<samples;++i){
		expected[i] = pool_request(mem, sizeof(double)*net.output->data.layer.width);
		for (uint32_t k = 0;k<net.output->data.layer.width;++k){
			expected[i][k] = 0;
		}
		expected[i][pos] = 1;
		pos += 1;
		if (pos == 8){
			pos = 0;
		}
	}
	grow_network_sparse(
		&net,
		training_data, samples, expected,
		1000, 100, 200
	);
	s->user.net = net;
	/*
	prediction_vector vec = predict_vector_batched(&net, mem, &training_data, 1, net.batch_size, net.input->data.input.width);
	printf("predicted %lu (%lf) \n", vec.class[0], vec.probability[0]);
	*/
}

void user_mutate(state* const s){

}

void user_clean(state* const s){

}

void render_node(state* const s, uint64_t id, float initial_x, float initial_y){
	layer* node = s->user.net.nodes[id];
	node_circle* circ = &s->user.positions[id];
	circ->x = initial_x;
	circ->y = initial_y;
	{
		if (node == s->user.net.input){
			render_set_color(&s->render, green);
		}
		else if (node == s->user.net.output){
			render_set_color(&s->render, red);
		}
		else{
			render_set_color(&s->render, white);
		}
		float inner = 0;
		float rot = 0;
		float ang = 2;
		float ax = len_dir_x(circ->rad, rot);
		float ay = len_dir_y(circ->rad, rot);
		inner += ang;
		float bx = len_dir_x(circ->rad, inner+rot);
		float by = len_dir_y(circ->rad, inner+rot);
		while (inner <= 360){
			draw_line(&s->render, circ->x+ax, circ->y+ay, circ->x+bx, circ->y+by);
			inner += ang;
			ax = bx;
			ay = by;
			bx = len_dir_x(circ->rad, inner+rot);
			by = len_dir_y(circ->rad, inner+rot);
		}
		render_set_color(&s->render, black);
	}
	float x = initial_x;
	float y = initial_y;
	if (node->next_count > 1){
		y = initial_y-(node->next_count*circ->rad*4);
	}
	for (uint64_t i = 0;i<node->next_count;++i){
		if (s->user.rendered[node->next[i]] == 1){
			continue;
		}
		render_node(s, node->next[i], x+circ->rad*4, y);
		y += circ->rad * 4;
	}
}

void user_render(state* const s){
	s->user.rendered = pool_request(&s->frame_mem, sizeof(uint8_t)*s->user.net.node_count);
	s->user.positions = pool_request(&s->frame_mem, sizeof(node_circle)*s->user.net.node_count);
	memset(s->user.rendered, 0, sizeof(uint8_t)*s->user.net.node_count);
	uint64_t start_id = 0;
	for (uint64_t i = 0;i<s->user.net.node_count;++i){
		s->user.positions[i].rad = 12;
		if (s->user.net.input == s->user.net.nodes[i]){
			start_id = i;
		}
	}
	render_node(s, start_id, 64, 96);
	uint8_t collisions = 1;
	while (collisions != 0){
		collisions = 0;
		for (uint64_t i = 0;i<s->user.net.node_count;++i){
			node_circle* a = &s->user.positions[i];
			for (uint64_t k = i+1;k<s->user.net.node_count;++k){
				node_circle* b = &s->user.positions[k];
				float dist = distance_point(a->x, a->y, b->x, b->y);
				while (dist < a->rad){
					collisions = 1;
					float bdir = direction_point(a->x, a->y, b->x, b->y);
					float adir = bdir + 180;
					a->x += len_dir_x(dist, adir);
					a->y += len_dir_y(dist, adir);
					b->x += len_dir_x(dist, bdir);
					b->y += len_dir_y(dist, bdir);
					dist = distance_point(a->x, a->y, b->x, b->y);
				}
			}
		}
	}
	for (uint64_t i = 0;i<s->user.net.node_count;++i){
		node_circle* dest  = &s->user.positions[i];
		layer* node = s->user.net.nodes[i];
		for (uint64_t k = 0;k<node->prev_count;++k){
			node_circle* source = &s->user.positions[node->prev[k]];
			float dir = direction_point(dest->x, dest->y, source->x, source->y);
			float src_off_x = source->x+len_dir_x(source->rad, dir+180);
			float src_off_y = source->y+len_dir_y(source->rad, dir+180);
			float dst_off_x = dest->x+len_dir_x(dest->rad, dir);
			float dst_off_y = dest->y+len_dir_y(dest->rad, dir);
			render_set_color(&s->render, white);

			draw_line(&s->render, dst_off_x, dst_off_y, src_off_x, src_off_y);

			float head_half_x = dst_off_x+len_dir_x(6, dir+45);
			float head_half_y = dst_off_y+len_dir_y(6, dir+45);
			draw_line(&s->render, dst_off_x, dst_off_y, head_half_x, head_half_y);

			head_half_x = dst_off_x+len_dir_x(6, dir-45);
			head_half_y = dst_off_y+len_dir_y(6, dir-45);
			draw_line(&s->render, dst_off_x, dst_off_y, head_half_x, head_half_y);

			render_set_color(&s->render, black);
		}
	}
}
