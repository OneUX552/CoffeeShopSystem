#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdbool.h>

// ANSI Terminal Colors
#define ANSI_RESET   "\x1b[0m"
#define ANSI_BOLD    "\x1b[1m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_WHITE   "\x1b[37m"

void utils_clear_screen(void);
void utils_get_timestamp(char *buffer, size_t max_len);
void utils_trim(char *str);
int utils_strcasecmp(const char *s1, const char *s2);
bool utils_read_string(const char *prompt, char *buffer, size_t max_len);
int utils_read_int(const char *prompt, int min_val, int max_val);
double utils_read_double(const char *prompt, double min_val, double max_val);
void utils_format_currency(double amount, char *buffer, size_t max_len);
void utils_print_header(const char *title);
void utils_pause(void);

#endif // UTILS_H
