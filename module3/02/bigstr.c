#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    
    char *longest = argv[1];
    
    for (int i = 2; i < argc; i++) {
        if (strlen(argv[i]) > strlen(longest)) {
            longest = argv[i];
        }
    }
    
    printf("%s\n", longest);
    return 0;
}