#pragma once

#include <stddef.h>
#include <stdbool.h>

bool get_test_directory(char *out, size_t out_size);
bool get_test_file_data(char *out, size_t out_size, char *relative_path);
bool file_exist(const char *path);
