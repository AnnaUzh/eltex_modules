#include "common.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Неверное количество аргументов\n");
    return 1;
  }
  int sem_id = get_semaphore(argv[1], 1);

  srand(time(NULL));
  int round = 20;

  while (round > 0) {
    char *str = generate_string();

    struct sembuf op_wait = {1, -1, 0};
    semop(sem_id, &op_wait, 1);

    FILE *f = fopen(argv[1], "a");
    fputs(str, f);
    fclose(f);

    struct sembuf op_signal = {1, +1, 0};
    semop(sem_id, &op_signal, 1);

    struct sembuf op_full = {0, +1, 0};
    semop(sem_id, &op_full, 1);
    printf("Записана строка: %s\n", str);

    round -= 1;
    free(str);
    sleep(4);
  }
  cleanup_semaphore(sem_id);
}