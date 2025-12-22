// -*- compile-command: "gcc -std=c99 -Wall -Wextra -O2 -o cpu_ram_gpu_vram cpu_ram_gpu_vram.c && ./cpu_ram_gpu_vram" -*-
/*=====================================================================
 * cpu_ram_gpu_vram.c
 *
 * Re‑implementation of the Python function
 *   cpu_ram_gpu_vram() -> "|"+render_braille_progress(... )+"|"
 *
 * It prints a Braille progress bar that encodes
 *   CPU user %, RAM used %, GPU utilisation %, GPU memory used %
 *
 * Build:
 *     gcc -std=c99 -Wall -Wextra -O2 -o cpu_ram_gpu_vram cpu_ram_gpu_vram.c
 *
 * Run (example):
 *     ./cpu_ram_gpu_vram
 *     |⣿⣟⣛⣛⣉⣉⣁⣀⣀|
 *
 * (c) 2025 – public domain / MIT‑style
 *====================================================================*/

#define _POSIX_C_SOURCE 2        /* for strdup() on older glibc */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>

/*--------------------------------------------------------------
 * Helper: round a double to the nearest multiple of 5 (as int)
 *--------------------------------------------------------------*/
static int round_to_nearest_5(int n)
{
    return 5*((n+2)/5);
}

/*-----------------------------------------------------------------
 * Braille renderer – identical to the Python version.
 *
 * Returns a newly allocated string (caller must free()).
 *-----------------------------------------------------------------*/
static char *render_braille_progress(int p1, int p2, int p3, int p4)
{
    char *out = calloc(10 * 4 + 1, 1);      /* up to 4 bytes per UTF‑8 char */
    if (!out) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    const int prog[4] = {p1, p2, p3, p4};
    size_t pos = 0;

    for (int col = 0; col < 10; ++col) {
        int left_limit  = col * 10;
        int right_limit = (col + 1) * 10;

        unsigned char dots = 0;

        if (prog[0] > left_limit)  dots |= 0b00000001;   /* dot 1 */
        if (prog[1] > left_limit)  dots |= 0b00000010;   /* dot 2 */
        if (prog[2] > left_limit)  dots |= 0b00000100;   /* dot 3 */

        if (prog[0] >= right_limit) dots |= 0b00001000;   /* dot 4 */
        if (prog[1] >= right_limit) dots |= 0b00010000;   /* dot 5 */
        if (prog[2] >= right_limit) dots |= 0b00100000;   /* dot 6 */

        if (prog[3] > left_limit)  dots |= 0b01000000;   /* dot 7 */
        if (prog[3] >= right_limit) dots |= 0b10000000;   /* dot 8 */

        /* Exact boundary → fill both left and right dots */
        for (int i = 0; i < 4; ++i) {
            if (prog[i] == right_limit) {
                switch (i) {
                    case 0: dots |= 0b00000001 | 0b00001000; break;
                    case 1: dots |= 0b00000010 | 0b00010000; break;
                    case 2: dots |= 0b00000100 | 0b00100000; break;
                    case 3: dots |= 0b01000000 | 0b10000000; break;
                }
            }
        }

        unsigned int code = 0x2800 + dots;        /* Braille base point */

        /* Encode UTF‑8 (max 4 bytes) */
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
    return out;
}

/*--------------------------------------------------------------
 * CPU – user % (derived from /proc/stat)
 *--------------------------------------------------------------*/
static double get_cpu_user_percent(void)
{
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) {
        perror("open /proc/stat");
        return 0.0;
    }

    char line[256];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return 0.0;
    }
    fclose(fp);

    /* Expected format: cpu  12345 678 910 ...  */
    unsigned long long user, nice, system, idle, iowait,
                        irq, softirq, steal, guest, guest_nice;
    int fields = sscanf(line,
                        "cpu  %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                        &user, &nice, &system, &idle, &iowait,
                        &irq, &softirq, &steal, &guest, &guest_nice);
    if (fields < 4)   /* at least user, nice, system, idle */
        return 0.0;

    unsigned long long total = user + nice + system + idle + iowait +
                              irq + softirq + steal + guest + guest_nice;
    if (total == 0) return 0.0;

    double user_percent = 100.0 * (user + nice) / (double)total;
    return user_percent;
}

/*--------------------------------------------------------------
 * RAM – used % (from /proc/meminfo)
 *--------------------------------------------------------------*/
static double get_ram_used_percent(void)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        perror("open /proc/meminfo");
        return 0.0;
    }

    unsigned long long mem_total = 0, mem_available = 0;
    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "MemTotal: %llu kB", &mem_total) == 1)
            continue;
        if (sscanf(line, "MemAvailable: %llu kB", &mem_available) == 1)
            continue;
    }
    fclose(fp);

    if (mem_total == 0) return 0.0;

    unsigned long long mem_used = mem_total - mem_available;
    double percent = 100.0 * mem_used / (double)mem_total;
    return percent;
}

/*--------------------------------------------------------------
 * GPU – utilisation % and memory used %.
 * Uses: nvidia-smi --query-gpu=utilization.gpu,memory.used,memory.total
 *       --format=csv,noheader,nounits
 * If nvidia‑smi cannot be executed, both values are reported as 0.
 *--------------------------------------------------------------*/
static void get_gpu_stats(double *out_util, double *out_mem_percent)
{
    *out_util = 0.0;
    *out_mem_percent = 0.0;

    /* The command works with NVIDIA drivers >= 340 */
    const char *cmd = "nvidia-smi --query-gpu=utilization.gpu,memory.used,memory.total "
                      "--format=csv,noheader,nounits 2>/dev/null";

    FILE *pipe = popen(cmd, "r");
    if (!pipe)
        return;   /* probably no driver installed */

    char buf[256];
    if (fgets(buf, sizeof(buf), pipe)) {
        /* Expected line: " 12, 1023, 4096" (optional spaces) */
        double util, mem_used, mem_total;
        if (sscanf(buf, " %lf , %lf , %lf", &util, &mem_used, &mem_total) == 3) {
            *out_util = util;
            if (mem_total > 0.0)
                *out_mem_percent = 100.0 * mem_used / mem_total;
        }
    }
    pclose(pipe);
}

/*--------------------------------------------------------------
 * main – gather stats, render a bar, print it surrounded by '|'
 *--------------------------------------------------------------*/
int main(void)
{
    double cpu_user   = get_cpu_user_percent();      /* 0‑100 */
    double ram_used   = get_ram_used_percent();      /* 0‑100 */

    double gpu_util, gpu_mem_percent;
    get_gpu_stats(&gpu_util, &gpu_mem_percent);     /* 0‑100 each */

    /* Round to nearest 5, exactly like the Python helper */
    int p_cpu = round_to_nearest_5(cpu_user);
    int p_ram = round_to_nearest_5(ram_used);
    int p_gpu = round_to_nearest_5(gpu_util);
    int p_mem = round_to_nearest_5(gpu_mem_percent);

    char *bar = render_braille_progress(p_cpu, p_ram, p_gpu, p_mem);
    printf("|%s|\n", bar);
    free(bar);
    return EXIT_SUCCESS;
}
