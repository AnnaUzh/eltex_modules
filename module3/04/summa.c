#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    double total = 0.0;
    
    for (int i = 1; i < argc; i++) {
        char *endptr;
        double num = strtod(argv[i], &endptr);
        
        if (*endptr == '\0') {
            total += num;
        }
    }
    
    printf("Сумма: %f\n", total);
    return 0;
}