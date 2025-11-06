#pragma once

#include<stdio.h>
#include<locale.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>
#define MAX_CONTACTS 1000
#define MAX_LENGTH 100
#define MAX_EMAILS 10
#define MAX_SOCIALS 20
#define MAX_MESSENGERS 20
#define MAX_PHONES 5

// Основная структура контакта
typedef struct {
    uint32_t id;
    char last_name[MAX_LENGTH];    // Фамилия (обязательное поле)
    char first_name[MAX_LENGTH];   // Имя (обязательное поле)
    char middle_name[MAX_LENGTH];  // Отчество
    char company[MAX_LENGTH];      // Место работы
    char position[MAX_LENGTH];     // Должность
    char phone[MAX_PHONES][MAX_LENGTH]; 
    int phone_count;
    char email[MAX_EMAILS][MAX_LENGTH];
    int email_count;
    char social_media[MAX_SOCIALS][MAX_LENGTH];
    int social_media_count;
    char messenger[MAX_MESSENGERS][MAX_LENGTH];
    int messenger_count;
} Contact;

// Структура для работы с телефонной книгой
typedef struct {
    Contact contacts[MAX_CONTACTS]; // Массив контактов
    int count;                      // Текущее количество контактов
} PhoneBook;

int is_empty_or_whitespace(const char *str);
void printContact(const uint32_t contact_id,const PhoneBook *phone_book);
int addContact(PhoneBook *phone_book);
void print_all_contacts(const PhoneBook *phone_book);
uint32_t name_search(PhoneBook *phonebook, char target[], int len);
uint32_t last_name_search(PhoneBook *phonebook, char target[], int len);
Contact delete_contact(PhoneBook *phone_book, uint32_t contact_id);
uint32_t search(PhoneBook *phone_book);
void read_string_with_default(const char *prompt, char *buffer, const char *default_value);
int edit_contact(PhoneBook *phone_book, uint32_t contact_id);
int is_id_exists(const PhoneBook *phone_book, int id);
uint32_t generate_unique_id(PhoneBook *phone_book);