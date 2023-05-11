#include "main.h"

#include <locale.h>
#include <ncurses.h>
#include <stdio.h>

#include "board.h"
#include "canvas.h"
#include "game.h"
#include "state.h"
#include "ui.h"

void game_reset_position(UIBoard* u) {
    u->dx = 0;
    u->dy = 0;
    u->cur->x = 0;
    u->cur->y = 0;
}

void game_reload(UIBoard* u, int boardSize) {
    Game* g = u->g;
    Board* b = u->g->b;
    int oldBoardSize = b->size;

    Board_destroy(b);
    Game_destroy(g);
    game_reset_position(u);

    if (boardSize) {
        Board_init(b, boardSize);
    } else {
        Board_init(b, oldBoardSize);
    }

    Game_init(g, b);
}

void game_init(Game* g, int boardSize) {
    Board* b = malloc(sizeof(Board));

    Board_init(b, boardSize);
    Game_init(g, b);
}

void game_destroy(Game* g) {
    Board_destroy(g->b);
    Game_destroy(g);
    free(g->b);
}

void resize_dialog_init(UIDialog* ud) {
    Canvas* c = malloc(sizeof(Canvas));

    Canvas_init(c, MAIN_C_DEF_X, MAIN_C_DEF_Y, MAIN_C_DEF_W, MAIN_C_RES_H);
    UIDialog_init(ud, c, 6, 1);
    UIDialog_add_label(ud, 0, L"Wybierz rozmiar");
    UIDialog_add_button(ud, 1, L"9x9");
    UIDialog_add_button(ud, 2, L"13x13");
    UIDialog_add_button(ud, 3, L"19x19");
    UIDialog_add_integer(ud, 4);
    UIDialog_add_button(ud, 5, L"Anuluj");
}

void finish_dialog_init(UIDialog* ud) {
    Canvas* c = malloc(sizeof(Canvas));

    Canvas_init(c, MAIN_C_DEF_X, MAIN_C_DEF_Y, MAIN_C_DEF_W, MAIN_C_DEF_H);
    UIDialog_init(ud, c, 3, 1);
    UIDialog_add_label(ud, 0, NULL);
    UIDialog_add_button(ud, 1, L"Nowa gra");
    UIDialog_add_button(ud, 2, L"Kontynuuj");
}

void save_dialog_init(UIDialog* ud) {
    Canvas* c = malloc(sizeof(Canvas));

    Canvas_init(c, MAIN_C_DEF_X, MAIN_C_DEF_Y, MAIN_C_DEF_W, MAIN_C_DEF_H);
    UIDialog_init(ud, c, 3, 1);
    UIDialog_add_label(ud, 0, L"Zapisz stan gry");
    UIDialog_add_text(ud, 1);
    UIDialog_add_button(ud, 2, L"Anuluj");
}

void load_dialog_init(UIDialog* ud) {
    Canvas* c = malloc(sizeof(Canvas));

    Canvas_init(c, MAIN_C_DEF_X, MAIN_C_DEF_Y, MAIN_C_DEF_W, MAIN_C_DEF_H);
    UIDialog_init(ud, c, 3, 1);
    UIDialog_add_label(ud, 0, L"Wczytaj stan gry");
    UIDialog_add_text(ud, 1);
    UIDialog_add_button(ud, 2, L"Anuluj");
}

void error_dialog_init(UIDialog* ud) {
    Canvas* c = malloc(sizeof(Canvas));

    Canvas_init(c, MAIN_C_ERR_X, MAIN_C_ERR_Y, MAIN_C_ERR_W, MAIN_C_ERR_H);
    UIDialog_init(ud, c, 2, 1);
    UIDialog_add_label(ud, 0, NULL);
    UIDialog_add_button(ud, 1, L"OK");
}

void dialog_destroy(UIDialog* ud) {
    free(ud->c);
    UIDialog_destroy(ud);
}

void ui_init(UI* u, Game* g) {
    Cursor* cur = malloc(sizeof(Cursor));
    Canvas* canvases = malloc(sizeof(Canvas) * 3);

    cur->x = 0;
    cur->y = 0;
    Canvas_init(canvases, MAIN_C_BRD_X, MAIN_C_BRD_Y, MAIN_C_BRD_W,
                MAIN_C_BRD_H);
    Canvas_init(canvases + 1, MAIN_C_LGD_X, MAIN_C_LGD_Y, MAIN_C_LGD_W,
                MAIN_C_LGD_H);
    Canvas_init(canvases + 2, MAIN_C_GMI_X, MAIN_C_GMI_Y, MAIN_C_GMI_W,
                MAIN_C_GMI_H);
    UIBoard_init(&u->b, g, cur, canvases);
    UILegend_init(&u->l, canvases + 1, MAIN_T_LGD_C);
    UIGameInfo_init(&u->gi, g, cur, canvases + 2);
    resize_dialog_init(&u->dr);
    load_dialog_init(&u->dl);
    save_dialog_init(&u->ds);
    error_dialog_init(&u->de);
    finish_dialog_init(&u->df);
}

