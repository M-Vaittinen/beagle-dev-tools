/* miniline.c - Minimal line editor with history (no external dependencies)
 *
 * Supports UTF-8 multibyte input (e.g. Scandinavian Ä, ä, Ö, ö …).
 * The buffer stores raw UTF-8 bytes; cursor movement and display are in
 * display-column units (each BMP code-point = 1 column).
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

#define ML_MAX_LINE  512   /* max bytes per input line               */
#define ML_HIST_SIZE  64   /* number of history entries to keep      */

/* ------------------------------------------------------------------ */
/* UTF-8 helpers                                                       */
/* ------------------------------------------------------------------ */

/* Number of bytes in the UTF-8 char whose leading byte is `c`. */
static int utf8_seq_len(unsigned char c)
{
    if (c < 0x80) return 1;
    if (c < 0xC0) return 1;  /* unexpected continuation byte — treat as 1 */
    if (c < 0xE0) return 2;
    if (c < 0xF0) return 3;
    return 4;
}

/* Display-column width of `nbytes` bytes of UTF-8 in `s`.
 * Assumes every code-point is 1 column wide (valid for Latin/BMP text). */
static int utf8_disp_width(const char *s, int nbytes)
{
    int cols = 0, i = 0;
    while (i < nbytes) {
        unsigned char c = (unsigned char)s[i];
        int slen = utf8_seq_len(c);
        /* Skip if sequence would exceed buffer */
        if (i + slen > nbytes) break;
        i += slen;
        cols++;
    }
    return cols;
}

/* Byte length of the UTF-8 character ending at byte index `pos`
 * (i.e. the character whose last byte is buf[pos-1]).
 * Used for backspace. */
static int utf8_prev_char_len(const char *buf, int pos)
{
    int len = 0;
    while (pos > 0) {
        pos--;
        len++;
        /* A UTF-8 sequence start byte is either < 0x80 (ASCII) or >= 0xC0 */
        if ((unsigned char)buf[pos] < 0x80 || (unsigned char)buf[pos] >= 0xC0)
            break;
    }
    return len;
}

/* ------------------------------------------------------------------ */
/* History ring buffer                                                 */
/* ------------------------------------------------------------------ */

static char  *g_hist[ML_HIST_SIZE];
static int    g_hist_count = 0;

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

