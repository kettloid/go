#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "board.h"

#define GAME_DEFAULT_KOMI     6.5
#define GAME_HANDICAPPED_KOMI 0.5

typedef struct {
    Stone s;
    int prisoners;
    int area;
    float komi;
} Player;

typedef struct {
    int x, y;
} Cursor;

typedef struct {
    Board* b;
    Player black_p, white_p;
    Player* cur_p;
    int alternate;
    int editor;
    int started;
    int confirmation_pending;
    int awaiting_prisoners;
} Game;

/* Calculates the player's score
 */
float Player_score(Player* p);

/* Initializes Game
 */
void Game_init(Game* g, Board* b);

/* Destroys Game
 */
void Game_destroy(Game* g);

/* Alternates between players
 */
void Game_alternate(Game* g);

/* Places the appropriate stone
 * ----------------------------
 * Returns:
 *  0 on success
 * -2 when the location is occupied
 * -3 when placing the stone would result in a suicide
 * -4 when ko is violated
 * -5 when the game is awaiting a confirmation
 */
int Game_place(Game* g, int x, int y);

/* Enables editor
 * --------------
 * Returns:
 *  0 on success
 * -1 when the game has already started
 */
int Game_editor(Game* g);

/* Starts the game
 * ---------------
 * Returns:
 *  0 on success
 * -1 when the game has already started
 */
int Game_start(Game* g);

/* Gets the current state
 * ----------------------
 * Returns:
 *  t. state when there is a confirmation pending
 *  ordinary state otherwise
 */
Stone* Game_current_state(Game* g);

/* Cancels the current move
 * ------------------------
 * Returns:
 *  0 on success
 * -1 when there is no move to cancel
 */
int Game_cancel(Game* g);

/* Confirms the current move
 * -------------------------
 * Returns:
 *  0 on success
 * -1 when there is no move to confirm
 */
int Game_confirm(Game* g);

#endif