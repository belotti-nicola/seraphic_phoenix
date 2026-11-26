#include "string_builder.h"

bool string_builder_init(string_builder_t *encoder,char *buffer,size_t buffer_size)
{
    if ( encoder == NULL || buffer == NULL || buffer_size == 0 ) return false;

    encoder->buffer       = buffer;
    encoder->buffer_size  = buffer_size;
    encoder->current_size = 0;

    return true;
}

bool string_builder_add(string_builder_t *encoder, gen_kind_t kind, uint64_t seed, size_t n)
{
    if ( encoder == NULL ) return false;

    if ( n == 0 ) return false;

    size_t max_size       = encoder->buffer_size;
    size_t current_size   = encoder->current_size;

    size_t available_size = max_size - current_size;

    if ( available_size < n + 1 ) return false;

    size_t offset = encoder->current_size;
    char  *buffer = encoder->buffer + offset;

    for ( int i = 0 ; i < n ; i++)
    {
        switch( kind )
        {
            case GEN_SAME:
            {
                buffer[ i ] = 'a';
                break;
            }
            case GEN_ALPHA:
            {
                buffer[ i ] = (char)('a' + (i % 26));
                break;
            }
            case GEN_DIGITS:
            {    
                buffer[ i ] = (char)('0' + (i % 10));
                break;
            }
            case GEN_COUNTER:
            {
                static const char HEX[] = "0123456789abcdef";
                size_t block = ( i / 4 ) % 4096;
                size_t slot  = i % 4;
                buffer[i] = ( slot == 3 ) ? ':' : HEX[ ( block >> ( 4 * ( 2 - slot ) ) ) & 0xF ];
                break;
            }
            case GEN_BYTES:
            {
                buffer[ i ] = (char)( 1 + ( i % 255 ) );
                break;
            }
            case GEN_RANDOM:
            {
                // xorshift algorithm to shuffle bytes
                uint64_t state = seed ? seed : 0x9E3779B97F4A7C15ULL;
                state ^= state << 13; state ^= state >> 7; state ^= state << 17;
                buffer[ i ] = (char)( 0x20 + ( state % 95 ) );
                break;
            }
            case GEN_CASE_MIX:
            {
                buffer[i] = ( i % 2 ) ? (char)('A' + ( ( i / 2 ) % 26 )) :
                                        (char)('a' + ( ( i / 2 ) % 26 )) ;
                break;
            }
            case GEN_SPACES:
            {
                static const char WS[] = { ' ', '\t', '\n', '\r' };
                buffer[i] = WS[ i % 4 ];
                break;
            }
        }
    }    

    encoder->current_size += n;

    encoder->buffer[encoder->current_size] = '\0';

    return true;

}

bool string_builder_recipe(string_builder_t *encoder, recipe_t *recipe)
{
    if ( encoder == NULL || recipe == NULL ) return false;

    size_t available_size = encoder->buffer_size;
    size_t expected_size  = string_recipe_expected_size(recipe);

    if ( available_size < expected_size ) 
    {
        return false;
    }

    for ( int i = 0 ; i < recipe->entries ; i++ )
    {
        gen_kind_t gen_kind = recipe->gen_token[ i ];
        size_t     gen_size = recipe->gen_size[  i ];

        if ( string_builder_add(encoder,gen_kind,0,gen_size) == false ) 
        {
            return false;
        }
    }

    return true;
}