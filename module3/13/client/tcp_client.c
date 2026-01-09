#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

void error(const char *msg) {
    perror(msg);
    exit(0);
}

// Функция для отправки файла
void send_file(int sock, const char* filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        send(sock, "ERROR", 5, 0);
        return;
    }
    
    // Получаем размер файла
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Отправляем размер файла
    send(sock, &file_size, sizeof(file_size), 0);
    
    // Ждем подтверждения от сервера
    char ack[10];
    if (recv(sock, ack, sizeof(ack), 0) <= 0) {
        fclose(file);
        return;
    }
    
    // Отправляем файл
    char buffer[4096];
    size_t bytes_read;
    long total_sent = 0;
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(sock, buffer, bytes_read, 0);
        total_sent += bytes_read;
        printf("\rSending: %ld/%ld bytes", total_sent, file_size);
        fflush(stdout);
    }
    
    fclose(file);
    printf("\nFile sent successfully: %s (%ld bytes)\n", filename, file_size);
}

int main(int argc, char *argv[]) {
    int my_sock, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buff[1024];
    
    printf("TCP DEMO CLIENT\n");
    
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    
    // извлечение порта
    portno = atoi(argv[2]);
    
    // Шаг 1 - создание сокета
    my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0)
        error("ERROR opening socket");
    
    // извлечение хоста
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    
    // заполнение структуры serv_addr
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    
    // установка порта
    serv_addr.sin_port = htons(portno);
    
    // Шаг 2 - установка соединения
    if (connect(my_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    
    printf("Connected to server\n");
    
    // Шаг 3 - чтение и передача сообщений
    while (1) {
        // Получаем сообщение от сервера
        bzero(buff, sizeof(buff));
        n = recv(my_sock, &buff[0], sizeof(buff) - 1, 0);
        
        if (n <= 0) {
            printf("Server disconnected\n");
            break;
        }
        
        buff[n] = 0;
        printf("Server: %s", buff);
        
        // Читаем пользовательский ввод
        printf("Client: ");
        fflush(stdout);
        
        bzero(buff, sizeof(buff));
        if (!fgets(&buff[0], sizeof(buff) - 1, stdin)) {
            break;
        }
        
        // Убираем символ новой строки
        if (buff[strlen(buff)-1] == '\n') {
            buff[strlen(buff)-1] = '\0';
        }
        
        // Проверка на "quit"
        if (strcmp(&buff[0], "quit") == 0) {
            printf("Exit...\n");
            send(my_sock, "quit", 4, 0);
            close(my_sock);
            return 0;
        }
        
        // Проверка на команду отправки файла
        if (strncmp(&buff[0], "sendfile ", 9) == 0) {
            char filename[256];
            strcpy(filename, &buff[9]);
            
            // Отправляем команду серверу
            send(my_sock, &buff[0], strlen(&buff[0]), 0);
            
            // Ждем подтверждения от сервера
            bzero(buff, sizeof(buff));
            n = recv(my_sock, &buff[0], sizeof(buff) - 1, 0);
            if (n > 0) {
                buff[n] = 0;
                printf("Server: %s", buff);
            }
            
            // Отправляем файл
            send_file(my_sock, filename);
            continue;
        }
        
        // Отправляем обычное сообщение серверу
        send(my_sock, &buff[0], strlen(&buff[0]), 0);
    }
    
    printf("Connection closed\n");
    close(my_sock);
    return 0;
}