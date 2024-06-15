#include "common.h"

int shmid, sem_empty, sem_full;
ring_buffer_t *ring_buffer;

static void cleanup() {
    shmdt(ring_buffer);
    // Do not remove the shared memory segment or semaphores here
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

    ring_buffer = init_ring_buffer(size, &shmid);
    get_semaphores(&sem_empty, &sem_full, size);

    struct sembuf sb_empty = {0, -1, 0};
    struct sembuf sb_full = {0, 1, 0};

    printf("Sender: Initialized with ring buffer size %d\n", size);

    char c;
    while ((c = getchar()) != EOF) {
        printf("Sender: Waiting to enqueue character '%c'\n", c);
        while (semop(sem_empty, &sb_empty, 1) == -1) {
            if (errno != EINTR) {
                perror("semop sem_empty");
                exit(EXIT_FAILURE);
            }
        }
        enqueue(ring_buffer, c);
        printf("Sender: Enqueued character '%c'\n", c);
        while (semop(sem_full, &sb_full, 1) == -1) {
            if (errno != EINTR) {
                perror("semop sem_full");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Signal receiver about EOF
    printf("Sender: Enqueuing EOF\n");
    while (semop(sem_empty, &sb_empty, 1) == -1) {
        if (errno != EINTR) {
            perror("semop sem_empty");
            exit(EXIT_FAILURE);
        }
    }
    enqueue(ring_buffer, EOF);
    while (semop(sem_full, &sb_full, 1) == -1) {
        if (errno != EINTR) {
            perror("semop sem_full");
            exit(EXIT_FAILURE);
        }
    }

    printf("Sender: Finished enqueuing characters\n");

    cleanup();
    return 0;
}
