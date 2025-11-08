#include "head.h"

// Объявляем тип для указателя на функцию
typedef double (*operation_func)(double, double);

void show_menu() {
    printf("\n==================================================\n");
    printf("                      МЕНЮ\n");
    printf("==================================================\n");
    printf("1. Сложение (+)\n");
    printf("2. Вычитание (-)\n");
    printf("3. Умножение (*)\n");
    printf("4. Деление (/)\n");
    printf("5. Возведение в степень (^)\n");
    printf("6. Выход\n");
    printf("==================================================\n");
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int choice;
    double num1, num2, result;
    
    operation_func operations[] = {
        add,     
        subtract,  
        multiply,
        divide,  
        power    
    };
    
    
    
    printf("Добро пожаловать в калькулятор!\n");
    
    while (1) {
        show_menu();
        printf("\nВыберите операцию (1-6): ");
        
        if (scanf("%d", &choice) != 1 || choice < 1 || choice > 6) {
            printf("Ошибка ввода!\n");
            clear_input_buffer();
            continue;
        }
        
        clear_input_buffer();
        
        if (choice == 6) {
            printf("Выход из программы.\n");
            break;
        }
        
        
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
        
        if (choice == 4 && num2 == 0) {
            printf("Ошибка: деление на ноль!\n");
            continue;
        }
        
        int operation_index = choice - 1;
        result = operations[operation_index](num1, num2);
        
        printf("%.2lf\n", result);
    }
    
    return 0;
}