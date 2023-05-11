#ifndef CANVAS_H_INCLUDED
#define CANVAS_H_INCLUDED

#include <wchar.h>

#define CANVAS_CURSOR_LINE 2
#define CANVAS_CURSOR_NONE 0
#define CANVAS_KEY_ENTER   10
#define CANVAS_KEY_ESCAPE  27

#define CANVAS_WHITELIST_ANY     NULL
#define CANVAS_WHITELIST_NUMBERS L"0123456789"

typedef struct {
    int wx, wy;
    int width, height;
    unsigned attr;
} Canvas;

/* Initializes Canvas
 * ------------------
 * Params
 *  wx - screen relative x
 *  wy - screen relative y
 *  width - screen width
 *  height - screen height
 */
void Canvas_init(Canvas* c, int wx, int wy, int width, int height);

/* Sets ncurses attribute
 */
void Canvas_set_attr(Canvas* c, unsigned attr);

/* Modifies ncurses attribute
 */
void Canvas_mod_attr(Canvas* c, unsigned attr);

/* Resets ncurses attribute
 */
void Canvas_reset_attr(Canvas* c);

/* Writes on canvas with respect to its coordinates
 * Returns:
 *  0 on success
 * -1 when coordinates are out of bounds
 */
int Canvas_write(Canvas* c, int x, int y, wchar_t* str);

/* Checks if given coordinates are inbounds
 * Returns:
 *  1 when inbounds
 *  0 otherwise
 */
int Canvas_inbounds(Canvas* c, int x, int y);

/* Fills the canvas with a given character
 * Returns:
 *  0 on success
 * -1 when coordinates are out of bounds
 */
int Canvas_fill(Canvas* c, wchar_t s);

/* Reads text at the specified coordinates into the buffer
 * Returns:
 *  0 on success
 * -1 when coordinates are out of bounds
 */
int Canvas_read(Canvas* c, int x, int y, wchar_t* buffer, int buffer_size,
                wchar_t* whitelist);

#endif