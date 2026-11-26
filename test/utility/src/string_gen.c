#include "string_gen.h"

char *string_gen(gen_kind_t kind, size_t n, uint64_t seed)
{
    char *b = (char *)malloc(n + 1);
    if ( b == NULL ) return NULL;

    uint64_t state = seed ? seed : 0x9E3779B97F4A7C15ULL;

    for ( size_t i = 0; i < n; i++ )
    {
        switch ( kind )
        {
            case GEN_SAME:    b[i] = 'a';                     break;
            case GEN_ALPHA:   b[i] = (char)('a' + (i % 26));  break;
            case GEN_DIGITS:  b[i] = (char)('0' + (i % 10));  break;

            case GEN_COUNTER:
            {
                // every 4096 it repeats.
                static const char HEX[] = "0123456789abcdef";
                size_t block = ( i / 4 ) % 4096;
                size_t slot  = i % 4;
                b[i] = ( slot == 3 ) ? ':' : HEX[ ( block >> ( 4 * ( 2 - slot ) ) ) & 0xF ];
                break;
            }
            case GEN_BYTES:
            {
                b[i] = (char)( 1 + ( i % 255 ) );
                break;
            }
            case GEN_RANDOM:
            {
                // xorshift algorithm to shuffle bytes
                state ^= state << 13; state ^= state >> 7; state ^= state << 17;
                b[i] = (char)( 0x20 + ( state % 95 ) );
                break;
            }
            case GEN_CASE_MIX:
            {
                b[i] = ( i % 2 ) ? (char)('A' + ( ( i / 2 ) % 26 ))
                                 : (char)('a' + ( ( i / 2 ) % 26 ));
                break;
            }
            case GEN_SPACES:
            {
                static const char WS[] = { ' ', '\t', '\n', '\r' };
                b[i] = WS[ i % 4 ];
                break;
            }
        }
    }

    b[n] = '\0';
    return b;
}

char *string_gen_with_needle(size_t n, const char *needle, size_t pos)
{
    size_t nlen = strlen(needle);
    if ( pos + nlen > n ) return NULL;

    char *b = string_gen(GEN_ALPHA, n, 0);
    if ( b == NULL ) return NULL;

    memcpy(b + pos, needle, nlen);
    return b;
}

char *string_gen_delimited(size_t fields, size_t field_len, char delim)
{
    if ( fields == 0 ) return (char *)calloc(1, 1);

    size_t total = fields * field_len + ( fields - 1 );
    char  *b     = (char *)malloc(total + 1);
    if ( b == NULL ) return NULL;

    size_t w = 0;
    for ( size_t f = 0; f < fields; f++ )
    {
        if ( f > 0 ) b[w++] = delim;
        for ( size_t i = 0; i < field_len; i++ )
            b[w++] = (char)( 'a' + ( f % 26 ) );
    }
    b[w] = '\0';
    return b;
}

char *string_gen_padded(const char *core, size_t left, size_t right)
{
    size_t clen = strlen(core);
    char  *b    = (char *)malloc(left + clen + right + 1);
    if ( b == NULL ) return NULL;

    memset(b, ' ', left);
    memcpy(b + left, core, clen);
    memset(b + left + clen, ' ', right);
    b[left + clen + right] = '\0';
    return b;
}
