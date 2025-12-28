#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_LINES 100
#define MAX_LINE_LEN 256

typedef struct {
    char lines[MAX_LINES][MAX_LINE_LEN];
    int line_count;
    int processed_count;
    int generation_complete;
} shared_data_t;

char *generate_string() {
    int length = rand() % 5 + 1;
    char *buffer = malloc(100 * sizeof(char));
    if (!buffer)
        return NULL;

    int pos = 0;
    for (int j = 0; j < length; j++) {
        pos += snprintf(buffer + pos, 100 - pos, "%d ", rand() % 100);
        if (pos >= 99)
            break;
    }
    pos += snprintf(buffer + pos, 100 - pos, "\n");
    return buffer;
}

void analyze_string(const char *str) {
    char *copy = strdup(str);
    char *token;
    int first = 1;
    int min, max, num;
    
    token = strtok(copy, " \n");
    while (token != NULL) {
        if (sscanf(token, "%d", &num) == 1) {
            if (first) {
                min = max = num;
                first = 0;
            } else {
                if (num < min) min = num;
                if (num > max) max = num;
            }
        }
        token = strtok(NULL, " \n");
    }
    
    if (!first) {
        printf("Строка: %s", str);
        printf("Min: %d, Max: %d\n\n", min, max);
    }
    
    free(copy);
}

int main() {
    srand(time(NULL));
    
    int fd = open("file.dat", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("Ошибка открытия файла");
        exit(1);
    }
    
    ftruncate(fd, sizeof(shared_data_t));
    
    shared_data_t *shared_data = mmap(NULL, sizeof(shared_data_t), 
                                      PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("Ошибка mmap");
        close(fd);
        exit(1);
    }
    
    memset(shared_data, 0, sizeof(shared_data_t));
    
    sem_unlink("/mutex_sem");
    sem_unlink("/data_ready_sem");
    sem_unlink("/data_processed_sem");
    
    sem_t *mutex_sem = sem_open("/mutex_sem", O_CREAT | O_EXCL, 0666, 1);
    sem_t *data_ready_sem = sem_open("/data_ready_sem", O_CREAT | O_EXCL, 0666, 0);
    sem_t *data_processed_sem = sem_open("/data_processed_sem", O_CREAT | O_EXCL, 0666, 0);
    
    if (mutex_sem == SEM_FAILED || data_ready_sem == SEM_FAILED || data_processed_sem == SEM_FAILED) {
        perror("Ошибка создания семафоров");
        munmap(shared_data, sizeof(shared_data_t));
        close(fd);
        exit(1);
    }
    
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("Ошибка fork");
        sem_close(mutex_sem);
        sem_close(data_ready_sem);
        sem_close(data_processed_sem);
        munmap(shared_data, sizeof(shared_data_t));
        close(fd);
        exit(1);
    }
    
    if (pid == 0) {
        
        while (1) {
            sem_wait(data_ready_sem);
            sem_wait(mutex_sem);
            
            if (shared_data->generation_complete && 
                shared_data->processed_count >= shared_data->line_count) {
                sem_post(mutex_sem);
                break;
            }
            
            if (shared_data->processed_count < shared_data->line_count) {
                int idx = shared_data->processed_count;
                analyze_string(shared_data->lines[idx]);
                shared_data->processed_count++;
            }
            sem_post(mutex_sem);
            
            sem_post(data_processed_sem);
        }
        
        printf("Дочерний процесс завершен\n");

        sem_close(mutex_sem);
        sem_close(data_ready_sem);
        sem_close(data_processed_sem);
        
        munmap(shared_data, sizeof(shared_data_t));
        close(fd);
        exit(0);
        
    } else {
        
        for (int i = 0; i < 7; i++) {
            char *str = generate_string();
            if (str) {
                sem_wait(mutex_sem);
                
                strncpy(shared_data->lines[shared_data->line_count], str, MAX_LINE_LEN - 1);
                shared_data->lines[shared_data->line_count][MAX_LINE_LEN - 1] = '\0';
                shared_data->line_count++;
                
                sem_post(mutex_sem);
                
                free(str);
                
                printf("%d ", shared_data->line_count);
                fflush(stdout);
                
                sem_post(data_ready_sem);
                
                sem_wait(data_processed_sem);
                
                sleep(1);
            }
        }
        
        sem_wait(mutex_sem);
        shared_data->generation_complete = 1;
        sem_post(mutex_sem);
        
        sem_post(data_ready_sem);
        
        printf("\nРодительский процесс завершил генерацию\n");
        
        wait(NULL);
        
        sem_close(mutex_sem);
        sem_close(data_ready_sem);
        sem_close(data_processed_sem);
        sem_unlink("/mutex_sem");
        sem_unlink("/data_ready_sem");
        sem_unlink("/data_processed_sem");
        munmap(shared_data, sizeof(shared_data_t));
        close(fd);
    }
    
    return 0;
}