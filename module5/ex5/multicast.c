#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define FAMILY_ID 35

int main() {

    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    
    struct sockaddr_nl addr;
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();  
    addr.nl_groups = 1;
    

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return 1;
    }
    
    char buf[4096];
    while (1) {
        int len = recv(sock, buf, sizeof(buf), 0);
        if (len > 0) {
            struct nlmsghdr *nlh = (struct nlmsghdr *)buf;
            
            printf("Получено multicast сообщение\n");
            
            if (nlh->nlmsg_type == FAMILY_ID) {
                printf("Hi genltest!\n");
            }
        } else if (len < 0) {
            perror("recv");
            break;
        }
    }
    
    close(sock);
    return 0;
}