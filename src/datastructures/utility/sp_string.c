#include "internal/datastructures/utility/sp_string.h"

#include <stdlib.h>
#include <string.h>

static inline bool spss_is_heap(const sp_string_t *s) 
{
    if ( s == NULL ) return false;

    return (s->size & HEAP_FLAG) != 0;
}

static inline bool spss_is_stack(const sp_string_t *s) 
{
    if ( s == NULL ) return false;
    
    return (spss_is_heap(s) == false);
}

sp_string_t spss_create(const char *buffer)
{
    if (buffer == NULL)
    {
        sp_string_t empty = {0};
        return empty;
    }

    size_t size = strlen(buffer);
    return spss_ncreate(buffer, size);
}

sp_string_t spss_ncreate(const char *buffer, size_t buffer_size)
{
    sp_string_t retVal = {0};

    if ( buffer == NULL || buffer_size == 0 )
    {
        return retVal;
    }

    if ( buffer_size < SSO_SIZE )
    {
        // SSO SCENARIO -- SIZE IS FINE AS IS

        memcpy(retVal.stack_string.stack,buffer,buffer_size);
        retVal.stack_string.stack[ buffer_size ] = '\0';
        retVal.size = buffer_size;

        return retVal;
    }
    else
    {
        // HEAP SCENARIO -- SIZE WILL BE FLAGGED

        size_t incremented_size = buffer_size * 2;

        char *ptr = malloc(incremented_size * sizeof(char));
        if ( ptr == NULL )
        {
            retVal.stack_string.stack[0] = '\0';
            retVal.size = 0;

            return retVal;
        }
        
        memcpy(ptr, buffer, buffer_size);
        ptr[buffer_size] = '\0';

        retVal.size = buffer_size | HEAP_FLAG;
        
        retVal.heap_string.heap          = ptr;
        retVal.heap_string.heap_max_size = incremented_size;

        return retVal;
    }
}

sp_string_t spss_clone(const sp_string_t *string)
{
    sp_string_t retVal = {0};

    if ( string == NULL )
    {
        retVal.stack_string.stack[0] = '\0';
        retVal.size = 0;

        return retVal;
    }

    size_t size = spss_length(string);
    const char *extracted = spss_cstr(string);
    return spss_ncreate(extracted,size);
}

void spss_destroy(sp_string_t *string)
{
    if ( string == NULL ) return;

    if ( spss_is_heap(string) == true ) 
    {
        free(string->heap_string.heap);
        string->heap_string.heap          = NULL;
        string->heap_string.heap_max_size = 0;
        string->size                      = 0;
    }
}

const char *spss_cstr(const sp_string_t *string)
{
    if ( string == NULL ) return "";

    if ( spss_is_heap( string ) == false )
    {
        return string->stack_string.stack;
    }
    
    return string->heap_string.heap;
}

size_t spss_length(const sp_string_t *string) 
{
    if ( string == NULL ) return 0;

    return string->size & ~HEAP_FLAG;
}

size_t spss_capacity(const sp_string_t *string)
{
    if ( string == NULL ) return 0;

    if ( spss_is_heap(string) == true ) 
    {
        return string->heap_string.heap_max_size;
    }

    return SSO_SIZE - 1;
}

bool spss_is_empty(const sp_string_t *string)
{
    if ( string == NULL ) return false;

    return spss_length(string) == 0;
}

bool spss_append(sp_string_t *dst, const sp_string_t *src)
{
    if ( dst == NULL || src == NULL ) return false;

    const char *src_cstr = spss_cstr(dst);
    size_t src_cstr_size = spss_length(dst);

    return spss_nappend_cstr(dst,src_cstr,src_cstr_size);
}

bool spss_append_cstr(sp_string_t *dst, const char *src)
{
    if ( dst == NULL || src == NULL ) return false;

    size_t src_size = strlen(src);

    return spss_nappend_cstr(dst,src,src_size);
}

