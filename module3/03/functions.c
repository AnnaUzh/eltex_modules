#include "struct.h"

int is_empty_or_whitespace(const char *str) {
  if (str == NULL || str[0] == '\0') {
    return 1;
  }

  for (int i = 0; str[i] != '\0'; i++) {
    if (!isspace((unsigned char)str[i])) {
      return 0;
    }
  }
  return 1;
}

void init_phonebook(PhoneBook *pb) {
    pb->head = NULL;
    pb->tail = NULL;
    pb->count = 0;
    pb->next_id = 1;
}

void printContact(Contact *current, const PhoneBook *phone_book) {
      printf("----------------------------------------\n");
      printf("Фамилия:     %s\n", current->last_name);
      printf("Имя:         %s\n", current->first_name);

      if (strlen(current->middle_name) > 0) {
        printf("Отчество:    %s\n", current->middle_name);
      }

      if (strlen(current->company) > 0) {
        printf("Место работы: %s\n", current->company);
      }

      if (strlen(current->position) > 0) {
        printf("Должность:    %s\n", current->position);
      }
      if (current->phone_count > 0) {
        for (int j = 0; j < current->phone_count; j++) {
          printf("Телефон %d:      %s\n", j + 1,
                 current->phone[j]);
        }
      }

      if (current->email_count > 0) {
        for (int j = 0; j < current->email_count; j++) {
          printf("Email%d:        %s\n", j + 1,
                 current->email[j]);
        }
      }

      if (current->social_media_count > 0) {
        for (int j = 0; j < current->social_media_count; j++) {
          printf("Соцсети%d:        %s\n", j + 1,
                 current->social_media[j]);
        }
      }

      if (current->messenger_count > 0) {
        for (int j = 0; j < current->messenger_count; j++) {
          printf("Мессенджер%d:        %s\n", j + 1,
                 current->messenger[j]);
        }
      }
      printf("----------------------------------------\n");
}

