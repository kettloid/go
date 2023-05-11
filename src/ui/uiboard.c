#include "uiboard.h"

#include <ncurses.h>

#include "../board.h"
#include "../canvas.h"
#include "../game.h"

int UIBoard_init(UIBoard *u, Game *g, Cursor *cur, Canvas *c) {
    u->g = g;
    u->cur = cur;
    u->c = c;
    u->dx = 0;
    u->dy = 0;
    return 0;
}

int UIBoard_destroy(UIBoard *u) { return 0; }

void UIBoard_canvas_coord(UIBoard *u, int bx, int by, int *cx, int *cy) {
    int x, y;
    int xskip = (u->c->width + 1) / 2;
    int yskip = u->c->height;

    bx -= (u->dx + 1) / xskip * xskip - 1;
    by -= (u->dy + 1) / yskip * yskip - 1;
    x = bx * 2;
    y = by;
    *cx = x;
    *cy = y;
}

int UIBoard_board_coord(UIBoard *u, int cx, int cy, int *bx, int *by) {
    int x, y;
    int xskip = (u->c->width + 1) / 2;
    int yskip = u->c->height;

    x = (cx + 1) / 2 + (u->dx + 1) / xskip * xskip - 1;
    y = cy + (u->dy + 1) / yskip * yskip - 1;

    if (Board_idx(u->g->b, x, y) < 0) {
        return -1;
    }

    *bx = x;
    *by = y;

    return 0;
}

void UIBoard_empty_board(UIBoard *u) {
    int bx, by, bs, x, y;

    bs = u->g->b->size;
    Canvas_set_attr(u->c, COLOR_PAIR(4));
    Canvas_fill(u->c, L' ');
    Canvas_set_attr(u->c, COLOR_PAIR(3));

    for (x = 0; x < u->c->width; x++) {
        for (y = 0; y < u->c->height; y++) {
            if (UIBoard_board_coord(u, x, y, &bx, &by) < 0) {
                continue;
            }

            if (x % 2 && bx == 0) {
                continue;
            }

            /* Write appriopriate symbol for given coordinates
             */
            if (x % 2) {
                Canvas_write(u->c, x, y, UI_CHARSET_LH);
            } else if (bx == 0 && by == 0) {
                Canvas_write(u->c, x, y, UI_CHARSET_CTL);
            } else if (bx == bs - 1 && by == 0) {
                Canvas_write(u->c, x, y, UI_CHARSET_CTR);
            } else if (bx == 0 && by == bs - 1) {
                Canvas_write(u->c, x, y, UI_CHARSET_CBL);
            } else if (bx == bs - 1 && by == bs - 1) {
                Canvas_write(u->c, x, y, UI_CHARSET_CBR);
            } else if (bx == 0) {
                Canvas_write(u->c, x, y, UI_CHARSET_TL);
            } else if (bx == bs - 1) {
                Canvas_write(u->c, x, y, UI_CHARSET_TR);
            } else if (by == 0) {
                Canvas_write(u->c, x, y, UI_CHARSET_TT);
            } else if (by == bs - 1) {
                Canvas_write(u->c, x, y, UI_CHARSET_TB);
            } else {
                Canvas_write(u->c, x, y, UI_CHARSET_CROSS);
            }
        }
    }
}

void UIBoard_place_stones(UIBoard *u) {
    int atari;
    int x, y, bx, by, idx;
    Stone s;

    for (x = 0; x < u->c->width; x += 2) {
        for (y = 0; y < u->c->height; y++) {
            if (UIBoard_board_coord(u, x, y, &bx, &by) < 0) {
                continue;
            }

            idx = Board_idx(u->g->b, bx, by);
            s = (Game_current_state(u->g))[idx];

            if (s == STONE_NONE) {
                continue;
            }

            atari = Board_atari(u->g->b, bx, by);

            if (s == STONE_WHITE) {
                if (atari) {
                    Canvas_set_attr(u->c, UI_STYLE_ATARI_WHITESTONE);
                    Canvas_write(u->c, x, y, UI_CHARSET_ATARI_WHITESTONE);
                } else {
                    Canvas_set_attr(u->c, UI_STYLE_WHITESTONE);
                    Canvas_write(u->c, x, y, UI_CHARSET_WHITESTONE);
                }
            }

            else if (s == STONE_BLACK) {
                if (atari) {
                    Canvas_set_attr(u->c, UI_STYLE_ATARI_BLACKSTONE);
                    Canvas_write(u->c, x, y, UI_CHARSET_ATARI_BLACKSTONE);
                } else {
                    Canvas_set_attr(u->c, UI_STYLE_BLACKSTONE);
                    Canvas_write(u->c, x, y, UI_CHARSET_BLACKSTONE);
                }
            }

            Canvas_reset_attr(u->c);
        }
    }
}

void UIBoard_cursor(UIBoard *u) {
    int cx, cy;

    if (u->g->cur_p->s == STONE_WHITE) {
        Canvas_set_attr(u->c, UI_STYLE_CURSOR_WHITE);
    }

    else {
        Canvas_set_attr(u->c, UI_STYLE_CURSOR_BLACK);
    }

    UIBoard_canvas_coord(u, u->cur->x, u->cur->y, &cx, &cy);
    Canvas_write(u->c, cx, cy, UI_CHARSET_CURSOR);
    Canvas_reset_attr(u->c);
}

void UIBoard_all(UIBoard *u) {
    UIBoard_empty_board(u);
    UIBoard_place_stones(u);
    UIBoard_cursor(u);
}

int UIBoard_handle(UIBoard *u) {
    int ch = getch();

    if (ch == KEY_RIGHT) {
        UIBoard_move_cursor(u, DIR_EAST);
    }

    else if (ch == KEY_UP) {
        UIBoard_move_cursor(u, DIR_NORTH);
    }

    else if (ch == KEY_LEFT) {
        UIBoard_move_cursor(u, DIR_WEST);
    }

    else if (ch == KEY_DOWN) {
        UIBoard_move_cursor(u, DIR_SOUTH);
    }

    return ch;
}

void UIBoard_place(UIBoard *u) {
    int ch;

    Game_place(u->g, u->cur->x, u->cur->y);
    keypad(stdscr, FALSE);

    while (u->g->confirmation_pending) {
        UIBoard_empty_board(u);
        UIBoard_place_stones(u);

        ch = getch();

        if (ch == CANVAS_KEY_ENTER) {
            Game_confirm(u->g);
        }

        else if (ch == CANVAS_KEY_ESCAPE) {
            Game_cancel(u->g);
        }
    }

    keypad(stdscr, TRUE);
}

int UIBoard_move_cursor(UIBoard *u, Direction dir) {
    int off_x, off_y;

    Board_offset(u->g->b, dir, &off_x, &off_y);

    if (Board_idx(u->g->b, off_x + u->cur->x, off_y + u->cur->y) < 0) {
        return -1;
    }

    u->dx += off_x;
    u->dy += off_y;
    u->cur->x += off_x;
    u->cur->y += off_y;

    return 0;
}