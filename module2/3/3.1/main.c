#include "header.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Incorrect arguments");
    return 1;
  }

  FILE *file = fopen(argv[1], "r");
  if (!file && !check_rights(argv[1])) {
    printf("Incorrect argument");
    return 1;
  }
  uint32_t result;
  if (file) {
    struct stat file_info;
    if (stat(argv[1], &file_info) == 0) {
      mode_t permissions = file_info.st_mode & 0777;
      char symbolic[10] = "\0";
      symbolic[0] = (permissions & 0400) ? 'r' : '-';
      symbolic[1] = (permissions & 0200) ? 'w' : '-';
      symbolic[2] = (permissions & 0100) ? 'x' : '-';
      symbolic[3] = (permissions & 0040) ? 'r' : '-';
      symbolic[4] = (permissions & 0020) ? 'w' : '-';
      symbolic[5] = (permissions & 0010) ? 'x' : '-';
      symbolic[6] = (permissions & 0004) ? 'r' : '-';
      symbolic[7] = (permissions & 0002) ? 'w' : '-';
      symbolic[8] = (permissions & 0001) ? 'x' : '-';
      symbolic[9] = '\0';
      printf("Цифровой вид: %o\n", file_info.st_mode & 0777);
      printf("Буквенное представление: %s \n", symbolic);
      char binary[12] = "\0";
      int pos = 0;
      for (int i = 8; i >= 0; i--) {
        binary[pos++] = ((file_info.st_mode >> i) & 1) ? '1' : '0';
        if (i == 6 || i == 3)
          binary[pos++] = ' ';
      }
      binary[pos] = '\0';
    int bit_count = 0;
    
    for (int i = strlen(binary) - 1; i >= 0 && bit_count < 9; i--) {
        if (binary[i] == '1') {
            result |= (1u << bit_count);
            bit_count++;
        } else if (binary[i] == '0') {
            bit_count++;
        }
    }
      print_binary(result);
    }
  } else {
    result = convert_to_bin(argv[1]);
    print_binary(result);
  }
  char answer[10];
  char rights[10];

  while (1) {
    printf("Будем ли мы дальше менять права доступа? (yes/no): ");
    scanf("%s", answer);

    for (int i = 0; answer[i]; i++) {
      answer[i] = tolower(answer[i]);
    }

    if (strcmp(answer, "yes") == 0 || strcmp(answer, "y") == 0) {
      printf("Введите права доступа: ");
      scanf("%s", rights);
      if (!check_rights(rights)) {
        printf("Incorrect rights: %s\n", argv[1]);
        return 1;
      }
      // print_binary(result);
      uint32_t new_rights = change_permissions(result, rights);
      print_binary(new_rights);
      result = new_rights;
    } else if (strcmp(answer, "no") == 0 || strcmp(answer, "n") == 0) {
      printf("Завершение программы.\n");
      break;
    } else {
      printf("Пожалуйста, введите 'yes' или 'no'\n");
    }
  }
  return 0;
}