int addContact(PhoneBook *phone_book) {
  if (phone_book == NULL || phone_book->count >= MAX_CONTACTS) {
    printf("Ошибка: невозможно создать контакт\n");
    return -1;
  }

  Contact *new_contact = (Contact *)malloc(sizeof(Contact)); // Инициализируем нулями
  new_contact->id = generate_unique_id(phone_book);
  printf("\n=== Создание нового контакта ===\n");

  // Ввод обязательных полей
  printf("Фамилия*: ");
  fgets(new_contact->last_name, MAX_LENGTH, stdin);
  new_contact->last_name[strcspn(new_contact->last_name, "\n")] = '\0';

  printf("Имя*: ");
  fgets(new_contact->first_name, MAX_LENGTH, stdin);
  new_contact->first_name[strcspn(new_contact->first_name, "\n")] = '\0';

  // Проверка обязательных полей
  if (is_empty_or_whitespace(new_contact->last_name) ||
      is_empty_or_whitespace(new_contact->first_name)) {
    printf("Ошибка: фамилия и имя являются обязательными полями\n");
    return -1;
  }

  // Ввод необязательных полей
  printf("Отчество: ");
  fgets(new_contact->middle_name, MAX_LENGTH, stdin);
  new_contact->middle_name[strcspn(new_contact->middle_name, "\n")] = '\0';

  printf("Место работы: ");
  fgets(new_contact->company, MAX_LENGTH, stdin);
  new_contact->company[strcspn(new_contact->company, "\n")] = '\0';

  printf("Должность: ");
  fgets(new_contact->position, MAX_LENGTH, stdin);
  new_contact->position[strcspn(new_contact->position, "\n")] = '\0';

  // Ввод множественных телефонов
  printf(
      "=== Номера телефонов (максимум %d, пустая строка для завершения) ===\n",
      MAX_PHONES);
  new_contact->phone_count = 0;
  for (int i = 0; i < MAX_PHONES; i++) {
    char temp[MAX_LENGTH];
    printf("Телефон %d: ", i + 1);
    fgets(temp, MAX_LENGTH, stdin);
    temp[strcspn(temp, "\n")] = '\0';

    if (strlen(temp) == 0) {
      break;
    }

    strncpy(new_contact->phone[i], temp, MAX_LENGTH - 1);
    new_contact->phone[i][MAX_LENGTH - 1] = '\0';
    new_contact->phone_count++;
  }

  // Ввод множественных email
  printf("=== Email адреса (максимум %d, пустая строка для завершения) ===\n",
         MAX_EMAILS);
  new_contact->email_count = 0;
  for (int i = 0; i < MAX_EMAILS; i++) {
    char temp[MAX_LENGTH];
    printf("Email %d: ", i + 1);
    fgets(temp, MAX_LENGTH, stdin);
    temp[strcspn(temp, "\n")] = '\0';

    if (strlen(temp) == 0) {
      break;
    }

    strncpy(new_contact->email[i], temp, MAX_LENGTH - 1);
    new_contact->email[i][MAX_LENGTH - 1] = '\0';
    new_contact->email_count++;
  }

  // Ввод множественных соцсетей
  printf("=== Соцсети (максимум %d, пустая строка для завершения) ===\n",
         MAX_SOCIALS);
  new_contact->social_media_count = 0;
  for (int i = 0; i < MAX_SOCIALS; i++) {
    char temp[MAX_LENGTH];
    printf("Соцсеть %d: ", i + 1);
    fgets(temp, MAX_LENGTH, stdin);
    temp[strcspn(temp, "\n")] = '\0';

    if (strlen(temp) == 0) {
      break;
    }

    strncpy(new_contact->social_media[i], temp, MAX_LENGTH - 1);
    new_contact->social_media[i][MAX_LENGTH - 1] = '\0';
    new_contact->social_media_count++;
  }

  // Ввод множественных мессенджеров
  printf("=== Мессенджеры (максимум %d, пустая строка для завершения) ===\n",
         MAX_MESSENGERS);
  new_contact->messenger_count = 0;
  for (int i = 0; i < MAX_MESSENGERS; i++) {
    char temp[MAX_LENGTH];
    printf("Мессенджер %d: ", i + 1);
    fgets(temp, MAX_LENGTH, stdin);
    temp[strcspn(temp, "\n")] = '\0';

    if (strlen(temp) == 0) {
      break;
    }

    strncpy(new_contact->messenger[i], temp, MAX_LENGTH - 1);
    new_contact->messenger[i][MAX_LENGTH - 1] = '\0';
    new_contact->messenger_count++;
  }

  // Копируем контакт в телефонную книгу
  // phone_book->contacts[phone_book->count] = new_contact;
  if (phone_book->count > 0) {
    phone_book->tail->next = new_contact;
    new_contact->prev = phone_book->tail;
    phone_book->tail = new_contact;
} else {
    phone_book->head = new_contact;
    phone_book->tail = new_contact;
}
phone_book->count++;

  printf("Контакт успешно создан! Всего контактов: %d\n", phone_book->count);
  return 0;
}

void print_all_contacts(const PhoneBook *phone_book) {
  if (phone_book == NULL) {
    printf("Ошибка: телефонная книга не инициализирована\n");
    return;
  }

  if (phone_book->count == 0) {
    printf("Телефонная книга пуста\n");
    return;
  }

  printf("\n=== ТЕЛЕФОННАЯ КНИГА ===\n");
  printf("Всего контактов: %d\n\n", phone_book->count);
  Contact *current = phone_book->head;
  for (int i = 0; i < phone_book->count && current != NULL; i++, current = current->next) {
    printf("─── КОНТАКТ #%d ───\n", i + 1);
    printContact(current, phone_book);
    printf("\n");
  }
}

