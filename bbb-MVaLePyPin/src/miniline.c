/* miniline.c - Minimal line editor with history (no external dependencies)
 *
 * Only POSIX headers are used: termios, unistd, stdio, stdlib, string.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include "miniline.h"

/* ------------------------------------------------------------------ */
/* Configuration                                                       */
/* ------------------------------------------------------------------ */

#define ML_MAX_LINE  512   /* max chars per input line               */
#define ML_HIST_SIZE  64   /* number of history entries to keep      */

/* ------------------------------------------------------------------ */
/* History ring buffer                                                 */
/* ------------------------------------------------------------------ */

static char  *g_hist[ML_HIST_SIZE];
static int    g_hist_count = 0;   /* total entries ever added (may exceed ring) */

void ml_init_history(void) { /* no-op */ }

void ml_add_history(const char *line)
{
    int slot;
    if (!line || !*line)
        return;
    slot = g_hist_count % ML_HIST_SIZE;
    free(g_hist[slot]);
    g_hist[slot] = strdup(line);
    if (g_hist[slot])
        g_hist_count++;
}

/* Return history entry `idx` (0 = oldest visible, g_hist_count-1 = newest).
 * Returns NULL if out of range. */
static const char *hist_get(int idx)
{
    int oldest, slot;
    if (g_hist_count == 0 || idx < 0 || idx >= g_hist_count)
        return NULL;
    oldest = (g_hist_count > ML_HIST_SIZE)
             ? g_hist_count - ML_HIST_SIZE
             : 0;
    if (idx < oldest)
        return NULL;
    slot = idx % ML_HIST_SIZE;
    return g_hist[slot];
}

/* ------------------------------------------------------------------ */
/* Terminal raw-mode helpers                                           */
/* ------------------------------------------------------------------ */

static struct termios g_orig_termios;
static int            g_raw_active = 0;

static void raw_off(void)
{
    if (g_raw_active) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_orig_termios);
        g_raw_active = 0;
    }
}

static int raw_on(void)
{
    struct termios raw;
    if (!isatty(STDIN_FILENO))
        return -1;
    if (tcgetattr(STDIN_FILENO, &g_orig_termios) < 0)
        return -1;
    raw = g_orig_termios;
    raw.c_iflag &= (tcflag_t)~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= (tcflag_t)~(OPOST);
    raw.c_cflag |= CS8;
    raw.c_lflag &= (tcflag_t)~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0)
        return -1;
    g_raw_active = 1;
    return 0;
}

/* ------------------------------------------------------------------ */
/* Low-level output helpers (write directly, bypassing stdio buffers) */
/* ------------------------------------------------------------------ */

static void out(const char *s, int len)
{
    (void)write(STDOUT_FILENO, s, (size_t)len);
}

static void outs(const char *s) { out(s, (int)strlen(s)); }

/* Move cursor left N columns */
static void cursor_left(int n)
{
    char buf[16];
    if (n <= 0) return;
    if (n == 1) { out("\x1b[D", 3); return; }
    int l = snprintf(buf, sizeof(buf), "\x1b[%dD", n);
    out(buf, l);
}

/* Move cursor right N columns */
static void cursor_right(int n)
{
    char buf[16];
    if (n <= 0) return;
    if (n == 1) { out("\x1b[C", 3); return; }
    int l = snprintf(buf, sizeof(buf), "\x1b[%dC", n);
    out(buf, l);
}

/* Redraw the line from position `pos` onward, then reposition cursor */
static void redraw_from(const char *buf, int len, int pos, int prompt_len)
{
    /* Erase from current cursor to end of line */
    outs("\x1b[K");
    /* Write tail of buffer */
    if (pos < len)
        out(buf + pos, len - pos);
    /* Move cursor back to `pos` */
    if (len > pos)
        cursor_left(len - pos);
    (void)prompt_len;
}

/* Erase the current line and reprint prompt + buffer, cursor at `pos` */
static void refresh_line(const char *prompt, const char *buf, int len, int pos)
{
    /* \r moves to column 0; \x1b[K erases to end of line */
    out("\r", 1);
    outs("\x1b[K");
    outs(prompt);
    out(buf, len);
    /* Reposition */
    if (len > pos)
        cursor_left(len - pos);
}

