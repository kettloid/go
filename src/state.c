#include "state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "game.h"

void State_init(State* s, Game* g) {
    s->g = g;
}

void State_from_player(StatePlayer* sp, Player* p) {
    sp->area = p->area;
    sp->komi = (int32_t)(p->komi * 2 + 0.3);
    sp->prisoners = p->prisoners;
    sp->s = p->s;
}

void State_from_game(StateGame* sg, Game* g) {
    State_from_player(&(sg->black_p), &(g->black_p));
    State_from_player(&(sg->white_p), &(g->white_p));

    sg->magic = STATE_MAGIC;
    sg->alternate = g->alternate;
    sg->editor = g->editor;
    sg->started = g->started;
    sg->size = g->b->size;
    sg->cur_p = &(g->black_p) == g->cur_p;
}

void State_to_player(StatePlayer* sp, Player* p) {
    p->area = sp->area;
    p->komi = sp->komi / 2.0;
    p->prisoners = sp->prisoners;
    p->s = sp->s;
}

void State_to_game(StateGame* sg, Game* g) {
    State_to_player(&(sg->black_p), &(g->black_p));
    State_to_player(&(sg->white_p), &(g->white_p));

    g->alternate = sg->alternate;
    g->editor = sg->editor;
    g->started = sg->started;
    g->b->size = sg->size;
    g->cur_p = sg->cur_p ? &(g->black_p) : &(g->white_p);
}

int State_save(State* s, char* filename) {
    StateGame sg;
    FILE* f = fopen(filename, "w");

    if (f == NULL) {
        return -1;
    }

    State_from_game(&sg, s->g);
    fwrite(&sg, sizeof(StateGame), 1, f);

    int size = s->g->b->size * s->g->b->size;
    fwrite(s->g->b->state, sizeof(Stone), size, f);
    fwrite(s->g->b->tran_state, sizeof(Stone), size, f);
    fwrite(s->g->b->prev_state, sizeof(Stone), size, f);
    fclose(f);

    return 0;
}

int State_load(State* s, char* filename) {
    StateGame sg;
    FILE* f = fopen(filename, "r");

    if (f == NULL) {
        return -1;
    }

    if (fread(&sg, sizeof(StateGame), 1, f) != 1) {
        return -2;
    }

    if (sg.magic != STATE_MAGIC) {
        return -3;
    }

    State_to_game(&sg, s->g);

    int size = sg.size * sg.size;
    Board_reset(s->g->b, sg.size);
    fread(s->g->b->state, sizeof(Stone), size, f);
    fread(s->g->b->tran_state, sizeof(Stone), size, f);
    fread(s->g->b->prev_state, sizeof(Stone), size, f);
    
    fclose(f);
    
    return 0;
}