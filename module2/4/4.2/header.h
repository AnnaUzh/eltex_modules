#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define MAX_PRIORITY 255
#define MIN_PRIORITY 0
#define MAX_MSG_LENGTH 100
#define INITIAL_CAPACITY 10

// Структура для узла очереди
typedef struct Node {
    int priority;
    double value;
    struct Node* next;
} Node;

// Структура для очереди с приоритетом
typedef struct {
    Node* front;
    Node* rear;
    int size;
} PriorityQueue;

// Прототипы функций
PriorityQueue* create_priority_queue();
void destroy_priority_queue(PriorityQueue* queue);
int is_empty(PriorityQueue* queue);
int get_size(PriorityQueue* queue);
int enqueue(PriorityQueue* queue, double content, int priority);
Node* dequeue_first(PriorityQueue* queue);
Node* dequeue_by_priority(PriorityQueue* queue, int priority);
Node* dequeue_min_priority(PriorityQueue* queue, int min_priority);
void display_queue(PriorityQueue* queue);
int is_valid_priority(int priority);

// Вспомогательные функции для тестирования
void generate_random_message(char* buffer, int buffer_size);
void test_basic_operations();
void test_advanced_scenarios();
void simulate_real_time_workload();