#ifndef UIDIALOG_H_INCLUDED
#define UIDIALOG_H_INCLUDED

#include <wchar.h>

#include "../board.h"
#include "../canvas.h"

#define UI_CHARSET_CTL   L"┌"
#define UI_CHARSET_CTR   L"┐"
#define UI_CHARSET_CBL   L"└"
#define UI_CHARSET_CBR   L"┘"
#define UI_CHARSET_TL    L"├"
#define UI_CHARSET_TR    L"┤"
#define UI_CHARSET_TT    L"┬"
#define UI_CHARSET_TB    L"┴"
#define UI_CHARSET_CROSS L"┼"
#define UI_CHARSET_LH    L"─"
#define UI_CHARSET_LV    L"│"

#define UI_TEXT_NUMPROMPT L"#"
#define UI_TEXT_TXTPROMPT L"_"

#define UI_STYLE_DIALOG COLOR_PAIR(0)

#define UI_MARGIN 2

typedef enum {
    DIALOG_NONE,
    DIALOG_INPUT_TEXT,
    DIALOG_INPUT_INTEGER,
    DIALOG_INPUT_BUTTON,
    DIALOG_LABEL
} UIDialogElementType;

typedef struct {
    UIDialogElementType type;
    wchar_t *text;
    Canvas *c;
    void *buffer;
    int buffer_size;
} UIDialogElement;

typedef struct {
    Canvas *c;
    UIDialogElement *inputs;
    int inputs_count;
    int pos;
    char is_open;
    int margin;
    int initial_pos;
} UIDialog;

int UIDialogElement_text_init(UIDialogElement *i, Canvas *c, wchar_t *buffer,
                              int buffer_size);

int UIDialogElement_int_init(UIDialogElement *i, Canvas *c, wchar_t *buffer,
                             int buffer_size);

int UIDialogElement_button_init(UIDialogElement *i, Canvas *c, wchar_t *text);

int UIDialogElement_label_init(UIDialogElement *i, Canvas *c, wchar_t *text);

int UIDialogElement_read_text(UIDialogElement *i, wchar_t **out);

int UIDialogElement_read_int(UIDialogElement *i, int *out);

int UIDialogElement_all(UIDialogElement *i, int selected);

int UIDialogElement_reset_buffer(UIDialogElement *i);

void UIDialog_init(UIDialog *d, Canvas *c, int inputs_count, int initial_pos);

void UIDialog_destroy(UIDialog *d);

int UIDialog_reset(UIDialog *d);

void UIDialog_close(UIDialog *d);

void UIDialog_open(UIDialog *d);

int UIDialog_move(UIDialog *d, Direction dir);

int UIDialog_all(UIDialog *d);

int UIDialog_select(UIDialog *d);

int UIDialog_add_button(UIDialog *d, int pos, wchar_t *text);

int UIDialog_add_text(UIDialog *d, int pos);

int UIDialog_add_integer(UIDialog *d, int pos);

int UIDialog_add_label(UIDialog *d, int pos, wchar_t *text);

int UIDialog_remove_button(UIDialog *d, int pos);

int UIDialog_remove_text(UIDialog *d, int pos);

int UIDialog_remove_label(UIDialog *d, int pos);

int UIDialog_remove_integer(UIDialog *d, int pos);

int UIDialog_handle(UIDialog *ud);

#endif