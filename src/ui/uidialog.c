#include "uidialog.h"

#include <ncurses.h>
#include <wchar.h>

#include "../board.h"
#include "../canvas.h"

int UIDialogElement_text_init(UIDialogElement *i, Canvas *c, wchar_t *buffer,
                              int buffer_size) {
    i->c = c;
    i->type = DIALOG_INPUT_TEXT;
    i->text = NULL;
    i->buffer = (void *)buffer;
    i->buffer_size = buffer_size;

    return 0;
}

int UIDialogElement_int_init(UIDialogElement *i, Canvas *c, wchar_t *buffer,
                             int buffer_size) {
    i->c = c;
    i->type = DIALOG_INPUT_INTEGER;
    i->text = NULL;
    i->buffer = (void *)buffer;
    i->buffer_size = buffer_size;

    return 0;
}

int UIDialogElement_button_init(UIDialogElement *i, Canvas *c, wchar_t *text) {
    i->c = c;
    i->type = DIALOG_INPUT_BUTTON;
    i->text = text;
    i->buffer = NULL;
    i->buffer_size = 0;

    return 0;
}

int UIDialogElement_label_init(UIDialogElement *i, Canvas *c, wchar_t *text) {
    i->c = c;
    i->type = DIALOG_LABEL;
    i->text = text;
    i->buffer = NULL;
    i->buffer_size = 0;

    return 0;
}

int UIDialogElement_read_text(UIDialogElement *i, wchar_t **out) {
    if (i->type != DIALOG_INPUT_TEXT) {
        return -1;
    }

    *out = (wchar_t *)(i->buffer);

    return i->buffer_size;
}

int UIDialogElement_read_int(UIDialogElement *i, int *out) {
    if (i->type != DIALOG_INPUT_INTEGER) {
        return -1;
    }

    if (swscanf((wchar_t *)(i->buffer), L"%d", out) == EOF) {
        return -1;
    }

    return 0;
}

int UIDialogElement_reset_buffer(UIDialogElement *i) {
    if (i->type != DIALOG_INPUT_TEXT && i->type != DIALOG_INPUT_INTEGER) {
        return -1;
    }

    wmemset(i->buffer, 0, i->buffer_size);

    return 0;
}

int UIDialogElement_all(UIDialogElement *i, int selected) {
    wchar_t *content;

    if (selected) {
        Canvas_set_attr(i->c, A_STANDOUT);
    }

    if (i->type == DIALOG_INPUT_BUTTON || i->type == DIALOG_LABEL) {
        content = i->text;
    }

    if (i->type == DIALOG_INPUT_TEXT) {
        if (!((wchar_t *)i->buffer)[0]) {
            content = UI_TEXT_TXTPROMPT;
        } else {
            content = i->buffer;
        }
    }

    if (i->type == DIALOG_INPUT_INTEGER) {
        if (!((wchar_t *)i->buffer)[0]) {
            content = UI_TEXT_NUMPROMPT;
        } else {
            content = i->buffer;
        }
    }

    Canvas_write(i->c, 0, 0, content);
    Canvas_reset_attr(i->c);

    return 0;
}

void UIDialog_init(UIDialog *d, Canvas *c, int inputs_count, int initial_pos) {
    int i;

    d->c = c;
    d->pos = initial_pos;
    d->is_open = 0;
    d->margin = UI_MARGIN;
    d->initial_pos = initial_pos;

    d->inputs = malloc(sizeof(UIDialogElement) * inputs_count);
    d->inputs_count = inputs_count;

    for (i = 0; i < d->inputs_count; i++) {
        d->inputs[i].type = DIALOG_NONE;
        d->inputs[i].c = NULL;
    }
}

void UIDialog_destroy(UIDialog *d) {
    int i;

    for (i = 0; i < d->inputs_count; i++) {
        switch (d->inputs[i].type) {
            case DIALOG_INPUT_BUTTON:
                UIDialog_remove_button(d, i);
                break;

            case DIALOG_INPUT_INTEGER:
                UIDialog_remove_integer(d, i);
                break;

            case DIALOG_INPUT_TEXT:
                UIDialog_remove_text(d, i);
                break;

            case DIALOG_LABEL:
                UIDialog_remove_label(d, i);
                break;

            default:
                break;
        }
    }

    free(d->inputs);
}

int UIDialog_reset(UIDialog *d) {
    int i;

    d->pos = d->initial_pos;
    for (i = 0; i < d->inputs_count; i++) {
        UIDialogElement_reset_buffer(d->inputs + i);
    }

    return 0;
}

int UIDialog_add_button(UIDialog *d, int pos, wchar_t *text) {
    UIDialogElement *e = d->inputs + pos;
    Canvas *c = malloc(sizeof(Canvas));

    Canvas_init(c, d->c->wx + d->margin, d->c->wy + d->margin + pos * 2,
                d->c->width - 2 * d->margin, 1);

    UIDialogElement_button_init(e, c, text);

    return 0;
}

int UIDialog_add_text(UIDialog *d, int pos) {
    UIDialogElement *e = d->inputs + pos;
    Canvas *c = malloc(sizeof(Canvas));
    int buffer_size = d->c->width + 1 - 2 * d->margin;
    wchar_t *buffer = calloc(buffer_size, sizeof(wchar_t));

    Canvas_init(c, d->c->wx + d->margin, d->c->wy + d->margin + pos * 2,
                buffer_size - 1, 1);

    UIDialogElement_text_init(e, c, buffer, buffer_size);

    return 0;
}

