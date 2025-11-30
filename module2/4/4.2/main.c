#include "header.h"

int main() {
    
    srand((unsigned int)time(NULL));
    PriorityQueue* queue = create_priority_queue();

    printf("Добавление тестовых данных:\n");
    printf("Генерация случайных данных:\n");
    for (int i = 0; i < 25; i++) {
        double value = (double)(rand() % 1000) / 10.0; // 0.0 - 99.9
        int priority = rand() % 256; // 0-255
        enqueue(queue, value, priority);
    }
    

    while (1) {
    display_queue(queue);
    printf("Выберите функцию для работы с таблицей\n");
    printf("1 - добавление элемента\n");
    printf("2 - извлечение первого элемента\n");
    printf("3 - извлечение элемента с заданным приоритетом\n");
    printf("4 - извлечение элемента с приоритетом не ниже заданного\n");
    printf("0 - завершить работу\n");

    char answer[10];
    if (fgets(answer, sizeof(answer), stdin) == NULL) {
      continue;
    }
    
    size_t len = strlen(answer);
    if (len > 0 && answer[len-1] == '\n') {
      answer[len-1] = '\0';
      len--;
    }

    if (len != 1) {
      printf("Введите корректную команду\n");
      continue;
    }
    if (answer[0] < '0' || answer[0] > '4') {
      printf("Введите корректную команду\n");
      continue;
    }
    
    int ans = answer[0] - '0';
    switch (ans) {
    case 1:
int pri;
double val;
char input[50];
    printf("Введите приоритет и число: \n");
    if (fgets(input, sizeof(input), stdin) == NULL) continue;
    
    if (sscanf(input, "%d %lf", &pri, &val) == 2) {
        enqueue(queue, val, pri);
        break;
    }
      break;
    case 2:
        Node* node = dequeue_first(queue);
    if (node != NULL) {
        printf("Извлечен первый элемент: значение=%.2f, приоритет=%d\n", 
               node->value, node->priority);
        free(node);
    }
      break;
    case 3:
        int num;
    printf("Введите приоритет: \n");
    if (fgets(input, sizeof(input), stdin) == NULL) continue;
    if (sscanf(input, "%d", &num) != 1 || num < 0 || num > 255) {
        printf("Error \n");
        break;
    }
        node = dequeue_by_priority(queue, num);
    if (node != NULL) {
        printf("Извлечен элемент: значение=%.2f\n", node->value);
        free(node);
    } else {
        printf("Элемент не найден\n");
    }
      break;
    case 4:
    printf("Введите приоритет: \n");
    if (fgets(input, sizeof(input), stdin) == NULL) continue;
    if (sscanf(input, "%d", &num) != 1 || num < 0 || num > 255) {
        printf("Error \n");
        break;
    }
    node = dequeue_min_priority(queue, num);
    if (node != NULL) {
        printf("Извлечен элемент: значение=%.2f, приоритет=%d\n", 
               node->value, node->priority);
        free(node);
    } else {
        printf("Элемент не найден\n");
    }
      break;
    case 0:
    return 0;
      break;
    }
  }
    
    return 0;
}