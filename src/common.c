#include "common.h"

// Initialize the ring buffer in shared memory
ring_buffer_t* init_ring_buffer(int size, int* shmid) {
    *shmid = shmget(SHM_KEY, sizeof(ring_buffer_t) + size * sizeof(char), IPC_CREAT | 0666);
    if (*shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    ring_buffer_t* ring_buffer = (ring_buffer_t *)shmat(*shmid, NULL, 0);
    if (ring_buffer == (void*) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    ring_buffer->size = size;
    ring_buffer->write_index = 0;
    ring_buffer->read_index = 0;

    return ring_buffer;
}

// Get semaphore ids and initialize their values
void get_semaphores(int* sem_empty, int* sem_full, int size) {
    *sem_empty = semget(SEM_KEY_EMPTY, 1, IPC_CREAT | 0666);
    *sem_full = semget(SEM_KEY_FULL, 1, IPC_CREAT | 0666);

    if (*sem_empty == -1 || *sem_full == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    semctl(*sem_empty, 0, SETVAL, size);
    semctl(*sem_full, 0, SETVAL, 0);
}

// Enqueue a character into the ring buffer
int enqueue(ring_buffer_t* ring_buffer, char c) {
    ring_buffer->buffer[ring_buffer->write_index] = c;
    ring_buffer->write_index = (ring_buffer->write_index + 1) % ring_buffer->size;
    return 0;
}

// Dequeue a character from the ring buffer
int dequeue(ring_buffer_t* ring_buffer, char* c) {
    *c = ring_buffer->buffer[ring_buffer->read_index];
    ring_buffer->read_index = (ring_buffer->read_index + 1) % ring_buffer->size;
    return 0;
}
