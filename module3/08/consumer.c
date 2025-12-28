#include "common.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Неверное количество аргументов\n");
    return 1;
  }
  int sem_id = get_semaphore(argv[1], 0);

  while (1) {
    struct sembuf op_wait_full = {0, -1, 0};
    semop(sem_id, &op_wait_full, 1);

    struct sembuf op_wait_mutex = {1, -1, 0};
    semop(sem_id, &op_wait_mutex, 1);

    FILE *f = fopen(argv[1], "r+");
    if (!f) {
      perror("Ошибка открытия файла");
      exit(1);
    }

    char line[256];
    long position = 0;
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
      if (line[0] != '*') {
        found = 1;
        break;
      }
      position = ftell(f);
    }
    if (found) {
      fseek(f, position, SEEK_SET);
      fputc('*', f);

      printf("обрабатываю строку: %s", line);

      int min_val = 100, max_val = -1;
      int num, count = 0;
      char *token = strtok(line, " \n");

      while (token) {
        num = atoi(token);
        if (num < min_val)
          min_val = num;
        if (num > max_val)
          max_val = num;
        count++;
        token = strtok(NULL, " \n");
      }

      if (count > 0) {
        printf("min=%d, max=%d\n", min_val, max_val);
      }
    }
    fclose(f);

    struct sembuf op_signal_mutex = {1, +1, 0};
    semop(sem_id, &op_signal_mutex, 1);

    sleep(2);
  }
}