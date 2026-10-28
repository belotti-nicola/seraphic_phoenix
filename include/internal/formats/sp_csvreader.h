#ifndef SP_CSVREADER_H
#define SP_CSVREADER_H

#include <stdio.h>
#include <stdbool.h>

#define BUFFER_SIZE 4096
#define FIELDS_SIZE 800

typedef struct sp_csvreader
{
    FILE  *file;
    char   buffer[BUFFER_SIZE];
    char  *records[FIELDS_SIZE];
    size_t records_size;
    size_t current_line;
    size_t starting_line;

} sp_csvreader;

bool sp_csvreader_open(sp_csvreader *reader, const char *path);
bool sp_csvreader_next(sp_csvreader *reader);
bool sp_csvreader_close(sp_csvreader *reader);


#endif