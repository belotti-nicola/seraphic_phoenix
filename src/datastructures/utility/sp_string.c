#include "internal/datastructures/utility/sp_string.h"

#include <stdlib.h>
#include <string.h>

static inline bool spss_is_heap(const sp_string_t *s) 
{
    return (s->size & HEAP_FLAG) != 0;
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

    if ( buffer == NULL )
    {
        return retVal;
    }

    if ( buffer_size <= SSO_SIZE - 1)
    {
        // SSO SCENARIO -- SIZE IS FINE AS IS

        strncpy(retVal.stack_string.stack,buffer,buffer_size);
        retVal.stack_string.stack[SSO_SIZE] = '\0';
        retVal.size = buffer_size;

        return retVal;
    }
    else
    {
        // HEAP SCENARIO -- SIZE WILL BE FLAGGED

        size_t incremented_size = buffer_size * 1.5;

        void *ptr = malloc(incremented_size * sizeof(uint8_t));
        if ( ptr == NULL )
        {
            retVal.stack_string.stack[0] = '\0';
            retVal.size = 0;

            return retVal;
        }

        retVal.size = buffer_size | HEAP_FLAG;
        
        retVal.heap_string.heap              = ptr;
        retVal.heap_string.heap[buffer_size] = '\0';
        retVal.heap_string.heap_max_size     = incremented_size;

        memcpy(ptr, buffer, buffer_size);

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

    const char *extracted = spss_cstr(string);
    return spss_ncreate(extracted,string->size);
}

void spss_destroy(sp_string_t *string)
{
    if ( spss_is_heap(string) == true ) 
    {
        free(string->heap_string.heap);
        string->heap_string.heap_max_size = 0;
        string->size = 0;
    }
}

const char *spss_cstr(const sp_string_t *string)
{
    if ( string == NULL ) return "";

    if ( spss_is_heap( string ) == false )
    {
        return string->heap_string.heap;
    }
    
    return string->stack_string.stack;
}

size_t spss_length(const sp_string_t *string) 
{
    if ( string == NULL ) return 0;

    return string->size & ~HEAP_FLAG;
}

size_t spss_capacity(const sp_string_t *string)
{
    if ( spss_is_heap(string) == true ) 
    {
        return string->heap_string.heap_max_size;
    }

    return SSO_SIZE;
}

bool spss_is_empty(const sp_string_t *string)
{
    return string->size == 0;
}

bool spss_append(sp_string_t *dst, const sp_string_t *src)
{
    if ( dst == NULL || src == NULL ) return false;

    size_t dst_size = dst->size;
    size_t src_size = src->size;

    if ( src_size == 0 ) return true;

    const char *src_chars = spss_cstr(src);

    if ( dst_size + src_size <= SSO_SIZE - 1 )
    {
        strncpy(dst->stack_string.stack,src_chars,src_size);
        dst->size += src_size;

        return true;
    }

    if ( spss_capacity(dst) >= dst_size + src_size )
    {
        strncpy(dst->heap_string.heap,src_chars,src_size);
        dst->size += src_size;

        return true;
    }


    size_t new_size = src_size * 1.5;
    void *new_ptr = malloc(new_size);
    if ( new_ptr == NULL ) return false;

    dst->heap_string.heap = new_ptr;
    dst->heap_string.heap_max_size = new_size;

    strncpy( new_ptr,src_chars,src_size );

    return true;
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

    size_t dst_size = spss_length(dst);

    if ( len == 0 ) return true;

    size_t new_size = len + dst_size;

    if ( spss_is_heap(dst) == false && new_size <= SSO_SIZE - 1)
    {
        char *target = dst->stack_string.stack + dst_size ;
        strncpy( target,src,len );
        target[new_size] = '\0';
        dst->size = new_size;
        
        return true;
    }

    if ( spss_is_heap(dst) == false && new_size > SSO_SIZE - 1)
    {
        // switch from stack to heap!

        char tmp[SSO_SIZE] = {0};
        
        char *target = dst->stack_string.stack;
        strncpy( tmp,target,dst_size );
        
        size_t incremented_size = new_size * 1.5;
        char *ptr = (char *)malloc(sizeof(char) * incremented_size);
        if ( ptr == NULL ) return false;
        
        strncpy( ptr,tmp,dst_size );
        
        char *src_from_here = ptr + dst_size;
        strncpy( src_from_here,src,len );
        ptr[new_size] = '\0';

        dst->heap_string.heap          = ptr;
        dst->heap_string.heap_max_size = incremented_size;
        dst->size = new_size | HEAP_FLAG;

        return true;
    }

    if ( spss_capacity(dst) > new_size )
    {
        char *target = dst->heap_string.heap + dst_size ;
        strncpy( target,src,len );

        dst->size = new_size | HEAP_FLAG;

        return true;
    }

    size_t incremented_size = new_size * 1.5;

    char *ptr = dst->heap_string.heap;
    if ( realloc( ptr, incremented_size) == NULL ) return false;

    strncpy( ptr + dst_size , src , len );
    *(ptr + new_size ) = '\0';

    dst->heap_string.heap          = ptr;
    dst->heap_string.heap_max_size = incremented_size;
    dst->size                      = new_size | HEAP_FLAG;

    return true;
}

bool spss_append_char(sp_string_t *dst, char c)
{
    size_t dst_size = dst->size;

    if ( dst_size + 1 <= SSO_SIZE - 1 )
    {
        strncpy(dst->stack_string.stack,&c,1);
        dst->size += 1;

        return true;
    }

    if ( spss_capacity(dst) >= dst_size + 1 )
    {
        strncpy(dst->heap_string.heap,&c,1);
        dst->size += 1;

        return true;
    }

    size_t previous_size = dst->size;
    size_t new_size = previous_size * 1.5;

    void *new_ptr = malloc(new_size);
    if ( new_ptr == NULL ) return false;

    dst->heap_string.heap = new_ptr;
    dst->heap_string.heap_max_size = new_size;

    strncpy( new_ptr,&c,1 );

    return true;
}

bool spss_reserve(sp_string_t *s, size_t new_cap)
{
    if ( s == NULL ) return false;

    if ( s->size >= new_cap ) return false;

    if ( SSO_SIZE >= new_cap ) return false;

    void *ptr = malloc(sizeof(char) * new_cap);
    if ( ptr == NULL ) return false;

    size_t s_size = spss_length(s);
    if ( s_size > 0 )
    {
        const char *buffer = spss_cstr(s);
        strncpy(ptr,buffer,s_size);      
    }

    s->heap_string.heap = ptr;
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

    string->size = 0;

    if ( spss_is_heap( string ) == true )
    {
        string->heap_string.heap[0] = '\0';
    }
    else 
    {
        string->stack_string.stack[0] = '\0';
    }
}

sp_string_t spss_substring(const sp_string_t *s, size_t start, size_t len)
{
    sp_string_t retVal = {0};

    if ( s == NULL ) return retVal;

    if ( start > s->size ) return retVal;

    const char *buffer = spss_cstr(s) + start ;
    return spss_ncreate(buffer,len);
}

bool spss_equals(const sp_string_t *a, const sp_string_t *b)
{
    if ( a == NULL || b == NULL ) return false;

    if ( a->size != b->size ) return false;

    size_t size = a->size;
    const char *a_ptr = spss_cstr(a);
    const char *b_ptr = spss_cstr(b);

    return strncmp(a_ptr,b_ptr,size) == 0;
}

bool spss_equals_cstr(const sp_string_t *a, const char *b)
{
    if ( a == NULL || b == NULL ) return false;

    size_t b_len = strlen(b);
    if ( a->size != b_len ) return false;

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