/* ------------------------------------------------------------------ */
/* Read one (possibly multi-byte) keypress; decode into action        */
/* ------------------------------------------------------------------ */

#define KEY_PRINTABLE  1
#define KEY_ENTER      2
#define KEY_BACKSPACE  3
#define KEY_DELETE     4
#define KEY_LEFT       5
#define KEY_RIGHT      6
#define KEY_UP         7
#define KEY_DOWN       8
#define KEY_HOME       9
#define KEY_END       10
#define KEY_CTRL_A    11   /* home */
#define KEY_CTRL_E    12   /* end  */
#define KEY_CTRL_U    13   /* clear line */
#define KEY_CTRL_K    14   /* kill to end */
#define KEY_CTRL_D    15   /* EOF */
#define KEY_CTRL_C    16   /* cancel */
#define KEY_CTRL_L    17   /* redraw */
#define KEY_UNKNOWN    0

static int read_key(unsigned char *ch_out)
{
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) <= 0)
        return KEY_CTRL_D;

    switch (c) {
    case '\r': case '\n':   return KEY_ENTER;
    case 127:  case '\b':   return KEY_BACKSPACE;
    case 1:                 return KEY_CTRL_A;
    case 5:                 return KEY_CTRL_E;
    case 4:                 return KEY_CTRL_D;
    case 3:                 return KEY_CTRL_C;
    case 11:                return KEY_CTRL_K;
    case 12:                return KEY_CTRL_L;
    case 21:                return KEY_CTRL_U;
    case 27: {
        unsigned char seq[3] = {0, 0, 0};
        if (read(STDIN_FILENO, &seq[0], 1) <= 0) return KEY_UNKNOWN;
        if (seq[0] == '[') {
            if (read(STDIN_FILENO, &seq[1], 1) <= 0) return KEY_UNKNOWN;
            switch (seq[1]) {
            case 'A': return KEY_UP;
            case 'B': return KEY_DOWN;
            case 'C': return KEY_RIGHT;
            case 'D': return KEY_LEFT;
            case 'H': return KEY_HOME;
            case 'F': return KEY_END;
            case '1': case '7':
                read(STDIN_FILENO, &seq[2], 1);
                return KEY_HOME;
            case '4': case '8':
                read(STDIN_FILENO, &seq[2], 1);
                return KEY_END;
            case '3':
                read(STDIN_FILENO, &seq[2], 1);
                return KEY_DELETE;
            default: return KEY_UNKNOWN;
            }
        }
        return KEY_UNKNOWN;
    }
    default:
        if (c >= 32 && c < 127) {
            *ch_out = c;
            return KEY_PRINTABLE;
        }
        return KEY_UNKNOWN;
    }
}

/* ------------------------------------------------------------------ */
/* Main line-editing function                                          */
/* ------------------------------------------------------------------ */

