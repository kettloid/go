#include "board.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Stone Stone_foe(Stone s) {
    return (s == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;
}

int Board_init(Board *b, size_t size) {
    b->size = size;
    b->state = calloc(size * size, sizeof(Stone));
    b->prev_state = calloc(size * size, sizeof(Stone));
    b->tran_state = calloc(size * size, sizeof(Stone));

    if (b->state == NULL || b->prev_state == NULL || b->tran_state == NULL)
        return -1;

    return 0;
}

void Board_destroy(Board *b) {
    free(b->state);
    free(b->prev_state);
    free(b->tran_state);

    b->state = NULL;
    b->prev_state = NULL;
    b->tran_state = NULL;
}

int Board_reset(Board *b, size_t size) {
    Board_destroy(b);

    return Board_init(b, size);
}

int Board_idx(Board *b, int x, int y) {
    if (!(x >= 0 && y >= 0 && x < b->size && y < b->size)) {
        return -1;
    }

    return b->size * y + x;
}

void Board_offset(Board *b, Direction dir, int *off_x, int *off_y) {
    *off_x = (dir == DIR_EAST) - (dir == DIR_WEST);
    *off_y = (dir == DIR_SOUTH) - (dir == DIR_NORTH);
}

void Board_commit(Board *b) {
    memcpy(b->prev_state, b->state, b->size * b->size * sizeof(Stone));
    memcpy(b->state, b->tran_state, b->size * b->size * sizeof(Stone));
}

void Board_rollback(Board *b) {
    memcpy(b->tran_state, b->state, b->size * b->size * sizeof(Stone));
}

int Board_count_liberties(Board *b, char *visited, int x, int y) {
    int idx, off_x, off_y, res = 0;
    Stone n, s;
    Direction dir;

    idx = Board_idx(b, x, y);
    if (idx < 0) {
        return -1;
    }

    s = b->tran_state[idx];
    if (s == STONE_NONE) {
        return -2;
    }

    for (dir = DIR_EAST; dir <= DIR_SOUTH; dir++) {
        Board_offset(b, dir, &off_x, &off_y);
        idx = Board_idx(b, x + off_x, y + off_y);

        if (idx < 0) {
            continue;
        }

        n = b->tran_state[idx];

        if (n == STONE_NONE && !visited[idx]) {
            visited[idx] = 1;
            res++;
        }

        else if (n == s && !visited[idx]) {
            visited[idx] = 1;
            res += Board_count_liberties(b, visited, x + off_x, y + off_y);
        }
    }
    return res;
}

int Board_area(Board *b, Stone *owner, int *err, char *visited, int x, int y) {
    int idx, off_x, off_y, res, out = 1;
    Stone s;
    Direction dir;

    idx = Board_idx(b, x, y);
    if (idx == -1) {
        return -1;
    }

    if (visited[idx]) {
        return 0;
    }

    s = b->tran_state[idx];

    if (s != STONE_NONE) {
        if (*owner == STONE_NONE) {
            *owner = s;
        }

        if (s != *owner) {
            *err = 1;
        }

        return 0;
    }

    visited[idx] = 1;

    for (dir = DIR_EAST; dir <= DIR_SOUTH; dir++) {
        Board_offset(b, dir, &off_x, &off_y);
        idx = Board_idx(b, x + off_x, y + off_y);

        if (idx < 0) {
            continue;
        }

        res = Board_area(b, owner, err, visited, x + off_x, y + off_y);
        out += res;
    }

    return out;
}

void Board_find_areas(Board *b, int *white_area, int *black_area) {
    int x, y, res, err;
    Stone s;
    char *visited = calloc(b->size * b->size, sizeof(char));

    *black_area = 0;
    *white_area = 0;

    for (x = 0; x < b->size; x++) {
        for (y = 0; y < b->size; y++) {
            err = 0;
            s = STONE_NONE;
            res = Board_area(b, &s, &err, visited, x, y);

            if (err) {
                continue;
            }

            if (s == STONE_WHITE) {
                *white_area += res;
            }

            else {
                *black_area += res;
            }
        }
    }

    free(visited);
}

int Board_remove_stones(Board *b, int x, int y) {
    int idx, off_x, off_y, res = 1;
    Stone n, s;
    Direction dir;

    idx = Board_idx(b, x, y);
    if (idx == -1) {
        return -1;
    }

    s = b->tran_state[idx];
    if (s == STONE_NONE) {
        return -2;
    }

    b->tran_state[idx] = STONE_NONE;

    for (dir = DIR_EAST; dir <= DIR_SOUTH; dir++) {
        Board_offset(b, dir, &off_x, &off_y);
        idx = Board_idx(b, x + off_x, y + off_y);

        if (idx < 0) {
            continue;
        }

        n = b->tran_state[idx];
        if (n == s) {
            res += Board_remove_stones(b, x + off_x, y + off_y);
        }
    }

    return res;
}

int Board_check_ko(Board *b) {
    return !memcmp(b->tran_state, b->prev_state,
                   b->size * b->size * sizeof(Stone));
}

int Board_place(Board *b, Stone s, int x, int y) {
    int idx, off_x, off_y, liberties, prisoners = 0;
    Direction dir;
    char *visited;

    idx = Board_idx(b, x, y);

    if (idx < 0) {
        return -1;
    }

    if (b->state[idx] != STONE_NONE) {
        return -2;
    }

    visited = malloc(b->size * b->size * sizeof(char));

    b->tran_state[idx] = s;

    for (dir = DIR_EAST; dir <= DIR_SOUTH; dir++) {
        memset(visited, 0, b->size * b->size * sizeof(char));
        Board_offset(b, dir, &off_x, &off_y);
        idx = Board_idx(b, x + off_x, y + off_y);

        if (idx < 0) {
            continue;
        }

        if (b->tran_state[idx] == Stone_foe(s)) {
            liberties = Board_count_liberties(b, visited, x + off_x, y + off_y);
            if (liberties == 0) {
                prisoners += Board_remove_stones(b, off_x + x, off_y + y);
            }
        }
    }

    memset(visited, 0, b->size * b->size * sizeof(char));
    liberties = Board_count_liberties(b, visited, x, y);

    free(visited);

    if (liberties == 0) {
        return -3;
    }

    return prisoners;
}

int Board_atari(Board *b, int x, int y) {
    char *visited = calloc(b->size * b->size, sizeof(char));
    int liberties;

    liberties = Board_count_liberties(b, visited, x, y);

    free(visited);

    return liberties == 1;
}
