#ifndef USER_HEADER
#define USER_HEADER

#include "media.h"

#define WINDOW_W 320
#define WINDOW_H 180
#define WINDOW_TITLE "title"

typedef struct state state;

typedef struct user_state {

} user_state;

void user_init(state* const s);
void user_mutate(state* const s);
void user_clean(state* const s);
void user_render(state* const s);

#endif
