#ifndef SP_STRING_H
#define SP_STRING_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// this is a small string optimization
// small strings will be treated in the stack
#define SSO_SIZE 1024

// size is 8byte. We will never use the 
// whole 8 bytes together, especially the most
// significative one, which is the right most.
// so we use this, to keep track of which part
// of the union is used 
#define HEAP_FLAG (1ULL << 63)

// this define is used to signal
// that find failed.
#define SP_STRING_NPOS SIZE_MAX

typedef struct stack_string_t
{
    char stack[SSO_SIZE];

} stack_string_t;

typedef struct heap_string_t
{
    char   *heap;
    size_t  heap_max_size;

} heap_string_t;

typedef struct sp_string_t
{
    size_t size;

    union 
    {
        stack_string_t stack_string;
        heap_string_t  heap_string;
    };

} sp_string_t;


//          Creation and Destruction
sp_string_t spss_create(const char *buffer);
sp_string_t spss_ncreate(const char *buffer, size_t buffer_size);
sp_string_t spss_clone(const sp_string_t *s);
void        spss_destroy(sp_string_t *s);


//          Query functions on properties
const char *spss_cstr(const sp_string_t *s);
size_t      spss_length(const sp_string_t *s);
size_t      spss_capacity(const sp_string_t *s);
bool        spss_is_empty(const sp_string_t *s);


//          Update and memory handling
bool        spss_append(sp_string_t *dst, const sp_string_t *src);
bool        spss_append_cstr(sp_string_t *dst, const char *src);
bool        spss_nappend_cstr(sp_string_t *dst, const char *src, size_t len);
bool        spss_append_char(sp_string_t *dst, char c);
bool        spss_reserve(sp_string_t *s, size_t new_cap);
void        spss_shrink_to_fit(sp_string_t *s);
void        spss_clear(sp_string_t *s);


//          Query functions on data
sp_string_t spss_substring(const sp_string_t *s, size_t start, size_t len);
bool        spss_equals(const sp_string_t *a, const sp_string_t *b);
bool        spss_equals_cstr(const sp_string_t *a, const char *b);
size_t      spss_find(const sp_string_t *s, const char *needle);


#endif