bool spss_nappend_cstr(sp_string_t *dst, const char *src, size_t len)
{
    if ( dst == NULL || src == NULL ) return false;

    if ( len == 0 ) return true;

    size_t dst_size = spss_length(dst);

    size_t new_size = len + dst_size;

    if ( spss_is_stack(dst) && new_size <= SSO_SIZE - 1)
    {
        // stack size is fine.

        char *target = dst->stack_string.stack + dst_size ;
        
        memcpy( target,src,len );
        target[new_size] = '\0';
        
        dst->size = new_size;
        
        return true;
    }

    if ( spss_is_stack(dst) && new_size > SSO_SIZE - 1)
    {
        // switch from stack to heap!
               
        size_t incremented_size = new_size * 2;
        char *ptr = (char *)malloc(sizeof(char) * incremented_size);
        if ( ptr == NULL ) return false;

        char *old_content = dst->stack_string.stack;
        memcpy( ptr, old_content, dst_size );
        
        char *new_content = ptr + dst_size;
        memcpy( new_content, src, len );

        ptr[ new_size ] = '\0';

        dst->heap_string.heap          = ptr;
        dst->heap_string.heap_max_size = incremented_size;
        dst->size = new_size | HEAP_FLAG;

        return true;
    }

    if ( spss_capacity(dst) > new_size )
    {
        // heap size is fine

        char *target = dst->heap_string.heap + dst_size ;
        strncpy( target,src,len );

        target[ new_size ] = '\0';

        dst->size = new_size | HEAP_FLAG;

        return true;
    }

    // realloc the heap.
    // adding the len bytes from src.

    size_t incremented_size = new_size * 2;

    // BIG DISCLAIMER HERE:
    //    it is possible that source char * is the same in the destination ( or contained )
    //    
    //    if in this case, the realloc move the buffer in another place. 
    //    (old pointer is not valid anymore)
    //
    //    ==> we compute offset for both case.
    const char *start = dst->heap_string.heap;
    const char *end   = start + dst_size;
    bool aliased = ( src >= start && src < end );

    size_t offset = aliased ? (size_t)( src - start ) : 0;

    char *reallocated = realloc( dst->heap_string.heap, incremented_size );
    if (  reallocated == NULL ) return false;   // old buffer is still valid

    if ( aliased ) src = reallocated + offset; // aliasing scenario: update the src pointer to reallocation value

    memcpy( reallocated + dst_size, src, len );
    reallocated[new_size] = '\0';

    dst->heap_string.heap          = reallocated;
    dst->heap_string.heap_max_size = incremented_size;
    dst->size                      = new_size | HEAP_FLAG;

    return true;
}

bool spss_append_char(sp_string_t *dst, char c)
{
    if (dst == NULL ) return false;

    size_t dst_size = spss_length(dst);
    size_t new_size = dst_size + 1;

    if ( spss_capacity(dst) > dst_size + 2 ) // c + '\0'
    {
        if ( spss_is_stack(dst) )
        {
            dst->stack_string.stack[dst_size]     = c;
            dst->stack_string.stack[dst_size + 1] = '\0';

            dst->size += new_size;

            return true;
        }
        else
        {
            dst->heap_string.heap[dst_size]     = c;
            dst->heap_string.heap[dst_size + 1] = '\0';

            dst->size = new_size | HEAP_FLAG;

            return true;
        }
    }

    size_t incremented_size = new_size * 2;

    char *reallocated = realloc( dst->heap_string.heap, incremented_size );
    if (  reallocated == NULL ) return false;
    reallocated[ new_size - 1 ] =   c ;
    reallocated[ new_size     ] = '\0';

    dst->heap_string.heap          = reallocated;
    dst->heap_string.heap_max_size = incremented_size;
    dst->size                      = new_size | HEAP_FLAG;

    return true;
}

bool spss_reserve(sp_string_t *s, size_t new_cap)
{
    if ( s == NULL ) return false;

    size_t current_size = spss_length(s);

    if ( current_size >= new_cap ) return true;

    if ( SSO_SIZE >= new_cap ) return true;

    void *ptr = malloc(sizeof(char) * new_cap);
    if ( ptr == NULL ) return false;

    const char *buffer = spss_cstr(s);

    size_t s_size = spss_length(s);
    if (   s_size > 0 )
    {
        memcpy(ptr,buffer,s_size);      
    }

    s->heap_string.heap = ptr;
    s->heap_string.heap_max_size = new_cap;
    s->size = s_size | HEAP_FLAG;


    return true;
}

