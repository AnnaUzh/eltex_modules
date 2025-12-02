#include "head.h"
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>

typedef struct {
    char name[50];
    char symbol[10];
    double (*func)(double, double);
    void* handle;
} DynamicOperation;

DynamicOperation operations[20];
int operation_count = 0;

void load_operations() {
    DIR* dir = opendir(".");
    struct dirent* entry;
    
    if (!dir) {
        printf("Ошибка: не могу открыть текущую папку!\n");
        return;
    }
    
    struct {
        char* name;
        char* symbol;
    } func_symbols[] = {
        {"add", "+"},
        {"subtract", "-"},
        {"multiply", "*"},
        {"divide", "/"},
        {"power", "^"}
    };
    
    while ((entry = readdir(dir)) != NULL && operation_count < 20) {
        char* dot = strstr(entry->d_name, ".so");
        if (dot && strlen(dot) == 3) {
            char func_name[50];
            strncpy(func_name, entry->d_name, dot - entry->d_name);
            func_name[dot - entry->d_name] = '\0';
            
            char lib_path[256];
            snprintf(lib_path, sizeof(lib_path), "./%.200s", entry->d_name);
            
            void* handle = dlopen(lib_path, RTLD_LAZY);
            if (!handle) {
                continue;
            }
    
            double (*func_ptr)(double, double) = dlsym(handle, func_name);
            if (!func_ptr) {
                dlclose(handle);
                continue;
            }
            
            strcpy(operations[operation_count].name, func_name);
            operations[operation_count].func = func_ptr;
            operations[operation_count].handle = handle;
            
            strcpy(operations[operation_count].symbol, "?");
            for (int i = 0; i < 5; i++) {
                if (strcmp(func_name, func_symbols[i].name) == 0) {
                    strcpy(operations[operation_count].symbol, func_symbols[i].symbol);
                    break;
                }
            }
            
            operation_count++;
        }
    }
    
    closedir(dir);
}


void cleanup_operations() {
    for (int i = 0; i < operation_count; i++) {
        dlclose(operations[i].handle);
    }
}


void show_menu() {
    printf("\n==================================================\n");
    printf("                      МЕНЮ\n");
    printf("==================================================\n");
    
    // Выводим все загруженные операции
    for (int i = 0; i < operation_count; i++) {
        printf("%d. %s (%s)\n", i + 1, operations[i].name, operations[i].symbol);
    }
    
    printf("%d. Выход\n", operation_count + 1);
    printf("==================================================\n");
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int choice;
    double num1, num2, result;

    load_operations();
    
    if (operation_count == 0) {
        printf("\nОШИБКА: Не загружено ни одной операции!\n");
        printf("Убедитесь, что в папке есть файлы:\n");
        printf("  add.so, subtract.so, multiply.so, divide.so, power.so\n");
        printf("Создайте их с помощью Makefile:\n");
        printf("  make clean\n");
        printf("  make\n");
        return 1;
    }
    
    printf("\nДобро пожаловать в калькулятор!\n");
    printf("Загружено %d операций\n", operation_count);
    
    while (1) {
        show_menu();
        printf("\nВыберите операцию (1-%d): ", operation_count + 1);
        
        if (scanf("%d", &choice) != 1) {
            printf("Ошибка ввода!\n");
            clear_input_buffer();
            continue;
        }
        
        clear_input_buffer();
        
        // Выход
        if (choice == operation_count + 1) {
            printf("Выход из программы.\n");
            break;
        }
        
        // Проверка диапазона
        if (choice < 1 || choice > operation_count) {
            printf("Неверный выбор! Выберите от 1 до %d\n", operation_count);
            continue;
        }
        
        // Ввод чисел
        printf("Введите первое число: ");
        if (scanf("%lf", &num1) != 1) {
            printf("Ошибка ввода числа!\n");
            clear_input_buffer();
            continue;
        }
        
        printf("Введите второе число: ");
        if (scanf("%lf", &num2) != 1) {
            printf("Ошибка ввода числа!\n");
            clear_input_buffer();
            continue;
        }
        
        clear_input_buffer();
        
        // Проверка деления на ноль (только для divide)
        if (strcmp(operations[choice - 1].name, "divide") == 0 && num2 == 0) {
            printf("Ошибка: деление на ноль!\n");
            continue;
        }
        
        // Выполнение операции
        int operation_index = choice - 1;
        result = operations[operation_index].func(num1, num2);
        
        // Красивый вывод
        printf("Результат: %.2lf %s %.2lf = %.2lf\n", 
               num1, operations[operation_index].symbol, num2, result);
    }
    
    cleanup_operations();
    
    return 0;
}