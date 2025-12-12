#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>

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


struct client_info {
    int client_id;
    bool active;
};

int msgid;
struct client_info clients[100];
int client_count = 0;

// Функция для создания нового клиента
int add_client(int client_id) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].client_id == client_id) {
            clients[i].active = true;
            return 0;
        }
    }
    
    if (client_count < 100) {
        clients[client_count].client_id = client_id;
        clients[client_count].active = true;
        client_count++;
        printf("Client %d connected\n", client_id);
        return 1;
    }
    return -1;
}

// Функция для деактивации клиента
void deactivate_client(int client_id) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].client_id == client_id) {
            clients[i].active = false;
            printf("Client %d disconnected\n", client_id);
            break;
        }
    }
}

// Функция для отправки сообщения клиенту
void send_to_client(int client_id, struct message *msg) {
    struct message forward_msg = *msg;
    forward_msg.mtype = client_id;
    
    if (msgsnd(msgid, &forward_msg, sizeof(struct message) - sizeof(long), 0) == -1) {
        perror("msgsnd to client");
    }
}

// Функция для отправки сообщения всем клиентам
void broadcast_to_all(struct message *msg, int exclude_id) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].active && clients[i].client_id != exclude_id) {
            send_to_client(clients[i].client_id, msg);
        }
    }
}

// Обработчик сигнала для корректного завершения
void cleanup(int sig) {
    printf("\nShutting down server...\n");
    
    struct message shutdown_msg;
    shutdown_msg.mtype = SERVER_PRIORITY;
    shutdown_msg.sender_id = 0;
    shutdown_msg.receiver_id = 0;
    shutdown_msg.msg_type = MSG_TEXT;
    strcpy(shutdown_msg.mtext, "Server shutting down");
    
    for (int i = 0; i < client_count; i++) {
        if (clients[i].active) {
            send_to_client(clients[i].client_id, &shutdown_msg);
        }
    }
    
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
    }
    
    printf("Message queue removed\n");
    exit(0);
}

int main() {
    key_t key;
    
    key = ftok("server.c", 'A');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }
    
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }
    
    printf("Server started. Queue ID: %d\n", msgid);
    printf("Waiting for client messages...\n");
    printf("Server priority: %d\n", SERVER_PRIORITY);
    printf("========================================\n");
    
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);
    
    struct message msg;
    
    while (1) {
        if (msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), 
                  SERVER_PRIORITY, 0) == -1) {
            perror("msgrcv");
            continue;
        }
        
        if (msg.msg_type == MSG_COMMAND) {
            printf("Command from client %d: %s\n", msg.sender_id, msg.mtext);
            
            if (strcmp(msg.mtext, "connect") == 0) {
                add_client(msg.sender_id);
                
                struct message response;
                response.mtype = msg.sender_id;
                response.sender_id = 0;
                response.receiver_id = msg.sender_id;
                response.msg_type = MSG_TEXT;
                snprintf(response.mtext, sizeof(response.mtext),
                        "Connected. Your ID: %d", msg.sender_id);
                msgsnd(msgid, &response, sizeof(struct message) - sizeof(long), 0);
                
                struct message notify_msg;
                notify_msg.mtype = SERVER_PRIORITY;
                notify_msg.sender_id = 0;
                notify_msg.receiver_id = 0;
                notify_msg.msg_type = MSG_TEXT;
                snprintf(notify_msg.mtext, sizeof(notify_msg.mtext),
                        "Client %d joined the chat", msg.sender_id);
                broadcast_to_all(&notify_msg, msg.sender_id);
            }
            else if (strcmp(msg.mtext, "shutdown") == 0) {
                printf("Client %d disconnecting...\n", msg.sender_id);
                deactivate_client(msg.sender_id);
                
                struct message notify_msg;
                notify_msg.mtype = SERVER_PRIORITY;
                notify_msg.sender_id = 0;
                notify_msg.receiver_id = 0;
                notify_msg.msg_type = MSG_TEXT;
                snprintf(notify_msg.mtext, sizeof(notify_msg.mtext),
                        "Client %d left the chat", msg.sender_id);
                broadcast_to_all(&notify_msg, -1);
            }
        }
        else if (msg.msg_type == MSG_TEXT) {
            printf("Public message from client %d: %s\n", 
                   msg.sender_id, msg.mtext);
            
            broadcast_to_all(&msg, msg.sender_id);
        }
        else if (msg.msg_type == MSG_PRIVATE) {
            printf("Private message from client %d to client %d\n",
                   msg.sender_id, msg.receiver_id);
            
            send_to_client(msg.receiver_id, &msg);
        }
    }
    
    return 0;
}