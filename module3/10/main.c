#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <time.h>

#define SHM_SIZE 1024
#define MAX_NUMBERS 100

#define SHM_KEY 0x1234
#define SEM_KEY 0x5678

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
int shmid, semid;
shared_data_t *shared_data;

void sigint_handler(int sig) {
    keep_running = 0;
}

void sem_wait(int semid, int semnum) {
    struct sembuf op = {semnum, -1, 0};
    semop(semid, &op, 1);
}

void sem_signal(int semid, int semnum) {
    struct sembuf op = {semnum, 1, 0};
    semop(semid, &op, 1);
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

int main() {
    srand(time(NULL));
    
    signal(SIGINT, sigint_handler);
    
    shmid = shmget(SHM_KEY, sizeof(shared_data_t), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Ошибка создания разделяемой памяти");
        exit(1);
    }
    
    shared_data = (shared_data_t *)shmat(shmid, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("Ошибка присоединения разделяемой памяти");
        exit(1);
    }
    
    memset(shared_data, 0, sizeof(shared_data_t));
    
    semid = semget(SEM_KEY, 3, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("Ошибка создания семафоров");
        shmdt(shared_data);
        shmctl(shmid, IPC_RMID, NULL);
        exit(1);
    }
    
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;
    
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);
    
    arg.val = 0;
    semctl(semid, 1, SETVAL, arg);
    
    arg.val = 0;
    semctl(semid, 2, SETVAL, arg);
    
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("Ошибка fork");
        shmdt(shared_data);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID, 0);
        exit(1);
    }
    
    if (pid == 0) {
        
        while (keep_running) {
            sem_wait(semid, 1);
            
            if (!keep_running) break;
            
            sem_wait(semid, 0);

            if (shared_data->ready_for_child) {
                find_min_max(shared_data);
            }
            
            sem_signal(semid, 0);
            
            sem_signal(semid, 2);
        }
        
        shmdt(shared_data);
        
        exit(0);
        
    } else {
        
        while (keep_running) {
            sem_wait(semid, 0);
            
            generate_numbers(shared_data);
            
            sem_signal(semid, 0);

            sem_signal(semid, 1);
            
            sem_wait(semid, 2);
            
            if (!keep_running) break;
            
            sem_wait(semid, 0);
            
            if (shared_data->ready_for_parent) {
                printf("Родитель получил результаты: ");
                printf("Min = %d, Max = %d\n", shared_data->min, shared_data->max);
                printf("Обработано наборов данных: %d\n\n", shared_data->processed_sets);
            }
            
            sem_signal(semid, 0);
            
            sleep(1);
        }
        
        sem_signal(semid, 1);
        
        wait(NULL);
        
        printf("Всего обработано наборов данных: %d\n", shared_data->processed_sets);
        
        shmdt(shared_data);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID, 0);
    }
    
    return 0;
}