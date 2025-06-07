#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Shared constants
#define MAX_MSG_LEN 256
#define MAX_CLIENTS 2
#define MAX_USERNAME_LEN 32

/**
 * @brief Handles a fatal system call error by printing a message and exiting.
 * * @param msg The message to print before the system error.
 */
void handle_error(const char *msg);

/**
 * @brief Removes a trailing newline character from a string, if present.
 * * @param str The string to trim.
 */
void trim_newline(char *str);

#endif // UTILS_H
