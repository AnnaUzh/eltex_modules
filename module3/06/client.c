#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>


#define SERVER_PRIORITY 10
#define MSG_TEXT 1
#define MSG_COMMAND 2
#define MSG_PRIVATE 3

struct message {
    long mtype;
    int sender_id;
    int receiver_id;
    int msg_type;
    char mtext[256];
};

int msgid;
int client_id;
bool running = true;

// Поток для приема сообщений
void* receiver_thread(void* arg) {
    struct message msg;
    
    while (running) {
        if (msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), 
                  client_id, IPC_NOWAIT) != -1) {
            if (msg.msg_type == MSG_PRIVATE) {
                printf("\n[PRIVATE from %d]: %s\n", msg.sender_id, msg.mtext);
            } else {
                printf("\n[Client %d]: %s\n", msg.sender_id, msg.mtext);
            }
            printf("Enter message: ");
            fflush(stdout);
        }
        usleep(100000);
    }
    
    return NULL;
}

// Подключение к серверу
void connect_to_server() {
    struct message msg;
    msg.mtype = SERVER_PRIORITY;
    msg.sender_id = client_id;
    msg.receiver_id = 0;
    msg.msg_type = MSG_COMMAND;
    strcpy(msg.mtext, "connect");
    
    msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long), 0);
    
    struct message response;
    msgrcv(msgid, &response, sizeof(struct message) - sizeof(long), 
           client_id, 0);
    printf("%s\n", response.mtext);
}

// Отключение от сервера
void disconnect_from_server() {
    struct message msg;
    msg.mtype = SERVER_PRIORITY;
    msg.sender_id = client_id;
    msg.receiver_id = 0;
    msg.msg_type = MSG_COMMAND;
    strcpy(msg.mtext, "shutdown");
    
    msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long), 0);
}

// Обработчик сигнала
void signal_handler(int sig) {
    printf("\nShutting down...\n");
    disconnect_from_server();
    running = false;
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <client_id>\n", argv[0]);
        printf("Client ID: 20, 30, 40, ...\n");
        exit(1);
    }
    
    client_id = atoi(argv[1]);
    if (client_id < 20 || client_id % 10 != 0) {
        printf("Error: Client ID must be 20, 30, 40, ...\n");
        exit(1);
    }
    
    key_t key = ftok("server.c", 'A');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }
    
    msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget");
        printf("Server not running\n");
        exit(1);
    }
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("Client %d started\n", client_id);
    connect_to_server();
    
    pthread_t receiver;
    pthread_create(&receiver, NULL, receiver_thread, NULL);
    
    printf("\nCommands:\n");
    printf("  @<ID> <message> - private message\n");
    printf("  <message> - public message\n");
    printf("  shutdown - disconnect\n");
    printf("  help - show help\n");
    printf("\nEnter message: ");
    fflush(stdout);
    
    char input[256];
    while (running) {
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;
        
        if (strlen(input) == 0) continue;
        
        struct message msg;
        msg.mtype = SERVER_PRIORITY;
        msg.sender_id = client_id;
        
        if (strcmp(input, "shutdown") == 0) {
            printf("Disconnecting...\n");
            disconnect_from_server();
            running = false;
            break;
        }
        else if (strcmp(input, "help") == 0) {
            printf("\nCommands:\n  @<ID> message - private\n  message - public\n  shutdown - exit\n  help - this help\n\nEnter message: ");
            continue;
        }
        else if (input[0] == '@') {
            // Приватное сообщение
            char* space = strchr(input, ' ');
            if (space != NULL) {
                *space = '\0';
                int target_id = atoi(input + 1);
                
                if (target_id >= 20 && target_id % 10 == 0 && target_id != client_id) {
                    msg.receiver_id = target_id;
                    msg.msg_type = MSG_PRIVATE;
                    strncpy(msg.mtext, space + 1, sizeof(msg.mtext) - 1);
                    msg.mtext[sizeof(msg.mtext) - 1] = '\0';
                    
                    msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long), 0);
                    printf("Private message sent to client %d\n", target_id);
                } else {
                    printf("Invalid client ID\n");
                }
            } else {
                printf("Format: @ID message\n");
            }
        }
        else {
            // Общее сообщение
            msg.receiver_id = 0;
            msg.msg_type = MSG_TEXT;
            strncpy(msg.mtext, input, sizeof(msg.mtext) - 1);
            msg.mtext[sizeof(msg.mtext) - 1] = '\0';
            
            msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long), 0);
            printf("Message sent to all\n");
        }
        
        printf("Enter message: ");
        fflush(stdout);
    }
    
    pthread_join(receiver, NULL);
    printf("Client %d finished\n", client_id);
    return 0;
}