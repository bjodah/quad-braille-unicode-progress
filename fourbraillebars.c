/*=====================================================================
 * fourbraillebars.c
 *
 * Re‑implementation of the Python “fourbraillebars” package in pure C.
 * Courtesy of gpt-oss-120b.
 *
 *   $ ./fourbraillebars 75 50 25 100
 *   ⣿⣿⣟⣛⣛⣉⣉⣁⣀⣀
 *
 * Compile with:
 *      gcc -std=c99 -Wall -Wextra -O2 -o fourbraillebars fourbraillebars.c
 *
 * (c) 2025 – public domain / MIT‑style
 *====================================================================*/

#define _POSIX_C_SOURCE 2   /* for strdup() on older glibc */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

/*--------------------------------------------------------------
 * Helper: round a double to the nearest multiple of 5 (as int)
 *--------------------------------------------------------------*/
static int round_to_nearest_5(double n)
{
    return (int)(5.0 * round(n / 5.0));
}

/*--------------------------------------------------------------
 * Convert four percentages (0‑100) into a Unicode Braille string.
 *
 * The algorithm is a literal translation of the Python version:
 *   * 10 columns → each column = 10 %
 *   * left‑dot = “progress > column‑start”
 *   * right‑dot = “progress ≥ column‑end”
 *   * a progress exactly on a boundary fills BOTH dots.
 *
 * The resulting Unicode characters are formed by adding the
 * dot‑bit‑mask to 0x2800 (Braille Pattern Blank).
 *--------------------------------------------------------------*/
static void render_braille_progress(char *out, int p1, int p2, int p3, int p4)
{
    /* allocate space for 10 Unicode characters + terminating NUL */
    const int prog[4] = {p1, p2, p3, p4};
    size_t pos = 0;      /* write position inside out[] */

    for (int col = 0; col < 10; ++col) {
        /* column limits (in %):  [col*10, (col+1)*10]  */
        int left_limit  = col * 10;
        int right_limit = (col + 1) * 10;

        /* decide which of the 8 dots are set */
        unsigned char dots = 0;   /* 8‑bit mask, LSB = dot 1 */

        /* left‑hand column – dots 1,2,3 (progresses 1‑3) */
        if (prog[0] > left_limit)  dots |= 0b00000001;   /* dot 1 */
        if (prog[1] > left_limit)  dots |= 0b00000010;   /* dot 2 */
        if (prog[2] > left_limit)  dots |= 0b00000100;   /* dot 3 */

        /* right‑hand column – dots 4,5,6 (progresses 1‑3) */
        if (prog[0] >= right_limit) dots |= 0b00001000;   /* dot 4 */
        if (prog[1] >= right_limit) dots |= 0b00010000;   /* dot 5 */
        if (prog[2] >= right_limit) dots |= 0b00100000;   /* dot 6 */

        /* fourth progress bar – dots 7,8 (left/right of column) */
        if (prog[3] > left_limit)  dots |= 0b01000000;   /* dot 7 */
        if (prog[3] >= right_limit) dots |= 0b10000000;   /* dot 8 */

        /* Special‑case: exact boundary → fill BOTH left & right dots */
        for (int i = 0; i < 4; ++i) {
            if (prog[i] == right_limit) {
                switch (i) {
                    case 0: dots |= 0b00000001 | 0b00001000; break;   /* dot 1 & 4 */
                    case 1: dots |= 0b00000010 | 0b00010000; break;   /* dot 2 & 5 */
                    case 2: dots |= 0b00000100 | 0b00100000; break;   /* dot 3 & 6 */
                    case 3: dots |= 0b01000000 | 0b10000000; break;   /* dot 7 & 8 */
                }
            }
        }

        /* Unicode code point = 0x2800 + mask */
        unsigned int code = 0x2800 + dots;

        /* Encode code point as UTF‑8 (max 4 bytes) */
        if (code < 0x80) {
            out[pos++] = (char)code;
        } else if (code < 0x800) {
            out[pos++] = (char)(0xC0 | (code >> 6));
            out[pos++] = (char)(0x80 | (code & 0x3F));
        } else if (code < 0x10000) {
            out[pos++] = (char)(0xE0 | (code >> 12));
            out[pos++] = (char)(0x80 | ((code >> 6) & 0x3F));
            out[pos++] = (char)(0x80 | (code & 0x3F));
        } else {
            out[pos++] = (char)(0xF0 | (code >> 18));
            out[pos++] = (char)(0x80 | ((code >> 12) & 0x3F));
            out[pos++] = (char)(0x80 | ((code >> 6) & 0x3F));
            out[pos++] = (char)(0x80 | (code & 0x3F));
        }
    }

    out[pos] = '\0';
}

/*--------------------------------------------------------------
 * tiny usage message
 *--------------------------------------------------------------*/
static void print_usage(const char *progname)
{
    fprintf(stderr,
            "Usage: %s <p1> <p2> <p3> <p4>\n"
            "  Each <p?> is a percent value (0‑100). The program rounds\n"
            "  them to the nearest 5 %% and prints a 10‑column Braille bar.\n",
            progname);
}

/*--------------------------------------------------------------
 * Main – parse arguments, render, print.
 *--------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    if (argc != 5) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    int vals[4];
    for (int i = 0; i < 4; ++i) {
        char *endptr = NULL;
        long v = strtol(argv[i + 1], &endptr, 10);
        if (endptr == argv[i + 1] || *endptr != '\0' || errno == ERANGE) {
            fprintf(stderr, "Invalid integer: %s\n", argv[i + 1]);
            return EXIT_FAILURE;
        }
        if (v < 0) v = 0;
        if (v > 100) v = 100;
        vals[i] = round_to_nearest_5((double)v);
    }

    char bar[41];
    render_braille_progress(bar, vals[0], vals[1], vals[2], vals[3]);
    //printf("%s\n", bar);
    puts(bar);
    return EXIT_SUCCESS;
}