char *miniline(const char *prompt)
{
    char    buf[ML_MAX_LINE];
    int     len = 0;   /* current length of input */
    int     pos = 0;   /* cursor position (0..len) */
    int     hist_pos;  /* index into history (-1 = new line) */
    char    saved[ML_MAX_LINE]; /* saved new line when browsing history */
    int     prompt_len;

    if (!prompt) prompt = "";
    prompt_len = (int)strlen(prompt);

    memset(buf, 0, sizeof(buf));
    memset(saved, 0, sizeof(saved));
    hist_pos = g_hist_count;  /* "one past end" = current (empty) line */

    /* Non-interactive (piped) input: fall back to fgets */
    if (!isatty(STDIN_FILENO)) {
        char *line;
        outs(prompt);
        fflush(stdout);
        line = malloc(ML_MAX_LINE);
        if (!line) return NULL;
        if (!fgets(line, ML_MAX_LINE, stdin)) {
            free(line);
            return NULL;
        }
        /* Strip trailing newline */
        {
            int n = (int)strlen(line);
            if (n > 0 && line[n-1] == '\n') line[n-1] = '\0';
        }
        return line;
    }

    if (raw_on() < 0) {
        /* Can't set raw mode; fall back to fgets */
        char *line = malloc(ML_MAX_LINE);
        if (!line) return NULL;
        outs(prompt);
        fflush(stdout);
        if (!fgets(line, ML_MAX_LINE, stdin)) { free(line); return NULL; }
        {
            int n = (int)strlen(line);
            if (n > 0 && line[n-1] == '\n') line[n-1] = '\0';
        }
        return line;
    }

    outs(prompt);

    for (;;) {
        unsigned char ch = 0;
        int key = read_key(&ch);

        switch (key) {

        case KEY_ENTER:
            raw_off();
            out("\r\n", 2);
            {
                char *result = malloc((size_t)len + 1);
                if (!result) return NULL;
                memcpy(result, buf, (size_t)len);
                result[len] = '\0';
                return result;
            }

        case KEY_CTRL_D:
            if (len == 0) {
                /* EOF on empty line */
                raw_off();
                out("\r\n", 2);
                return NULL;
            }
            /* Delete char under cursor (like Del key) */
            goto do_delete;

        case KEY_CTRL_C:
            /* Cancel: clear line, return empty string */
            raw_off();
            out("\r\n", 2);
            { char *r = malloc(1); if (r) *r = '\0'; return r; }

        case KEY_CTRL_L:
            refresh_line(prompt, buf, len, pos);
            break;

        case KEY_CTRL_A: case KEY_HOME:
            if (pos > 0) { cursor_left(pos); pos = 0; }
            break;

        case KEY_CTRL_E: case KEY_END:
            if (pos < len) { cursor_right(len - pos); pos = len; }
            break;

        case KEY_CTRL_U:
            /* Delete everything to the left of the cursor */
            if (pos > 0) {
                memmove(buf, buf + pos, (size_t)(len - pos));
                len -= pos;
                buf[len] = '\0';
                pos = 0;
                refresh_line(prompt, buf, len, pos);
            }
            break;

        case KEY_CTRL_K:
            /* Kill from cursor to end of line */
            if (pos < len) {
                len = pos;
                buf[len] = '\0';
                outs("\x1b[K");
            }
            break;

        case KEY_LEFT:
            if (pos > 0) { cursor_left(1); pos--; }
            break;

        case KEY_RIGHT:
            if (pos < len) { cursor_right(1); pos++; }
            break;

        case KEY_UP: {
            /* Go back in history */
            int new_pos = hist_pos - 1;
            const char *he = hist_get(new_pos);
            if (!he) break;
            if (hist_pos == g_hist_count)
                memcpy(saved, buf, (size_t)len + 1);  /* save current line */
            hist_pos = new_pos;
            strncpy(buf, he, ML_MAX_LINE - 1);
            buf[ML_MAX_LINE - 1] = '\0';
            len = pos = (int)strlen(buf);
            refresh_line(prompt, buf, len, pos);
            break;
        }

        case KEY_DOWN: {
            /* Go forward in history */
            if (hist_pos >= g_hist_count) break;
            hist_pos++;
            if (hist_pos == g_hist_count) {
                memcpy(buf, saved, ML_MAX_LINE);
            } else {
                const char *he = hist_get(hist_pos);
                if (!he) break;
                strncpy(buf, he, ML_MAX_LINE - 1);
                buf[ML_MAX_LINE - 1] = '\0';
            }
            len = pos = (int)strlen(buf);
            refresh_line(prompt, buf, len, pos);
            break;
        }

        case KEY_BACKSPACE:
            if (pos > 0) {
                memmove(buf + pos - 1, buf + pos, (size_t)(len - pos));
                len--;
                pos--;
                buf[len] = '\0';
                cursor_left(1);
                redraw_from(buf, len, pos, prompt_len);
            }
            break;

        case KEY_DELETE:
        do_delete:
            if (pos < len) {
                memmove(buf + pos, buf + pos + 1, (size_t)(len - pos - 1));
                len--;
                buf[len] = '\0';
                redraw_from(buf, len, pos, prompt_len);
            }
            break;

        case KEY_PRINTABLE:
            if (len < ML_MAX_LINE - 1) {
                if (pos < len)
                    memmove(buf + pos + 1, buf + pos, (size_t)(len - pos));
                buf[pos] = (char)ch;
                len++;
                pos++;
                buf[len] = '\0';
                /* Optimise: if inserting at end just echo the char */
                if (pos == len) {
                    out((char*)&ch, 1);
                } else {
                    redraw_from(buf, len, pos, prompt_len);
                }
            }
            break;

        case KEY_UNKNOWN:
        default:
            break;
        }
    }
}
