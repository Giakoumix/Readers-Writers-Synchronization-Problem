
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KB 1024
#define LINE 100

int process_args(int argc, char **argv, char **file_name, int *segmentation, int *request_num, int *children);
void destroy_args(char *file_name, int segmentation, int request_num, int children);

void segment_request(char *segment_block, FILE *file, int segment, int segmentation);

void make_files(FILE **file, int name);

int get_number_of_lines(FILE *file);

char *get_requested_line(char *segment, int line);

int *create_request(int *request, int prev_segment, int segments, int segmentation);