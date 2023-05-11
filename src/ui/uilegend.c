#include "uilegend.h"

#include <ncurses.h>
#include <wchar.h>

#include "../canvas.h"
#include "../game.h"

int UILegend_init(UILegend *u, Canvas *c, wchar_t *content) {
    u->c = c;
    u->content = content;

    return 0;
}

void UILegend_destroy(UILegend *u) {
    return;
}

void UILegend_all(UILegend *u) {
    int i;
    wchar_t *s, *wcs, *ptr = NULL;

    wcs = calloc(wcslen(u->content) + 1, sizeof(wchar_t));
    wcscpy(wcs, u->content);

    Canvas_set_attr(u->c, UI_STYLE_LEGEND);
    Canvas_fill(u->c, L' ');

    s = wcstok(wcs, L"\n", &ptr);

    for (i = 0; s != NULL; i++) {
        Canvas_write(u->c, 0, i, s);
        s = wcstok(NULL, L"\n", &ptr);
    }

    free(wcs);
    Canvas_reset_attr(u->c);
}

int UIGameInfo_init(UIGameInfo *u, Game *g, Cursor *cur, Canvas *c) {
    u->c = c;
    u->cur = cur;
    u->g = g;

    return 0;
}

int UIGameInfo_destroy(UIGameInfo *u) {
    return 0;
}

int UIGameInfo_all(UIGameInfo *u) {
    wchar_t buffer[64];
    unsigned const bSize = sizeof(buffer) / sizeof(buffer[0]);

    Canvas_set_attr(u->c, UI_STYLE_LEGEND);
    Canvas_fill(u->c, L' ');

    Canvas_write(u->c, 0, 0, UI_TEXT_CURSORLOC);
    swprintf(buffer, bSize, UI_TEXT_X L" %d " UI_TEXT_Y L" %d", u->cur->x,
             u->cur->y);
    Canvas_write(u->c, 0, 1, buffer);

    Canvas_write(u->c, 0, 2, UI_TEXT_SCORE);
    swprintf(buffer, bSize, UI_TEXT_BLACK L" %.1f " UI_TEXT_WHITE L" %.1f",
             Player_score(&(u->g->black_p)), Player_score(&(u->g->white_p)));
    Canvas_write(u->c, 0, 3, buffer);

    Canvas_reset_attr(u->c);

    return 0;
}