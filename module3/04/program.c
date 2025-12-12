#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_ARGUMENTS 64
#define MAX_COMMANDS 32
#define MAX_FILENAME 256

volatile sig_atomic_t interrupted = 0;

void handle_sigint(int sig) {
    interrupted = 1;
    write(STDOUT_FILENO, "\nmeow> ", 7);
}

typedef struct {
    char *args[MAX_ARGUMENTS];
    int argc;
    char input_file[MAX_FILENAME];
    char output_file[MAX_FILENAME];
    int append_output;
} Command;

typedef struct {
    Command commands[MAX_COMMANDS];
    int num_commands;
} Pipeline;

char* my_strdup(const char* src) {
    size_t len = strlen(src) + 1;
    char* dst = malloc(len);
    if (dst == NULL) return NULL;
    strcpy(dst, src);
    return dst;
}

int parse_pipeline(char *input, Pipeline *pipeline) {
    memset(pipeline, 0, sizeof(Pipeline));
    
    char *input_copy = my_strdup(input);
    if (!input_copy) {
        return 0;
    }
    
    char *commands[MAX_COMMANDS];
    int num_pipes = 0;
    
    char *token = strtok(input_copy, "|");
    while (token != NULL && num_pipes < MAX_COMMANDS) {
        while (*token == ' ' || *token == '\t') token++;
        commands[num_pipes++] = token;
        token = strtok(NULL, "|");
    }
    
    if (num_pipes == 0) {
        free(input_copy);
        return 0;
    }
    
    pipeline->num_commands = num_pipes;
    
    for (int i = 0; i < num_pipes; i++) {
        Command *cmd = &pipeline->commands[i];
        cmd->input_file[0] = '\0';
        cmd->output_file[0] = '\0';
        cmd->append_output = 0;
        
        char *cmd_copy = my_strdup(commands[i]);
        if (!cmd_copy) {
            continue;
        }
        
        char *saveptr;
        char *arg_token = strtok_r(cmd_copy, " \t\n", &saveptr);
        int arg_index = 0;
        
        while (arg_token != NULL && arg_index < MAX_ARGUMENTS - 1) {
            if (strcmp(arg_token, "<") == 0) {
                arg_token = strtok_r(NULL, " \t\n", &saveptr);
                if (arg_token != NULL) {
                    strncpy(cmd->input_file, arg_token, MAX_FILENAME - 1);
                    cmd->input_file[MAX_FILENAME - 1] = '\0';
                }
            } else if (strcmp(arg_token, ">") == 0) {
                arg_token = strtok_r(NULL, " \t\n", &saveptr);
                if (arg_token != NULL) {
                    strncpy(cmd->output_file, arg_token, MAX_FILENAME - 1);
                    cmd->output_file[MAX_FILENAME - 1] = '\0';
                    cmd->append_output = 0;
                }
            } else if (strcmp(arg_token, ">>") == 0) {
                arg_token = strtok_r(NULL, " \t\n", &saveptr);
                if (arg_token != NULL) {
                    strncpy(cmd->output_file, arg_token, MAX_FILENAME - 1);
                    cmd->output_file[MAX_FILENAME - 1] = '\0';
                    cmd->append_output = 1;
                }
            } else {
                cmd->args[arg_index++] = my_strdup(arg_token);
            }
            arg_token = strtok_r(NULL, " \t\n", &saveptr);
        }
        
        cmd->args[arg_index] = NULL;
        cmd->argc = arg_index;
        
        free(cmd_copy);
    }
    
    free(input_copy);
    return 1;
}

void free_pipeline(Pipeline *pipeline) {
    for (int i = 0; i < pipeline->num_commands; i++) {
        Command *cmd = &pipeline->commands[i];
        for (int j = 0; j < cmd->argc; j++) {
            if (cmd->args[j]) {
                free(cmd->args[j]);
            }
        }
    }
}

