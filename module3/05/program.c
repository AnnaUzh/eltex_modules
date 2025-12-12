#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define FILENAME "counter_output.txt"

volatile sig_atomic_t counter = 0;
volatile sig_atomic_t sigint_count = 0;
volatile sig_atomic_t should_exit = 0;
FILE *output_file = NULL;

// Функция для записи в файл
void write_to_file(const char *message) {
    if (output_file == NULL) {
        output_file = fopen(FILENAME, "a");
        if (output_file == NULL) {
            perror("Не удалось открыть файл");
            exit(EXIT_FAILURE);
        }
    }
    
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(output_file, "[%s] %s\n", timestamp, message);
    fflush(output_file);
}

// Обработчик сигналов
void signal_handler(int signum) {
    char msg[100];
    
    switch (signum) {
        case SIGINT:
            sigint_count++;
            snprintf(msg, sizeof(msg), "Получен и обработан сигнал SIGINT (%d из 3)", sigint_count);
            write_to_file(msg);
            printf("%s\n", msg);
            
            if (sigint_count >= 3) {
                write_to_file("Получено 3 сигнала SIGINT. Завершаю работу...");
                printf("Получено 3 сигнала SIGINT. Завершаю работу...\n");
                should_exit = 1;
            }
            break;
            
        case SIGQUIT:
            snprintf(msg, sizeof(msg), "Получен и обработан сигнал SIGQUIT");
            write_to_file(msg);
            printf("%s\n", msg);
            break;
            
        default:
            snprintf(msg, sizeof(msg), "Получен сигнал %d", signum);
            write_to_file(msg);
            printf("%s\n", msg);
            break;
    }
}

int main() {
    output_file = fopen(FILENAME, "w");
    if (output_file == NULL) {
        perror("Не удалось открыть файл");
        return EXIT_FAILURE;
    }
    
    write_to_file("Программа запущена");
    printf("Программа запущена. PID: %d\n", getpid());
    printf("Вывод записывается в файл: %s\n", FILENAME);
    
    // обработчики сигналов
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    // SIGINT и SIGQUIT
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Ошибка установки обработчика SIGINT");
        fclose(output_file);
        return EXIT_FAILURE;
    }
    
    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        perror("Ошибка установки обработчика SIGQUIT");
        fclose(output_file);
        return EXIT_FAILURE;
    }
    
    // Игнорируем SIGTSTP
    signal(SIGTSTP, SIG_IGN);
    
    while (!should_exit) {
        counter++;
        char msg[50];
        snprintf(msg, sizeof(msg), "Счетчик: %d", counter);
        write_to_file(msg);
        sleep(1);
    }
    
    write_to_file("Программа завершена");
    fclose(output_file);
    output_file = NULL;
    
    printf("Программа завершена. Всего итераций: %d\n", counter);
    return EXIT_SUCCESS;
}