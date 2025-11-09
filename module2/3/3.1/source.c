#include "header.h"
void print_rights(char rights[]){
    printf("Symbolic:\n");
    printf("-");
    for (int i = 1; i < 4; i++){
        if (rights[i] == '7'){
            printf("rwx");
        }
        else if (rights[i] == '6'){
            printf("rw-");
        }
        else if (rights[i] == '5'){
            printf("r-x");
        }
        else if (rights[i] == '4'){
            printf("r--");
        }
        else if (rights[i] == '3'){
            printf("-wx");
        }
        else if (rights[i] == '2'){
            printf("-w-");
        }
        else if (rights[i] == '1'){
            printf("--x");
        }
        else if (rights[i] == '0'){
            printf("---");
        }
    }
    printf("\n");
    printf("Numeric:\n");
    printf("-%c%c%c\n", rights[1], rights[2], rights[3]);
    printf("Binary:\n");
    convert_to_bin(rights);
}

int check_rights(char str[]) {   
    int len = strlen(str);
    int i = 0;
    while (i < len) {
        
        if (strchr("ugoa", str[i]) == NULL) {
            return 0;
        }
        i++;
        
        while (i < len && strchr("ugoa", str[i]) != NULL) {
            i++;
        }
        
        if (i >= len || strchr("+-=", str[i]) == NULL) {
            return 0;
        }
        i++;
        
        int has_rights = 0;
        while (i < len && strchr("rwx", str[i]) != NULL) {
            has_rights = 1;
            i++;
        }
        
        if (!has_rights) {
            return 0;
        }
    }
    
    return 1;
}

uint32_t convert_to_bin(char rights[]) {
    uint32_t result = 0;
    if (rights[0] == 'u' || rights[0] == 'g' || rights[0] == 'o' || rights[0] == 'a') {
        int user_mask = 0;
        int i = 0;

        while (rights[i] == 'u'|| rights[i] == 'g' || rights[i] == 'o' || rights[i] == 'a'){
            switch (rights[i]) {
            case 'u': user_mask |= 0700; break; // владелец
            case 'g': user_mask |= 0070; break; // группа
            case 'o': user_mask |= 0007; break; // остальные
            case 'a': user_mask |= 0777; break; // все
        }
        i++;
        }
        
        if (rights[i] == '+' || rights[i] == '-' || rights[i] == '=') {
            i++;
        }
        
        while (rights[i] != '\0') {
            switch (rights[i]) {
                case 'r': result |= (user_mask & 0444); break; // read
                case 'w': result |= (user_mask & 0222); break; // write
                case 'x': result |= (user_mask & 0111); break; // execute
            }
            i++;
        }
    } else if (isdigit(rights[0])) {
        result = strtol(rights, NULL, 8) & 0777;
    }
    
    return result;
}
void print_binary(uint32_t num) {
    printf("Битовое представление: ");
    
    for (int i = 8; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
        if (i == 6 || i == 3) printf(" ");
    }
    printf("\n");
}

void mode_to_string(uint32_t mode, char *result) {
    if (result == NULL) return;
    
    result[0] = (mode & 0400) ? 'r' : '-'; // user read
    result[1] = (mode & 0200) ? 'w' : '-'; // user write
    result[2] = (mode & 0100) ? 'x' : '-'; // user execute
    result[3] = (mode & 0040) ? 'r' : '-'; // group read
    result[4] = (mode & 0020) ? 'w' : '-'; // group write
    result[5] = (mode & 0010) ? 'x' : '-'; // group execute
    result[6] = (mode & 0004) ? 'r' : '-'; // other read
    result[7] = (mode & 0002) ? 'w' : '-'; // other write
    result[8] = (mode & 0001) ? 'x' : '-'; // other execute
    result[9] = '\0';
}

uint32_t change_permissions(uint32_t current_perms, const char* modification) {
    uint32_t new_perms = current_perms & 0x1FF;
    const char* ptr = modification;
    printf("current perms: ");
    print_binary(current_perms);
    printf("\n");
    
    while (*ptr) {
        uint32_t user_mask = 0;
        uint32_t group_mask = 0;
        uint32_t other_mask = 0;
        
        while (*ptr && *ptr != '+' && *ptr != '-' && *ptr != '=') {
            switch (*ptr) {
                case 'u': user_mask = 0x1C0; break; // 111000000
                case 'g': group_mask = 0x38; break;  // 000111000
                case 'o': other_mask = 0x7; break;   // 000000111
                case 'a': 
                    user_mask = 0x1C0;
                    group_mask = 0x38;
                    other_mask = 0x7;
                    break;
            }
            ptr++;
        }
        char operation = *ptr;
        if (operation != '+' && operation != '-' && operation != '=') {
            break;
        }
        ptr++;
        uint32_t perm_mask = 0;
        while (*ptr && *ptr != ',' && *ptr != ';') {
            switch (*ptr) {
                case 'r': perm_mask |= 0x124; break; // 100100100
                case 'w': perm_mask |= 0x92; break;  // 010010010
                case 'x': perm_mask |= 0x49; break;  // 001001001
            }
            ptr++;
        }
        printf("new perms: ");
        print_binary(new_perms);
        printf("\n");
        switch (operation) {
            case '+':
                if (user_mask) new_perms |= (perm_mask & user_mask);
                if (group_mask) new_perms |= (perm_mask & group_mask);
                if (other_mask) new_perms |= (perm_mask & other_mask);
                break;
                
            case '-':
                if (user_mask) new_perms &= ~(perm_mask & user_mask);
                if (group_mask) new_perms &= ~(perm_mask & group_mask);
                if (other_mask) new_perms &= ~(perm_mask & other_mask);
                break;
                
            case '=':
                if (user_mask) {
                    new_perms &= ~user_mask;
                    new_perms |= (perm_mask & user_mask);
                }
                if (group_mask) {
                    new_perms &= ~group_mask;
                    new_perms |= (perm_mask & group_mask);
                }
                if (other_mask) {
                    new_perms &= ~other_mask;
                    new_perms |= (perm_mask & other_mask);
                }
                break;
        }
        
        if (*ptr == ',' || *ptr == ';') {
            ptr++;
        }
    }
    
    return new_perms;
}

