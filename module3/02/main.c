#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_ARGUMENTS 64

// Функция для разделения строки на аргументы
int parse_input(char *input, char *args[]) {
    int argc = 0;
    char *token = strtok(input, " \t\n");
    
    while (token != NULL && argc < MAX_ARGUMENTS - 1) {
        args[argc++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[argc] = NULL;
    
    return argc;
}

void shell_loop() {
    char input[MAX_INPUT_LENGTH];
    char *args[MAX_ARGUMENTS];
    
    while (1) {
        printf("meow> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        if (strlen(input) <= 1) {
            continue;
        }
        
        int argc = parse_input(input, args);
        if (argc == 0) {
            continue;
        }
        
        if (strcmp(args[0], "exit") == 0) {
            printf("Выход из командного интерпретатора\n");
            break;
        }
        
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Ошибка при создании процесса");
            continue;
        }
        else if (pid == 0) {
            
            execvp(args[0], args);

            fprintf(stderr, "Ошибка: программа '%s' не найдена\n", args[0]);
            exit(1);
        }
        else {
            int status;
            waitpid(pid, &status, 0);
            printf("Программа завершена сигналом: %d\n", WTERMSIG(status));
        }
    }
}

int main() {
    printf("=== Простой командный интерпретатор ===\n");
    printf("Доступные программы:\n\n");
    printf("summa.c - выводит сумму всех аргументов\n");
    printf("bigstr.c - выводит самый длинный аргумент\n");
    printf("length.c - выводит суммарную длину всех аргументов\n\n");
    printf("Введите 'exit' для выхода\n\n");

    shell_loop();
    
    return 0;
}