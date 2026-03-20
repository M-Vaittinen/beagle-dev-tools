/* miniline.h - Minimal readline-compatible line editor (no external deps)
 *
 * Implements the subset of GNU readline used by bbb-pinctrl:
 *   miniline(prompt)    - display prompt, read a line (caller must free())
 *   ml_add_history(s)  - add string to history
 *   ml_init_history()  - initialise history (no-op, provided for symmetry)
 *
 * Features: printable input, backspace/delete, left/right arrows,
 *           up/down history, Ctrl-A/E (home/end), Ctrl-U (clear),
 *           Ctrl-D (EOF), Ctrl-C (cancel line), Ctrl-L (redraw).
 *
 * Falls back to plain fgets when stdin is not a terminal (e.g. piped input).
 */

#ifndef MINILINE_H
#define MINILINE_H

/* Read a line of input, displaying `prompt`.
 * Returns a malloc'd string (caller must free), or NULL on EOF/error. */
char *miniline(const char *prompt);

/* Add `line` to the history ring. */
void ml_add_history(const char *line);

/* Initialise history (currently a no-op). */
void ml_init_history(void);

#endif /* MINILINE_H */
