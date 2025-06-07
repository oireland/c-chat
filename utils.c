#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handle_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

// Helper to remove trailing newline from fgets
void trim_newline(char *str) {
    char *newline = strchr(str, '\n');
    if (newline) {
        *newline = '\0';
    }
}