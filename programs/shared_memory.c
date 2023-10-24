
#include "shared_memory.h"
#include "process_txt.h"

struct shared_memory *make_shared_memory(int segmentation, int children_count) {
    struct shared_memory *shm;

    key_t key = ftok("shared_memory.c", 0);
    int shmid = shmget(key, sizeof(struct shared_memory), 0666 | IPC_CREAT);
    shm = (struct shared_memory *) shmat(shmid, NULL, 0);
    
    int seg_id = shmget((key_t) 1234, segmentation*LINE*sizeof(char), 0666 | IPC_CREAT);
    void *pointer = shmat(seg_id, NULL, 0);
    shm->segment = (char *) pointer;
    
    shm->counter = 0;
    shm->read_count = 0;
    shm->children_count = children_count;
    shm->seg_id = -1;

    return shm;
}