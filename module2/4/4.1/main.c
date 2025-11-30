#include "struct.h"

void main() {
  setlocale(LC_ALL, "Russian");

  PhoneBook phone_book;
  init_phonebook(&phone_book);

  while (1) {
    printf("Выберите функцию для работы с таблицей\n");
    printf("1 - добавление контакта\n");
    printf("2 - редактирование контакта\n");
    printf("3 - удаление контакта;\n");
    printf("4 - просмотра телефонной книги\n");
    printf("5 - просмотр контакта \n");
    printf("0 - завершить работу\n");

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
    if (answer[0] - '0' < 0 || answer[0] - '0' > 5) {
      printf("Введите корректную команду\n");
      continue;
    }
    struct student *sorted;
    int ans = answer[0] - '0';
    switch (ans) {
    case 1:
      addContact(&phone_book);
      sort_by_last_name(&phone_book);
      break;
    case 2:
      uint32_t id2 = search(&phone_book);
      edit_contact(&phone_book, id2);
      sort_by_last_name(&phone_book);
      break;
    case 3:
      uint32_t id3 = search(&phone_book);
      delete_contact(&phone_book, id3);
      break;
    case 4:
      print_all_contacts(&phone_book);
      break;
    case 5:
      uint32_t id5 = search(&phone_book);
      // printContact(id5, &phone_book);
      break;
    case 0:
      return;
      break;
    }
  }
}