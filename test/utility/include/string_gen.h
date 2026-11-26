#ifndef STRING_GEN_H
#define STRING_GEN_H

// C generators for chars array
// every array is obtained with malloc

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "string_common_defs.h"

//    ENTRY API FOR TEST
char *string_gen(gen_kind_t kind, size_t n, uint64_t seed);

//    GEN_ALPHA but needle is at pos-o.. 
//    requires pos + strlen(needle) <= n.
char *string_gen_with_needle(size_t n, const char *needle, size_t pos);

//    Campi separati da `delim`, ognuno lungo field_len ("aaa,bbb,ccc").
//    Utile per split / join / count.
char *string_gen_delimited(size_t fields, size_t field_len, char delim);

//    padding core with left bytes and right bytes
char *string_gen_padded(const char *core, size_t left, size_t right);

#endif