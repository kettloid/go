#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <stdint.h>

#include "board.h"
#include "game.h"

#define STATE_MAGIC 0x2A1B3D7DUL

typedef struct __attribute__((packed)) {
    Stone s;
    int32_t prisoners, area, komi;
} StatePlayer;

typedef struct __attribute__((packed)) {
    uint32_t magic;
    StatePlayer black_p, white_p;
    int32_t alternate, editor, started;
    int32_t size;
    int32_t cur_p;
} StateGame;

typedef struct {
    Game* g;
} State;

/* Initializes State
 */
void State_init(State* s, Game* g);

/* Fills StatePlayer with Player
 */
void State_from_player(StatePlayer* sp, Player* p);

/* Fills StateGame with Game
 */
void State_from_game(StateGame* sg, Game* g);

/* Fills Player with StatePlayer
 */
void State_to_player(StatePlayer* sp, Player* p);

/* Fills Game with StateGame
 */
void State_to_game(StateGame* sg, Game* g);

/* Saves State
 * -----------
 * Returns:
 *  0 on success
 * -1 on failure
 */
int State_save(State* s, char* filename);

/* Loads State
 * -----------
 * Returns:
 *  0 on success
 * -1 on failure
 */
int State_load(State* s, char* filename);

#endif