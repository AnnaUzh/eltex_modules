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

void printContact(const uint32_t contact_id, const PhoneBook *phone_book) {
  for (int i = 0; i < phone_book->count; i++) {
    if (phone_book->contacts[i].id == contact_id) {
      printf("----------------------------------------\n");
      printf("Фамилия:     %s\n", phone_book->contacts[i].last_name);
      printf("Имя:         %s\n", phone_book->contacts[i].first_name);

      if (strlen(phone_book->contacts[i].middle_name) > 0) {
        printf("Отчество:    %s\n", phone_book->contacts[i].middle_name);
      }

      if (strlen(phone_book->contacts[i].company) > 0) {
        printf("Место работы: %s\n", phone_book->contacts[i].company);
      }

      if (strlen(phone_book->contacts[i].position) > 0) {
        printf("Должность:    %s\n", phone_book->contacts[i].position);
      }
      if (phone_book->contacts[i].phone_count > 0) {
        for (int j = 0; j < phone_book->contacts[i].phone_count; j++) {
          printf("Телефон %d:      %s\n", j + 1,
                 phone_book->contacts[i].phone[j]);
        }
      }

      if (phone_book->contacts[i].email_count > 0) {
        for (int j = 0; j < phone_book->contacts[i].email_count; j++) {
          printf("Email%d:        %s\n", j + 1,
                 phone_book->contacts[i].email[j]);
        }
      }

      if (phone_book->contacts[i].social_media_count > 0) {
        for (int j = 0; j < phone_book->contacts[i].social_media_count; j++) {
          printf("Соцсети%d:        %s\n", j + 1,
                 phone_book->contacts[i].social_media[j]);
        }
      }

      if (phone_book->contacts[i].messenger_count > 0) {
        for (int j = 0; j < phone_book->contacts[i].messenger_count; j++) {
          printf("Мессенджер%d:        %s\n", j + 1,
                 phone_book->contacts[i].messenger[j]);
        }
      }
      printf("----------------------------------------\n");
      return;
    }
  }
}

int addContact(PhoneBook *phone_book) {
  if (phone_book == NULL || phone_book->count >= MAX_CONTACTS) {
    printf("Ошибка: невозможно создать контакт\n");
    return -1;
  }

  Contact new_contact = {0}; // Инициализируем нулями
  new_contact.id = generate_unique_id(phone_book);
  printf("\n=== Создание нового контакта ===\n");

  // Ввод обязательных полей
  printf("Фамилия*: ");
  fgets(new_contact.last_name, MAX_LENGTH, stdin);
  new_contact.last_name[strcspn(new_contact.last_name, "\n")] = '\0';

  printf("Имя*: ");
  fgets(new_contact.first_name, MAX_LENGTH, stdin);
  new_contact.first_name[strcspn(new_contact.first_name, "\n")] = '\0';

  // Проверка обязательных полей
  if (is_empty_or_whitespace(new_contact.last_name) ||
      is_empty_or_whitespace(new_contact.first_name)) {
    printf("Ошибка: фамилия и имя являются обязательными полями\n");
    return -1;
  }

  // Ввод необязательных полей
  printf("Отчество: ");
  fgets(new_contact.middle_name, MAX_LENGTH, stdin);
  new_contact.middle_name[strcspn(new_contact.middle_name, "\n")] = '\0';

  printf("Место работы: ");
  fgets(new_contact.company, MAX_LENGTH, stdin);
  new_contact.company[strcspn(new_contact.company, "\n")] = '\0';

  printf("Должность: ");
  fgets(new_contact.position, MAX_LENGTH, stdin);
  new_contact.position[strcspn(new_contact.position, "\n")] = '\0';

  // Ввод множественных телефонов
  printf(
      "=== Номера телефонов (максимум %d, пустая строка для завершения) ===\n",
      MAX_PHONES);
  new_contact.phone_count = 0;
  for (int i = 0; i < MAX_PHONES; i++) {
    char temp[MAX_LENGTH];
    printf("Телефон %d: ", i + 1);
    fgets(temp, MAX_LENGTH, stdin);
    temp[strcspn(temp, "\n")] = '\0';

    if (strlen(temp) == 0) {
      break; // Пустая строка - завершаем ввод
    }

    strncpy(new_contact.phone[i], temp, MAX_LENGTH - 1);
    new_contact.phone[i][MAX_LENGTH - 1] = '\0';
    new_contact.phone_count++;
  }

  // Ввод множественных email
  printf("=== Email адреса (максимум %d, пустая строка для завершения) ===\n",
         MAX_EMAILS);
  new_contact.email_count = 0;
  for (int i = 0; i < MAX_EMAILS; i++) {
    char temp[MAX_LENGTH];
    printf("Email %d: ", i + 1);
    fgets(temp, MAX_LENGTH, stdin);
    temp[strcspn(temp, "\n")] = '\0';

    if (strlen(temp) == 0) {
      break;
    }

    strncpy(new_contact.email[i], temp, MAX_LENGTH - 1);
    new_contact.email[i][MAX_LENGTH - 1] = '\0';
    new_contact.email_count++;
  }

  // Ввод множественных соцсетей
  printf("=== Соцсети (максимум %d, пустая строка для завершения) ===\n",
         MAX_SOCIALS);
  new_contact.social_media_count = 0;
  for (int i = 0; i < MAX_SOCIALS; i++) {
    char temp[MAX_LENGTH];
    printf("Соцсеть %d: ", i + 1);
    fgets(temp, MAX_LENGTH, stdin);
    temp[strcspn(temp, "\n")] = '\0';

    if (strlen(temp) == 0) {
      break;
    }

    strncpy(new_contact.social_media[i], temp, MAX_LENGTH - 1);
    new_contact.social_media[i][MAX_LENGTH - 1] = '\0';
    new_contact.social_media_count++;
  }

  // Ввод множественных мессенджеров
  printf("=== Мессенджеры (максимум %d, пустая строка для завершения) ===\n",
         MAX_MESSENGERS);
  new_contact.messenger_count = 0;
  for (int i = 0; i < MAX_MESSENGERS; i++) {
    char temp[MAX_LENGTH];
    printf("Мессенджер %d: ", i + 1);
    fgets(temp, MAX_LENGTH, stdin);
    temp[strcspn(temp, "\n")] = '\0';

    if (strlen(temp) == 0) {
      break;
    }

    strncpy(new_contact.messenger[i], temp, MAX_LENGTH - 1);
    new_contact.messenger[i][MAX_LENGTH - 1] = '\0';
    new_contact.messenger_count++;
  }

  // Копируем контакт в телефонную книгу
  phone_book->contacts[phone_book->count] = new_contact;
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

  for (int i = 0; i < phone_book->count; i++) {
    printf("─── КОНТАКТ #%d ───\n", i + 1);
    printContact(phone_book->contacts[i].id, phone_book);
    printf("\n");
  }
}

