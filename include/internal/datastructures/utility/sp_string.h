#ifndef SP_STRING_H
#define SP_STRING_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// this is a small string optimization
// small strings will be treated in the stack
#define SP_SSO_SIZE 64

// size is 8byte (or could be 4). We will never use the 
// whole 8 bytes together, especially the most
// significative one, which is the right most.
// so we use this, to keep track of which part
// of the union is used 
#define SP_STRING_HEAP_FLAG (SIZE_MAX ^ (SIZE_MAX >> 1))

// this define is used to signal
// that find or similiar functions failed.
#define SP_STRING_NPOS SIZE_MAX

// use this to raise warnings when
// results are not used.
#define SP_NODISCARD __attribute__((warn_unused_result))

typedef struct stack_string_t
{
    char stack[SP_SSO_SIZE];

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

//          Access / modify
char        spss_at(const sp_string_t *s, size_t index);
bool        spss_set_at(sp_string_t *s, size_t index, char c);
char        spss_front(const sp_string_t *s);
char        spss_back(const sp_string_t *s);
bool        spss_pop_back(sp_string_t *s);
bool        spss_pop_front(sp_string_t *s);

//          Insertion and removal
bool        spss_insert(sp_string_t *s, size_t pos, const char *src);
bool        spss_ninsert(sp_string_t *s, size_t pos, const char *src, size_t len);
bool        spss_erase(sp_string_t *s, size_t pos, size_t len);
bool        spss_replace(sp_string_t *s, const char *from, const char *to);
size_t      spss_replace_all(sp_string_t *s, const char *from, const char *to);
bool        spss_resize(sp_string_t *s, size_t new_len, char fill);
bool        spss_assign(sp_string_t *dst, const char *src);
bool        spss_swap(sp_string_t *a, sp_string_t *b);
bool        spss_splice(sp_string_t *s, size_t pos, size_t del_len, const char *src, size_t src_len);

//          Search
size_t      spss_find_from(const sp_string_t *s, const char *needle, size_t start);
size_t      spss_find_char(const sp_string_t *s, char c);
size_t      spss_rfind(const sp_string_t *s, const char *needle);
size_t      spss_find_first_of(const sp_string_t *s, const char *charset);
size_t      spss_count(const sp_string_t *s, const char *needle);
bool        spss_contains(const sp_string_t *s, const char *needle);
bool        spss_starts_with(const sp_string_t *s, const char *prefix);
bool        spss_ends_with(const sp_string_t *s, const char *suffix);

//          Comparison
int         spss_compare(const sp_string_t *a, const sp_string_t *b);
int         spss_compare_cstr(const sp_string_t *a, const char *b);
bool        spss_equals_ignore_case(const sp_string_t *a, const sp_string_t *b);

//          Transformation functions
void        spss_to_upper(sp_string_t *s);
void        spss_to_lower(sp_string_t *s);
void        spss_trim(sp_string_t *s);
void        spss_ltrim(sp_string_t *s);
void        spss_rtrim(sp_string_t *s);
void        spss_reverse(sp_string_t *s);

#endif