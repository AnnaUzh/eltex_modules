#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>

#define QUEUE_1 "/chat_queue_1"
#define QUEUE_2 "/chat_queue_2"
#define MAX_MSG_SIZE 256
#define EXIT_PRIORITY 255

int main() {
    mqd_t mq_send_fd, mq_receive_fd;  // ИЗМЕНИЛИ ИМЕНА
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];
    int priority;
    
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    
    mq_send_fd = mq_open(QUEUE_1, O_CREAT | O_WRONLY, 0644, &attr);
    mq_receive_fd = mq_open(QUEUE_2, O_CREAT | O_RDONLY, 0644, &attr);
    
    if (mq_receive_fd == (mqd_t)-1 || mq_send_fd == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }
    
    printf("=== Чат 2 запущен ===\n");
    printf("Введите 'exit' для завершения\n");
    printf("Ожидание сообщения от 1...\n");
    
    while (1) {
        if (mq_receive(mq_receive_fd, buffer, MAX_MSG_SIZE, &priority) == -1) {
            perror("mq_receive");
            break;
        }
        
        if (priority == EXIT_PRIORITY) {
            printf("Собеседник завершил чат\n");
            break;
        }
        
        printf("1: %s\n", buffer);
        
        printf("2: ");
        fgets(buffer, MAX_MSG_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if (strcmp(buffer, "exit") == 0) {
            if (mq_send(mq_send_fd, buffer, strlen(buffer) + 1, EXIT_PRIORITY) == -1) {
                perror("mq_send exit");
            }
            printf("Завершение чата...\n");
            break;
        }
        
        if (mq_send(mq_send_fd, buffer, strlen(buffer) + 1, 0) == -1) {
            perror("mq_send");
            break;
        }
    }
    
    mq_close(mq_send_fd);
    mq_close(mq_receive_fd);
    mq_unlink(QUEUE_2);
    
    printf("Чат 2 завершен\n");
    return 0;
}