uint32_t name_search(PhoneBook *phonebook, char target[], int len) {
  float max_sim = 0;
  uint32_t max_sim_contact = phonebook->contacts[0].id;
  for (int i = 0; i < MAX_CONTACTS; i++) {
    float similar = 0, total = 0;
    for (int j = 0; phonebook->contacts[i].first_name[j] != '\0'; j++) {
      total++;
      if (j >= len)
        continue;

      if (phonebook->contacts[i].first_name[j] == target[j])
        similar++;
    }
    if (len > total)
      total = len;

    if (similar == total)
      return phonebook->contacts[i].id;
    if (similar / total > max_sim) {
      max_sim = similar / total;
      max_sim_contact = phonebook->contacts[i].id;
    }
  }
  return max_sim_contact;
}
uint32_t last_name_search(PhoneBook *phone_book, char target[], int len) {
  float max_sim = 0;
  uint32_t max_sim_contact = phone_book->contacts[0].id;
  for (int i = 0; i < MAX_CONTACTS; i++) {
    float similar = 0, total = 0;
    for (int j = 0; phone_book->contacts[i].last_name[j] != '\0'; j++) {
      total++;
      if (j >= len)
        continue;

      if (phone_book->contacts[i].last_name[j] == target[j])
        similar++;
    }
    if (len > total)
      total = len;

    if (similar == total)
      return phone_book->contacts[i].id;
    if (similar / total > max_sim) {
      max_sim = similar / total;
      max_sim_contact = phone_book->contacts[i].id;
    }
  }
  return max_sim_contact;
}

Contact delete_contact(PhoneBook *phone_book, uint32_t contact_id) {
  for (int j = 0; j < phone_book->count; j++) {
    if (phone_book->contacts[j].id == contact_id) {
      Contact deleted_contact = phone_book->contacts[j];
      for (int i = j; i < phone_book->count - 1; i++) {
        phone_book->contacts[i] = phone_book->contacts[i + 1];
      }
      phone_book->count--;

      printf("Контакт успешно удален\n");
      return deleted_contact;
    }
  }
}

