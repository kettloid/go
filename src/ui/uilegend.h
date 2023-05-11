#ifndef UILEGEND_H_INCLUDED
#define UILEGEND_H_INCLUDED

#include <wchar.h>

#include "../canvas.h"
#include "../game.h"

#define UI_TEXT_CURSORLOC L"Położenie kursora:"
#define UI_TEXT_X         L"x:"
#define UI_TEXT_Y         L"y:"
#define UI_TEXT_SCORE     L"Wynik:"
#define UI_TEXT_BLACK     L"Czarny:"
#define UI_TEXT_WHITE     L"Biały:"

#define UI_STYLE_LEGEND COLOR_PAIR(4)

typedef struct {
    Canvas *c;
    wchar_t *content;
} UILegend;

typedef struct {
    Canvas *c;
    Game *g;
    Cursor *cur;
} UIGameInfo;

int UILegend_init(UILegend *u, Canvas *c, wchar_t *content);

void UILegend_destroy(UILegend *u);

void UILegend_all(UILegend *u);

int UIGameInfo_init(UIGameInfo *u, Game *g, Cursor *cur, Canvas *c);

int UIGameInfo_destroy(UIGameInfo *u);

int UIGameInfo_all(UIGameInfo *u);

#endif