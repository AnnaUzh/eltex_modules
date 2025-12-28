#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>


int get_semaphore(const char *filename, int create_new);
char *generate_string();
void cleanup_semaphore(int sem_id);

#endif