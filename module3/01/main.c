#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// Функция для проверки, является ли строка числом
int is_number(const char *str) {
    char *endptr;
    
    strtol(str, &endptr, 10); 
    if (*endptr == '\0') {
        return 1;
    }
    
    strtod(str, &endptr);
    if (*endptr == '\0') {
        return 2;
    }
    
    return 0;
}

// Функция для обработки аргументов
void process_arguments(char *argv[], int start, int count) {
    for (int i = start; i < start + count; i++) {
        int num_type = is_number(argv[i]);
        
        if (num_type == 1) {
            long int_val = strtol(argv[i], NULL, 10);
            printf(" %ld, %ld\n", int_val, int_val * 2);
        }
        else if (num_type == 2) {
            double double_val = strtod(argv[i], NULL);
            printf("%.2f, %.2f\n", double_val, double_val * 2);
        }
        else {
            printf(" %s\n", argv[i]);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }
    
    int total_args = argc - 1;
    int parent_count = total_args / 2;
    int child_count = total_args - parent_count;
    
    pid_t pid = fork();
    if (pid < 0) return 1;
    
    if (pid == 0) {
        // Дочерний процесс
        printf("=== Дочерний процесс (PID: %d) ===\n", getpid());
        process_arguments(argv, 1 + parent_count, child_count);
    }
    else {
        // Родительский процесс
        printf("=== Родительский процесс (PID: %d) ===\n", getpid());
        process_arguments(argv, 1, parent_count);
        wait(NULL);
    }
    
    return 0;
}