uint32_t name_search(PhoneBook *phonebook, char target[], int len) {
  if (phonebook == NULL || phonebook->head == NULL) {
    printf("Телефонная книга пуста\n");
    return 0;
  }
  float max_sim = 0;
  uint32_t max_sim_contact = phonebook->head->id;
  Contact *current = phonebook->head;
  for (int i = 0; i < MAX_CONTACTS && current != NULL; i++, current = current->next) {
    float similar = 0, total = 0;
    for (int j = 0; current->first_name[j] != '\0'; j++) {
      total++;
      if (j >= len)
        continue;

      if (current->first_name[j] == target[j])
        similar++;
    }
    if (len > total)
      total = len;

    if (similar == total)
      return current->id;
    if (similar / total > max_sim) {
      max_sim = similar / total;
      max_sim_contact = current->id;
    }
  }
  return max_sim_contact;
}
uint32_t last_name_search(PhoneBook *phone_book, char target[], int len) {
  if (phone_book == NULL || phone_book->head == NULL) {
    printf("Телефонная книга пуста\n");
    return 0;
  }
Contact *current = phone_book->head;
  printf("Проверяем контакт с id: %u\n", current->id);
  float max_sim = 0;
  uint32_t max_sim_contact = phone_book->head->id;
  
  for (int i = 0; i < MAX_CONTACTS && current != NULL; i++, current = current->next) {
    float similar = 0, total = 0;
    for (int j = 0; current->last_name[j] != '\0'; j++) {
      total++;
      if (j >= len)
        continue;

      if (current->last_name[j] == target[j])
        similar++;
    }
    if (len > total)
      total = len;

    if (similar == total)
      return current->id;
    if (similar / total > max_sim) {
      max_sim = similar / total;
      max_sim_contact = current->id;
    }
  }
  return max_sim_contact;
}


Contact delete_contact(PhoneBook *phone_book, uint32_t contact_id) {
    Contact deleted_contact = {0};
    
    if (phone_book == NULL || phone_book->head == NULL) {
        printf("Телефонная книга пуста\n");
        return deleted_contact;
    }
    
    Contact *current = phone_book->head;
    
    for (int j = 0; j < phone_book->count; j++) {
        if (current->id == contact_id) {
            deleted_contact = *current; // Сохраняем копию
            
            // УДАЛЕНИЕ ПЕРВОГО ЭЛЕМЕНТА
            if (current == phone_book->head) {
                phone_book->head = current->next;
                if (phone_book->head != NULL) {
                    phone_book->head->prev = NULL;
                }
            }
            // УДАЛЕНИЕ ПОСЛЕДНЕГО ЭЛЕМЕНТА
            else if (current->next == NULL) {
                if (current->prev != NULL) {
                    current->prev->next = NULL;
                }
            }
            // УДАЛЕНИЕ ЭЛЕМЕНТА ИЗ СЕРЕДИНЫ
            else {
                if (current->prev != NULL) {
                    current->prev->next = current->next;
                }
                if (current->next != NULL) {
                    current->next->prev = current->prev;
                }
            }
            
            phone_book->count--;
            printf("Контакт успешно удален\n");
            
            // Если список стал пустым
            if (phone_book->count == 0) {
                phone_book->head = NULL;
            }
            
            return deleted_contact;
        }
        current = current->next;
    }
    
    printf("Контакт с ID %u не найден\n", contact_id);
    return deleted_contact;
}

uint32_t search(PhoneBook *phone_book) {
  int ans;
  while (1) {
    printf("Выберите поле\n");
    printf("1-фамилия\n");
    printf("2-имя\n");

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
    if (answer[0] < '1' || answer[0] > '2') {
      printf("Введите корректную команду\n");
      continue;
    }
    ans = answer[0] - '0';
    break;
  }

  uint32_t result = 0;
  char search_str[15];
  
  switch (ans) {
    case 1:
      printf("Введите фамилию: ");
      if (fgets(search_str, sizeof(search_str), stdin) == NULL) {
        break;
      }
      size_t len = strlen(search_str);
      if (len > 0 && search_str[len-1] == '\n') {
        search_str[len-1] = '\0';
        len--;
      }

      result = last_name_search(phone_book, search_str, len);
      break;
      
    case 2:
      printf("Введите имя: ");
      if (fgets(search_str, sizeof(search_str), stdin) == NULL) {
        break;
      }
      
      len = strlen(search_str);
      if (len > 0 && search_str[len-1] == '\n') {
        search_str[len-1] = '\0';
        len--;
      }

      result = name_search(phone_book, search_str, len);
      break;
  }
  return result;
}