static const char *hist_get(int idx)
{
    int oldest;
    if (g_hist_count == 0 || idx < 0 || idx >= g_hist_count)
        return NULL;
    oldest = (g_hist_count > ML_HIST_SIZE) ? g_hist_count - ML_HIST_SIZE : 0;
    if (idx < oldest)
        return NULL;
    return g_hist[idx % ML_HIST_SIZE];
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
/* Low-level output helpers (bypass stdio buffering)                  */
/* ------------------------------------------------------------------ */

static void out(const char *s, int len)  { (void)write(STDOUT_FILENO, s, (size_t)len); }
static void outs(const char *s)          { out(s, (int)strlen(s)); }

static void cursor_left(int n)
{
    char buf[16];
    if (n <= 0) return;
    if (n == 1) { out("\x1b[D", 3); return; }
    out(buf, snprintf(buf, sizeof(buf), "\x1b[%dD", n));
}

static void cursor_right(int n)
{
    char buf[16];
    if (n <= 0) return;
    if (n == 1) { out("\x1b[C", 3); return; }
    out(buf, snprintf(buf, sizeof(buf), "\x1b[%dC", n));
}

/* Erase from cursor to end-of-line, rewrite buf[pos..len], reposition. */
static void redraw_from(const char *buf, int len, int pos)
{
    int trail = len - pos;
    int trail_cols = (trail > 0) ? utf8_disp_width(buf + pos, trail) : 0;
    outs("\x1b[K");
    if (trail > 0)
        out(buf + pos, trail);
    if (trail_cols > 0)
        cursor_left(trail_cols);
}

/* Move to column 0, erase line, reprint prompt + buf, place cursor at pos. */
static void refresh_line(const char *prompt, const char *buf, int len, int pos)
{
    int disp_after = utf8_disp_width(buf + pos, len - pos);
    out("\r", 1);
    outs("\x1b[K");
    outs(prompt);
    out(buf, len);
    if (disp_after > 0)
        cursor_left(disp_after);
}

/* ------------------------------------------------------------------ */
/* Key reading — returns action code; mb[]/mblen carry printable chars */
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
#define KEY_CTRL_A    11
#define KEY_CTRL_E    12
#define KEY_CTRL_U    13
#define KEY_CTRL_K    14
#define KEY_CTRL_D    15
#define KEY_CTRL_C    16
#define KEY_CTRL_L    17
#define KEY_UNKNOWN    0

/* mb must point to a buffer of at least 5 bytes. */
static int read_key(char *mb, int *mblen)
{
    unsigned char c;
    int slen, i;

    *mblen = 0;
    if (read(STDIN_FILENO, &c, 1) <= 0)
        return KEY_CTRL_D;

    switch (c) {
    case '\r': case '\n':  return KEY_ENTER;
    case 127:  case '\b':  return KEY_BACKSPACE;
    case 1:                return KEY_CTRL_A;
    case 5:                return KEY_CTRL_E;
    case 4:                return KEY_CTRL_D;
    case 3:                return KEY_CTRL_C;
    case 11:               return KEY_CTRL_K;
    case 12:               return KEY_CTRL_L;
    case 21:               return KEY_CTRL_U;
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
            case '1': case '7': read(STDIN_FILENO, &seq[2], 1); return KEY_HOME;
            case '4': case '8': read(STDIN_FILENO, &seq[2], 1); return KEY_END;
            case '3': read(STDIN_FILENO, &seq[2], 1); return KEY_DELETE;
            default:  return KEY_UNKNOWN;
            }
        }
        return KEY_UNKNOWN;
    }
    default:
        /* ASCII printable */
        if (c >= 32 && c < 127) {
            mb[0] = (char)c; mb[1] = '\0'; *mblen = 1;
            return KEY_PRINTABLE;
        }
        /* UTF-8 multi-byte sequence: leading byte >= 0xC0 */
        if (c >= 0xC0) {
            slen = utf8_seq_len(c);
            mb[0] = (char)c;
            for (i = 1; i < slen; i++) {
                unsigned char cont;
                if (read(STDIN_FILENO, &cont, 1) <= 0) return KEY_UNKNOWN;
                /* Validate continuation byte (should be 0x80–0xBF) */
                if ((cont & 0xC0) != 0x80) return KEY_UNKNOWN;
                mb[i] = (char)cont;
            }
            mb[slen] = '\0'; *mblen = slen;
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
    int     len = 0;
    int     pos = 0;
    int     hist_pos;
    char    saved[ML_MAX_LINE];
    char    mb[5];
    int     mblen;

    if (!prompt) prompt = "";
    memset(buf,   0, sizeof(buf));
    memset(saved, 0, sizeof(saved));
    hist_pos = g_hist_count;

    /* Non-interactive: fall back to fgets */
    if (!isatty(STDIN_FILENO)) {
        char *line;
        outs(prompt);
        fflush(stdout);
        line = malloc(ML_MAX_LINE);
        if (!line) return NULL;
        if (!fgets(line, ML_MAX_LINE, stdin)) { free(line); return NULL; }
        { int n = (int)strlen(line); if (n > 0 && line[n-1] == '\n') line[n-1] = '\0'; }
        return line;
    }

    if (raw_on() < 0) {
        char *line = malloc(ML_MAX_LINE);
        if (!line) return NULL;
        outs(prompt);
        fflush(stdout);
        if (!fgets(line, ML_MAX_LINE, stdin)) { free(line); return NULL; }
        { int n = (int)strlen(line); if (n > 0 && line[n-1] == '\n') line[n-1] = '\0'; }
        return line;
    }

    outs(prompt);

    for (;;) {
        int key = read_key(mb, &mblen);

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
            if (len == 0) { raw_off(); out("\r\n", 2); return NULL; }
            /* fall through: delete char under cursor */
            goto do_delete;

        case KEY_CTRL_C:
            raw_off(); out("\r\n", 2);
            { char *r = malloc(1); if (r) *r = '\0'; return r; }

        case KEY_CTRL_L:
            refresh_line(prompt, buf, len, pos);
            break;

        case KEY_CTRL_A: case KEY_HOME:
            if (pos > 0) {
                cursor_left(utf8_disp_width(buf, pos));
                pos = 0;
            }
            break;

        case KEY_CTRL_E: case KEY_END:
            if (pos < len) {
                cursor_right(utf8_disp_width(buf + pos, len - pos));
                pos = len;
            }
            break;

        case KEY_CTRL_U:
            if (pos > 0) {
                memmove(buf, buf + pos, (size_t)(len - pos));
                len -= pos;
                buf[len] = '\0';
                pos = 0;
                refresh_line(prompt, buf, len, pos);
            }
            break;

        case KEY_CTRL_K:
            if (pos < len) { len = pos; buf[len] = '\0'; outs("\x1b[K"); }
            break;

        case KEY_LEFT:
            if (pos > 0) {
                int clen = utf8_prev_char_len(buf, pos);
                pos -= clen;
                cursor_left(1);
            }
            break;

        case KEY_RIGHT:
            if (pos < len) {
                int clen = utf8_seq_len((unsigned char)buf[pos]);
                cursor_right(1);
                pos += clen;
            }
            break;

        case KEY_UP: {
            int new_pos = hist_pos - 1;
            const char *he = hist_get(new_pos);
            if (!he) break;
            if (hist_pos == g_hist_count)
                memcpy(saved, buf, (size_t)len + 1);
            hist_pos = new_pos;
            strncpy(buf, he, ML_MAX_LINE - 1);
            buf[ML_MAX_LINE - 1] = '\0';
            len = pos = (int)strlen(buf);
            refresh_line(prompt, buf, len, pos);
            break;
        }

        case KEY_DOWN: {
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
                int clen = utf8_prev_char_len(buf, pos);
                memmove(buf + pos - clen, buf + pos, (size_t)(len - pos));
                len -= clen;
                pos -= clen;
                buf[len] = '\0';
                cursor_left(1);
                redraw_from(buf, len, pos);
            }
            break;

        case KEY_DELETE:
        do_delete:
            if (pos < len) {
                int clen = utf8_seq_len((unsigned char)buf[pos]);
                memmove(buf + pos, buf + pos + clen, (size_t)(len - pos - clen));
                len -= clen;
                buf[len] = '\0';
                redraw_from(buf, len, pos);
            }
            break;

        case KEY_PRINTABLE:
            if (len + mblen < ML_MAX_LINE) {
                if (pos < len)
                    memmove(buf + pos + mblen, buf + pos, (size_t)(len - pos));
                memcpy(buf + pos, mb, (size_t)mblen);
                len += mblen;
                pos += mblen;
                buf[len] = '\0';
                if (pos == len)
                    out(mb, mblen);        /* inserting at end: just echo */
                else
                    redraw_from(buf, len, pos);
            }
            break;

        case KEY_UNKNOWN:
        default:
            break;
        }
    }
}
