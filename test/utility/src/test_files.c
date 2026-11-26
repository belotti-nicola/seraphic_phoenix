#include "test_files.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>


bool get_test_directory(char *out, size_t out_size)
{
    if (out == NULL || out_size < 2)
    {
        return false;
    }

    ssize_t len = readlink("/proc/self/exe", out, out_size - 1);
    if (len < 0) 
    {
        perror("readlink /proc/self/exe");
        return false;
    }

    if ((size_t)len >= out_size - 1)
    {
        fprintf(stderr, "Error: output buffer too small for executable path\n");
        return false;
    }

    out[len] = '\0';

    char *dir = dirname(out);
    if (dir == NULL)
    {
        perror("dirname");
        return false;
    }

    if (dir != out) 
    {
        size_t dir_len = strlen(dir);
        if (dir_len >= out_size) {
            fprintf(stderr, "Error: output buffer too small for directory path\n");
            return false;
        }
        memmove(out, dir, dir_len + 1);
    }

    return true;
}

bool get_test_file_data(char *out, size_t out_size, char *relative_path)
{
    bool b = get_test_directory(out,out_size);
    if ( b == false ) return false;

    size_t out_curr_len      = strlen(out);
    size_t relative_path_len = strlen(relative_path);
    if ( relative_path_len + out_curr_len + 1 > out_size ) return false;

    strcat( out,relative_path);

    return file_exist(out);
}

bool file_exist(const char *path)
{
    struct stat buffer;
    return stat(path, &buffer) == 0 ? true : false;
}