void read_string_with_default(const char *prompt, char *buffer,
                              const char *default_value) {
  printf("%s", prompt);
  if (default_value != NULL && strlen(default_value) > 0) {
    printf(" [%s]: ", default_value);
  } else {
    printf(": ");
  }

  fgets(buffer, MAX_LENGTH, stdin);
  buffer[strcspn(buffer, "\n")] = '\0';

  if (strlen(buffer) == 0 && default_value != NULL) {
    strncpy(buffer, default_value, MAX_LENGTH - 1);
    buffer[MAX_LENGTH - 1] = '\0';
  }
}

int edit_contact(PhoneBook *phone_book, uint32_t contact_id) {
  if (phone_book == NULL) {
    printf("Ошибка: телефонная книга не инициализирована\n");
    return -1;
  }

  if (phone_book->count == 0) {
    printf("Телефонная книга пуста\n");
    return -1;
  }
  Contact *current = phone_book->head;
  for (int i = 0; i < phone_book->count; i++, current = current->next) {
    if (current->id == contact_id) {
      char temp_buffer[MAX_LENGTH];

      printf("\n=== РЕДАКТИРОВАНИЕ КОНТАКТА ===\n");
      printf("Текущие данные контакта:\n");
      printf("\n");

      printf("Введите новые данные (оставьте пустым для сохранения текущего "
             "значения):\n");

      do {
        read_string_with_default("Фамилия*", temp_buffer, current->last_name);
        if (strlen(temp_buffer) == 0) {
          printf("Фамилия не может быть пустой!\n");
        }
      } while (strlen(temp_buffer) == 0);
      strncpy(current->last_name, temp_buffer, MAX_LENGTH - 1);
      current->last_name[MAX_LENGTH - 1] = '\0';

      do {
        read_string_with_default("Имя*", temp_buffer, current->first_name);
        if (strlen(temp_buffer) == 0) {
          printf("Имя не может быть пустым!\n");
        }
      } while (strlen(temp_buffer) == 0);
      strncpy(current->first_name, temp_buffer, MAX_LENGTH - 1);
      current->first_name[MAX_LENGTH - 1] = '\0';

      // Редактирование необязательных полей
      read_string_with_default("Отчество", temp_buffer,
                               current->middle_name);
      strncpy(current->middle_name, temp_buffer, MAX_LENGTH - 1);
      current->middle_name[MAX_LENGTH - 1] = '\0';

      read_string_with_default("Место работы", temp_buffer,
                               current->company);
      strncpy(current->company, temp_buffer, MAX_LENGTH - 1);
      current->company[MAX_LENGTH - 1] = '\0';

      read_string_with_default("Должность", temp_buffer,
                               current->position);
      strncpy(current->position, temp_buffer, MAX_LENGTH - 1);
      current->position[MAX_LENGTH - 1] = '\0';

      // Редактирование множественных телефонов
      printf("\n=== Редактирование телефонов ===\n");
      for (int j = 0; j < current->phone_count; j++) {
        printf("Телефон %d: ", j + 1);
        read_string_with_default("", temp_buffer,
                                 current->phone[j]);
        strncpy(current->phone[j], temp_buffer, MAX_LENGTH - 1);
        current->phone[j][MAX_LENGTH - 1] = '\0';
      }

      // Редактирование множественных email
      printf("\n=== Редактирование email ===\n");
      for (int j = 0; j < current->email_count; j++) {
        printf("Email %d: ", j + 1);
        read_string_with_default("", temp_buffer,
                                 current->email[j]);
        strncpy(current->email[j], temp_buffer, MAX_LENGTH - 1);
        current->email[j][MAX_LENGTH - 1] = '\0';
      }

      // Редактирование множественных соцсетей
      printf("\n=== Редактирование соцсетей ===\n");
      for (int j = 0; j < current->social_media_count; j++) {
        printf("Соцсеть %d: ", j + 1);
        read_string_with_default("", temp_buffer,
                                 current->social_media[j]);
        strncpy(current->social_media[j], temp_buffer,
                MAX_LENGTH - 1);
        current->social_media[j][MAX_LENGTH - 1] = '\0';
      }

      // Редактирование множественных мессенджеров
      printf("\n=== Редактирование мессенджеров ===\n");
      for (int j = 0; j < current->messenger_count; j++) {
        printf("Мессенджер %d: ", j + 1);
        read_string_with_default("", temp_buffer,
                                 current->messenger[j]);
        strncpy(current->messenger[j], temp_buffer,
                MAX_LENGTH - 1);
        current->messenger[j][MAX_LENGTH - 1] = '\0';
      }
      printf("\nКонтакт успешно обновлен!\n");
      printf("Новые данные:\n");
      // printContact(contact);
      return 0;
    }
  }

  return 1;
}