void ui_destroy(UI* u) {
    free(u->b.cur);
    free(u->b.c);

    dialog_destroy(&u->de);
    dialog_destroy(&u->df);
    dialog_destroy(&u->dl);
    dialog_destroy(&u->dr);
    dialog_destroy(&u->ds);
}

void ncurses_init() {
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    raw();
    start_color();
    curs_set(0);
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_WHITE, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
    init_pair(3, COLOR_YELLOW, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);
}

void error_dialog_handle_input(UIDialog* ud, wchar_t* err) {
    ud->inputs[0].text = err;
    UIDialog_open(ud);
    UIDialog_handle(ud);
    UIDialog_close(ud);
    ud->inputs[0].text = NULL;
}

void resize_dialog_handle_input(UIDialog* ud, UIDialog* e, UIBoard* ub) {
    int newBoardSize = 0, sel;

    UIDialog_open(ud);

    sel = UIDialog_handle(ud);
    if (sel == 1) {
        newBoardSize = 9;
    } else if (sel == 2) {
        newBoardSize = 13;
    } else if (sel == 3) {
        newBoardSize = 19;
    } else if (sel == 4) {
        UIDialogElement_read_int(ud->inputs + 4, &newBoardSize);
    }

    UIDialog_close(ud);

    if (sel == 5) {
        return;
    }

    if (newBoardSize < 1 || newBoardSize > MAIN_I_BRD_S) {
        error_dialog_handle_input(e, MAIN_T_RES_E);

        return;
    }

    game_reload(ub, newBoardSize);
}

void finish_dialog_handle_input(UIDialog* ud, UIBoard* ub) {
    wchar_t mess[1000];
    int sel;

    swprintf(mess, 500, L"Biały: %.2f Czarny: %.2f",
             Player_score(&(ub->g->white_p)), Player_score(&(ub->g->black_p)));
    ud->inputs[0].text = mess;

    UIDialog_open(ud);

    sel = UIDialog_handle(ud);
    if (sel == 1) {
        game_reload(ub, 0);
    }

    UIDialog_close(ud);

    ud->inputs[0].text = NULL;
}

void save_handle_input(UIDialog* ud, UIDialog* e, State* s) {
    int sel;
    wchar_t* filenamew;
    char filename[500] = {0};

    UIDialog_open(ud);

    sel = UIDialog_handle(ud);
    if (sel == 1) {
        UIDialogElement_read_text(ud->inputs + 1, &filenamew);
        wcstombs(filename, filenamew, 300);

        if (State_save(s, filename) < 0) {
            error_dialog_handle_input(e, MAIN_T_SAV_E);
        }
    }

    UIDialog_close(ud);
}

void load_handle_input(UIDialog* ud, UIDialog* e, UIBoard* ub, State* s) {
    int sel;
    wchar_t* filenamew;
    char filename[500] = {0};

    UIDialog_open(ud);

    sel = UIDialog_handle(ud);
    if (sel == 1) {
        UIDialogElement_read_text(ud->inputs + 1, &filenamew);
        wcstombs(filename, filenamew, 450);
        if (State_load(s, filename) < 0) {
            error_dialog_handle_input(e, MAIN_T_LOD_E);
        } else {
            game_reset_position(ub);
        }
    }
    UIDialog_close(ud);
}

int main() {
    Game g;
    State s;
    UI u;
    int res, boardSize = 19;

    game_init(&g, boardSize);
    State_init(&s, &g);
    ui_init(&u, &g);

    setlocale(LC_ALL, "");
    ncurses_init();

    while (1) {
        erase();
        UIBoard_all(&u.b);
        UILegend_all(&u.l);
        UIGameInfo_all(&u.gi);
        refresh();

        res = UIBoard_handle(&u.b);
        if (res == MAIN_K_QUITG) break;
        if (res == MAIN_K_RSIZE) resize_dialog_handle_input(&u.dr, &u.de, &u.b);
        if (res == MAIN_K_FNISH) finish_dialog_handle_input(&u.df, &u.b);
        if (res == MAIN_K_PLACE) UIBoard_place(&u.b);
        if (res == MAIN_K_SAVEG) save_handle_input(&u.ds, &u.de, &s);
        if (res == MAIN_K_LOADG) load_handle_input(&u.dl, &u.de, &u.b, &s);
        if (res == MAIN_K_STRNG) game_reload(&u.b, 0);
        if (res == MAIN_K_EDITG) {
            if (!g.editor)
                Game_editor(&g);
            else
                Game_start(&g);
        }
    }
    endwin();
    ui_destroy(&u);
    game_destroy(&g);
}