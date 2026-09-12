#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

void utils_clear_screen(void) {
    // Standard VT100 clear screen and reset cursor
    printf("\033[2J\033[H");
}

void utils_get_timestamp(char *buffer, size_t max_len) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    if (tm_info && buffer && max_len > 0) {
        strftime(buffer, max_len, "%Y-%m-%d %H:%M:%S", tm_info);
    }
}

void utils_trim(char *str) {
    if (!str) return;
    
    // Trim trailing whitespace and newlines
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || isspace((unsigned char)str[len - 1]))) {
        str[--len] = '\0';
    }

    // Trim leading whitespace
    size_t start = 0;
    while (str[start] != '\0' && isspace((unsigned char)str[start])) {
        start++;
    }
    if (start > 0) {
        memmove(str, str + start, len - start + 1);
    }
}

int utils_strcasecmp(const char *s1, const char *s2) {
    if (!s1 || !s2) return -1;
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

bool utils_read_string(const char *prompt, char *buffer, size_t max_len) {
    if (prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }
    if (!fgets(buffer, (int)max_len, stdin)) {
        return false;
    }
    utils_trim(buffer);
    return true;
}

int utils_read_int(const char *prompt, int min_val, int max_val) {
    char input[64];
    int value = 0;
    while (1) {
        if (!utils_read_string(prompt, input, sizeof(input))) {
            return min_val;
        }
        if (strlen(input) == 0) continue;
        char *endptr;
        long val = strtol(input, &endptr, 10);
        if (endptr != input && *endptr == '\0' && val >= min_val && val <= max_val) {
            value = (int)val;
            break;
        }
        printf(ANSI_RED "Invalid input. Please enter an integer between %d and %d: " ANSI_RESET, min_val, max_val);
    }
    return value;
}

double utils_read_double(const char *prompt, double min_val, double max_val) {
    char input[64];
    double value = 0.0;
    while (1) {
        if (!utils_read_string(prompt, input, sizeof(input))) {
            return min_val;
        }
        if (strlen(input) == 0) continue;
        char *endptr;
        double val = strtod(input, &endptr);
        if (endptr != input && *endptr == '\0' && val >= min_val && val <= max_val) {
            value = val;
            break;
        }
        printf(ANSI_RED "Invalid input. Please enter a number between %.2f and %.2f: " ANSI_RESET, min_val, max_val);
    }
    return value;
}

void utils_format_currency(double amount, char *buffer, size_t max_len) {
    if (buffer && max_len > 0) {
        snprintf(buffer, max_len, "$%.2f", amount);
    }
}

void utils_print_header(const char *title) {
    printf(ANSI_BOLD ANSI_CYAN "\n=======================================================\n" ANSI_RESET);
    printf(ANSI_BOLD ANSI_YELLOW "  COFFEE SHOP SYSTEM - %s\n" ANSI_RESET, title);
    printf(ANSI_BOLD ANSI_CYAN "=======================================================\n" ANSI_RESET);
}

void utils_pause(void) {
    printf(ANSI_WHITE "\nPress [ENTER] to continue..." ANSI_RESET);
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}
