#include "common.h"

int shmid, sem_empty, sem_full;
ring_buffer_t *ring_buffer;

static void cleanup() {
    shmdt(ring_buffer);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(sem_empty, 0, IPC_RMID);
    semctl(sem_full, 0, IPC_RMID);
}

static void handle_signal(int sig) {
    cleanup();
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-m") != 0) {
        fprintf(stderr, "Usage: %s -m <ringbuffer elements>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int size = atoi(argv[2]);
    if (size <= 0) {
        fprintf(stderr, "Invalid ring buffer size.\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    shmid = shmget(SHM_KEY, sizeof(ring_buffer_t) + size * sizeof(char), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    struct shmid_ds shm_info;
    if (shmctl(shmid, IPC_STAT, &shm_info) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    if (shm_info.shm_segsz != sizeof(ring_buffer_t) + size * sizeof(char)) {
        fprintf(stderr, "Error: Shared memory size does not match expected size.\n");
        exit(EXIT_FAILURE);
    }

    ring_buffer = (ring_buffer_t *)shmat(shmid, NULL, 0);
    if (ring_buffer == (void*) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    sem_empty = semget(SEM_KEY_EMPTY, 1, 0666);
    sem_full = semget(SEM_KEY_FULL, 1, 0666);

    if (sem_empty == -1 || sem_full == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    struct sembuf sb_empty = {0, 1, 0};
    struct sembuf sb_full = {0, -1, 0};

    fprintf(stderr, "Receiver: Initialized with ring buffer size %d\n", size);

    while (1) {
        fprintf(stderr, "Receiver: Waiting to dequeue character\n");
        while (semop(sem_full, &sb_full, 1) == -1) {
            if (errno != EINTR) {
                perror("semop sem_full");
                exit(EXIT_FAILURE);
            }
        }
        char c;
        dequeue(ring_buffer, &c);
        fprintf(stderr, "Receiver: Dequeued character '%c'\n", c);
        while (semop(sem_empty, &sb_empty, 1) == -1) {
            if (errno != EINTR) {
                perror("semop sem_empty");
                exit(EXIT_FAILURE);
            }
        }

        if (c == EOF) break;
        putchar(c);
    }

    fprintf(stderr, "Receiver: Finished dequeuing characters\n");

    cleanup();
    return 0;
}
