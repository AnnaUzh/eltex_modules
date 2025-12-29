#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdint.h>

#define BUFFER_SIZE 1024
#define PORT_1 9090
#define PORT_2 9091

void* receive(void* socket1) {
    int socket = (intptr_t)socket1;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in from_addr;
    socklen_t addr_len = sizeof(from_addr);
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        
        int n = recvfrom(socket, buffer, BUFFER_SIZE, 0,
                        (struct sockaddr*)&from_addr, &addr_len);
        
        if (n > 0) {
            buffer[n] = '\0';
            printf("\n[Сообщение получено] %s\n", buffer);
            printf("Введите сообщение: ");
            fflush(stdout);
        }
        
        usleep(10000);
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in my_addr, other_addr;
    char buffer[BUFFER_SIZE];
    pthread_t tid;
    
    if (argc != 2) {
        printf("Использование: %s <client number>\n", argv[0]);
        exit(1);
    }
    
    int mode = atoi(argv[1]);
    if (mode != 1 && mode != 2) {
        exit(1);
    }
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    memset(&other_addr, 0, sizeof(other_addr));
    other_addr.sin_family = AF_INET;
    
    if (mode == 1) {
        my_addr.sin_port = htons(PORT_1);
        other_addr.sin_port = htons(PORT_2);
        other_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    } else {
        my_addr.sin_port = htons(PORT_2);
        other_addr.sin_port = htons(PORT_1);
        other_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    }
    
    if (bind(sock, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0) {
        perror("Ошибка привязки");
        close(sock);
        exit(EXIT_FAILURE);
    }
    
    pthread_create(&tid, NULL, receive, (void*)(intptr_t)sock);
    
    while (1) {
        printf("Введите сообщение: ");
        fflush(stdout);
        
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = '\0';
            
            if (strlen(buffer) == 0) continue;
            
            if (strcmp(buffer, "/exit") == 0) {
                printf("exit...\n");
                break;
            }
            
            sendto(sock, buffer, strlen(buffer), 0,
                  (struct sockaddr*)&other_addr, sizeof(other_addr));
            
            printf("[Сообщение отправлено]\n");
        }
    }
    
    close(sock);
    return 0;
}