#include "internal/datastructures/utility/sp_string.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static inline bool spss_is_heap(const sp_string_t *s) 
{
    if ( s == NULL ) return false;

    return (s->size & SP_STRING_HEAP_FLAG) != 0;
}

static inline bool spss_is_stack(const sp_string_t *s) 
{
    if ( s == NULL ) return false;
    
    return (spss_is_heap(s) == false);
}

static inline char *spss_buffer(sp_string_t *s)
{
    if ( s == NULL ) return '\0';

    if ( spss_is_stack(s) ) return s->stack_string.stack;
    else                    return s->heap_string.heap;
}

static inline void spss_set_length(sp_string_t *s, size_t new_len)
{
    if ( s == NULL ) return ;

    if ( spss_is_stack(s) ) s->size = new_len;
    else                    s->size = new_len | SP_STRING_HEAP_FLAG;
}

// WARNING: may relocate the buffer (malloc/realloc). Any pointer into
// the old buffer (e.g. a src aliasing s) becomes invalid after the call.
// In this scenario, caller must save an offset BEFORE and recompute the pointer.
// new_capacity is in usable characters (the '\0' is added internally).
static inline bool spss_ensure_capacity(sp_string_t *s, size_t new_capacity)
{
    if ( s == NULL ) return false;

    if ( spss_capacity(s) > new_capacity ) return true;

    // min_cap + 1 (terminator) and growth must not overflow, and the
    // length must fit in 63 bits (bit 63 is SP_STRING_HEAP_FLAG)
    if ( new_capacity >= (SP_STRING_HEAP_FLAG >> 1) ) return false;

    size_t incremented_size = new_capacity + new_capacity / 2;

    if ( spss_is_stack(s) == true )
    {
        // swith from stack to heap

        char *ptr = (char *)malloc( sizeof(char) * incremented_size );
        if (  ptr == NULL ) return false;

        s->heap_string.heap          = ptr;
        s->heap_string.heap_max_size = incremented_size;
        s->size                      = s->size | SP_STRING_HEAP_FLAG;

        return true;
    }

    // reallocation of heap -> no changes in size 

    char *ptr = realloc(s->heap_string.heap,incremented_size);
    if ( ptr == NULL ) return false;

    s->heap_string.heap          = ptr;
    s->heap_string.heap_max_size = incremented_size;

    return true;
}

static int spss_compare_mem(const char *a, size_t alen,
                            const char *b, size_t blen)
{
    size_t min = ( alen < blen ) ? alen : blen;

    int res = memcmp(a, b, min);
    if ( res != 0 ) return res;

    // common prefix: the minor is the shorter
    return ( alen > blen ) - ( alen < blen );
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

    if ( buffer_size < SP_SSO_SIZE )
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

        retVal.size = buffer_size | SP_STRING_HEAP_FLAG;
        
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
    sp_string_t *casted = (sp_string_t *)string;
    return spss_buffer(casted);
}

size_t spss_length(const sp_string_t *string) 
{
    if ( string == NULL ) return 0;

    return string->size & ~SP_STRING_HEAP_FLAG;
}

size_t spss_capacity(const sp_string_t *string)
{
    if ( string == NULL ) return 0;

    if ( spss_is_heap(string) == true ) 
    {
        return string->heap_string.heap_max_size;
    }

    return SP_SSO_SIZE - 1;
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

    if ( spss_is_stack(dst) && new_size <= SP_SSO_SIZE - 1)
    {
        // stack size is fine.

        char *target = dst->stack_string.stack + dst_size ;
        
        memcpy( target,src,len );
        target[new_size] = '\0';
        
        dst->size = new_size;
        
        return true;
    }

    if ( spss_is_stack(dst) && new_size > SP_SSO_SIZE - 1)
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
        dst->size = new_size | SP_STRING_HEAP_FLAG;

        return true;
    }

    if ( spss_capacity(dst) > new_size )
    {
        // heap size is fine

        char *target = dst->heap_string.heap + dst_size ;
        strncpy( target,src,len );

        target[ new_size ] = '\0';

        dst->size = new_size | SP_STRING_HEAP_FLAG;

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
    dst->size                      = new_size | SP_STRING_HEAP_FLAG;

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

            dst->size = new_size | SP_STRING_HEAP_FLAG;

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
    dst->size                      = new_size | SP_STRING_HEAP_FLAG;

    return true;
}

