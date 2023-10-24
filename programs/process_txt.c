
#include "process_txt.h"

int process_args(int argc, char **argv, char **file_name, int *segmentation, int *request_num, int *children) {
    if (argc < 5)
        return 0;

    *file_name = malloc((strlen(argv[1])+1)*sizeof(argv[1]));
    strcpy(*file_name, argv[1]);

    *segmentation = atoi(argv[2]);
    *request_num = atoi(argv[3]);
    *children = atoi(argv[4]);
    
    return 1;
}

void segment_request(char *segment_block, FILE *file, int segment, int segmentation) {
    char *pointer = segment_block;
    for (int i = 0; i < segment*segmentation + segmentation; i++) {
        if (feof(file))
            break;
        
        char buffer[LINE];
        fgets(buffer, LINE, file);
        
        if (i < segment*segmentation)
            continue;

        strcpy(pointer, buffer);
        pointer += LINE*sizeof(char);
    }
}

int get_number_of_lines(FILE *file) {
    int lines = 0;
    char str[LINE];
    while(1) {
        fgets(str, LINE, file);
        
        if (feof(file))
            break;

        lines++;
    }
    
    return lines;
}

int *create_request(int *request, int prev_segment, int segments, int segmentation) {
    int probability = rand()/RAND_MAX;
    if (prev_segment<0 || probability < 0.3) {
        request[0] = rand()%segments;
        request[1] = rand()%segmentation;
        return request;
    }

    request[0] = prev_segment;
    request[1] = rand()%segmentation;

    return request;
}

void make_files(FILE **file, int name) {
    char integer[10], buffer[100] = "files/";
    sprintf(integer, "%d", name);
    strcat(buffer, integer);
    *file = fopen(buffer, "wr");
}

void destroy_args(char *file_name, int segmentation,int request_num, int children) {
    free(file_name);
}

