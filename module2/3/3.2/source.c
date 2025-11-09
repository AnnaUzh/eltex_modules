#include "header.h"

void generate_ip_and_check(char ipv[], char subnet_mask[], int n){
    int numeric_ipv[4], numeric_mask[4];
    
    char ip_copy[100], mask_copy[100];
    strcpy(ip_copy, ipv);
    strcpy(mask_copy, subnet_mask);
    
    char *token = strtok(ip_copy, ".");
    for (int i = 0; i < 4 && token != NULL; i++) {
        numeric_ipv[i] = atoi(token);
        token = strtok(NULL, ".");
    }
    
    token = strtok(mask_copy, ".");
    for (int i = 0; i < 4 && token != NULL; i++) {
        numeric_mask[i] = atoi(token);
        token = strtok(NULL, ".");
    }
    
    int network_ip[4];
    for (int i = 0; i < 4; i++) {
        network_ip[i] = numeric_ipv[i] & numeric_mask[i];
    }
    
    int count_to_local = 0, count_to_other = 0;
    
    for (int i = 0; i < n; i++) {
        int new_ip[4];
        
        new_ip[0] = numeric_ipv[0];
        new_ip[1] = numeric_ipv[1];
        if (i%2 == 0) new_ip[2] = rand() % 256;
        else new_ip[2] =  numeric_ipv[2];
        new_ip[3] = rand() % 256;
        
        int same_network = 1;
        for (int j = 0; j < 4; j++) {
            if ((new_ip[j] & numeric_mask[j]) != network_ip[j]) {
                same_network = 0;
                break;
            }
        }
        
        if (same_network) {
            count_to_local++;
        } else {
            count_to_other++;
        }
    }
    
    double local_percent = (double)count_to_local / n * 100;
    double other_percent = (double)count_to_other / n * 100;
    
    printf("To local:\n");
    printf("Packages: %d - Percent: %.1lf%%\n", count_to_local, local_percent);
    printf("To other:\n");
    printf("Packages: %d - Percent: %.1lf%%\n", count_to_other, other_percent);
}

char check_ip_mask(char ip[]){
    char ip_copy[100];
    strcpy(ip_copy, ip);
    
    char *token = strtok(ip_copy, ".");
    int dot_count = 0;
    int octet_count = 0;
    
    while (token != NULL) {
        octet_count++;
        dot_count = octet_count - 1;
        
        for (int i = 0; token[i] != '\0'; i++) {
            if (token[i] < '0' || token[i] > '9') {
                return 0;
            }
        }
        
        int octet = atoi(token);
        if (octet < 0 || octet > 255) {
            return 0;
        }
        
        if (strlen(token) > 1 && token[0] == '0') {
            return 0;
        }
        
        token = strtok(NULL, ".");
    }
    
    return (octet_count == 4) ? 1 : 0;
}