int is_id_exists(const PhoneBook *phone_book, int id) {
  if (phone_book == NULL || id < 0) {
    return 0;
  }
  Contact *current = phone_book->head;
  for (int i = 0; i < phone_book->count; i++, current = current->next) {
    if (current->id == id) {
      return 1;
    }
  }
  return 0;
}

uint32_t generate_unique_id(PhoneBook *phone_book) {
  if (phone_book == NULL) {
    return -1;
  }
  uint32_t new_id;

  do {
    new_id = (uint32_t)(time(NULL) & 0xFFFF) | (rand() << 16);
  } while (is_id_exists(phone_book, new_id));

  return new_id;
}

int safe_strcmp(const char *str1, const char *str2) {
    // Оба NULL - равны
    if (str1 == NULL && str2 == NULL) {
        return 0;
    }
    // Только str1 NULL - считается меньшей
    if (str1 == NULL) {
        return -1;
    }
    // Только str2 NULL - считается большей
    if (str2 == NULL) {
        return 1;
    }
    // Обе не NULL - обычное сравнение
    return strcmp(str1, str2);
}

void sort_by_last_name(PhoneBook *phone_book) {
    if (phone_book == NULL || phone_book->head == NULL || phone_book->count < 2) {
        return;
    }

    Contact *sorted = NULL;
    Contact *current = phone_book->head;

    while (current != NULL) {
        Contact *next = current->next;
        
        // Безопасное сравнение с обработкой NULL
        if (sorted == NULL || safe_strcmp(current->last_name, sorted->last_name) <= 0) {
            // Вставка в начало
            current->next = sorted;
            current->prev = NULL;
            if (sorted != NULL) {
                sorted->prev = current;
            }
            sorted = current;
        } else {
            // Поиск позиции для вставки
            Contact *temp = sorted;
            while (temp->next != NULL && 
                   safe_strcmp(current->last_name, temp->next->last_name) > 0) {
                temp = temp->next;
            }
            // Вставка после temp
            current->next = temp->next;
            current->prev = temp;
            if (temp->next != NULL) {
                temp->next->prev = current;
            }
            temp->next = current;
        }
        current = next;
    }

    phone_book->head = sorted;
    
    // Обновление tail
    Contact *tail = sorted;
    while (tail != NULL && tail->next != NULL) {
        tail = tail->next;
    }
    phone_book->tail = tail;
}


void save_contact_to_file(FILE *file, Contact *contact) {
    // Записываем все поля структуры
    fwrite(&contact->id, sizeof(uint32_t), 1, file);
    fwrite(contact->last_name, sizeof(char), MAX_LENGTH, file);
    fwrite(contact->first_name, sizeof(char), MAX_LENGTH, file);
    fwrite(contact->middle_name, sizeof(char), MAX_LENGTH, file);
    fwrite(contact->company, sizeof(char), MAX_LENGTH, file);
    fwrite(contact->position, sizeof(char), MAX_LENGTH, file);
    
    // Записываем массивы строк с их счетчиками
    fwrite(&contact->phone_count, sizeof(int), 1, file);
    for (int i = 0; i < contact->phone_count; i++) {
        fwrite(contact->phone[i], sizeof(char), MAX_LENGTH, file);
    }
    
    fwrite(&contact->email_count, sizeof(int), 1, file);
    for (int i = 0; i < contact->email_count; i++) {
        fwrite(contact->email[i], sizeof(char), MAX_LENGTH, file);
    }
    
    fwrite(&contact->social_media_count, sizeof(int), 1, file);
    for (int i = 0; i < contact->social_media_count; i++) {
        fwrite(contact->social_media[i], sizeof(char), MAX_LENGTH, file);
    }
    
    fwrite(&contact->messenger_count, sizeof(int), 1, file);
    for (int i = 0; i < contact->messenger_count; i++) {
        fwrite(contact->messenger[i], sizeof(char), MAX_LENGTH, file);
    }
}

