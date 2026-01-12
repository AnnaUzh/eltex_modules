#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>

#define BUFFER_SIZE 256
#define DUMP_FILE "packet_dump.bin"
#define LOG_FILE "sniffer_log.txt"
#define PORTS_COUNT 2
#define TARGET_PORTS {9090, 9091}

volatile sig_atomic_t stop_sniffing = 0;

// Структура для заголовка пакета
struct ip_header {
    unsigned char ihl:4;
    unsigned char version:4;
    unsigned char tos;
    unsigned short tot_len;
    unsigned short id;
    unsigned short frag_off;
    unsigned char ttl;
    unsigned char protocol;
    unsigned short check;
    unsigned int saddr;
    unsigned int daddr;
};

// Обработчик сигнала для корректного завершения
void handle_signal(int sig) {
    stop_sniffing = 1;
    printf("\nЗавершение работы сниффера...\n");
}

// Функция для записи дампа пакета в бинарный файл
void dump_packet_to_file(FILE *dump_file, const unsigned char *buffer, int size) {
    fwrite(&size, sizeof(int), 1, dump_file);
    fwrite(buffer, 1, size, dump_file);
    fflush(dump_file);
}

// Функция для записи лога в текстовый файл
void log_packet_to_file(FILE *log_file, const struct ip_header *ip_hdr, 
                       const struct udphdr *udp_hdr, 
                       const char *payload, int payload_len,
                       const char *timestamp) {
    fprintf(log_file, "Время: %s\n", timestamp);

    fprintf(log_file, "Порт от: %d -> Порт к: %d\n", 
            ntohs(udp_hdr->source), ntohs(udp_hdr->dest));
    fprintf(log_file, "Длина пакета: %d байт\n", ntohs(ip_hdr->tot_len));
    fprintf(log_file, "Длина UDP: %d байт\n", ntohs(udp_hdr->len));
    fprintf(log_file, "Длина данных: %d байт\n", payload_len);
    
    if (payload_len > 0) {
        fprintf(log_file, "Данные (ASCII): ");
        for (int i = 0; i < payload_len && i < 100; i++) {
            if (payload[i] >= 32 && payload[i] <= 126)
                fprintf(log_file, "%c", payload[i]);
            else
                fprintf(log_file, ".");
        }
        fprintf(log_file, "\n");
    }
    fprintf(log_file, "--------------------------\n");
    fflush(log_file);
}

void analyze_dump(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Ошибка открытия файла дампа");
        return;
    }
    
    printf("Анализ бинарного дампа:\n");
    
    int packet_count = 0;
    while (!feof(file)) {
        int packet_size;
        
        if (fread(&packet_size, sizeof(int), 1, file) != 1) {
            break;
        }
        
        printf("\nПакет #%d, размер: %d байт\n", ++packet_count, packet_size);
        
        unsigned char *packet = malloc(packet_size);
        if (!packet) {
            printf("Ошибка выделения памяти\n");
            break;
        }
        
        if (fread(packet, 1, packet_size, file) != packet_size) {
            free(packet);
            break;
        }
        
        struct ip_header *ip_hdr = (struct ip_header *)packet;
        
        unsigned int ip_header_len = ip_hdr->ihl * 4;
        struct udphdr *udp_hdr = (struct udphdr *)(packet + ip_header_len);
        
        int data_length = packet_size - (ip_header_len + sizeof(struct udphdr));
        if (data_length > 0) {
            char *data = (char *)(packet + ip_header_len + sizeof(struct udphdr));
            
            printf("  Данные (%d байт): ", data_length);
            for (int i = 0; i < data_length && i < 50; i++) {
                if (data[i] >= 32 && data[i] <= 126)
                    printf("%c", data[i]);
                else
                    printf(".");
            }
            printf("\n");
        }
        
        free(packet);
    }
    
    fclose(file);
    printf("\nВсего пакетов: %d\n", packet_count);
}

// Функция для вывода информации о пакете на экран
void print_packet_info(const struct ip_header *ip_hdr, 
                      const struct udphdr *udp_hdr, 
                      const char *payload, int payload_len,
                      const char *timestamp) {
    
    printf("Время: %s\n", timestamp);
    printf("От: %d\n", ntohs(udp_hdr->source));
    printf("К: %d\n", ntohs(udp_hdr->dest));
    printf("Размер данных: %d байт\n", payload_len);
    
    if (payload_len > 0) {
        printf("Сообщение: ");
        for (int i = 0; i < payload_len; i++) {
            if (payload[i] >= 32 && payload[i] <= 126)
                printf("%c", payload[i]);
            else if (payload[i] == '\n' || payload[i] == '\r')
                printf(" ");
            else
                printf(".");
        }
        printf("\n");
    }
}

int main() {
    int raw_socket;
    unsigned char buffer[BUFFER_SIZE];
    FILE *dump_file = NULL;
    FILE *log_file = NULL;
    int target_ports[PORTS_COUNT] = TARGET_PORTS;
    
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    dump_file = fopen(DUMP_FILE, "wb");
    if (!dump_file) {
        perror("Ошибка открытия файла");
        return 1;
    }
    
    log_file = fopen(LOG_FILE, "w");
    if (!log_file) {
        perror("Ошибка открытия файла");
        fclose(dump_file);
        return 1;
    }
    
    // Создание RAW сокета
    raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (raw_socket < 0) {
        perror("Ошибка создания RAW сокета");
        fclose(dump_file);
        fclose(log_file);
        return 1;
    }
    int optval = 1;
    if (setsockopt(raw_socket, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) < 0) {
        perror("Ошибка setsockopt");
        close(raw_socket);
        fclose(dump_file);
        fclose(log_file);
        return 1;
    }
    
    while (!stop_sniffing) {
        struct sockaddr_in saddr;
        socklen_t saddr_size = sizeof(saddr);
        int packet_size;
        
        packet_size = recvfrom(raw_socket, buffer, BUFFER_SIZE, 0,
                              (struct sockaddr *)&saddr, &saddr_size);
        

        time_t now = time(NULL);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        
        // Парсинг IP заголовка
        struct ip_header *ip_hdr = (struct ip_header *)buffer;
        unsigned int ip_header_len = ip_hdr->ihl * 4;
        
        if (ip_hdr->protocol != IPPROTO_UDP) {
            continue;
        }
        
        // Парсинг UDP заголовка
        struct udphdr *udp_hdr = (struct udphdr *)(buffer + ip_header_len);
        int udp_header_len = sizeof(struct udphdr);
        
        int src_port = ntohs(udp_hdr->source);
        int dst_port = ntohs(udp_hdr->dest);
        
        int port_found = 0;
        for (int i = 0; i < PORTS_COUNT; i++) {
            if (src_port == target_ports[i] || dst_port == target_ports[i]) {
                port_found = 1;
                break;
            }
        }
        
        if (!port_found) {
            continue;
        }
        
        int total_length = ntohs(ip_hdr->tot_len);
        int data_length = total_length - (ip_header_len + udp_header_len);
        
        if (data_length <= 0) {
            continue;
        }
        
        char *payload = (char *)(buffer + ip_header_len + udp_header_len);
        
        dump_packet_to_file(dump_file, buffer, packet_size);
        
        log_packet_to_file(log_file, ip_hdr, udp_hdr, payload, data_length, timestamp);
        
        print_packet_info(ip_hdr, udp_hdr, payload, data_length, timestamp);
        
        usleep(1000);
    }

    
    
    close(raw_socket);
    fclose(dump_file);
    fclose(log_file);
    
    analyze_dump("packet_dump.bin");
    
    return 0;
}