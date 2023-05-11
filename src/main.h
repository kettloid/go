#include <wchar.h>

#include "ui.h"
#include "state.h"
#include "game.h"

#define MAIN_I_BRD_S 999

#define MAIN_C_DEF_X 15
#define MAIN_C_DEF_Y 5
#define MAIN_C_DEF_W 40
#define MAIN_C_DEF_H 9

#define MAIN_C_RES_H 16

#define MAIN_C_ERR_X 10
#define MAIN_C_ERR_Y 7
#define MAIN_C_ERR_W 60
#define MAIN_C_ERR_H 6

#define MAIN_C_BRD_X 38
#define MAIN_C_BRD_Y 1
#define MAIN_C_BRD_W (MAIN_C_BRD_H * 2 - 1)
#define MAIN_C_BRD_H 21

#define MAIN_C_LGD_X 0
#define MAIN_C_LGD_Y 1
#define MAIN_C_LGD_W 30
#define MAIN_C_LGD_H 16

#define MAIN_C_GMI_X MAIN_C_LGD_X
#define MAIN_C_GMI_Y (MAIN_C_LGD_Y + MAIN_C_LGD_H + 1)
#define MAIN_C_GMI_W MAIN_C_LGD_W
#define MAIN_C_GMI_H 5

#define MAIN_T_LGD_C                   \
    L"go\n \n"  \
    "strzałki — ruszanie kursora\n" \
    "q — wyjdź z gry\n"             \
    "n — zacznij nową grę\n"       \
    "i — postaw kamień\n"           \
    "enter — zatwierdź ruch\n"      \
    "escape — anuluj ruch\n"         \
    "s — zapisz stan gry\n"          \
    "l — wczytaj stan gry\n"         \
    "f — zakończ grę\n"            \
    "e — przełącz tryb edycji\n"   \
    "r — zmień rozmiar planszy\n \n"

#define MAIN_T_SAV_E L"Utworzenie pliku zakończyło się niepowodzeniem."
#define MAIN_T_LOD_E L"Odczytanie pliku zakończyło się niepowodzeniem."
#define MAIN_T_RES_E L"Nieprawidłowy rozmiar planszy."

#define MAIN_K_QUITG 'q'
#define MAIN_K_RSIZE 'r'
#define MAIN_K_FNISH 'f'
#define MAIN_K_PLACE 'i'
#define MAIN_K_SAVEG 's'
#define MAIN_K_LOADG 'l'
#define MAIN_K_EDITG 'e'
#define MAIN_K_STRNG 'n'

typedef struct {
    UIBoard b;
    UILegend l;
    UIGameInfo gi;
    UIDialog dr, df, ds, dl, de;
} UI;

void game_reset_position(UIBoard* u);

void game_reload(UIBoard* u, int boardSize);

void game_init(Game* g, int boardSize);

void game_destroy(Game* g);

void resize_dialog_init(UIDialog* ud);

void finish_dialog_init(UIDialog* ud);

void save_dialog_init(UIDialog* ud);

void load_dialog_init(UIDialog* ud);

void error_dialog_init(UIDialog* ud);

void dialog_destroy(UIDialog* ud);

void ui_init(UI* u, Game* g);

void ui_destroy(UI* u);

void ncurses_init();

void error_dialog_handle_input(UIDialog* ud, wchar_t* err);

void resize_dialog_handle_input(UIDialog* ud, UIDialog* e, UIBoard* ub);

void finish_dialog_handle_input(UIDialog* ud, UIBoard* ub);

void save_handle_input(UIDialog* ud, UIDialog* e, State* s);

void load_handle_input(UIDialog* ud, UIDialog* e, UIBoard* ub, State* s) ;