bool spss_reserve(sp_string_t *s, size_t new_cap)
{
    if ( s == NULL ) return false;

    size_t current_size = spss_length(s);

    if ( current_size >= new_cap ) return true;

    if ( SP_SSO_SIZE >= new_cap ) return true;

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
    s->size = s_size | SP_STRING_HEAP_FLAG;


    return true;
}

void spss_shrink_to_fit(sp_string_t *s)
{
    if ( s == NULL ) return;

    size_t string_size = spss_length(s);

    if ( spss_is_heap(s) == false ) return;

    if ( string_size < SP_SSO_SIZE )
    {
        char tmp[SP_SSO_SIZE] = {0};

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
        string->size = 0 | SP_STRING_HEAP_FLAG;
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

    if ( s == NULL || len == 0 ) return retVal;

    size_t current_size = spss_length(s);

    if ( start > current_size ) return retVal;

    const char *buffer = spss_cstr(s) + start ;

    return spss_ncreate( buffer , len );
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

    if ( needle == NULL ) return SP_STRING_NPOS;

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

    s->size = spss_is_heap(s) ? (new_size | SP_STRING_HEAP_FLAG) : new_size;
    
    return true;
}

bool spss_pop_front(sp_string_t *s)
{
    // this function could be a spss_erase scenario 

    if ( s == NULL ) return false;

    size_t size = spss_length(s);

    if ( size == 0 ) return false;

    size_t new_size = size - 1;

    char *buf = spss_is_heap(s) ? s->heap_string.heap
                                : s->stack_string.stack;

    memmove(buf, buf + 1, size);

    s->size = spss_is_heap(s) ? ((size - 1) | SP_STRING_HEAP_FLAG) : (size - 1);
    
    return true;
}

bool spss_insert(sp_string_t *s, size_t pos, const char *src)
{
    if ( s == NULL || src == NULL ) return false;

    size_t src_len = strlen(src);

    return spss_ninsert(s,pos,src,src_len);
}

bool spss_ninsert(sp_string_t *s, size_t pos, const char *src, size_t len)
{
    if ( s == NULL || src == NULL || len == 0 ) return false;

    size_t size = spss_length(s);

    if ( pos > size ) return false;
    
    size_t available = spss_capacity(s);
    size_t requested = spss_length(s) + len + 1;

    size_t buffer_start = (size_t)spss_buffer(s);

    bool aliased = ( buffer_start >= (size_t)src &&
                     buffer_start <  (size_t)src + len ) ;

    size_t offset = aliased ? ( size_t )(src - s->heap_string.heap) : 0;

    if ( available < requested )
    {
        if ( spss_ensure_capacity(s,requested) == false ) return false;

        if ( aliased ) src = s->heap_string.heap + offset ;
    }

    char *buffer = spss_buffer(s);

    // move buffer from pos to pos + len

    char  *move_src  = buffer + pos;
    char  *move_dst  = buffer + pos + len ;
    size_t move_size = size - pos + 1 ; // the '\0' also
    memmove( move_dst, move_src, move_size );

    // insertion in pos place
    // watch out: if src is contained and moved
    // we should perform a double memcpy if it is the case
    if ( aliased == false )
    {
        const char *insert_src = src + pos - len; 
        char   *insert_dst = buffer + pos;
        size_t insert_size = len ;
        memcpy( insert_dst, insert_src, insert_size );

        spss_set_length(s, size + len);

        return true;
    }

    // copy left bytes, if any
   
    bool left_bytes_cond = (size_t)src < pos ;
    if ( left_bytes_cond == true )
    {
        const char *src_left = (const char *)src;
        char *dst_left   = (char *)pos;
        size_t size_left = len - (size_t)src_left;

        memcpy( dst_left, src_left, size_left );

        spss_set_length(s, size + len);
    }

    // copy right bytes, if any

    bool right_bytes_cond = (size_t)src + len >= pos;
    if ( right_bytes_cond == true )
    {
        const char *src_right   =  (const char *)src + pos;
        char *dst_right   =  (char *)src + pos + len ;
        size_t size_right =  (size_t)src;

        memcpy( dst_right, src_right, size_right );

        spss_set_length(s, size + len);
    }

    return true;
}

bool spss_erase(sp_string_t *s, size_t pos, size_t len)
{
    if ( s == NULL || len == 0 ) return false;

    if ( pos >= spss_length(s) ) return false;

    char *buffer = spss_buffer(s);

    sp_string_t delete_this = spss_substring(s,pos,len);
    const char *delete_this_cstr = spss_cstr(s);

    return spss_replace(s,delete_this_cstr,"");
}

bool spss_replace(sp_string_t *s, const char *from, const char *to)
{
    if ( s == NULL || from == NULL || to == NULL ) return false;

    size_t from_len = strlen(from);
    if ( from_len == 0 ) return false;

    const char *buf = spss_cstr(s);
    const char *hit = strstr(buf, from);
    if ( hit == NULL ) return false;

    size_t pos = (size_t)(hit - buf);
    return spss_splice(s, pos, from_len, to, strlen(to));
}

size_t spss_replace_all(sp_string_t *s, const char *from, const char *to)
{
    if ( s == NULL || from == NULL || to == NULL) return 0;

    if ( spss_length(s) == 0 ) return 0;

    size_t old_len  = spss_length(s);
    size_t counter  = spss_find(s,from);
    size_t from_len = strlen(from);
    size_t to_len   = strlen(to);
    
    if ( counter == 0 ) return 0;

    size_t new_len = old_len + counter * strlen(to) - counter * strlen(from);
    
    char *buffer = spss_buffer(s);

    sp_string_t out = spss_create("");
    if ( spss_ensure_capacity(&out, new_len) == false ) return 0;
    
    const char *p = from;
    const char *hit;
    while ( (hit = strstr(p, from)) != NULL )
    {
        spss_nappend_cstr(&out, p, (size_t)(hit - p));
        spss_nappend_cstr(&out, to, to_len);
        p = hit + from_len;
    }
    spss_nappend_cstr(&out, p, old_len - (size_t)(p - from)); 

    spss_destroy(s);
    *s = out;

    return counter; 
}

bool spss_resize(sp_string_t *s, size_t new_len, char fill)
{
    if ( s == NULL ) return false;

    if ( spss_capacity(s) > new_len )
    {

    }
    else 
    {

    }

    return true;  
}

bool spss_assign(sp_string_t *s, const char *src)
{
    if ( s == NULL ) return false;

    size_t src_len = strlen(src);

    if ( spss_ensure_capacity(s,src_len) == false ) return false;

    char *buffer = spss_buffer(s);

    memcpy(buffer,src,src_len);

    spss_set_length(s,src_len);
    
    return true;
}

bool spss_swap(sp_string_t *a, sp_string_t *b)
{
    if ( a == NULL || b == NULL ) return false;

    sp_string_t *tmp = a;

    a = b;
    b = tmp;

    spss_destroy(tmp);

    return true;
}

bool spss_splice(sp_string_t *s, size_t pos, size_t del_len, const char *src, size_t src_len)
{
    // delete del_len bytes from pos.
    // append src 

    if ( s == NULL ) return false;

    if ( pos > spss_length(s) ) return false;

    if ( del_len == 0 ) return true;

    if ( spss_erase(s,pos,del_len) == false ) return false;

    if ( spss_nappend_cstr(s,src,src_len) == false ) return false;

    return true;    
}

size_t spss_find_from(const sp_string_t *s, const char *needle, size_t start)
{
    if ( s == NULL ||  needle == NULL ) return 0;

    size_t size = spss_length(s);

    if ( start > size ) return 0;

    size_t substr_size = size - start;

    sp_string_t tmp = spss_substring(s,start,substr_size);

    return spss_find(&tmp,needle);
}

size_t spss_find_char(const sp_string_t *s, char c)
{
    if ( s == NULL ) return 0;

    const char *buffer = spss_buffer((sp_string_t *)s);
    size_t size  = spss_length(s);

    size_t counter = 0;

    for ( int i = 0 ; i < size ; i++ )
    {
        if ( buffer[ i ] == c ) counter++;
    }

    return counter;
}

size_t spss_rfind(const sp_string_t *s, const char *needle)
{
    size_t nlen = strlen(needle);
    size_t len  = spss_length(s);
    if ( nlen == 0 || nlen > len ) return SP_STRING_NPOS;

    const char *buf = spss_cstr(s);

    for ( size_t i = len - nlen + 1; i-- > 0; )
        if ( memcmp(buf + i, needle, nlen) == 0 ) return i;

    return SP_STRING_NPOS;
}

size_t spss_find_first_of(const sp_string_t *s, const char *charset)
{
    if ( s == NULL ) return 0;

    char *buffer = spss_buffer((sp_string_t *)s);
    size_t size  = spss_length(s);

    size_t retVal = SP_STRING_NPOS;

    for ( int i = 0 ; i < size ; i++ )
    {
        if ( strchr( charset, buffer[i] ))
        {
            retVal = i ;
            break;
        }
    }

    return retVal;
}

size_t spss_count(const sp_string_t *s, const char *needle)
{
    if ( s == NULL || needle == NULL ) return 0;
 
    size_t nlen = strlen(needle);
    if ( nlen == 0 ) return 0;
 
    const char *p     = spss_cstr(s);
    size_t      count = 0;
 
    while ( ( p = strstr(p, needle) ) != NULL )
    {
        count++;
        p += nlen;
    }
 
    return count;
}
 
bool spss_contains(const sp_string_t *s, const char *needle)
{
    if ( s == NULL || needle == NULL ) return false;
 
    return strstr(spss_cstr(s), needle) != NULL;
}
 
bool spss_starts_with(const sp_string_t *s, const char *prefix)
{
    if ( s == NULL || prefix == NULL ) return false;
 
    size_t plen = strlen(prefix);
    size_t len  = spss_length(s);
 
    if ( plen > len ) return false;
 
    return memcmp(spss_cstr(s), prefix, plen) == 0;
}
 
bool spss_ends_with(const sp_string_t *s, const char *suffix)
{
    if ( s == NULL || suffix == NULL ) return false;
 
    size_t slen = strlen(suffix);
    size_t len  = spss_length(s);
 
    if ( slen > len ) return false;
 
    return memcmp(spss_cstr(s) + ( len - slen ), suffix, slen) == 0;
}

int spss_compare(const sp_string_t *a, const sp_string_t *b)
{
    if ( a == NULL && b == NULL ) return  0;
    if ( a == NULL )              return -1;
    if ( b == NULL )              return  1;
 
    return spss_compare_mem(spss_cstr(a), spss_length(a),
                            spss_cstr(b), spss_length(b));
}
 
int spss_compare_cstr(const sp_string_t *a, const char *b)
{
    if ( a == NULL && b == NULL ) return  0;
    if ( a == NULL )              return -1;
    if ( b == NULL )              return  1;
 
    return spss_compare_mem(spss_cstr(a), spss_length(a),
                            b,            strlen(b));
}
 
bool spss_equals_ignore_case(const sp_string_t *a, const sp_string_t *b)
{
    if ( a == NULL || b == NULL ) return false;
 
    size_t len = spss_length(a);
    if ( len != spss_length(b) ) return false;
 
    const char *pa = spss_cstr(a);
    const char *pb = spss_cstr(b);
 
    for ( size_t i = 0; i < len; i++ )
    {
        // cast to unsigned char since it could be UB
        if ( tolower((unsigned char)pa[i]) != tolower((unsigned char)pb[i]) )
            return false;
    }
 
    return true;
}

void spss_to_upper(sp_string_t *s)
{
    if ( s == NULL ) return;
 
    size_t len = spss_length(s);
    char  *buf = spss_buffer(s);
 
    for ( size_t i = 0; i < len; i++ )
        buf[i] = (char)toupper((unsigned char)buf[i]);
}
 
void spss_to_lower(sp_string_t *s)
{
    if ( s == NULL ) return;
 
    size_t len = spss_length(s);
    char  *buf = spss_buffer(s);
 
    for ( size_t i = 0; i < len; i++ )
        buf[i] = (char)tolower((unsigned char)buf[i]);
}

void spss_rtrim(sp_string_t *s)
{
    if ( s == NULL ) return;
 
    size_t len = spss_length(s);
    char  *buf = spss_buffer(s);
 
    size_t new_len = len;
    while ( new_len > 0 && isspace((unsigned char)buf[new_len - 1]) )
        new_len--;
 
    if ( new_len != len ) spss_set_length(s, new_len);
}
 
void spss_ltrim(sp_string_t *s)
{
    if ( s == NULL ) return;
 
    size_t len = spss_length(s);
    char  *buf = spss_buffer(s);
 
    size_t skip = 0;
    while ( skip < len && isspace((unsigned char)buf[skip]) )
        skip++;
 
    if ( skip == 0 ) return;
 
    memmove(buf, buf + skip, len - skip + 1);
    spss_set_length(s, len - skip);
}

void spss_trim(sp_string_t *s)
{
    if ( s == NULL ) return;
 
    spss_rtrim(s);
    spss_ltrim(s);
}

void spss_reverse(sp_string_t *s)
{
    if ( s == NULL ) return;
 
    size_t len = spss_length(s);

    if ( len < 2 ) return; // 1 char sp_string is equal to itself 
 
    char *buf = spss_buffer(s);
 
    for ( size_t i = 0, j = len - 1; i < j; i++, j-- )
    {
        char tmp = buf[i];
        buf[i]   = buf[j];
        buf[j]   = tmp;
    }
}