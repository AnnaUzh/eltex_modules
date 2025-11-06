#include "head.h"


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
    int int_num;
    
    printf("Добро пожаловать в калькулятор!\n");
    
    while (1) {
        show_menu();
        printf("\nВыберите операцию (1-6): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Ошибка ввода!\n");
            clear_input_buffer();
            continue;
        }
        
        clear_input_buffer();
        
        if (choice == 6) {
            break;
        }
        
        switch (choice) {
            case 1: // Сложение
                printf("Введите первое число: ");
                scanf("%lf", &num1);
                printf("Введите второе число: ");
                scanf("%lf", &num2);
                clear_input_buffer();
                result = add(num1, num2);
                printf("Результат: %.2lf + %.2lf = %.2lf\n", num1, num2, result);
                break;
                
            case 2: // Вычитание
                printf("Введите первое число: ");
                scanf("%lf", &num1);
                printf("Введите второе число: ");
                scanf("%lf", &num2);
                clear_input_buffer();
                result = subtract(num1, num2);
                printf("Результат: %.2lf - %.2lf = %.2lf\n", num1, num2, result);
                break;
                
            case 3: // Умножение
                printf("Введите первое число: ");
                scanf("%lf", &num1);
                printf("Введите второе число: ");
                scanf("%lf", &num2);
                clear_input_buffer();
                result = multiply(num1, num2);
                printf("Результат: %.2lf * %.2lf = %.2lf\n", num1, num2, result);
                break;
                
            case 4: // Деление
                printf("Введите первое число: ");
                scanf("%lf", &num1);
                printf("Введите второе число: ");
                scanf("%lf", &num2);
                clear_input_buffer();
                result = divide(num1, num2);
                if (num2 != 0) {
                    printf("Результат: %.2lf / %.2lf = %.2lf\n", num1, num2, result);
                }
                break;
                
            case 5: // Возведение в степень
                printf("Введите основание: ");
                scanf("%lf", &num1);
                printf("Введите степень: ");
                scanf("%lf", &num2);
                clear_input_buffer();
                result = power(num1, num2);
                printf("Результат: %.2lf ^ %.2lf = %.2lf\n", num1, num2, result);
                break;
                
            default:
                printf("Ошибка: неверный выбор! Введите число от 1 до 8.\n");
                break;
        }
        
        printf("\nНажмите Enter для продолжения...");
        getchar(); // Ожидание нажатия Enter
    }
    
    return 0;
}