#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>

#define MAX_NUMBERS 100
#define SHM_NAME "/posix_shm_example"
#define SEM_MUTEX_NAME "/posix_sem_mutex"
#define SEM_DATA_READY_NAME "/posix_sem_data_ready"
#define SEM_RESULTS_READY_NAME "/posix_sem_results_ready"

typedef struct {
    int numbers[MAX_NUMBERS];
    int count;
    int min;
    int max;
    int ready_for_child;
    int ready_for_parent;
    int processed_sets;
} shared_data_t;

volatile sig_atomic_t keep_running = 1;
shared_data_t *shared_data;
sem_t *sem_mutex, *sem_data_ready, *sem_results_ready;
int shm_fd;


void sigint_handler(int sig) {
    keep_running = 0;
}

void generate_numbers(shared_data_t *data) {
    data->count = rand() % 13 + 3;
    
    printf("Родитель сгенерировал %d чисел: ", data->count);
    for (int i = 0; i < data->count; i++) {
        data->numbers[i] = rand() % 1000;
        printf("%d ", data->numbers[i]);
    }
    printf("\n");
    
    data->min = 0;
    data->max = 0;
    data->ready_for_child = 1;
    data->ready_for_parent = 0;
}


void find_min_max(shared_data_t *data) {
    if (data->count <= 0) return;
    
    data->min = data->numbers[0];
    data->max = data->numbers[0];
    
    for (int i = 1; i < data->count; i++) {
        if (data->numbers[i] < data->min) {
            data->min = data->numbers[i];
        }
        if (data->numbers[i] > data->max) {
            data->max = data->numbers[i];
        }
    }
    
    data->ready_for_child = 0;
    data->ready_for_parent = 1;
    data->processed_sets++;
}

void cleanup_resources() {
    munmap(shared_data, sizeof(shared_data_t));
    
    close(shm_fd);
    shm_unlink(SHM_NAME);
    
    sem_close(sem_mutex);
    sem_unlink(SEM_MUTEX_NAME);
    
    sem_close(sem_data_ready);
    sem_unlink(SEM_DATA_READY_NAME);
    
    sem_close(sem_results_ready);
    sem_unlink(SEM_RESULTS_READY_NAME);
}

int main() {
    srand(time(NULL));
    signal(SIGINT, sigint_handler);
    
    shared_data = NULL;
    sem_mutex = sem_data_ready = sem_results_ready = NULL;
    shm_fd = -1;
    
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_MUTEX_NAME);
    sem_unlink(SEM_DATA_READY_NAME);
    sem_unlink(SEM_RESULTS_READY_NAME);
    
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);

    ftruncate(shm_fd, sizeof(shared_data_t));
    
    shared_data = mmap(NULL, sizeof(shared_data_t), 
                       PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    
    memset(shared_data, 0, sizeof(shared_data_t));
    
    sem_mutex = sem_open(SEM_MUTEX_NAME, O_CREAT | O_EXCL, 0666, 1);
    sem_data_ready = sem_open(SEM_DATA_READY_NAME, O_CREAT | O_EXCL, 0666, 0);
    sem_results_ready = sem_open(SEM_RESULTS_READY_NAME, O_CREAT | O_EXCL, 0666, 0);
    
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("Ошибка fork");
        cleanup_resources();
        exit(1);
    }
    
    if (pid == 0) {
        while (keep_running) {
            sem_wait(sem_data_ready);
            
            if (!keep_running) break;
            
            sem_wait(sem_mutex);
            
            if (shared_data->ready_for_child) {
                find_min_max(shared_data);
            }
            
            sem_post(sem_mutex);
            sem_post(sem_results_ready);
        }
        
        sem_close(sem_mutex);
        sem_close(sem_data_ready);
        sem_close(sem_results_ready);
        
        munmap(shared_data, sizeof(shared_data_t));
        close(shm_fd);
        
        exit(0);
        
    } else {
        
        while (keep_running) {
            sem_wait(sem_mutex);

            generate_numbers(shared_data);
            
            sem_post(sem_mutex);
            sem_post(sem_data_ready);
            sem_wait(sem_results_ready);
            
            if (!keep_running) break;

            sem_wait(sem_mutex);
            
            if (shared_data->ready_for_parent) {
                printf("Родитель получил результаты: ");
                printf("Min = %d, Max = %d\n", shared_data->min, shared_data->max);
                printf("Обработано наборов данных: %d\n\n", shared_data->processed_sets);
            }
            sem_post(sem_mutex);
            sleep(1);
        }
        sem_post(sem_data_ready);
        wait(NULL);
        
        printf("Всего обработано наборов данных: %d\n", shared_data->processed_sets);
        
        cleanup_resources();
    }
    
    return 0;
}