int UIDialog_add_integer(UIDialog *d, int pos) {
    UIDialogElement *e = d->inputs + pos;
    Canvas *c = malloc(sizeof(Canvas));
    int buffer_size = d->c->width + 1 - 2 * d->margin;
    wchar_t *buffer = calloc(buffer_size, sizeof(wchar_t));

    Canvas_init(c, d->c->wx + d->margin, d->c->wy + d->margin + pos * 2,
                buffer_size - 1, 1);

    UIDialogElement_int_init(e, c, buffer, buffer_size);

    return 0;
}

int UIDialog_add_label(UIDialog *d, int pos, wchar_t *text) {
    UIDialogElement *e = d->inputs + pos;
    Canvas *c = malloc(sizeof(Canvas));

    Canvas_init(c, d->c->wx + d->margin, d->c->wy + d->margin + pos * 2,
                d->c->width - 2 * d->margin, 1);
    UIDialogElement_label_init(e, c, text);

    return 0;
}

int UIDialog_remove_button(UIDialog *d, int pos) {
    if (d->inputs[pos].type != DIALOG_INPUT_BUTTON) {
        return -1;
    }

    free(d->inputs[pos].c);

    d->inputs[pos].c = NULL;
    d->inputs[pos].type = DIALOG_NONE;

    return 0;
}

int UIDialog_remove_label(UIDialog *d, int pos) {
    if (d->inputs[pos].type != DIALOG_LABEL) {
        return -1;
    }

    free(d->inputs[pos].c);

    d->inputs[pos].c = NULL;
    d->inputs[pos].type = DIALOG_NONE;

    return 0;
}

int UIDialog_remove_text(UIDialog *d, int pos) {
    if (d->inputs[pos].type != DIALOG_INPUT_TEXT) {
        return -1;
    }

    free(d->inputs[pos].c);
    free(d->inputs[pos].buffer);

    d->inputs[pos].c = NULL;
    d->inputs[pos].buffer = NULL;
    d->inputs[pos].type = DIALOG_NONE;

    return 0;
}

int UIDialog_remove_integer(UIDialog *d, int pos) {
    if (d->inputs[pos].type != DIALOG_INPUT_INTEGER) {
        return -1;
    }

    free(d->inputs[pos].c);
    free(d->inputs[pos].buffer);

    d->inputs[pos].c = NULL;
    d->inputs[pos].buffer = NULL;
    d->inputs[pos].type = DIALOG_NONE;

    return 0;
}

void UIDialog_close(UIDialog *d) { d->is_open = 0; }

void UIDialog_open(UIDialog *d) {
    UIDialog_reset(d);
    d->is_open = 1;
}

int UIDialog_move(UIDialog *d, Direction dir) {
    int i;

    /* iterate to find the nearest suitable position
     */
    if (dir == DIR_NORTH) {
        for (i = d->pos - 1; i >= 0; i--) {
            if (d->inputs[i].type != DIALOG_LABEL) {
                d->pos = i;
                return 0;
            }
        }
    }

    if (dir == DIR_SOUTH) {
        for (i = d->pos + 1; i < d->inputs_count; i++) {
            if (d->inputs[i].type != DIALOG_LABEL) {
                d->pos = i;
                return 0;
            }
        }
    }

    return -1;
}

int UIDialog_select(UIDialog *d) {
    UIDialogElement *i = d->inputs + d->pos;

    if (i->type == DIALOG_INPUT_TEXT) {
        Canvas_read(i->c, 0, 0, i->buffer, i->buffer_size - 1,
                    CANVAS_WHITELIST_ANY);
    }

    else if (i->type == DIALOG_INPUT_INTEGER) {
        Canvas_read(i->c, 0, 0, i->buffer, i->buffer_size - 1,
                    CANVAS_WHITELIST_NUMBERS);
    }

    return d->pos;
}

void UIDialog_border(UIDialog *d) {
    int x, y;

    /* Write appriopriate symbol for given coordinates
     */
    for (x = 0; x < d->c->width; x++) {
        for (y = 0; y < d->c->height; y++) {
            if (x == 0 && y == 0) {
                Canvas_write(d->c, x, y, UI_CHARSET_CTL);
            } else if (x == d->c->width - 1 && y == 0) {
                Canvas_write(d->c, x, y, UI_CHARSET_CTR);
            } else if (x == 0 && y == d->c->height - 1) {
                Canvas_write(d->c, x, y, UI_CHARSET_CBL);
            } else if (x == d->c->width - 1 && y == d->c->height - 1) {
                Canvas_write(d->c, x, y, UI_CHARSET_CBR);
            } else if (x == 0) {
                Canvas_write(d->c, x, y, UI_CHARSET_LV);
            } else if (x == d->c->width - 1) {
                Canvas_write(d->c, x, y, UI_CHARSET_LV);
            } else if (y == 0) {
                Canvas_write(d->c, x, y, UI_CHARSET_LH);
            } else if (y == d->c->height - 1) {
                Canvas_write(d->c, x, y, UI_CHARSET_LH);
            }
        }
    }
}

int UIDialog_all(UIDialog *d) {
    int i;

    Canvas_set_attr(d->c, UI_STYLE_DIALOG);
    Canvas_fill(d->c, L' ');
    Canvas_reset_attr(d->c);

    if (!d->is_open) {
        return 0;
    }

    UIDialog_border(d);

    for (i = 0; i < d->inputs_count; i++) {
        UIDialogElement_all(d->inputs + i, i == d->pos);
    }

    return 0;
}

int UIDialog_handle(UIDialog *ud) {
    int ch;

    if (!ud->is_open) {
        return -1;
    }

    while (1) {
        UIDialog_all(ud);

        ch = getch();
        if (ch == KEY_UP) {
            UIDialog_move(ud, DIR_NORTH);
        }

        else if (ch == KEY_DOWN) {
            UIDialog_move(ud, DIR_SOUTH);
        }

        else if (ch == CANVAS_KEY_ENTER) {
            return UIDialog_select(ud);
        }
    }

    return 0;
}