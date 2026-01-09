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

// функция обслуживания подключившихся пользователей
void dostuff(int);
// функция обработки ошибок
void error(const char *msg) {
    perror(msg);
    exit(1);
}
// количество активных пользователей
int nclients = 0;

// печать количества активных пользователей
void printusers() {
    if (nclients) {
        printf("%d user on-line\n", nclients);
    } else {
        printf("No User on line\n");
    }
}

// функция обработки данных
int add_func(int a, int b) { return a + b; }
int sub_func(int a, int b) { return a - b; }
int mul_func(int a, int b) { return a * b; }
int div_func(int a, int b) { return a / b; }

// функция для приема файла
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
    
    // Получаем размер файла
    if (recv(sock, &file_size, sizeof(file_size), 0) <= 0) {
        fclose(file);
        return;
    }
    
    send(sock, "READY", 5, 0);  // Подтверждаем готовность принять файл
    
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

int main(int argc, char *argv[]) {
    int sockfd, newsockfd; // дескрипторы сокетов
    int portno;            // номер порта
    int pid;               // id номер потока
    socklen_t clilen; // размер адреса клиента типа socklen_t
    struct sockaddr_in serv_addr, cli_addr; // структура сокета сервера и клиента
    
    printf("TCP SERVER DEMO\n");
    
    // ошибка в случае если мы не указали порт
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    
    // Шаг 1 - создание сокета
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    
    // Шаг 2 - связывание сокета с локальным адресом
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // сервер принимает подключения на все IP-адреса
    serv_addr.sin_port = htons(portno);
    
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    
    // Шаг 3 - ожидание подключений, размер очереди - 5
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    
    // Шаг 4 - извлекаем сообщение из очереди (цикл извлечения запросов на подключение)
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        
        nclients++;
        
        // вывод сведений о клиенте
        printf("+[%s:%d] new connect!\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
        printusers();
        
        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        
        if (pid == 0) {
            close(sockfd);
            dostuff(newsockfd);
            exit(0);
        } else {
            close(newsockfd);
        }
    }
    
    close(sockfd);
    return 0;
}

void dostuff(int sock) {
    int bytes_recv; // размер принятого сообщения
    int a, b;       // переменные для функций
    char buff[1024];
    
    // Отправляем приветствие и список команд
    char *welcome_msg = "Welcome! Available commands:\n"
                        "1. Calculator mode: enter two numbers and operation (add/sub/mul/div)\n"
                        "2. Send file: type 'sendfile' followed by filename\n"
                        "3. Type 'quit' to exit\n\n"
                        "Enter 1 parameter\r\n";
    
    send(sock, welcome_msg, strlen(welcome_msg), 0);
    
    while (1) {
        bzero(buff, sizeof(buff));
        bytes_recv = recv(sock, buff, sizeof(buff) - 1, 0);
        
        if (bytes_recv <= 0) {
            break;
        }
        
        buff[bytes_recv] = '\0';
        
        // Убираем символ новой строки в конце
        if (buff[strlen(buff)-1] == '\n') {
            buff[strlen(buff)-1] = '\0';
        }
        
        // Проверка на quit
        if (strcmp(buff, "quit") == 0) {
            send(sock, "Goodbye!\n", 9, 0);
            break;
        }
        
        // Проверка на команду отправки файла
        if (strncmp(buff, "sendfile ", 9) == 0) {
            char filename[256];
            strcpy(filename, buff + 9);
            send(sock, "READY to receive file\n", 22, 0);
            receive_file(sock, filename);
            
            // Возвращаемся в режим калькулятора
            send(sock, "File received. Enter 1 parameter\r\n", 35, 0);
            continue;
        }
        
        // Режим калькулятора - получаем первый параметр
        a = atoi(buff);
        
        // Запрашиваем второй параметр
        send(sock, "Enter 2 parameter\r\n", 20, 0);
        
        bzero(buff, sizeof(buff));
        bytes_recv = recv(sock, buff, sizeof(buff) - 1, 0);
        if (bytes_recv <= 0) break;
        buff[bytes_recv] = '\0';
        if (buff[strlen(buff)-1] == '\n') buff[strlen(buff)-1] = '\0';
        
        b = atoi(buff);
        
        // Запрашиваем операцию
        send(sock, "Enter operation (add/sub/mul/div)\r\n", 35, 0);
        
        bzero(buff, sizeof(buff));
        bytes_recv = recv(sock, buff, sizeof(buff) - 1, 0);
        if (bytes_recv <= 0) break;
        buff[bytes_recv] = '\0';
        if (buff[strlen(buff)-1] == '\n') buff[strlen(buff)-1] = '\0';
        
        int result;
        char result_str[100];
        
        if (strcmp(buff, "add") == 0) {
            result = add_func(a, b);
            sprintf(result_str, "Result: %d\n", result);
        }
        else if (strcmp(buff, "sub") == 0) {
            result = sub_func(a, b);
            sprintf(result_str, "Result: %d\n", result);
        }
        else if (strcmp(buff, "mul") == 0) {
            result = mul_func(a, b);
            sprintf(result_str, "Result: %d\n", result);
        }
        else if (strcmp(buff, "div") == 0) {
            if (b == 0) {
                strcpy(result_str, "ERROR: Division by zero\n");
            } else {
                result = div_func(a, b);
                sprintf(result_str, "Result: %d\n", result);
            }
        }
        else {
            strcpy(result_str, "ERROR: Unknown operation\n");
        }
        
        send(sock, result_str, strlen(result_str), 0);
        send(sock, "Enter 1 parameter (or 'quit' to exit)\r\n", 40, 0);
    }
    
    nclients--; // уменьшаем счетчик активных клиентов
    printf("-disconnect\n");
    printusers();
    close(sock);
}