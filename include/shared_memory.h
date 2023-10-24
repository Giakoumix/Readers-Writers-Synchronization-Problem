
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>


struct shared_memory {
    char *segment;
    // char **segment;
    int seg_id;
    int read_count;
    int children_count;
    int counter;
    // sem_t mutex;
    // sem_t wrt;
    // semt_t fifo;
    // semt_t eor; /* End of reading */
};

struct shared_memory *make_shared_memory(int segmentation, int children_count);