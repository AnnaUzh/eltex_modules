#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>

int check_rights(char rights[]);
void print_rights(char rights[]);
uint32_t convert_to_bin(char rights[]);
void print_binary(uint32_t num);
void mode_to_string(uint32_t mode, char *result);
uint32_t change_permissions(uint32_t current_perms, const char* modification);