Contact* load_contact_from_file(FILE *file, PhoneBook *phone_book) {
    Contact *contact = (Contact*)malloc(sizeof(Contact));
    if (!contact) return NULL;
    
    // Читаем основные поля
    fread(&contact->id, sizeof(uint32_t), 1, file);
    fread(contact->last_name, sizeof(char), MAX_LENGTH, file);
    fread(contact->first_name, sizeof(char), MAX_LENGTH, file);
    fread(contact->middle_name, sizeof(char), MAX_LENGTH, file);
    fread(contact->company, sizeof(char), MAX_LENGTH, file);
    fread(contact->position, sizeof(char), MAX_LENGTH, file);
    
    // Обновляем next_id в phone_book если нужно
    if (contact->id >= phone_book->next_id) {
        phone_book->next_id = contact->id + 1;
    }
    
    // Читаем массивы строк с их счетчиками
    fread(&contact->phone_count, sizeof(int), 1, file);
    for (int i = 0; i < contact->phone_count; i++) {
        fread(contact->phone[i], sizeof(char), MAX_LENGTH, file);
    }
    
    fread(&contact->email_count, sizeof(int), 1, file);
    for (int i = 0; i < contact->email_count; i++) {
        fread(contact->email[i], sizeof(char), MAX_LENGTH, file);
    }
    
    fread(&contact->social_media_count, sizeof(int), 1, file);
    for (int i = 0; i < contact->social_media_count; i++) {
        fread(contact->social_media[i], sizeof(char), MAX_LENGTH, file);
    }
    
    fread(&contact->messenger_count, sizeof(int), 1, file);
    for (int i = 0; i < contact->messenger_count; i++) {
        fread(contact->messenger[i], sizeof(char), MAX_LENGTH, file);
    }
    
    contact->next = NULL;
    contact->prev = NULL;
    
    return contact;
}

void save_phonebook_to_file(const PhoneBook *phone_book) {
    FILE *file = fopen(FILENAME, "wb");
    if (!file) {
        printf("Ошибка при открытии файла для записи.\n");
        return;
    }
    
    // Записываем количество контактов
    fwrite(&phone_book->count, sizeof(int), 1, file);
    
    // Записываем все контакты
    Contact *current = phone_book->head;
    while (current != NULL) {
        save_contact_to_file(file, current);
        current = current->next;
    }
    
    fclose(file);
    printf("Телефонная книга сохранена в файл '%s'.\n", FILENAME);
}


void load_phonebook_from_file(PhoneBook *phone_book) {
    FILE *file = fopen(FILENAME, "rb");
    if (!file) {
        printf("Файл телефонной книги не найден. Будет создана новая книга.\n");
        phone_book->head = NULL;
        phone_book->tail = NULL;
        phone_book->count = 0;
        phone_book->next_id = 1;
        return;
    }
    
    // Инициализируем телефонную книгу
    init_phonebook(phone_book);
    
    // Читаем количество контактов
    int contact_count;
    fread(&contact_count, sizeof(int), 1, file);
    
    // Загружаем все контакты
    for (int i = 0; i < contact_count; i++) {
        Contact *contact = load_contact_from_file(file, phone_book);
        if (!contact) {
            printf("Ошибка при загрузке контакта %d\n", i + 1);
            continue;
        }
        
        // Добавляем контакт в список
        if (phone_book->head == NULL) {
            phone_book->head = contact;
            phone_book->tail = contact;
        } else {
            phone_book->tail->next = contact;
            contact->prev = phone_book->tail;
            phone_book->tail = contact;
        }
        phone_book->count++;
    }
    
    fclose(file);
    printf("Загружено %d контактов из файла '%s'.\n", contact_count, FILENAME);
}





