#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

// Keys for shared memory and semaphores
#define SHM_KEY 1234
#define SEM_KEY_EMPTY 5678
#define SEM_KEY_FULL 91011

// Define the structure for the ring buffer
typedef struct {
    int size;
    int write_index;
    int read_index;
    char buffer[];
} ring_buffer_t;

// Function to initialize the ring buffer in shared memory
ring_buffer_t* init_ring_buffer(int size, int* shmid);

// Function to get semaphore ids
void get_semaphores(int* sem_empty, int* sem_full, int size);

// Function to enqueue a character into the ring buffer
int enqueue(ring_buffer_t* ring_buffer, char c);

// Function to dequeue a character from the ring buffer
int dequeue(ring_buffer_t* ring_buffer, char* c);

#endif // COMMON_H
