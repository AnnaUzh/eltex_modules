#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>


void error(const char *msg) {
    perror(msg);
    exit(1);
}
int nclients = 0;

void printusers() {
    if (nclients) {
        printf("%d user on-line\n", nclients);
    } else {
        printf("No User on line\n");
    }
}

int add_func(int a, int b) { return a + b; }
int sub_func(int a, int b) { return a - b; }
int mul_func(int a, int b) { return a * b; }
int div_func(int a, int b) { return a / b; }

void receive_file(int sock, const char* filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to create file");
        send(sock, "ERROR: Cannot create file\n", 26, 0);
        return;
    }
    
    char buffer[4096];
    ssize_t bytes_received;
    long file_size;
    
    if (recv(sock, &file_size, sizeof(file_size), 0) <= 0) {
        fclose(file);
        return;
    }
    
    send(sock, "READY", 5, 0);
    
    long total_received = 0;
    while (total_received < file_size) {
        bytes_received = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break;
        }
        fwrite(buffer, 1, bytes_received, file);
        total_received += bytes_received;
    }
    
    fclose(file);
    printf("File received: %s (%ld bytes)\n", filename, total_received);
}

typedef struct {
    int sockfd;
    int state;
    int param1;
    int param2;
    char operation[10];
    char filename[256];
} client_state_t;

int main(int argc, char *argv[]) {
    int sockfd, newsockfd;
    int portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    

    int client_sockets[FD_SETSIZE];
    client_state_t client_states[FD_SETSIZE];
    
    for (int i = 0; i < FD_SETSIZE; i++) {
        client_sockets[i] = -1;
        client_states[i].sockfd = -1;
        client_states[i].state = 0;
    }
    
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    
    listen(sockfd, 5);
    
    fd_set readfds;
    int max_sd;
    
    while (1) {
        FD_ZERO(&readfds);
        
        FD_SET(sockfd, &readfds);
        max_sd = sockfd;
        
        for (int i = 0; i < FD_SETSIZE; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &readfds);
                if (client_sockets[i] > max_sd) {
                    max_sd = client_sockets[i];
                }
            }
        }
        
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        
        if (activity < 0) {
            perror("select error");
            continue;
        }
        
        if (FD_ISSET(sockfd, &readfds)) {
            clilen = sizeof(cli_addr);
            newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
            if (newsockfd < 0)
                error("ERROR on accept");
            
            nclients++;
            
            printf("+[%s:%d] new connect!\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            printusers();
            
            int i;
            for (i = 0; i < FD_SETSIZE; i++) {
                if (client_sockets[i] == -1) {
                    client_sockets[i] = newsockfd;
                    client_states[i].sockfd = newsockfd;
                    client_states[i].state = 0;
                    
                    char *welcome_msg = "Enter 1 parameter\r\n";
                    
                    send(newsockfd, welcome_msg, strlen(welcome_msg), 0);
                    break;
                }
            }
            if (i == FD_SETSIZE) {
                printf("Too many clients\n");
                close(newsockfd);
                nclients--;
            }
        }
        
        for (int i = 0; i < FD_SETSIZE; i++) {
            if (client_sockets[i] > 0 && FD_ISSET(client_sockets[i], &readfds)) {
                int sock = client_sockets[i];
                int bytes_recv;
                char buff[1024];
                
                bzero(buff, sizeof(buff));
                bytes_recv = recv(sock, buff, sizeof(buff) - 1, 0);
                
                if (bytes_recv <= 0) {
                    nclients--;
                    printf("-disconnect\n");
                    printusers();
                    close(sock);
                    client_sockets[i] = -1;
                    client_states[i].sockfd = -1;
                    continue;
                }
                
                buff[bytes_recv] = '\0';
                
                if (buff[strlen(buff)-1] == '\n') {
                    buff[strlen(buff)-1] = '\0';
                }
                
                if (strcmp(buff, "quit") == 0) {
                    nclients--;
                    printf("-disconnect\n");
                    printusers();
                    close(sock);
                    client_sockets[i] = -1;
                    client_states[i].sockfd = -1;
                    continue;
                }
                
                if (strncmp(buff, "sendfile ", 9) == 0) {
                    char filename[256];
                    strcpy(filename, buff + 9);
                    send(sock, "READY to receive file\n", 22, 0);
                    receive_file(sock, filename);
                    
                    send(sock, "File received. Enter 1 parameter\r\n", 35, 0);
                    client_states[i].state = 0;
                    continue;
                }
                
                switch (client_states[i].state) {
                    case 0:
                        client_states[i].param1 = atoi(buff);
                        client_states[i].state = 1;
                        send(sock, "Enter 2 parameter\r\n", 20, 0);
                        break;
                        
                    case 1:
                        client_states[i].param2 = atoi(buff);
                        client_states[i].state = 2;
                        send(sock, "Enter operation (add/sub/mul/div)\r\n", 35, 0);
                        break;
                        
                    case 2:
                        strcpy(client_states[i].operation, buff);
                        
                        int result;
                        char result_str[100];
                        
                        if (strcmp(client_states[i].operation, "add") == 0) {
                            result = add_func(client_states[i].param1, client_states[i].param2);
                            sprintf(result_str, "Result: %d\n", result);
                        }
                        else if (strcmp(client_states[i].operation, "sub") == 0) {
                            result = sub_func(client_states[i].param1, client_states[i].param2);
                            sprintf(result_str, "Result: %d\n", result);
                        }
                        else if (strcmp(client_states[i].operation, "mul") == 0) {
                            result = mul_func(client_states[i].param1, client_states[i].param2);
                            sprintf(result_str, "Result: %d\n", result);
                        }
                        else if (strcmp(client_states[i].operation, "div") == 0) {
                            if (client_states[i].param2 == 0) {
                                strcpy(result_str, "ERROR: Division by zero\n");
                            } else {
                                result = div_func(client_states[i].param1, client_states[i].param2);
                                sprintf(result_str, "Result: %d\n", result);
                            }
                        }
                        else {
                            strcpy(result_str, "ERROR: Unknown operation\n");
                        }
                        
                        send(sock, result_str, strlen(result_str), 0);
                        send(sock, "Enter 1 parameter (or 'quit' to exit)\r\n", 40, 0);
                        client_states[i].state = 0;
                        break;
                }
            }
        }
    }
    
    close(sockfd);
    return 0;
}
