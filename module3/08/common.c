#include "common.h"

int get_semaphore(const char *filename, int create_new) {
  key_t sem_key = ftok(filename, 'P');
  if (sem_key == -1) {
    perror("Ошибка ftok");
    exit(1);
  }

  int flags = create_new ? (IPC_CREAT | 0666) : 0;
  int sem_id = semget(sem_key, 2, flags);

  if (sem_id == -1) {
    perror("Ошибка semget");
    exit(1);
  }

  if (create_new) {
    semctl(sem_id, 0, SETVAL, 0);
    semctl(sem_id, 1, SETVAL, 1);

    printf("Создан семафор для файла '%s'\n", filename);
  }

  return sem_id;
}

void cleanup_semaphore(int sem_id) {
  if (semctl(sem_id, 0, IPC_RMID) == -1) {
    perror("Ошибка удаления семафора");
  }
}

char *generate_string() {
  int length = rand() % 5 + 1;
  char *buffer = malloc(100 * sizeof(char));
  if (!buffer)
    return NULL;

  int pos = 0;
  for (int j = 0; j < length; j++) {
    pos += sprintf(buffer + pos, "%d ", rand() % 100);
    if (pos >= 99)
      break;
  }
  pos += sprintf(buffer + pos, "\n");
  return buffer;
}