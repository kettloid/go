#include "canvas.h"

#include <wchar.h>

#include "board.h"
#include "game.h"

#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>

void Canvas_init(Canvas *c, int wx, int wy, int width, int height) {
    c->wx = wx;
    c->wy = wy;
    c->width = width;
    c->height = height;
    c->attr = A_NORMAL;
}

void Canvas_set_attr(Canvas *c, unsigned attr) {
    c->attr = attr;
}

void Canvas_mod_attr(Canvas *c, unsigned attr) {
    c->attr |= attr;
}

void Canvas_reset_attr(Canvas *c) {
    c->attr = A_NORMAL;
}

int Canvas_inbounds(Canvas *c, int x, int y) {
    return x >= 0 && y >= 0 && x < c->width && y < c->height;
}

int Canvas_write(Canvas *c, int x, int y, wchar_t *str) {
    if (!Canvas_inbounds(c, x, y)) {
        return -1;
    }

    if (!Canvas_inbounds(c, x + wcslen(str) - 1, y)) {
        return -1;
    }

    attrset(c->attr);
    mvaddwstr(c->wy + y, c->wx + x, str);
    attrset(A_NORMAL);

    return 0;
}

int Canvas_read(Canvas *c, int x, int y, wchar_t *buffer, int buffer_size,
                wchar_t *whitelist) {
    int n, pos = 0, res;
    wchar_t ch;

    n = c->width - x;
    if (!Canvas_inbounds(c, x, y) || n > buffer_size) {
        return -1;
    }

    curs_set(CANVAS_CURSOR_LINE);

    while (1) {
        Canvas_fill(c, L' ');
        Canvas_write(c, x, y, buffer);

        res = mvget_wch(c->wy + y, c->wx + x + pos, &ch);

        if (res == OK && ch == CANVAS_KEY_ENTER) {
            break;
        }

        if (res == KEY_CODE_YES) {
            if (ch == KEY_BACKSPACE && pos > 0 && !buffer[pos]) {
                pos--;
                buffer[pos] = 0;
            } else if (ch == KEY_LEFT && pos > 0) {
                pos--;
            } else if (ch == KEY_RIGHT && pos < n && buffer[pos]) {
                pos++;
            }
        }

        else if (res == OK && pos < n) {
            if (whitelist != CANVAS_WHITELIST_ANY &&
                wcschr(whitelist, (wchar_t)ch) == NULL) {
                continue;
            }

            buffer[pos] = (wchar_t)ch;
            pos++;
        }
    }
    curs_set(CANVAS_CURSOR_NONE);

    return 0;
}

int Canvas_fill(Canvas *c, wchar_t s) {
    int i, err;
    wchar_t *content = malloc(sizeof(wchar_t) * (c->width + 1));

    wmemset(content, s, c->width);
    content[c->width] = 0;

    for (i = 0; i < c->height; i++) {
        err = Canvas_write(c, 0, i, content);
        
        if (err < 0) {
            return err;
        }
    }

    free(content);

    return 0;
}