void spss_shrink_to_fit(sp_string_t *s)
{
    if ( s == NULL ) return;

    size_t string_size = spss_length(s);

    if ( spss_is_heap(s) == false ) return;

    if ( string_size < SSO_SIZE )
    {
        char tmp[SSO_SIZE] = {0};

        strncpy(tmp,s->heap_string.heap,string_size);

        strncpy(s->stack_string.stack,tmp,string_size);
    }
    
    if ( string_size < s->heap_string.heap_max_size )
    {
        char *tmp = malloc(sizeof(char) * string_size );
        if ( tmp == NULL )
        {
            s->stack_string.stack[0] = '\0';
            s->size = 0;
            return;
        }
        
        strncpy(tmp,s->heap_string.heap,string_size);
        tmp[string_size] = '\0';
    }   
}

void spss_clear(sp_string_t *string)
{
    if ( string == NULL )
    {
        return;
    }

    if ( spss_is_heap( string ) == true )
    {
        string->heap_string.heap[0] = '\0';
        string->size = 0 | HEAP_FLAG;
    }
    else 
    {
        string->stack_string.stack[0] = '\0';
        string->size = 0;
    }
}

sp_string_t spss_substring(const sp_string_t *s, size_t start, size_t len)
{
    sp_string_t retVal = {0};

    if ( s == NULL ) return retVal;

    size_t current_size = spss_length(s);

    if ( start > current_size ) return retVal;

    const char *buffer = spss_cstr(s) + start ;
    return spss_ncreate(buffer,len - start);
}

bool spss_equals(const sp_string_t *a, const sp_string_t *b)
{
    if ( a == NULL || b == NULL ) return false;

    size_t a_size = spss_length(a);
    size_t b_size = spss_length(b);

    if ( a_size != b_size ) return false;

    const char *a_ptr = spss_cstr(a);
    const char *b_ptr = spss_cstr(b);

    return strncmp(a_ptr,b_ptr,a_size) == 0;
}

bool spss_equals_cstr(const sp_string_t *a, const char *b)
{
    if ( a == NULL || b == NULL ) return false;

    size_t a_len = spss_length(a);
    size_t b_len = strlen(b);
    
    if ( a_len != b_len ) return false;

    const char *a_ptr = spss_cstr(a);
    return strncmp(a_ptr,b,b_len) == 0;
}

size_t spss_find(const sp_string_t *s, const char *needle)
{
    if ( s == NULL ) return SP_STRING_NPOS;

    const char *s_cstr = spss_cstr(s);

    needle = strstr(s_cstr,needle);

    if ( needle == NULL ) return SP_STRING_NPOS;

    return 0;
}

char spss_at(const sp_string_t *s, size_t index)
{
    if ( s == NULL ) return '\0';

    if ( index >= spss_length(s)) return '\0';


    if ( spss_is_stack(s) )
    {
        return s->stack_string.stack[ index ];
    }

    return s->heap_string.heap[ index ];
}

bool spss_set_at(sp_string_t *s, size_t index, char c)
{
    if ( s == NULL ) return false;

    if ( index > spss_length(s)) return false;


    if ( spss_is_stack(s) )
    {
        s->stack_string.stack[ index ] = c;
    }
    else
    {
        s->heap_string.heap[ index ] = c;
    }

    return true;
}

char spss_front(const sp_string_t *s)
{
    if ( s == NULL ) return '\0';

    return spss_at(s,0);
}

char spss_back(const sp_string_t *s)
{
    if ( s == NULL ) return '\0';

    size_t index = spss_length(s) - 1;

    return spss_at(s,index);
}

bool spss_pop_back(sp_string_t *s)
{
    if ( s == NULL ) return false;

    size_t size = spss_length(s);

    if ( size == 0 ) return false;

    size_t new_size = size - 1;

    char *ptr = spss_is_stack(s) ? s->stack_string.stack : 
                                   s->heap_string.heap;

    ptr[new_size] = '\0';

    s->size = spss_is_heap(s) ? (new_size | HEAP_FLAG) : new_size;
    
    return true;
}


