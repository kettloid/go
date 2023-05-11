#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include <stdlib.h>

typedef enum Stone_e { STONE_NONE = 0, STONE_BLACK = 1, STONE_WHITE = 2 } Stone;

typedef enum { DIR_EAST, DIR_NORTH, DIR_WEST, DIR_SOUTH } Direction;

typedef struct {
    size_t size;
    Stone* state;
    Stone* prev_state;
    Stone* tran_state;
} Board;

Stone Stone_foe(Stone s);

/* Initializes Board
 * -----------------
 * Returns:
 *  0 on success
 * -1 on failure
 */
int Board_init(Board* b, size_t size);

/* Destroys Board
 */
void Board_destroy(Board* b);

/* Reinitializes Board with a different size
 * -----------------------------------------
 * Returns:
 *  0 on success
 * -1 on failure
 */
int Board_reset(Board* b, size_t size);

/* Calculates 1D index from 2D board coordinates
 * ---------------------------------------------
 * Returns:
 *  index (>=0) on success
 *  -1 on failure
 */
int Board_idx(Board* b, int x, int y);

/* Calculates board 2D offset from a direction
 */
void Board_offset(Board* b, Direction dir, int* off_x, int* off_y);

/* Commits a transaction by copying transactional state to current state
 * and current state to previous state
 */
void Board_commit(Board* b);

/* Reverts transactional state to current state
 */
void Board_rollback(Board* b);

/* Counts liberties of a chain in the t. state
 * ---------------------------
 * Params:
 *  visited - array with no fewer than b->size * b->size elements
 *
 * Returns:
 *  liberties (>=0) on success
 * -1 when x or y are out of bounds
 * -2 when there is no stone at selected coordinates
 */
int Board_count_liberties(Board* b, char* visited, int x, int y);

/* Removes a chain in the t. state
 * ---------------------------
 * Returns:
 *  removed stones (>0) on success
 * -1 when x or y are out of bounds
 * -2 when there is no stone at selected coordinates
 */
int Board_remove_stones(Board* b, int x, int y);

/* Checks for ko in the t. state
 */
int Board_check_ko(Board* b);

/* Places a stone in the t. state
 * --------------
 * Returns:
 *  number of prisoners (>=0) on success
 *  -2 when the location is occupied
 *  -3 when placing the stone would result in a suicide
 */
int Board_place(Board* b, Stone s, int x, int y);

/* Checks for atari in the t. state
 */
int Board_atari(Board* b, int x, int y);

/* Calculates area at coordinates and determines its owner in the t. state
 * -------------------------------------------------------
 * Params:
 *  owner - pointer to Stone set to None
 *  err - pointer to integer set to 0
 *  visited - array with no fewer than b->size * b->size elements
 * Returns:
 *  area (>=0) on success
 * -1 when x or y are out of bounds
 */
int Board_area(Board* b, Stone* owner, int* err, char* visited, int x, int y);

/* Finds present territories in the t. state
 */
void Board_find_areas(Board* b, int* white_area, int* black_area);

#endif