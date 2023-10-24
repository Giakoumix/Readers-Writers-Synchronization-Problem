
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "process_txt.h"
#include "shared_memory.h"

#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

int main(int argc, char **argv) {

    FILE *file, *process_file;
    time_t rawtime;
    struct timeval submition_t, response_t;
    struct timeval segment_in, segment_out;

    char *file_name;
    int segmentation, request_num, children, segments, f_time = 0;
    int *request = malloc(2*sizeof(int));
    char *requested_line = malloc(LINE*sizeof(*requested_line));

    // Initialize the arguments
    if (!process_args(argc, argv, &file_name, &segmentation, &request_num, &children)) {
        printf("Wrong arguments\n");
        exit(1);
    }

    file = fopen(file_name, "r");
    segments = get_number_of_lines(file)/segmentation;

    // Initialize mutexes
    sem_t *mutex = sem_open("/mutex", O_CREAT | O_EXCL, 0644, 1);
    sem_t *wrt = sem_open("/wrt", O_CREAT | O_EXCL, 0644, 1);
    sem_t *fifo = sem_open("/fifo", O_CREAT | O_EXCL, 0644, 1);
    sem_t *eor = sem_open("/eor", O_CREAT | O_EXCL, 0644, 0); /* End of reading */

    sem_unlink("/mutex");
    sem_unlink("/wrt");
    sem_unlink("/fifo");
    sem_unlink("/eor");

    // Make shared memory
    struct shared_memory *shm = make_shared_memory(segmentation, children);

    // Fork the children
    int pid;
    for (int i = 0 ; i < children ; i++) {
        pid = fork();
        if (pid != 0)
            break;
    }

    // Create the files for writing the data
    make_files(&process_file, pid);

    srand(time(NULL) + pid);
    if (pid != 0) {
        gettimeofday(&submition_t, NULL);
        create_request(request,-1, segments, segmentation);
        
        // First request 
        sem_wait(fifo);
        sem_wait(mutex);
        shm->counter++;
        if (shm->counter == 1) {
            sem_wait(wrt);
            shm->seg_id = request[0];
            sem_post(eor);
            sem_post(wrt);
        }
        if (shm->counter < shm->children_count)
            sem_post(fifo);
        sem_post(mutex);

        do {
            // Reader enters
            sem_wait(mutex);
            shm->read_count++;
            if (shm->read_count == 1) {
                sem_wait(wrt);
            }
            sem_post(mutex);

            // Current reader performs reading process 
            do {
                if (shm->seg_id == request[0]) {
                    strncpy(requested_line, shm->segment+request[1]*LINE, LINE);
                    gettimeofday(&response_t, NULL);
                    fprintf(process_file, "Submition time: %ld Response time: %ld <%d,%d>: %s\n", submition_t.tv_usec, response_t.tv_usec, request[0], request[1], requested_line);
                    fflush(process_file);
                    usleep(20000);
                    request_num--;
                }
                else {
                    break;
                }
                // End of requests
                if (!request_num) {
                    sem_wait(mutex);
                    shm->children_count--;
                    shm->read_count--;
                    sem_post(mutex);
                    if (!shm->children_count)
                        sem_post(eor);
                    else if (shm->counter)
                        sem_post(fifo);
                    if (shm->read_count==0)
                        sem_post(wrt);
                    exit(0);
                }

                gettimeofday(&submition_t, NULL);
                create_request(request, request[0], segments, segmentation);

            } while(1);

            // Reader leaves
            sem_wait(mutex);
            shm->read_count--;
            if (!shm->read_count) {
                shm->counter = 0;
                sem_post(fifo);
                sem_post(wrt);
            }
            sem_post(mutex);

            sem_wait(fifo);
            sem_wait(mutex);
            shm->counter++;
            if (shm->counter==1) {
                shm->seg_id = request[0];
                sem_post(eor);
            }
            if (shm->counter < shm->children_count)
                sem_post(fifo);
            sem_post(mutex);

        } while(1);

    }
    else {
        do {
            sem_wait(eor);
    
            if (!shm->children_count)
                break;

            sem_wait(wrt);

            gettimeofday(&segment_out, NULL);
            if (f_time) {
                fprintf(process_file, "In: %ld Out: %ld\n", segment_in.tv_usec, segment_out.tv_usec);
                fflush(process_file);
            }
            f_time++;
            segment_in = segment_out;
            segment_request(shm->segment, file, shm->seg_id, segmentation);
            sem_post(wrt);

        } while (1);

        while (pid = waitpid (-1, NULL, 0)){
            if (errno == ECHILD)
                break;
        }
    }

    sem_close(mutex);
    sem_close(wrt);
    sem_close(fifo);
    sem_close(eor);
    
    destroy_args(file_name, segmentation, request_num, children);
    free(request);
    free(requested_line);
    return 0;
}