#include "header.h"

int main(){
    char gw[16];
    char mask[16];
    int N = 0;
    printf("Enter IP address:\n");
    scanf("%s", gw);
    while (!check_ip_mask(gw)){
        printf("Incorrect IP address!\n");
        printf("Enter IP address:\n");
        scanf("%s", gw);
    }
    printf("Enter subnet mask:\n");
    scanf("%s", mask);
    while (!check_ip_mask(mask)){
        printf("Incorrect subnet mask!\n");
        printf("Enter subnet mask:\n");
        scanf("%s", gw);
    }
    printf("Enter N:\n");
    scanf("%d", &N);
    while (N < 1){
        printf("Incorrect N!\n");
        printf("Enter N:\n");
        scanf("%d", &N);
    }
    generate_ip_and_check(gw, mask, N);
    return 0;
}
