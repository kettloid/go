#ifndef UIBOARD_H_INCLUDED
#define UIBOARD_H_INCLUDED

#include <ncurses.h>

#include "../board.h"
#include "../canvas.h"
#include "../game.h"

#define UI_CHARSET_CTL L"┌"
#define UI_CHARSET_CTR L"┐"
#define UI_CHARSET_CBL L"└"
#define UI_CHARSET_CBR L"┘"
#define UI_CHARSET_TL L"├"
#define UI_CHARSET_TR L"┤"
#define UI_CHARSET_TT L"┬"
#define UI_CHARSET_TB L"┴"
#define UI_CHARSET_CROSS L"┼"
#define UI_CHARSET_LH L"─"
#define UI_CHARSET_LV L"│"
#define UI_CHARSET_WHITESTONE L"⬤"
#define UI_CHARSET_BLACKSTONE L"⬤"
#define UI_CHARSET_ATARI_WHITESTONE L"○"
#define UI_CHARSET_ATARI_BLACKSTONE L"○"

#define UI_CHARSET_CURSOR L"✕"

#define UI_STYLE_WHITESTONE COLOR_PAIR(1)
#define UI_STYLE_BLACKSTONE COLOR_PAIR(2)
#define UI_STYLE_CURSOR_WHITE (UI_STYLE_WHITESTONE | A_BLINK)
#define UI_STYLE_CURSOR_BLACK (UI_STYLE_BLACKSTONE | A_BLINK)
#define UI_STYLE_ATARI_WHITESTONE UI_STYLE_WHITESTONE
#define UI_STYLE_ATARI_BLACKSTONE UI_STYLE_BLACKSTONE

typedef struct {
    Canvas *c;
    Game *g;
    Cursor *cur;
    int dx, dy;
} UIBoard;

int UIBoard_init(UIBoard *u, Game *g, Cursor *cur, Canvas *c);

int UIBoard_destroy(UIBoard *u);

void UIBoard_empty_board(UIBoard *u);

void UIBoard_place_stones(UIBoard *u);

void UIBoard_cursor(UIBoard *u);

void UIBoard_all(UIBoard *u);

int UIBoard_move_cursor(UIBoard *u, Direction dir);

int UIBoard_handle(UIBoard *u);

void UIBoard_place(UIBoard *u);

#endif