void execute_pipeline(Pipeline *pipeline) {
    int num_commands = pipeline->num_commands;
    if (num_commands == 0) return;
    
    int pipefds[2 * (num_commands - 1)];
    pid_t pids[num_commands];
    
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("Ошибка создания pipe");
            return;
        }
    }
    
    for (int i = 0; i < num_commands; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            perror("Ошибка при создании процесса");
            for (int j = 0; j < 2 * (num_commands - 1); j++) {
                close(pipefds[j]);
            }
            return;
        }
        
        if (pids[i] == 0) {
            Command *cmd = &pipeline->commands[i];
            
            if (cmd->input_file[0] != '\0') {
                int fd = open(cmd->input_file, O_RDONLY);
                if (fd < 0) {
                    perror("Ошибка открытия файла ввода");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            else if (cmd->output_file[0] != '\0') {
                int flags = O_WRONLY | O_CREAT;
                flags |= cmd->append_output ? O_APPEND : O_TRUNC;
                
                int fd = open(cmd->output_file, flags, 0644);
                if (fd < 0) {
                    perror("Ошибка открытия файла вывода");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            
            if (i > 0) {
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
            }
            
            if (i < num_commands - 1) {
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
            }
            
            for (int j = 0; j < 2 * (num_commands - 1); j++) {
                close(pipefds[j]);
            }
            
            execvp(cmd->args[0], cmd->args);
            
            if (access(cmd->args[0], X_OK) == 0) {
                execv(cmd->args[0], cmd->args);
            }
            
            fprintf(stderr, "Ошибка: программа '%s' не найдена\n", cmd->args[0]);
            exit(127);
        }
    }
    
    for (int i = 0; i < 2 * (num_commands - 1); i++) {
        close(pipefds[i]);
    }
    
    for (int i = 0; i < num_commands; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        
        if (i == num_commands - 1 && WIFEXITED(status)) {
            // Выводим код завершения только для последней команды
            // printf("Конвейер завершился с кодом: %d\n", WEXITSTATUS(status));
        }
    }
}

int execute_builtin(Command *cmd) {
    if (cmd->argc == 0) return 0;
    
    if (strcmp(cmd->args[0], "exit") == 0) {
        printf("Выход из командного интерпретатора\n");
        exit(0);
    } else if (strcmp(cmd->args[0], "cd") == 0) {
        if (cmd->argc < 2) {
            fprintf(stderr, "Использование: cd <директория>\n");
        } else if (chdir(cmd->args[1]) != 0) {
            perror("cd");
        }
        return 1;
    } else if (strcmp(cmd->args[0], "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
        return 1;
    }
    
    return 0;
}

void shell_loop() {
    char input[MAX_INPUT_LENGTH];
    
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    
    while (1) {
        if (interrupted) {
            interrupted = 0;
        }
        
        printf("meow> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (feof(stdin)) {
                printf("\n");
                break;
            }
            continue;
        }
        
        input[strcspn(input, "\n")] = 0;
        
        if (strlen(input) == 0) {
            continue;
        }
        
        Pipeline pipeline;
        if (!parse_pipeline(input, &pipeline)) {
            fprintf(stderr, "Ошибка разбора команды\n");
            continue;
        }
        
        if (pipeline.num_commands == 1) {
            if (execute_builtin(&pipeline.commands[0])) {
                free_pipeline(&pipeline);
                continue;
            }
        }
        
        execute_pipeline(&pipeline);
        free_pipeline(&pipeline);
    }
}

int main() {
    printf("=== Продвинутый командный интерпретатор ===\n");
    printf("Поддерживает: конвейеры (|), перенаправления (<, >, >>)\n\n");
    printf("Доступные программы:\n");
    printf("  summa.o   - выводит сумму всех аргументов\n");
    printf("  bigstr.o  - выводит самый длинный аргумент\n");
    printf("  length.o  - выводит суммарную длину всех аргументов\n");
    
    shell_loop();
    
    return 0;
}