uint32_t search(PhoneBook *phone_book) {
  int ans;
  while (1) {
    printf("Выберите поле\n");
    printf("1-фамилия\n");
    printf("2-имя\n");

    char answer[10], ch;
    int c = 0;
    do {
      ch = getchar();
      answer[c] = ch;
      c++;
    } while (ch != '\n');
    c--;
    answer[c] = '\0';

    if (c != 1) {
      printf("Введите корректную команду\n");
      continue;
    }
    if (answer[0] - '0' < 1 || answer[0] - '0' > 2) {
      printf("Введите корректную команду\n");
      continue;
    }
    ans = answer[0] - '0';
    break;
  }

  // Contact result = {0};
  uint32_t result = 0;
  int answ = 0;
  char answer[15], ch;
  int c = 0;
    switch (ans) {
    case 1:
      printf("Введите фамилию: ");
      do {
        ch = getchar();
        answer[c] = ch;
        c++;
      } while (ch != '\n');
      c--;
      answer[c] = '\0';

      result = last_name_search(phone_book, answer, c);
      break;
    case 2:
      while (1) {
        printf("Введите имя: ");
        do {
          ch = getchar();
          answer[c] = ch;
          c++;
        } while (ch != '\n');
        c--;
        answer[c] = '\0';

        result = name_search(phone_book, answer, c);
        break;
      }
      break;
    }
  printContact(result, phone_book);
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

  for (int i = 0; i < phone_book->count; i++) {
    if (phone_book->contacts[i].id == contact_id) {
      char temp_buffer[MAX_LENGTH];

      printf("\n=== РЕДАКТИРОВАНИЕ КОНТАКТА ===\n");
      printf("Текущие данные контакта:\n");
      // printContact(contact);
      printf("\n");

      printf("Введите новые данные (оставьте пустым для сохранения текущего "
             "значения):\n");

      do {
        read_string_with_default("Фамилия*", temp_buffer,
                                 phone_book->contacts[i].last_name);
        if (strlen(temp_buffer) == 0) {
          printf("Фамилия не может быть пустой!\n");
        }
      } while (strlen(temp_buffer) == 0);
      strncpy(phone_book->contacts[i].last_name, temp_buffer, MAX_LENGTH - 1);
      phone_book->contacts[i].last_name[MAX_LENGTH - 1] = '\0';

      do {
        read_string_with_default("Имя*", temp_buffer,
                                 phone_book->contacts[i].first_name);
        if (strlen(temp_buffer) == 0) {
          printf("Имя не может быть пустым!\n");
        }
      } while (strlen(temp_buffer) == 0);
      strncpy(phone_book->contacts[i].first_name, temp_buffer, MAX_LENGTH - 1);
      phone_book->contacts[i].first_name[MAX_LENGTH - 1] = '\0';

      // Редактирование необязательных полей
      read_string_with_default("Отчество", temp_buffer,
                               phone_book->contacts[i].middle_name);
      strncpy(phone_book->contacts[i].middle_name, temp_buffer, MAX_LENGTH - 1);
      phone_book->contacts[i].middle_name[MAX_LENGTH - 1] = '\0';

      read_string_with_default("Место работы", temp_buffer,
                               phone_book->contacts[i].company);
      strncpy(phone_book->contacts[i].company, temp_buffer, MAX_LENGTH - 1);
      phone_book->contacts[i].company[MAX_LENGTH - 1] = '\0';

      read_string_with_default("Должность", temp_buffer,
                               phone_book->contacts[i].position);
      strncpy(phone_book->contacts[i].position, temp_buffer, MAX_LENGTH - 1);
      phone_book->contacts[i].position[MAX_LENGTH - 1] = '\0';

      // Редактирование множественных телефонов
      printf("\n=== Редактирование телефонов ===\n");
      for (int j = 0; j < phone_book->contacts[i].phone_count; j++) {
        printf("Телефон %d: ", j + 1);
        read_string_with_default("", temp_buffer,
                                 phone_book->contacts[i].phone[j]);
        strncpy(phone_book->contacts[i].phone[j], temp_buffer, MAX_LENGTH - 1);
        phone_book->contacts[i].phone[j][MAX_LENGTH - 1] = '\0';
      }

      // Редактирование множественных email
      printf("\n=== Редактирование email ===\n");
      for (int j = 0; j < phone_book->contacts[i].email_count; j++) {
        printf("Email %d: ", j + 1);
        read_string_with_default("", temp_buffer,
                                 phone_book->contacts[i].email[j]);
        strncpy(phone_book->contacts[i].email[j], temp_buffer, MAX_LENGTH - 1);
        phone_book->contacts[i].email[j][MAX_LENGTH - 1] = '\0';
      }

      // Редактирование множественных соцсетей
      printf("\n=== Редактирование соцсетей ===\n");
      for (int j = 0; j < phone_book->contacts[i].social_media_count; j++) {
        printf("Соцсеть %d: ", j + 1);
        read_string_with_default("", temp_buffer,
                                 phone_book->contacts[i].social_media[j]);
        strncpy(phone_book->contacts[i].social_media[j], temp_buffer,
                MAX_LENGTH - 1);
        phone_book->contacts[i].social_media[j][MAX_LENGTH - 1] = '\0';
      }

      // Редактирование множественных мессенджеров
      printf("\n=== Редактирование мессенджеров ===\n");
      for (int j = 0; j < phone_book->contacts[i].messenger_count; j++) {
        printf("Мессенджер %d: ", j + 1);
        read_string_with_default("", temp_buffer,
                                 phone_book->contacts[i].messenger[j]);
        strncpy(phone_book->contacts[i].messenger[j], temp_buffer,
                MAX_LENGTH - 1);
        phone_book->contacts[i].messenger[j][MAX_LENGTH - 1] = '\0';
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

  for (int i = 0; i < phone_book->count; i++) {
    if (phone_book->contacts[i].id == id) {
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
