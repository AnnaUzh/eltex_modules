#include "header.h"

// Создание новой очереди
PriorityQueue* create_priority_queue() {
    PriorityQueue* queue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (queue == NULL) {
        return NULL;
    }
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

// Уничтожение очереди и освобождение памяти
void destroy_priority_queue(PriorityQueue* queue) {
    if (queue == NULL) return;
    
    Node* current = queue->front;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
    free(queue);
}

// Проверка, пуста ли очередь
int is_empty(PriorityQueue* queue) {
    return queue == NULL || queue->front == NULL;
}

// Получение размера очереди
int get_size(PriorityQueue* queue) {
    if (queue == NULL) return 0;
    return queue->size;
}

// Проверка валидности приоритета
int is_valid_priority(int priority) {
    return priority >= MIN_PRIORITY && priority <= MAX_PRIORITY;
}

// Добавление элемента в очередь с сохранением порядка приоритетов
int enqueue(PriorityQueue* queue, double content, int priority) {
    if (queue == NULL) {
        return 0;
    }
    
    if (!is_valid_priority(priority)) {
        printf("Ошибка: приоритет %d вне диапазона %d-%d\n", 
               priority, MIN_PRIORITY, MAX_PRIORITY);
        return 0;
    }
    
    // Создание нового узла
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        return 0;
    }
    
    // Заполнение данных сообщения
    new_node->value = content;
    new_node->priority = priority;
    new_node->next = NULL;
    
    // Вставка в конец очереди
    if (is_empty(queue)) {
        // Очередь пуста - новый элемент становится первым и последним
        queue->front = new_node;
        queue->rear = new_node;
    } else {
        // Вставляем в конец
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
    
    queue->size++;
    return 1;
}

// Извлечение первого элемента очереди
Node* dequeue_first(PriorityQueue* queue) {
    if (is_empty(queue)) {
        return NULL;
    }
    
    Node* temp = queue->front;
    
    // Обновляем указатели очереди
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    
    temp->next = NULL; // Отсоединяем узел от очереди
    queue->size--;
    
    return temp;
}

// Извлечение элемента с указанным приоритетом
Node* dequeue_by_priority(PriorityQueue* queue, int priority) {
    if (is_empty(queue) || !is_valid_priority(priority)) {
        return NULL;
    }
    
    Node* current = queue->front;
    Node* previous = NULL;
    
    // Поиск элемента с заданным приоритетом
    while (current != NULL && current->priority != priority) {
        previous = current;
        current = current->next;
    }
    
    if (current == NULL) {
        return NULL; // Элемент не найден
    }
    
    // Удаление узла из списка
    if (previous == NULL) {
        // Удаляем первый элемент
        queue->front = current->next;
    } else {
        previous->next = current->next;
    }
    
    // Если удаляем последний элемент, обновляем rear
    if (current == queue->rear) {
        queue->rear = previous;
    }
    
    current->next = NULL; // Отсоединяем узел от очереди
    queue->size--;
    
    return current;
}
// Извлечение элемента с приоритетом не ниже заданного
Node* dequeue_min_priority(PriorityQueue* queue, int min_priority) {
    if (is_empty(queue) || !is_valid_priority(min_priority)) {
        return NULL;
    }
    
    Node* current = queue->front;
    Node* previous = NULL;
    
    // Поиск первого элемента с приоритетом >= min_priority
    while (current != NULL && current->priority < min_priority) {
        previous = current;
        current = current->next;
    }
    
    if (current == NULL) {
        return NULL; // Подходящий элемент не найден
    }
    
    // Удаление узла из списка
    if (previous == NULL) {
        // Удаляем первый элемент
        queue->front = current->next;
    } else {
        previous->next = current->next;
    }
    
    // Если удаляем последний элемент, обновляем rear
    if (current == queue->rear) {
        queue->rear = previous;
    }
    
    current->next = NULL; // Отсоединяем узел от очереди
    queue->size--;
    
    return current;
}

// Отображение содержимого очереди
void display_queue(PriorityQueue* queue) {
    if (queue == NULL || is_empty(queue)) {
        printf("Очередь пуста\n");
        return;
    }
    
    printf("Содержимое очереди (всего элементов: %d):\n", queue->size);
    Node* current = queue->front;
    int position = 1;
    
    while (current != NULL) {
        printf("  %d. Приоритет: %3d | Значение: %.2f\n", 
               position++, current->priority, current->value);
        current = current->next;
    }
}
