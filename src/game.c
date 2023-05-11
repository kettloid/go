#include "game.h"

#include "board.h"

float Player_score(Player* p) {
    return p->komi + p->area + p->prisoners;
}

void Game_init(Game* g, Board* b) {
    g->b = b;
    g->black_p = (Player){STONE_BLACK, 0, 0, 0.0};
    g->white_p = (Player){STONE_WHITE, 0, 0, 0.0};
    g->cur_p = &(g->black_p);
    g->alternate = 1;
    g->editor = 0;
    g->started = 0;
    g->confirmation_pending = 0;
    g->awaiting_prisoners = 0;
}

void Game_destroy(Game* g) {
    return;
}

void Game_alternate(Game* g) {
    if (!g->alternate) {
        return;
    }

    if (g->cur_p == &(g->black_p)) {
        g->cur_p = &(g->white_p);
    }

    else if (g->cur_p == &(g->white_p)) {
        g->cur_p = &(g->black_p);
    }
}

int Game_editor(Game* g) {
    if (g->started) {
        return -1;
    }

    g->alternate = 0;
    g->editor = 1;
    g->white_p.komi = GAME_HANDICAPPED_KOMI;

    return 0;
}

int Game_start(Game* g) {
    if (g->started) {
        return -1;
    }

    if (!g->editor) {
        g->white_p.komi += GAME_DEFAULT_KOMI;
    }

    g->alternate = 1;
    g->editor = 0;
    g->started = 1;

    return 0;
}

int Game_confirm(Game* g) {
    if (!g->confirmation_pending) {
        return -1;
    }

    Board_commit(g->b);

    g->cur_p->prisoners += g->awaiting_prisoners;

    Board_find_areas(g->b, &(g->white_p.area), &(g->black_p.area));

    Game_alternate(g);

    g->confirmation_pending = 0;
    g->awaiting_prisoners = 0;

    return 0;
}

int Game_cancel(Game* g) {
    if (!g->confirmation_pending) {
        return -1;
    }

    Board_rollback(g->b);
    g->confirmation_pending = 0;

    return 0;
}

Stone* Game_current_state(Game* g) {
    if (g->confirmation_pending) {
        return g->b->tran_state;
    }

    return g->b->state;
}

int Game_place(Game* g, int x, int y) {
    int place_res;

    if (g->confirmation_pending) {
        return -5;
    }

    if (!g->started && !g->editor) {
        Game_start(g);
    }

    Board_rollback(g->b);
    place_res = Board_place(g->b, g->cur_p->s, x, y);

    if (place_res < 0) {
        Board_rollback(g->b);
        return place_res;
    };

    if (Board_check_ko(g->b)) {
        Board_rollback(g->b);
        return -4;
    }

    g->confirmation_pending = 1;
    g->awaiting_prisoners = place_res;

    return 0;
}